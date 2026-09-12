#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cstdlib>

#include "models/task.h"
#include "models/node.h"
#include "models/region.h"
#include "models/scheduling_event.h"
#include "models/metrics.h"
#include "utils/timer.h"
#include "utils/logger.h"
#include "utils/json_writer.h"
#include "workload/workload_generator.h"
#include "workload/load_controller.h"
#include "cloud_state/cloud_state.h"
#include "carbon_engine/carbon_engine.h"
#include "daa_engine/daa_engine.h"
#include "allocation/resource_allocator.h"
#include "execution/execution_engine.h"
#include "metrics/metrics_engine.h"
#include "baseline/baseline_scheduler.h"
#include "server/server.h"

using namespace carbongrid;

int main(int argc, char* argv[]) {
    Logger::get_instance().set_min_level(LogLevel::INFO);
    Logger::get_instance().log(LogLevel::INFO, "=== CarbonGrid - Carbon-Aware Cloud Scheduler ===");

    // === PHASE 1: Initialize Data Sources ===
    Logger::get_instance().log(LogLevel::INFO, "Loading node profiles...");
    CloudState cloud_state;
    cloud_state.load_node_profiles("data/nodes/node_profiles.csv");
    cloud_state.initialize_regions();
    Logger::get_instance().log(LogLevel::INFO,
        "Loaded " + std::to_string(cloud_state.total_nodes()) + " nodes across " +
        std::to_string(cloud_state.get_regions().size()) + " regions");

    Logger::get_instance().log(LogLevel::INFO, "Loading carbon intensity data...");
    CarbonEngine carbon_engine;
    carbon_engine.load_carbon_data("data/carbon/carbon_intensity.csv");
    Logger::get_instance().log(LogLevel::INFO, "Carbon data loaded for " +
        std::to_string(carbon_engine.get_regions().size()) + " regions");

    // === PHASE 2: Initialize Workload Generator ===
    WorkloadGenerator workload_gen;
    LoadController load_ctrl;
    load_ctrl.set_profile(LoadController::moderate_ramp());

    // === PHASE 3: Initialize Engines ===
    DAAEngine daa_engine(cloud_state, carbon_engine);
    ResourceAllocator allocator(cloud_state, carbon_engine);
    ExecutionEngine execution_engine(cloud_state, allocator);
    MetricsEngine metrics;
    BaselineScheduler baseline(BaselineStrategy::ROUND_ROBIN);

    // === PHASE 4: Initialize Dashboard Server ===
    DashboardServer server;
    server.set_command_callback([&](const std::string& cmd, const std::string& val) {
        if (cmd == "set_rate") {
            double rate = std::stod(val);
            workload_gen.set_arrival_rate(rate);
            Logger::get_instance().log(LogLevel::INFO, "Rate changed to " + val + " tasks/sec");
        }
    });
    server.start();
    Logger::get_instance().log(LogLevel::INFO, "Dashboard: open dashboard/index.html in browser");

    // === PHASE 5: Simulation Loop ===
    SimulationClock sim_clock;
    sim_clock.set_speed(10.0);  // 10x speed
    sim_clock.start();

    double next_task_time = 0.0;
    double metrics_interval = 5.0;  // snapshot every 5 sim seconds
    double last_metrics_time = 0.0;
    int total_tasks_to_generate = 500;
    int tasks_generated = 0;

    // Store tasks on heap since ExecutionEngine stores Task* pointers
    std::vector<Task*> all_tasks;

    Logger::get_instance().log(LogLevel::INFO, "Starting simulation (" +
        std::to_string(total_tasks_to_generate) + " tasks)...");

    try {
    while (tasks_generated < total_tasks_to_generate || execution_engine.running_count() > 0) {
        double now = sim_clock.now();

        // Update execution (check for completed tasks)
        execution_engine.update(now);

        // Generate new tasks
        if (tasks_generated < total_tasks_to_generate && now >= next_task_time) {
            double rate = load_ctrl.get_rate_at(now);
            workload_gen.set_arrival_rate(rate);

            Task* task = new Task(workload_gen.generate_task(now));
            all_tasks.push_back(task);
            tasks_generated++;

            Logger::get_instance().log(LogLevel::DEBUG,
                "Task " + task->task_id + " cpu=" + std::to_string(task->cpu_required) +
                " ram=" + std::to_string(task->ram_required) +
                " deadline=" + std::to_string(task->deadline));

            // === CarbonGrid Scheduling ===
            auto start_time = std::chrono::high_resolution_clock::now();
            Logger::get_instance().log(LogLevel::DEBUG, "Calling daa_engine.schedule...");
            SchedulingDecision decision = daa_engine.schedule(*task, now);
            Logger::get_instance().log(LogLevel::DEBUG,
                "Decision: success=" + std::to_string(decision.success) +
                " node=" + decision.node_id);
            auto end_time = std::chrono::high_resolution_clock::now();
            double sched_time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();

            if (decision.success) {
                execution_engine.submit_task(*task);
                execution_engine.schedule_task(*task, decision.node_id, now, decision.time_slot);

                metrics.record_scheduling_event();
                metrics.record_task_scheduled(*task);
                metrics.record_scheduling_time(sched_time_ms);
                metrics.record_carbongrid_carbon(task->carbon_cost);
                metrics.record_carbongrid_cost(task->monetary_cost);

                // === Baseline comparison ===
                std::string baseline_node = baseline.select_node(*task, cloud_state);
                if (!baseline_node.empty()) {
                    const Node& bnode = cloud_state.get_node(baseline_node);
                    double b_carbon = carbon_engine.compute_task_carbon(
                        task->cpu_required, task->execution_duration,
                        carbon_engine.get_carbon_intensity(bnode.region_id, now),
                        bnode.power_usage_effectiveness);
                    double b_cost = carbon_engine.compute_task_cost(
                        task->cpu_required, task->execution_duration,
                        carbon_engine.get_electricity_cost(bnode.region_id, now),
                        bnode.power_usage_effectiveness);
                    metrics.record_baseline_carbon(b_carbon);
                    metrics.record_baseline_cost(b_cost);
                }

                Logger::get_instance().log(LogLevel::DEBUG,
                    "Task " + task->task_id + " -> " + decision.node_id +
                    " [" + decision.algorithm_used + "] score=" + std::to_string(decision.score));
            } else {
                Logger::get_instance().log(LogLevel::WARNING, "No feasible node for task " + task->task_id);
            }

            next_task_time = now + workload_gen.next_inter_arrival_time();
        }

        // Periodic metrics snapshot
        if (now - last_metrics_time >= metrics_interval) {
            metrics.take_snapshot(now, cloud_state);

            // Build JSON for dashboard
            JsonObject json;
            json.set("type", std::string("metrics"));
            json.set("timestamp", now);
            json.set("task_rate", workload_gen.get_arrival_rate());
            json.set("pending_count", execution_engine.pending_count());
            json.set("running_count", execution_engine.running_count());
            json.set("completed_count", execution_engine.completed_count());
            json.set("total_events", metrics.total_scheduling_events());
            json.set("scheduled_count", metrics.total_scheduled());
            json.set("avg_scheduling_time", metrics.avg_scheduling_time_ms());
            json.set("avg_cpu", cloud_state.avg_cpu_utilization());
            json.set("avg_ram", cloud_state.avg_ram_utilization());
            json.set("queue_length", execution_engine.pending_count());
            json.set("throughput", metrics.throughput(now));
            json.set("carbon_reduction", metrics.carbon_reduction_pct());
            json.set("cost_reduction", metrics.cost_reduction_pct());
            json.set("baseline_carbon", metrics.baseline_total_carbon());
            json.set("carbongrid_carbon", metrics.total_carbon());
            json.set("baseline_cost", metrics.baseline_total_cost());
            json.set("carbongrid_cost", metrics.total_cost());

            server.write_metrics_file(json.to_string());

            last_metrics_time = now;
        }

        // Advance simulation clock
        sim_clock.advance(0.1);

        // Progress log every 100 tasks
        if (tasks_generated > 0 && tasks_generated % 100 == 0) {
            static int last_logged = 0;
            if (tasks_generated != last_logged) {
                last_logged = tasks_generated;
                Logger::get_instance().log(LogLevel::INFO,
                    "Progress: " + std::to_string(tasks_generated) + "/" +
                    std::to_string(total_tasks_to_generate) + " tasks generated");
            }
        }
    }
    } catch (const std::exception& e) {
        Logger::get_instance().log(LogLevel::ERROR, std::string("Simulation error: ") + e.what());
    }

    // === PHASE 6: Final Report ===
    Logger::get_instance().log(LogLevel::INFO, "\n========== SIMULATION COMPLETE ==========");
    Logger::get_instance().log(LogLevel::INFO, "Tasks Generated:     " + std::to_string(tasks_generated));
    Logger::get_instance().log(LogLevel::INFO, "Tasks Completed:     " + std::to_string(metrics.total_completed()));
    Logger::get_instance().log(LogLevel::INFO, "Tasks Failed:        " + std::to_string(metrics.total_failed()));
    Logger::get_instance().log(LogLevel::INFO, "Deadline Violations: " + std::to_string(metrics.total_deadline_violations()));
    Logger::get_instance().log(LogLevel::INFO, "Carbon Reduction:    " + std::to_string(metrics.carbon_reduction_pct()) + "%");
    Logger::get_instance().log(LogLevel::INFO, "Cost Reduction:      " + std::to_string(metrics.cost_reduction_pct()) + "%");
    Logger::get_instance().log(LogLevel::INFO, "Avg Scheduling Time: " + std::to_string(metrics.avg_scheduling_time_ms()) + "ms");
    Logger::get_instance().log(LogLevel::INFO, "Throughput:          " + std::to_string(metrics.throughput(sim_clock.now())) + " tasks/sec");
    Logger::get_instance().log(LogLevel::INFO, "=========================================");

    // Cleanup heap-allocated tasks
    for (Task* t : all_tasks) {
        delete t;
    }

    server.stop();
    return 0;
}

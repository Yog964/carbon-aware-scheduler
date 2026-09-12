#include "mcmf_scheduler.h"
#include "carbon_engine/carbon_engine.h"
#include <queue>
#include <limits>
#include <algorithm>
#include <cmath>

namespace carbongrid {

MCMFScheduler::MCMFScheduler() {}

MCMFScheduler::MCMFScheduler(const ScoringWeights& weights) : scoring_(weights) {}

MCMFResult MCMFScheduler::schedule_batch(const std::vector<Task*>& tasks,
                                         std::vector<Node*>& feasible_nodes,
                                         const CarbonEngine& carbon_engine,
                                         double current_time,
                                         int num_time_slots) {
    if (tasks.empty() || feasible_nodes.empty()) {
        return {false, 0.0, 0.0, {}};
    }
    
    auto graph_mapping = build_graph(tasks, feasible_nodes, carbon_engine, current_time, num_time_slots);
    auto flow_cost = solve_mcmf(graph_mapping.first, graph_mapping.second.source, graph_mapping.second.sink);
    
    MCMFResult result = extract_assignments(graph_mapping.first, graph_mapping.second);
    result.total_cost = flow_cost.second;
    result.total_flow = flow_cost.first;
    result.success = (flow_cost.first > 0);
    
    return result;
}

std::pair<FlowGraph, MCMFScheduler::GraphMapping> MCMFScheduler::build_graph(
    const std::vector<Task*>& tasks,
    std::vector<Node*>& feasible_nodes,
    const CarbonEngine& carbon_engine,
    double current_time,
    int num_time_slots) {
    
    int num_tasks = tasks.size();
    int num_nodes = feasible_nodes.size();
    
    // 1. Calculate total vertices: 1 (source) + num_tasks + num_nodes * num_time_slots + 1 (sink)
    int total_vertices = 1 + num_tasks + (num_nodes * num_time_slots) + 1;
    FlowGraph graph(total_vertices);
    
    GraphMapping mapping;
    mapping.source = 0;
    mapping.sink = total_vertices - 1;
    
    // Calculate avg task CPU for capacity estimation
    double avg_task_cpu = 0;
    for (const Task* t : tasks) {
        avg_task_cpu += t->cpu_required;
    }
    if (num_tasks > 0) avg_task_cpu /= num_tasks;
    if (avg_task_cpu == 0) avg_task_cpu = 1.0;
    
    // 4. Source -> Task edges
    for (int i = 0; i < num_tasks; ++i) {
        int task_vertex = 1 + i;
        mapping.vertex_to_task[task_vertex] = tasks[i]->task_id;
        graph.add_edge(mapping.source, task_vertex, 1.0, 0.0);
    }
    
    // 5. Task -> Node-TimeSlot edges
    for (int i = 0; i < num_tasks; ++i) {
        int task_vertex = 1 + i;
        for (int j = 0; j < num_nodes; ++j) {
            for (int k = 0; k < num_time_slots; ++k) {
                int node_slot_vertex = 1 + num_tasks + (j * num_time_slots + k);
                mapping.vertex_to_node_slot[node_slot_vertex] = {feasible_nodes[j]->node_id, k};
                
                // Estimate time for this slot
                double slot_time = current_time + (k * 3600.0); // Assuming 1 hour slots for example
                
                double intensity = carbon_engine.get_carbon_intensity(feasible_nodes[j]->region_id, slot_time);
                double cost = carbon_engine.get_electricity_cost(feasible_nodes[j]->region_id, slot_time);
                double renewable = carbon_engine.get_renewable_pct(feasible_nodes[j]->region_id, slot_time);
                
                NodeScore score = scoring_.compute(*tasks[i], *feasible_nodes[j], intensity, cost, renewable, slot_time);
                
                // Add edge with capacity 1 and cost = edge_cost
                graph.add_edge(task_vertex, node_slot_vertex, 1.0, score.total_score);
            }
        }
    }
    
    // 6. Node-TimeSlot -> Sink edges
    for (int j = 0; j < num_nodes; ++j) {
        for (int k = 0; k < num_time_slots; ++k) {
            int node_slot_vertex = 1 + num_tasks + (j * num_time_slots + k);
            
            // Capacity based on how many tasks node can handle
            double cpu_avail = feasible_nodes[j]->cpu_capacity - feasible_nodes[j]->cpu_used;
            double capacity = std::max(1.0, std::floor(cpu_avail / avg_task_cpu));
            
            graph.add_edge(node_slot_vertex, mapping.sink, capacity, 0.0);
        }
    }
    
    return {graph, mapping};
}

MCMFScheduler::DijkstraResult MCMFScheduler::dijkstra(const FlowGraph& graph, int source, int sink,
                                                      const std::vector<double>& potential) {
    int n = graph.num_nodes();
    DijkstraResult result;
    result.dist.assign(n, std::numeric_limits<double>::infinity());
    result.parent_vertex.assign(n, -1);
    result.parent_edge.assign(n, -1);
    result.sink_reachable = false;
    
    using PDI = std::pair<double, int>;
    std::priority_queue<PDI, std::vector<PDI>, std::greater<PDI>> pq;
    
    result.dist[source] = 0;
    pq.push({0, source});
    
    while (!pq.empty()) {
        auto top = pq.top();
        double d = top.first;
        int u = top.second;
        pq.pop();
        
        if (d > result.dist[u]) continue;
        if (u == sink) {
            result.sink_reachable = true;
        }
        
        const auto& adj = graph.adj(u);
        for (size_t i = 0; i < adj.size(); ++i) {
            const auto& edge = adj[i];
            if (edge.residual_capacity() > 1e-9) {
                // Reduced cost with Johnson's potentials
                double reduced_cost = edge.cost + potential[u] - potential[edge.to];
                if (result.dist[u] + reduced_cost < result.dist[edge.to]) {
                    result.dist[edge.to] = result.dist[u] + reduced_cost;
                    result.parent_vertex[edge.to] = u;
                    result.parent_edge[edge.to] = i;
                    pq.push({result.dist[edge.to], edge.to});
                }
            }
        }
    }
    
    return result;
}

std::pair<double, double> MCMFScheduler::solve_mcmf(FlowGraph& graph, int source, int sink) {
    int n = graph.num_nodes();
    std::vector<double> potential(n, 0.0);
    double total_flow = 0;
    double total_cost = 0;
    
    while (true) {
        DijkstraResult result = dijkstra(graph, source, sink, potential);
        if (!result.sink_reachable) break;
        
        // Find bottleneck capacity
        double path_flow = std::numeric_limits<double>::infinity();
        int v = sink;
        while (v != source) {
            int u = result.parent_vertex[v];
            int e_idx = result.parent_edge[v];
            const auto& edge = graph.adj(u)[e_idx];
            path_flow = std::min(path_flow, edge.residual_capacity());
            v = u;
        }
        
        // Augment flow
        v = sink;
        while (v != source) {
            int u = result.parent_vertex[v];
            int e_idx = result.parent_edge[v];
            auto& edge = graph.adj(u)[e_idx];
            edge.flow += path_flow;
            graph.adj(v)[edge.rev].flow -= path_flow;
            v = u;
        }
        
        total_flow += path_flow;
        // Cost is actual cost without potentials
        // Since we are dealing with potentials, it's easier to accumulate using standard edge cost
        double path_cost = 0;
        int curr = sink;
        while (curr != source) {
            int u = result.parent_vertex[curr];
            int e_idx = result.parent_edge[curr];
            path_cost += graph.adj(u)[e_idx].cost;
            curr = u;
        }
        total_cost += path_flow * path_cost;
        
        // Update potentials
        for (int i = 0; i < n; ++i) {
            if (result.dist[i] != std::numeric_limits<double>::infinity()) {
                potential[i] += result.dist[i];
            }
        }
    }
    
    return {total_flow, total_cost};
}

MCMFResult MCMFScheduler::extract_assignments(const FlowGraph& graph,
                                              const GraphMapping& mapping) {
    MCMFResult result;
    
    // For each task vertex, check outgoing edges
    for (const auto& pair : mapping.vertex_to_task) {
        int task_vertex = pair.first;
        const std::string& task_id = pair.second;
        const auto& adj = graph.adj(task_vertex);
        for (const auto& edge : adj) {
            if (edge.flow > 1e-9 && mapping.vertex_to_node_slot.count(edge.to)) {
                auto node_slot = mapping.vertex_to_node_slot.at(edge.to);
                result.assignments[task_id] = {node_slot.first, node_slot.second};
                break; // One task assigned to one node-slot
            }
        }
    }
    
    return result;
}

}

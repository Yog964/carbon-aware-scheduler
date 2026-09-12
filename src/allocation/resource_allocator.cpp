#include "resource_allocator.h"
#include "../cloud_state/cloud_state.h"
#include "../carbon_engine/carbon_engine.h"

namespace carbongrid {

ResourceAllocator::ResourceAllocator(CloudState& cloud_state, CarbonEngine& carbon_engine)
    : cloud_state_(cloud_state), carbon_engine_(carbon_engine) {}

bool ResourceAllocator::allocate(Task& task, const std::string& node_id, double timestamp, int time_slot) {
    // 1. Check & allocate resources on the node
    if (!cloud_state_.allocate_resources(node_id, task.cpu_required, task.ram_required, task.task_id)) {
        return false;  // Node cannot fit this task
    }

    // 2. Update task fields
    const Node& node = cloud_state_.get_node(node_id);
    task.assigned_node_id = node_id;
    task.assigned_region = node.region_id;
    task.assigned_time_slot = time_slot;
    task.scheduled_time = timestamp;
    task.state = TaskState::SCHEDULED;

    // 3. Compute carbon and monetary costs using CarbonEngine
    double carbon_intensity = carbon_engine_.get_carbon_intensity(node.region_id, timestamp);
    double electricity_cost = carbon_engine_.get_electricity_cost(node.region_id, timestamp);

    task.carbon_cost = carbon_engine_.compute_task_carbon(
        task.cpu_required, task.execution_duration,
        carbon_intensity, node.power_usage_effectiveness);

    task.monetary_cost = carbon_engine_.compute_task_cost(
        task.cpu_required, task.execution_duration,
        electricity_cost, node.power_usage_effectiveness);

    return true;
}

bool ResourceAllocator::release(Task& task) {
    if (task.assigned_node_id.empty()) {
        return false;  // Not allocated
    }

    cloud_state_.release_resources(task.assigned_node_id, task.cpu_required, task.ram_required, task.task_id);
    return true;
}

} // namespace carbongrid

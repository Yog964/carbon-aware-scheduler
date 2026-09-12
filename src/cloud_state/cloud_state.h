#ifndef CARBONGRID_CLOUD_STATE_H
#define CARBONGRID_CLOUD_STATE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../models/node.h"
#include "../models/region.h"

namespace carbongrid {

class CloudState {
public:
    CloudState();
    
    // Initialize from data files
    void load_node_profiles(const std::string& filepath);
    void initialize_regions();
    
    // Node operations
    Node& get_node(const std::string& node_id);
    const Node& get_node(const std::string& node_id) const;
    std::vector<Node*> get_nodes_in_region(const std::string& region_id);
    std::vector<Node*> get_all_nodes();
    std::vector<Region>& get_regions();
    
    // Pod/Task state tracking
    void add_pending_task(const std::string& task_id);
    void mark_task_scheduled(const std::string& task_id);
    void mark_task_running(const std::string& task_id);
    void mark_task_completed(const std::string& task_id);
    void mark_task_failed(const std::string& task_id);
    
    // Resource operations
    bool allocate_resources(const std::string& node_id, double cpu, double ram, const std::string& task_id);
    void release_resources(const std::string& node_id, double cpu, double ram, const std::string& task_id);
    
    // Queries
    int pending_count() const;
    int running_count() const;
    int completed_count() const;
    int failed_count() const;
    int total_nodes() const;
    double avg_cpu_utilization() const;
    double avg_ram_utilization() const;
    
private:
    std::unordered_map<std::string, Node> nodes_;
    std::vector<Region> regions_;
    std::unordered_set<std::string> pending_tasks_;
    std::unordered_set<std::string> scheduled_tasks_;
    std::unordered_set<std::string> running_tasks_;
    std::unordered_set<std::string> completed_tasks_;
    std::unordered_set<std::string> failed_tasks_;
};

} // namespace carbongrid

#endif // CARBONGRID_CLOUD_STATE_H

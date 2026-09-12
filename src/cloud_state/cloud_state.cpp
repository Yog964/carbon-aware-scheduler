#include "cloud_state.h"
#include "../utils/csv_parser.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>

namespace carbongrid {

CloudState::CloudState() = default;

void CloudState::load_node_profiles(const std::string& filepath) {
    // Parse CSV: node_id,region_id,cpu_capacity,ram_capacity_gb,pue,is_available
    auto rows = CSVParser::parse(filepath);
    for (const auto& row : rows) {
        Node n;
        n.node_id = row[0];
        n.region_id = row[1];
        n.cpu_capacity = std::stod(row[2]);
        n.ram_capacity = std::stod(row[3]);
        n.power_usage_effectiveness = std::stod(row[4]);
        n.is_available = (row[5] == "true" || row[5] == "1");
        n.cpu_used = 0.0;
        n.ram_used = 0.0;
        
        nodes_[n.node_id] = n;
    }
}

void CloudState::initialize_regions() {
    std::unordered_map<std::string, Region> region_map;
    
    for (const auto& pair : nodes_) {
        const auto& node_id = pair.first;
        const auto& node = pair.second;
        if (region_map.find(node.region_id) == region_map.end()) {
            Region r;
            r.region_id = node.region_id;
            r.region_name = node.region_id;
            region_map[node.region_id] = r;
        }
        region_map[node.region_id].node_ids.push_back(node_id);
    }
    
    regions_.clear();
    for (const auto& pair : region_map) {
        regions_.push_back(pair.second);
    }
}

Node& CloudState::get_node(const std::string& node_id) {
    return nodes_.at(node_id);
}

const Node& CloudState::get_node(const std::string& node_id) const {
    return nodes_.at(node_id);
}

std::vector<Node*> CloudState::get_nodes_in_region(const std::string& region_id) {
    std::vector<Node*> res;
    for (auto& pair : nodes_) {
        auto& node = pair.second;
        if (node.region_id == region_id) {
            res.push_back(&node);
        }
    }
    return res;
}

std::vector<Node*> CloudState::get_all_nodes() {
    std::vector<Node*> res;
    for (auto& pair : nodes_) {
        res.push_back(&pair.second);
    }
    return res;
}

std::vector<Region>& CloudState::get_regions() {
    return regions_;
}

void CloudState::add_pending_task(const std::string& task_id) {
    pending_tasks_.insert(task_id);
}

void CloudState::mark_task_scheduled(const std::string& task_id) {
    pending_tasks_.erase(task_id);
    scheduled_tasks_.insert(task_id);
}

void CloudState::mark_task_running(const std::string& task_id) {
    scheduled_tasks_.erase(task_id);
    running_tasks_.insert(task_id);
}

void CloudState::mark_task_completed(const std::string& task_id) {
    running_tasks_.erase(task_id);
    completed_tasks_.insert(task_id);
}

void CloudState::mark_task_failed(const std::string& task_id) {
    running_tasks_.erase(task_id);
    pending_tasks_.erase(task_id);
    scheduled_tasks_.erase(task_id);
    failed_tasks_.insert(task_id);
}

bool CloudState::allocate_resources(const std::string& node_id, double cpu, double ram, const std::string& task_id) {
    auto& node = nodes_.at(node_id);
    
    if (!node.is_available) return false;
    if (!node.can_fit(cpu, ram)) return false;
    
    node.allocate(cpu, ram, task_id);
    return true;
}

void CloudState::release_resources(const std::string& node_id, double cpu, double ram, const std::string& task_id) {
    auto& node = nodes_.at(node_id);
    node.release(cpu, ram, task_id);
}

int CloudState::pending_count() const { return static_cast<int>(pending_tasks_.size()); }
int CloudState::running_count() const { return static_cast<int>(running_tasks_.size()); }
int CloudState::completed_count() const { return static_cast<int>(completed_tasks_.size()); }
int CloudState::failed_count() const { return static_cast<int>(failed_tasks_.size()); }
int CloudState::total_nodes() const { return static_cast<int>(nodes_.size()); }

double CloudState::avg_cpu_utilization() const {
    if (nodes_.empty()) return 0.0;
    double total_util = 0.0;
    for (const auto& pair : nodes_) {
        const auto& node = pair.second;
        total_util += node.cpu_utilization();
    }
    return total_util / nodes_.size();
}

double CloudState::avg_ram_utilization() const {
    if (nodes_.empty()) return 0.0;
    double total_util = 0.0;
    for (const auto& pair : nodes_) {
        const auto& node = pair.second;
        total_util += node.ram_utilization();
    }
    return total_util / nodes_.size();
}

} // namespace carbongrid

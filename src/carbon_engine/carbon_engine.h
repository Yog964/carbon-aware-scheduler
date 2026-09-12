#ifndef CARBONGRID_CARBON_ENGINE_H
#define CARBONGRID_CARBON_ENGINE_H

#include <string>
#include <vector>
#include <unordered_map>
#include "../models/node.h" // Includes definitions for Node and Region

namespace carbongrid {

struct CarbonData {
    double timestamp;
    std::string region_id;
    double carbon_intensity;  // gCO2/kWh
    double renewable_pct;     // 0-100
    double electricity_cost;  // $/kWh
};

class CarbonEngine {
public:
    CarbonEngine();
    
    // Load data
    void load_carbon_data(const std::string& filepath);
    void set_predictions(const std::string& predictions_filepath);
    
    // Lookups
    CarbonData get_carbon_data(const std::string& region_id, double timestamp) const;
    double get_carbon_intensity(const std::string& region_id, double timestamp) const;
    double get_renewable_pct(const std::string& region_id, double timestamp) const;
    double get_electricity_cost(const std::string& region_id, double timestamp) const;
    
    // Carbon score: combines intensity, renewable, cost into a single score [0,1]
    // Lower is better (greener)
    double compute_carbon_score(const std::string& region_id, double timestamp) const;
    
    // Compute carbon emission for running a task on a node
    // carbon = power_consumption_kw * duration_hours * carbon_intensity * PUE
    double compute_task_carbon(double cpu_used, double duration_secs,
                                double carbon_intensity, double pue) const;
    
    // Compute monetary cost for running a task
    double compute_task_cost(double cpu_used, double duration_secs,
                             double electricity_cost, double pue) const;
    
    // Get all regions with data
    std::vector<std::string> get_regions() const;
    
private:
    // Indexed by region_id -> sorted vector of CarbonData by timestamp
    std::unordered_map<std::string, std::vector<CarbonData>> carbon_data_;
    std::unordered_map<std::string, std::vector<CarbonData>> predictions_;
    
    // Find nearest data point for a given timestamp
    // Algorithm complexity: O(log N) where N is number of data points via binary search
    const CarbonData& find_nearest(const std::vector<CarbonData>& data, double timestamp) const;
};

} // namespace carbongrid

#endif // CARBONGRID_CARBON_ENGINE_H

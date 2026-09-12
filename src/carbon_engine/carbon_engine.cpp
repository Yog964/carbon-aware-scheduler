#include "carbon_engine.h"
#include "../utils/csv_parser.h"
#include <algorithm>
#include <stdexcept>
#include <cmath>

namespace carbongrid {

CarbonEngine::CarbonEngine() = default;

void CarbonEngine::load_carbon_data(const std::string& filepath) {
    auto rows = CSVParser::parse(filepath);
    for (const auto& row : rows) {
        CarbonData data;
        data.timestamp = std::stod(row[0]);
        data.region_id = row[1];
        data.carbon_intensity = std::stod(row[2]);
        data.renewable_pct = std::stod(row[3]);
        data.electricity_cost = std::stod(row[4]);
        
        carbon_data_[data.region_id].push_back(data);
    }
    
    // Sort data for binary search
    for (auto& pair : carbon_data_) {
        auto& data_vec = pair.second;
        std::sort(data_vec.begin(), data_vec.end(), 
                  [](const CarbonData& a, const CarbonData& b) {
                      return a.timestamp < b.timestamp;
                  });
    }
}

void CarbonEngine::set_predictions(const std::string& predictions_filepath) {
    auto rows = CSVParser::parse(predictions_filepath);
    for (const auto& row : rows) {
        CarbonData data;
        data.timestamp = std::stod(row[0]);
        data.region_id = row[1];
        data.carbon_intensity = std::stod(row[2]);
        data.renewable_pct = std::stod(row[3]);
        data.electricity_cost = std::stod(row[4]);
        
        predictions_[data.region_id].push_back(data);
    }
    
    for (auto& pair : predictions_) {
        auto& data_vec = pair.second;
        std::sort(data_vec.begin(), data_vec.end(), 
                  [](const CarbonData& a, const CarbonData& b) {
                      return a.timestamp < b.timestamp;
                  });
    }
}

const CarbonData& CarbonEngine::find_nearest(const std::vector<CarbonData>& data, double timestamp) const {
    if (data.empty()) {
        throw std::runtime_error("No carbon data available");
    }
    
    // Binary search for closest timestamp. DAA Complexity: O(log N)
    auto it = std::lower_bound(data.begin(), data.end(), timestamp,
                               [](const CarbonData& cd, double ts) {
                                   return cd.timestamp < ts;
                               });
                               
    if (it == data.begin()) {
        return *it;
    } else if (it == data.end()) {
        return *(it - 1);
    } else {
        auto prev = it - 1;
        if ((timestamp - prev->timestamp) < (it->timestamp - timestamp)) {
            return *prev;
        } else {
            return *it;
        }
    }
}

CarbonData CarbonEngine::get_carbon_data(const std::string& region_id, double timestamp) const {
    auto it = carbon_data_.find(region_id);
    if (it == carbon_data_.end()) {
        throw std::runtime_error("Region not found in carbon data: " + region_id);
    }
    return find_nearest(it->second, timestamp);
}

double CarbonEngine::get_carbon_intensity(const std::string& region_id, double timestamp) const {
    return get_carbon_data(region_id, timestamp).carbon_intensity;
}

double CarbonEngine::get_renewable_pct(const std::string& region_id, double timestamp) const {
    return get_carbon_data(region_id, timestamp).renewable_pct;
}

double CarbonEngine::get_electricity_cost(const std::string& region_id, double timestamp) const {
    return get_carbon_data(region_id, timestamp).electricity_cost;
}

double CarbonEngine::compute_carbon_score(const std::string& region_id, double timestamp) const {
    auto data = get_carbon_data(region_id, timestamp);
    
    // Normalize logic: assuming max possible intensity is ~1000 gCO2/kWh
    double max_intensity = 1000.0; 
    double normalized_intensity = std::min(data.carbon_intensity / max_intensity, 1.0);
    
    // Normalize logic: assuming max cost is ~0.5 $/kWh
    double max_cost = 0.5;
    double normalized_cost = std::min(data.electricity_cost / max_cost, 1.0);
    
    double w1 = 0.5;
    double w2 = 0.3;
    double w3 = 0.2;
    
    // Score combines intensity, renewable, cost into a single score [0,1]
    return w1 * normalized_intensity + w2 * (1.0 - data.renewable_pct / 100.0) + w3 * normalized_cost;
}

double CarbonEngine::compute_task_carbon(double cpu_used, double duration_secs,
                                        double carbon_intensity, double pue) const {
    // power_consumption_kw = cpu_used * 0.2
    double power_kw = cpu_used * 0.2;
    double duration_hours = duration_secs / 3600.0;
    return power_kw * duration_hours * carbon_intensity * pue;
}

double CarbonEngine::compute_task_cost(double cpu_used, double duration_secs,
                                       double electricity_cost, double pue) const {
    double power_kw = cpu_used * 0.2;
    double duration_hours = duration_secs / 3600.0;
    return power_kw * duration_hours * electricity_cost * pue;
}

std::vector<std::string> CarbonEngine::get_regions() const {
    std::vector<std::string> regions;
    for (const auto& pair : carbon_data_) {
        regions.push_back(pair.first);
    }
    return regions;
}

} // namespace carbongrid

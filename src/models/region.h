#pragma once

#include <string>
#include <vector>

namespace carbongrid {

/**
 * @brief Represents a geographic deployment region.
 * Used for regional carbon-aware routing and placement.
 */
struct Region {
    std::string region_id;
    std::string region_name; // e.g., "US-East", "EU-West"
    std::vector<std::string> node_ids;
    double latitude;
    double longitude;
};

} // namespace carbongrid

#pragma once

#include <string>
#include "../models/task.h"

namespace carbongrid {

// Forward declarations
class CloudState;
class CarbonEngine;

class ResourceAllocator {
public:
    ResourceAllocator(CloudState& cloud_state, CarbonEngine& carbon_engine);
    
    bool allocate(Task& task, const std::string& node_id, double timestamp, int time_slot);
    bool release(Task& task);

private:
    CloudState& cloud_state_;
    CarbonEngine& carbon_engine_;
};

} // namespace carbongrid

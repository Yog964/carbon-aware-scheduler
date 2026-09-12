#pragma once

#include <string>
#include <vector>
#include "../models/task.h"
#include "../cloud_state/cloud_state.h"

namespace carbongrid {

enum class BaselineStrategy {
    ROUND_ROBIN,
    LEAST_LOADED,
    RANDOM
};

class BaselineScheduler {
public:
    explicit BaselineScheduler(BaselineStrategy strategy = BaselineStrategy::ROUND_ROBIN);
    
    // Select a node for the task (carbon-unaware)
    std::string select_node(const Task& task, CloudState& cloud_state);
    
    // Set strategy
    void set_strategy(BaselineStrategy strategy);
    BaselineStrategy get_strategy() const;
    
    // Reset state
    void reset();
    
private:
    BaselineStrategy strategy_;
    int round_robin_index_ = 0;
    
    std::string round_robin_select(const Task& task, CloudState& cloud_state);
    std::string least_loaded_select(const Task& task, CloudState& cloud_state);
    std::string random_select(const Task& task, CloudState& cloud_state);
};

}

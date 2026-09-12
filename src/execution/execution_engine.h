#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include "../models/task.h"

namespace carbongrid {

class CloudState;
class ResourceAllocator;

class ExecutionEngine {
public:
    ExecutionEngine(CloudState& cloud_state, ResourceAllocator& allocator);

    void submit_task(Task& task);
    bool schedule_task(Task& task, const std::string& node_id, double timestamp, int time_slot);
    void update(double current_time);
    void fail_task(Task& task);

    std::vector<Task*> get_pending_tasks();
    std::vector<Task*> get_running_tasks();

    int pending_count() const;
    int running_count() const;
    int completed_count() const;
    int failed_count() const;

private:
    CloudState& cloud_state_;
    ResourceAllocator& allocator_;

    std::unordered_map<std::string, Task*> all_tasks_;
    
    int pending_count_ = 0;
    int running_count_ = 0;
    int completed_count_ = 0;
    int failed_count_ = 0;
};

} // namespace carbongrid

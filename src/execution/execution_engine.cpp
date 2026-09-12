#include "execution_engine.h"
#include "../allocation/resource_allocator.h"
#include <algorithm>

namespace carbongrid {

ExecutionEngine::ExecutionEngine(CloudState& cloud_state, ResourceAllocator& allocator)
    : cloud_state_(cloud_state), allocator_(allocator) {}

void ExecutionEngine::submit_task(Task& task) {
    task.state = TaskState::QUEUED;
    all_tasks_[task.task_id] = &task;
    pending_count_++;
}

bool ExecutionEngine::schedule_task(Task& task, const std::string& node_id, double timestamp, int time_slot) {
    if (task.state != TaskState::QUEUED) {
        return false;
    }
    
    if (allocator_.allocate(task, node_id, timestamp, time_slot)) {
        task.state = TaskState::RUNNING;
        pending_count_--;
        running_count_++;
        return true;
    }
    return false;
}

void ExecutionEngine::update(double current_time) {
    for (auto& pair : all_tasks_) {
        auto& task = pair.second;
        if (task->state == TaskState::RUNNING) {
            if (current_time >= task->scheduled_time + task->execution_duration) {
                // Task has completed
                allocator_.release(*task);
                task->state = TaskState::COMPLETED;
                task->completion_time = current_time;
                running_count_--;
                completed_count_++;
            }
        }
    }
}

void ExecutionEngine::fail_task(Task& task) {
    if (task.state == TaskState::RUNNING) {
        allocator_.release(task);
        running_count_--;
    } else if (task.state == TaskState::QUEUED) {
        pending_count_--;
    }
    
    task.state = TaskState::FAILED;
    failed_count_++;
}

std::vector<Task*> ExecutionEngine::get_pending_tasks() {
    std::vector<Task*> pending;
    for (auto& pair : all_tasks_) {
        auto& task = pair.second;
        if (task->state == TaskState::QUEUED) {
            pending.push_back(task);
        }
    }
    
    // Sort primarily by priority (1 is highest), secondarily by arrival time
    std::sort(pending.begin(), pending.end(), [](const Task* a, const Task* b) {
        if (a->priority != b->priority) {
            return a->priority < b->priority;
        }
        return a->arrival_time < b->arrival_time;
    });
    
    return pending;
}

std::vector<Task*> ExecutionEngine::get_running_tasks() {
    std::vector<Task*> running;
    for (auto& pair : all_tasks_) {
        auto& task = pair.second;
        if (task->state == TaskState::RUNNING) {
            running.push_back(task);
        }
    }
    return running;
}

int ExecutionEngine::pending_count() const { return pending_count_; }
int ExecutionEngine::running_count() const { return running_count_; }
int ExecutionEngine::completed_count() const { return completed_count_; }
int ExecutionEngine::failed_count() const { return failed_count_; }

} // namespace carbongrid

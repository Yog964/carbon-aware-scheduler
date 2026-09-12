#pragma once

#include <string>
#include "task.h"

namespace carbongrid {

enum class EventType {
    NEW_TASK,
    TASK_COMPLETED,
    TASK_FAILED,
    RESCHEDULING
};

/**
 * @brief Represents a scheduling event within the discrete event simulation engine.
 */
struct SchedulingEvent {
    std::string event_id;
    double timestamp;
    Task* task; // Reference to task
    EventType type;
};

} // namespace carbongrid

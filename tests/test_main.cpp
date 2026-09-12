#include <iostream>
#include <cassert>
#include <cmath>
#include "models/task.h"
#include "models/node.h"
#include "daa_engine/scoring.h"

using namespace carbongrid;

int tests_passed = 0;
int tests_failed = 0;

#define ASSERT_TRUE(condition, message) \
    if (condition) { tests_passed++; } \
    else { tests_failed++; std::cerr << "[FAIL] " << message << std::endl; }

#define ASSERT_EQUAL_FLOAT(a, b, epsilon, message) \
    if (std::abs((a) - (b)) < (epsilon)) { tests_passed++; } \
    else { tests_failed++; std::cerr << "[FAIL] " << message << " (Expected " << (b) << " got " << (a) << ")" << std::endl; }

void test_task_slack() {
    Task t;
    t.execution_duration = 100.0;
    t.deadline = 200.0;
    
    // Slack = 200 - (50 + 100) = 50
    ASSERT_EQUAL_FLOAT(t.slack(50.0), 50.0, 1e-6, "Task slack calculation should be correct");
    
    // Urgent if slack < 30
    ASSERT_TRUE(!t.is_urgent(50.0, 30.0), "Task should not be urgent if slack > threshold");
    
    // Slack = 200 - (80 + 100) = 20
    ASSERT_TRUE(t.is_urgent(80.0, 30.0), "Task should be urgent if slack <= threshold");
}

void test_node_allocation() {
    Node n;
    n.cpu_capacity = 16.0;
    n.ram_capacity = 32.0;
    n.cpu_used = 0.0;
    n.ram_used = 0.0;
    n.is_available = true;
    
    ASSERT_TRUE(n.can_fit(4.0, 8.0), "Empty node should fit small task");
    
    n.allocate(4.0, 8.0, "task1");
    ASSERT_EQUAL_FLOAT(n.cpu_used, 4.0, 1e-6, "CPU used should update");
    ASSERT_EQUAL_FLOAT(n.ram_used, 8.0, 1e-6, "RAM used should update");
    ASSERT_EQUAL_FLOAT(n.cpu_utilization(), 25.0, 1e-6, "CPU util should be 25%");
    
    ASSERT_TRUE(!n.can_fit(16.0, 8.0), "Node should not fit task exceeding capacity");
    
    n.release(4.0, 8.0, "task1");
    ASSERT_EQUAL_FLOAT(n.cpu_used, 0.0, 1e-6, "CPU should be 0 after release");
}

void test_scoring_function() {
    ScoringFunction sf;
    Task t; t.execution_duration = 100; t.deadline = 200; t.priority = 1;
    Node n; n.cpu_capacity = 16.0; n.ram_capacity = 32.0; n.cpu_used = 8.0; n.ram_used = 16.0; // 50% util
    
    double intensity = 425.0; // Mid intensity
    double cost = 0.115;      // Mid cost
    double renewable = 50.0;  // Mid renewable
    double current_time = 0.0;
    
    NodeScore score = sf.compute(t, n, intensity, cost, renewable, current_time);
    
    ASSERT_TRUE(score.total_score >= 0.0 && score.total_score <= 1.0, "Total score should be between 0 and 1");
}

int main() {
    std::cout << "Running CarbonGrid Unit Tests..." << std::endl;
    
    test_task_slack();
    test_node_allocation();
    test_scoring_function();
    
    std::cout << "Tests Passed: " << tests_passed << std::endl;
    std::cout << "Tests Failed: " << tests_failed << std::endl;
    
    return (tests_failed == 0) ? 0 : 1;
}

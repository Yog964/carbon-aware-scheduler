# CarbonGrid: Carbon-Aware Cloud Scheduler Project Documentation

## 1. Project Overview & Objectives

**Core Mission:**
CarbonGrid is a high-performance, carbon-aware cloud resource scheduling and optimization system. Its primary mission is to minimize the carbon footprint and electricity costs of executing cloud workloads without violating strict service-level agreements (SLAs) or performance deadlines. 

**Target Scope:**
The system is designed to simulate and optimize scheduling decisions for Kubernetes-style workloads across multiple geographic cloud regions. By treating resource allocation as a multi-variable optimization problem, CarbonGrid bridges the gap between conventional resource-centric scheduling (which only looks at CPU/RAM) and environmentally sustainable computing.

**Background Context & Problems Addressed:**
*   **Carbon Footprint of Cloud Computing:** Data centers consume vast amounts of electricity. Standard schedulers prioritize speed and resource packing, ignoring the real-time carbon intensity (gCO₂/kWh) of the energy grids powering those data centers.
*   **Variable Energy Costs:** Electricity costs fluctuate significantly based on region and time of day. 
*   **Strict Deadlines vs. Flexibility:** Some workloads (urgent) need immediate execution, while others (batch/flexible) can be deferred or routed to distant, greener regions. Traditional schedulers often fail to exploit this flexibility to reduce emissions.

**Key Objectives:**
*   Develop a multi-region cloud simulation environment to monitor dynamic cloud cluster states.
*   Optimize resource allocation by dynamically considering CPU/RAM availability, real-time carbon intensity, electricity cost, task priority, and execution deadlines.
*   Compare CarbonGrid's Dynamic Allocation Algorithms (DAA) against conventional baseline schedulers to measure environmental and economic impact.

---

## 2. Architecture, Workflow & Methodology

**Technical Stack:**
*   **Backend / Simulation Engine:** C++17/20 (for ultra-low latency scheduling)
*   **Build System:** CMake
*   **Data Processing:** Python 3 (`generate_carbon.py`)
*   **Frontend / Dashboard:** HTML, CSS, Vanilla JS with WebSockets/Polling for real-time visualization.

**System Architecture Layers:**
1.  **Data Sources:** Real workload traces (e.g., Alibaba), synthetic tasks, historical carbon datasets, and node profiles.
2.  **Workload Generation:** Replays traces, controls arrival rates, and manages the simulation clock.
3.  **Cloud State Model:** Maintains the real-time state (CPU, RAM, usage, PUE) of multiple Regions and Nodes.
4.  **Carbon & Cost Engine:** Provides real-time lookup (via Binary Search) of carbon intensity and energy prices.
5.  **DAA Engine (Dynamic Allocation Algorithm):** The core routing logic.
6.  **Resource Allocator & Execution Engine:** Handles the actual placement and lifecycle of the task (Queued → Running → Completed).
7.  **Metrics & Dashboard:** Aggregates data and streams to the Operator Dashboard.

**Algorithmic Methodology (DAA):**
The DAA Engine handles scheduling events by splitting workloads into two distinct paths based on their deadline slack:

*   **Fast Path (Urgent Tasks):** 
    *   **Algorithm:** Greedy Selection + Min-Heap (Priority Queue)
    *   **Workflow:** Computes a composite score (carbon + cost + penalty) for all feasible nodes. Pushes scores to a Min-Heap. The scheduler immediately pops the lowest-scoring node and assigns the task.
    *   **Complexity:** O(N log N)
*   **Batch Path (Flexible Tasks):**
    *   **Algorithm:** Minimum Cost Maximum Flow (MCMF) + Dijkstra + Johnson's Reweighting
    *   **Workflow:** Batches multiple deferrable tasks. Constructs a flow network graph connecting tasks to node-timeslots. Edges are weighted by environmental and economic costs. Dijkstra's algorithm finds the absolute cheapest path through the network to allocate resources.
    *   **Complexity:** O(F * E * log V)

**Workflow Summary:**
`Workload Arrival → Feasibility Filter → DAA Engine (Fast/Batch Path) → Node Selection → Execution → Metrics Engine → Operator Dashboard`

---

## 3. Key Data, Metrics & Analysis

To accurately score and route workloads, CarbonGrid continuously processes multi-dimensional data arrays and tracks exhaustive metrics.

**Input Data Entities:**
*   **Workload Data:** Task/Pod ID, Arrival Time, CPU/RAM Request, Execution Duration, Priority, Deadline.
*   **Carbon Data:** Timestamp, Region ID, Carbon Intensity (gCO₂/kWh), Renewable Energy %, Electricity Cost ($/kWh).
*   **Node Data:** Node ID, Region, Total CPU/RAM Capacity, Current CPU/RAM Usage, Power Usage Effectiveness (PUE).

**Key Tracked Metrics:**
*   **Environmental Metrics:** Total Carbon Emission, Average Carbon Intensity of executed tasks, **Net Carbon Reduction** (vs. baseline).
*   **Economic Metrics:** Total Cloud/Electricity Cost, **Net Cost Reduction** (vs. baseline).
*   **Scheduling Metrics:** Number of Scheduling Events, Queue Length, Average Scheduling Time (Latency), System Throughput.
*   **Performance Metrics:** Average execution latency, Deadline Violations, Task Completion Rate, Overall CPU/RAM Node Utilization.

**Analytical Approach:**
The system runs the exact same workload trace through both a Baseline Scheduler (Resource-only packing) and the CarbonGrid DAA. By comparing the deltas in the Environmental and Economic metrics, the system objectively quantifies the benefit of carbon-aware routing.

---

## 4. Current Progress & Roadmap

**Completed Milestones:**
*   **Core C++ Engine Built:** Simulation framework, clock synchronization, and data structures are fully operational.
*   **Cloud State & Carbon Data Ingestion:** Python data generation scripts and C++ CSV loaders are completed. Binary Search lookups for carbon data are implemented.
*   **DAA Engine Implementation:** Both the Greedy (Fast Path) and MCMF (Batch Path) algorithms have been fully written and integrated.
*   **Metrics & Telemetry Server:** Local HTTP/WebSocket server implemented to serve real-time metrics.
*   **Operator Dashboard:** Web frontend is complete and successfully polling data from the C++ backend.

**Active & Upcoming Deliverables (Next Steps):**
*   **Extensive Benchmarking:** Run large-scale simulations using the full Alibaba cluster dataset to generate high-fidelity comparative charts (CarbonGrid vs. Baseline).
*   **Algorithm Tuning:** Adjust the weighting parameters (alpha, beta, gamma) in the scoring function to find the optimal balance between cost, carbon, and latency.
*   **Prediction Layer Integration:** Move from static historical carbon data to a predictive model that anticipates carbon intensity spikes in the next N hours.
*   **Real-world Kubernetes Integration:** Design a Kubernetes Custom Scheduler plugin that utilizes the CarbonGrid C++ engine via gRPC for actual live-cluster pod placement.

---

## 5. Critical References & Notes

**Important Terminology Guidelines:**
*   Use "Cloud Workload" to describe computational work, and "Task/Job" as the individual unit in the simulation.
*   Do not say "User request goes to Kubernetes Scheduler." Instead, outline the exact flow: `Application → Cloud Workload → Pod Creation → Scheduling Event → CarbonGrid Policy → Node Selection`.

**Reference Materials:**
*   **Workload Traces:** Alibaba Cluster Trace Data (industry standard for representing realistic cloud load).
*   **Carbon Intensity Data:** Formatted using standards similar to *Electricity Maps* or *WattTime* APIs.
*   **Build Instructions:** Ensure CMake (v3.20+) is used. Initialization requires running `python generate_carbon.py` before `cmake --build build`.
*   **Local UI Access:** Dashboard is available locally at `dashboard/index.html` while the `CarbonGrid` executable is running.

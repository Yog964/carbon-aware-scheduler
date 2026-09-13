# Carbon-Aware Cloud Scheduler (CarbonGrid)

A high-performance C++ simulation engine for carbon-aware cloud resource scheduling. It utilizes advanced algorithmic techniques to dynamically schedule cloud workloads across multiple geographic regions based on real-time carbon intensity and electricity costs.

## Features
* **Dual-Algorithm Scheduling Engine**: Uses a **Greedy + Min-Heap** approach for low-latency urgent tasks, and **MCMF (Minimum Cost Maximum Flow) + Dijkstra** for batch scheduling of flexible tasks.
* **Multi-Region Cloud State**: Simulates global cloud nodes with varying Power Usage Effectiveness (PUE), tracking dynamic CPU/RAM utilization.
* **Carbon & Cost Optimization**: Balances strict deadlines with carbon minimization and cost reduction goals using a configurable scoring function.
* **Web Dashboard**: An HTML/JS dashboard that visualizes real-time metrics, carbon reduction, and scheduling throughput over WebSockets/file-polling.
* **Data Preprocessing**: Includes Python tools to parse and ingest real-world cloud workload traces (e.g., Alibaba Cluster Data).

## Requirements

To build and run this project, you need the following installed:
* **C++17 Compatible Compiler** (e.g., GCC, Clang, or MSVC)
* **CMake** (version 3.20 or higher)
* **Python 3.x** (to run data generation scripts like `generate_carbon.py`)
* **Modern Web Browser** (to view the real-time simulation dashboard)

## Architecture

The engine is structured to isolate data generation, scheduling logic, resource allocation, and metrics tracking into independent, highly cohesive components.

```mermaid
flowchart TD
    A["Workload Sources<br>Real Traces & Synthetic"] --> B("Workload Generator")
    B --> C{"CarbonGrid DAA Engine<br>Scheduling Event"}
    
    subgraph Data Inputs
        A
        D["Historical Carbon Data"] --> E("Prediction Layer")
        E --> F["Carbon & Cost Engine<br>Intensity & Cost"]
    end
    
    subgraph Cloud Infrastructure
        G["Cloud State Model<br>Regions & Nodes"]
    end
    
    F --> C
    G --> C
    
    C -->|Urgent Tasks| H["Fast Path<br>Greedy + Min Heap"]
    C -->|Flexible Tasks| I["Batch Path<br>Graph + MCMF + Dijkstra"]
    
    H --> J["Resource Allocation<br>Select Best Node"]
    I --> J
    
    J --> K["Execution & State<br>Queued -> Running -> Completed"]
    K --> G
    
    K --> L["Metrics Engine<br>Carbon, Cost, Latency"]
    L --> M["Operator Dashboard<br>Live Monitoring"]
```

## Algorithm Flowchart (DAA Flow)

The Dynamic Allocation Algorithm (DAA) handles task scheduling based on urgency and feasibility:

```mermaid
flowchart TD
    A(["Task Arrives"]) --> B["Read Task Parameters:<br>CPU, RAM, Deadline"]
    B --> C["Filter Nodes"]
    C --> D{"Feasible Nodes?"}
    D -- NO --> E["Wait Queue"]
    D -- YES --> F["Calculate Slack"]
    
    F --> G{"Urgent Task?"}
    
    G -- YES --> H["FAST PATH<br>Greedy + Min Heap"]
    
    G -- NO --> I["BATCH PATH<br>Build Graph"]
    I --> J["Min-Cost Max-Flow<br>(MCMF)"]
    J --> K["Dijkstra Shortest Path"]
    
    H --> L["Select Resource<br>Region + Node + Slot"]
    K --> L
    
    L --> M["Update State"]
    M --> N["Execute Task"]
    N --> O(["Calculate Metrics"])
```

## Build Instructions

```bash
# 1. Generate the required initial carbon data
python generate_carbon.py

# 2. Build the C++ project
cmake -B build -S .
cmake --build build
```

## Running the Simulation

Run the generated executable:
```bash
./build/CarbonGrid
```
*(On Windows, you may need to run `.\build\Debug\CarbonGrid.exe` or `.\build\CarbonGrid.exe` depending on your build system)*

While the simulation is running, open `dashboard/index.html` in any modern web browser to view the live telemetry and carbon reduction metrics.

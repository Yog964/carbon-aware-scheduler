# Carbon-Aware Cloud Scheduler (CarbonGrid)

A high-performance C++ simulation engine for carbon-aware cloud resource scheduling. It utilizes advanced algorithmic techniques to dynamically schedule cloud workloads across multiple geographic regions based on real-time carbon intensity and electricity costs.

## Features
* **Dual-Algorithm Scheduling Engine**: Uses a **Greedy + Min-Heap** approach for low-latency urgent tasks, and **MCMF (Minimum Cost Maximum Flow) + Dijkstra** for batch scheduling of flexible tasks.
* **Multi-Region Cloud State**: Simulates global cloud nodes with varying Power Usage Effectiveness (PUE), tracking dynamic CPU/RAM utilization.
* **Carbon & Cost Optimization**: Balances strict deadlines with carbon minimization and cost reduction goals using a configurable scoring function.
* **Web Dashboard**: An HTML/JS dashboard that visualizes real-time metrics, carbon reduction, and scheduling throughput over WebSockets/file-polling.
* **Data Preprocessing**: Includes Python tools to parse and ingest real-world cloud workload traces (e.g., Alibaba Cluster Data).

## Architecture
The engine is structured across 10 layers, isolating data generation, scheduling logic, resource allocation, and metrics tracking into independent, highly cohesive components.

## Build Instructions
Requires CMake and a C++17 compatible compiler (e.g., GCC/MinGW).

```bash
cmake -B build -S .
cmake --build build
```

## Running the Simulation
Run the generated executable:
```bash
./build/CarbonGrid
```

While the simulation is running, open `dashboard/index.html` in any modern web browser to view the live telemetry and carbon reduction metrics.

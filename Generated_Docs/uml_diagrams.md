# CarbonGrid: Master UML & Architecture Diagrams

Here is the complete suite of software engineering diagrams you requested. These diagrams model the system from every possible angle (Objects, Time, States, and Architecture).

---

## 1. Class Diagram
Shows the primary C++ classes, their properties, methods, and how they relate to one another (Composition and Dependency).

```mermaid
classDiagram
    class CloudState {
        +load_node_profiles()
        +get_all_nodes()
        +avg_cpu_utilization()
    }
    class CarbonEngine {
        +load_carbon_data()
        +get_carbon_intensity(region, time)
    }
    class DAAEngine {
        -CloudState cloud_state
        -CarbonEngine carbon_engine
        +schedule(Task, time)
        +schedule_batch(Tasks, time)
    }
    class WorkloadGenerator {
        +generate_task(time)
    }
    class ExecutionEngine {
        +submit_task(Task)
        +schedule_task(Task, Node)
        +update(time)
    }
    class Node {
        +string node_id
        +double cpu_capacity
        +double cpu_used
        +can_fit(cpu, ram)
    }
    class Task {
        +string task_id
        +double cpu_required
        +double deadline
        +TaskState state
    }
    
    DAAEngine "1" *-- "1" CloudState : Uses
    DAAEngine "1" *-- "1" CarbonEngine : Uses
    CloudState "1" *-- "many" Node : Contains
    ExecutionEngine "1" *-- "many" Task : Manages
```

---

## 2. Sequence Diagram
Shows the chronological order of operations across the system over time when a single task is generated and scheduled.

```mermaid
sequenceDiagram
    participant Main
    participant Workload as WorkloadGenerator
    participant DAA as DAAEngine
    participant Exec as ExecutionEngine
    participant Metrics as MetricsEngine
    
    Main->>Workload: generate_task(current_time)
    Workload-->>Main: Return new Task Object
    
    Main->>DAA: schedule(Task, current_time)
    activate DAA
    DAA->>DAA: Filter Feasible Nodes
    DAA->>DAA: Run Greedy / MCMF Selection
    DAA-->>Main: Return SchedulingDecision
    deactivate DAA
    
    alt Decision Success
        Main->>Exec: submit_task(Task)
        Main->>Exec: schedule_task(Task, Node, TimeSlot)
        Main->>Metrics: record_task_scheduled()
    else Decision Failed
        Main->>Metrics: record_task_failed()
    end
```

---

## 3. State Machine Diagram
Tracks the lifecycle of a single `Task` object from the moment it is born to the moment it dies.

```mermaid
stateDiagram-v2
    [*] --> QUEUED : WorkloadGenerator creates Task
    
    QUEUED --> RUNNING : ExecutionEngine allocates Node CPU
    QUEUED --> FAILED : DAAEngine finds no feasible node
    
    RUNNING --> COMPLETED : current_time >= start + duration
    RUNNING --> FAILED : Node crashes / Premature halt
    
    COMPLETED --> [*] : Memory Freed
    FAILED --> [*] : Memory Freed
```

---

## 4. Activity Diagram
Visualizes the control flow of the massive infinite `while` loop running inside `main.cpp`.

```mermaid
flowchart TD
    Start([Start Simulation]) --> Init[Load Node & Carbon CSV Profiles]
    Init --> Check{Tasks Generated<br>< Total Limit?}
    
    Check -- Yes --> Time[Advance Simulation Clock]
    Time --> Update[ExecutionEngine checks for completed tasks]
    Update --> CheckGen{Is it time to<br>generate a new task?}
    
    CheckGen -- Yes --> Gen[Generate Task]
    Gen --> Sched[DAA Engine Schedules Task]
    Sched --> Exec[Allocate Resources]
    Exec --> Metric{Is it time for<br>Metrics Snapshot?}
    
    CheckGen -- No --> Metric
    
    Metric -- Yes --> Snap[Write UI data to metrics.json]
    Snap --> Check
    
    Metric -- No --> Check
    
    Check -- No --> End([End Simulation])
```

---

## 5. Component Diagram
Shows the logical grouping of internal software components and how they communicate.

```mermaid
flowchart LR
    subgraph Data Layer
        Profiles[(node_profiles.csv)]
        Carbon[(carbon_intensity.csv)]
    end
    
    subgraph Core C++ Engines
        CE[Carbon Engine Component]
        CS[Cloud State Component]
        DAA[DAA Scheduler Component]
        Exec[Execution Component]
    end
    
    Profiles --> CS
    Carbon --> CE
    
    CS --> DAA
    CE --> DAA
    DAA --> Exec
    
    subgraph Output Layer
        JSON[(metrics.json)]
    end
    
    Exec --> JSON
```

---

## 6. System Architecture Diagram
The high-level macro view showing how the Frontend Browser, Python Middleware, and C++ Backend interact across different processes.

```mermaid
flowchart TD
    User((User)) -->|Inputs Config & Clicks Start| Browser
    
    subgraph Web Frontend
        Browser[Dashboard HTML / JS]
    end
    
    subgraph Middleware Server
        PyServer[Python API Server : Port 8080]
    end
    
    Browser <-->|HTTP POST / GET| PyServer
    
    subgraph Compiled Backend
        Cpp[CarbonGrid.exe C++ Process]
        MetricFile[(dashboard/data/metrics.json)]
    end
    
    PyServer -->|subprocess.Popen| Cpp
    Cpp -->|Overwrites File| MetricFile
    MetricFile -->|Reads File| PyServer
```

---

## 7. Node Lifecycle Diagram
Shows the physical lifecycle states of a simulated Cloud Server Node as tasks arrive and leave.

```mermaid
flowchart LR
    Load[(CSV Parsed)] --> Init[Node Created in RAM]
    
    Init --> Idle[Idle State<br>0% CPU]
    
    Idle --> Active[Task Assigned<br>CPU > 0%]
    Active --> Maxed[CPU Reaches 100%<br>Removed from Feasibility Pool]
    
    Maxed --> Active[Task Completes<br>CPU Usage Drops]
    Active --> Idle[All Tasks Complete]
    
    Idle --> Del([Simulation Ends<br>Node Destroyed])
```

---

## 8. Entity-Relationship Diagram (ERD)
This diagram maps out the strict data relationships between the core structures in the system. It shows how regions contain nodes, nodes execute tasks, and tasks generate metrics.

```mermaid
erDiagram
    REGION ||--o{ NODE : "contains"
    REGION {
        string region_id
        float current_carbon_intensity
        float electricity_cost
    }
    NODE ||--o{ TASK : "executes"
    NODE {
        string node_id
        float cpu_capacity
        float ram_capacity
        float pue
    }
    TASK ||--|| METRICS : "generates"
    TASK {
        string task_id
        float cpu_required
        float deadline
        int priority
    }
```

---

## 9. Advanced Lifecycle: The "Flexible" Task
In our earlier state diagram, tasks went straight from QUEUED to RUNNING. However, if a task is "Flexible" (not urgent), it goes through a completely different deferred lifecycle state.

```mermaid
stateDiagram-v2
    [*] --> GENERATED
    
    GENERATED --> FEASIBILITY_CHECK : Sent to DAA
    FEASIBILITY_CHECK --> FAILED : No space anywhere
    FEASIBILITY_CHECK --> URGENCY_CHECK : Space found
    
    URGENCY_CHECK --> GREEDY_SCHEDULER : Urgent Deadline
    URGENCY_CHECK --> BATCH_QUEUE : Flexible Deadline
    
    BATCH_QUEUE --> BATCH_QUEUE : Waiting for batch to fill up
    BATCH_QUEUE --> MCMF_SCHEDULER : Batch Window is Full
    
    GREEDY_SCHEDULER --> SCHEDULED_IN_FUTURE : Assigned to Node
    MCMF_SCHEDULER --> SCHEDULED_IN_FUTURE : Assigned to Node & TimeSlot
    
    SCHEDULED_IN_FUTURE --> RUNNING : Simulation clock reaches scheduled time
    RUNNING --> COMPLETED : Task duration finishes
    
    COMPLETED --> [*]
```

---

## 10. Dashboard Server Request Lifecycle
This maps the exact lifecycle of what happens when you press buttons on the web interface, showing how the Python server acts as a middleman protecting the C++ process.

```mermaid
sequenceDiagram
    participant Browser
    participant PythonServer as Python server.py
    participant CppEngine as C++ CarbonGrid.exe
    participant FS as File System
    
    %% Start Process
    Browser->>PythonServer: POST /api/start (rate, delay)
    PythonServer->>CppEngine: subprocess.Popen() [Spawns Process]
    PythonServer-->>Browser: HTTP 200 OK
    
    %% Polling Loop
    loop Every 100ms
        Browser->>PythonServer: GET /api/data
        PythonServer->>FS: Read metrics.json
        
        alt C++ is currently overwriting file
            FS-->>PythonServer: File locked / empty
            PythonServer-->>Browser: HTTP 200 (Empty Object)
        else Read Successful
            FS-->>PythonServer: JSON String
            PythonServer-->>Browser: JSON Payload
            Browser->>Browser: Animate UI / Progress Bars
        end
    end
    
    %% Stop Process
    Browser->>PythonServer: POST /api/stop
    PythonServer->>CppEngine: Process.terminate() [Kills Process]
    PythonServer-->>Browser: HTTP 200 OK
```

---

# Event and Metrics Pipeline

This diagram illustrates how scheduling and execution events trigger telemetry recordings, which are then aggregated into the dashboard snapshot.

```mermaid
flowchart LR
    subgraph Execution Engine
        Event1[Task State Changes<br>QUEUED -> RUNNING]
        Event2[Task State Changes<br>RUNNING -> COMPLETED]
    end
    
    subgraph DAA Engine
        Event3[Scheduling Decision Made]
    end
    
    subgraph Metrics Engine
        Rec1([record_task_scheduled])
        Rec2([record_carbongrid_carbon])
        Rec3([record_scheduling_time])
        
        Event1 --> Rec1
        Event3 --> Rec2
        Event3 --> Rec3
        
        Snap([take_snapshot])
        Mem[(In-Memory Metrics State)]
        
        Rec1 --> Mem
        Rec2 --> Mem
        Rec3 --> Mem
        
        Snap --> Mem
    end
    
    Snap --> Export[export_to_json / csv]
    Export --> File[(metrics.json)]
```


---

# Algorithm Selection and Comparison

This diagram and table break down the dual-path routing algorithm, showing how the engine chooses between speed and global optimality based on task urgency.

```mermaid
flowchart TD
    Start{Incoming Task Urgency}
    
    subgraph "Fast Path (Greedy)"
        Urgent[Urgent Task / Tight Deadline] --> Greedy[Greedy + Min-Heap]
        Greedy --> O1[O(1) Selection]
        O1 --> HighSpd[High Speed, Local Optimum]
    end
    
    subgraph "Batch Path (MCMF)"
        Flexible[Flexible Task / Loose Deadline] --> Batch[Batch Window]
        Batch --> Graph[Build Flow Network]
        Graph --> Dijkstra[MCMF + Dijkstra]
        Dijkstra --> GlobalOpt[Slower Speed, Global Optimum]
    end
    
    Start --> Urgent
    Start --> Flexible
    
    HighSpd --> Exec[Execution Engine]
    GlobalOpt --> Exec
```

## Algorithm Comparison Table

| Feature | Fast Path (Greedy Min-Heap) | Batch Path (MCMF + Dijkstra) |
| :--- | :--- | :--- |
| **Use Case** | Urgent, low-latency tasks | Flexible, batch processing tasks |
| **Complexity** | $O(N \log N)$ | Polynomial (Flow Network) |
| **Optimality** | Local (Best available right now) | Global (Perfect mapping across future time) |
| **Data Structure** | `std::priority_queue` | `std::vector<std::vector<Edge>>` (Adjacency List) |
| **Carbon Impact** | Good | Excellent (Can defer execution to greener times) |


---

# Simulation Timeline

This sequence diagram illustrates the chronological ticks of the internal C++ simulation clock, demonstrating how time progresses and events fire.

```mermaid
sequenceDiagram
    participant Clock as Simulation Clock
    participant WG as Workload Generator
    participant Exec as Execution Engine
    participant ME as Metrics Engine
    
    Note over Clock: Simulation Starts
    Clock->>Clock: Time T = 0.0s
    Clock->>WG: Generate Tasks up to T=0.0
    WG-->>Exec: Tasks Queued
    
    Note over Clock: Clock Ticks Forward
    Clock->>Clock: Time T = 0.1s
    Clock->>Exec: update(T=0.1)
    
    Note over Clock: Periodic Metrics Flush
    Clock->>Clock: Time T = 5.0s
    Clock->>ME: take_snapshot(T=5.0)
    ME-->>FileSystem: write metrics.json
    
    Note over Clock: Task Completion Event
    Clock->>Clock: Time T = 10.0s
    Clock->>Exec: update(T=10.0)
    Exec->>Exec: Task A duration reached
    Exec->>Exec: Release Node CPU/RAM
    
    Note over Clock: Simulation Ends
```


---

# C4 Architecture Diagrams

The C4 model provides different levels of zoom for software architecture. Below are the **System Context** (Level 1) and **Container** (Level 2) diagrams.

## 1. System Context Diagram (Level 1)
Shows the big picture: how the user and external data systems interact with CarbonGrid.

```mermaid
flowchart TD
    User([Cloud Operator / User])
    
    subgraph CarbonGrid [CarbonGrid Cloud Scheduler]
        direction TB
        System[Core Simulation System]
    end
    
    Ext1[(Historical Workload Traces)]
    Ext2[(Global Carbon Intensity Datasets)]
    
    User -->|Monitors & Configures| System
    Ext1 -->|Provides Task Data to| System
    Ext2 -->|Provides Grid Data to| System
```

## 2. Container Diagram (Level 2)
Zooms inside the system to show the major containers (Frontend, API, Core Engine, Database/Files).

```mermaid
flowchart TD
    User([Cloud Operator])
    
    subgraph CarbonGrid System
        UI[Web Dashboard<br>HTML / JavaScript / CSS]
        API[Web API Server<br>Python 3]
        Core[Core Simulation Engine<br>C++17]
        MetricsDB[(Metrics File Store<br>JSON / CSV)]
    end
    
    User -->|Views and Interacts| UI
    UI -->|Polls for Data (HTTP GET)| API
    API -->|Spawns & Monitors Process| Core
    Core -->|Writes Telemetry| MetricsDB
    API -->|Reads Telemetry| MetricsDB
```


---

# Carbon-Aware Scheduling Decision Flowchart

This flowchart outlines the exact mathematical and logical steps taken by the scoring function to rank nodes purely based on carbon and monetary cost.

```mermaid
flowchart TD
    Start([Task Requires Scheduling])
    
    GetParams[Read Task CPU, RAM, and Deadline] --> CheckNodes{Check Physical Capacities}
    CheckNodes -- Filtered --> Feasible[Generate List of Feasible Nodes]
    
    Feasible --> ReadCarbon[Fetch Real-time Carbon Intensity for each Node's Region]
    ReadCarbon --> ReadCost[Fetch Real-time Electricity Cost for each Node's Region]
    
    ReadCost --> ScoreCalc[Calculate Unified Score:<br>Alpha * Carbon + Beta * Cost]
    
    ScoreCalc --> Rank[Rank Nodes in Min-Heap Data Structure]
    Rank --> Pick[Pop Node with the Absolute Lowest Score]
    
    Pick --> Output([Return Winning Node ID])
```


---


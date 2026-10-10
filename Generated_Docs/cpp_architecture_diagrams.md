# CarbonGrid C++ Architecture Diagrams

Below are the Mermaid diagrams detailing how the entire C++ codebase connects together, followed by isolated diagrams explaining the internal logic of each specific file.

---

## 1. Global Architecture (How the Files Connect)
This diagram shows how `main.cpp` acts as the conductor, orchestrating the flow of data between the various `.cpp` engines.

```mermaid
flowchart TD
    %% Main orchestrator
    Main[<b>main.cpp</b><br>Simulation Loop] 

    %% Core Engines
    WG[<b>workload_generator.cpp</b><br>Generates Tasks]
    CS[<b>cloud_state.cpp</b><br>Tracks Node CPU/RAM]
    CE[<b>carbon_engine.cpp</b><br>Carbon/Cost Data]
    DAA[<b>daa_engine.cpp</b><br>Decision Maker]
    EE[<b>execution_engine.cpp</b><br>Runs Tasks]
    ME[<b>metrics_engine.cpp</b><br>Telemetry]
    Srv[<b>server.cpp</b><br>File I/O]

    %% Dependencies and Flow
    Main --> |1. Polls| WG
    Main --> |2. Sends Tasks| DAA
    
    WG -.-> |Produces| Tasks[[models/task.h]]
    CS -.-> |Contains| Nodes[[models/node.h]]

    CE --> |Grid Data| DAA
    CS --> |Node Data| DAA

    %% Inside DAA
    subgraph DAA Sub-System
        DAA --> |Filters| Filter[feasibility_filter.cpp]
        DAA --> |Urgent Tasks| Greedy[greedy_scheduler.cpp]
        DAA --> |Flexible Tasks| MCMF[mcmf_scheduler.cpp]
        Greedy --> Scoring[scoring.cpp]
        MCMF --> Scoring
    end
    
    DAA --> |3. Scheduling Decisions| EE
    
    %% Inside Execution
    subgraph Execution Sub-System
        EE --> |Locks CPU/RAM| Allocator[resource_allocator.cpp]
        Allocator --> |Updates| Nodes
    end
    
    %% Telemetry
    DAA --> |Event Logs| ME
    EE --> |State Changes| ME
    Main --> |4. Triggers Snapshot| ME
    ME --> |JSON Data| Srv
```

---

## 2. File-Level Deep Dives

### A. `workload_generator.cpp`
This file is responsible for creating realistic tasks using statistical mathematics.

```mermaid
flowchart LR
    Start([generate_task]) --> GenCPU[Randomly Sample CPU/RAM<br>from Normal Distribution]
    GenCPU --> GenDur[Sample Execution Duration]
    GenDur --> Urgent{Random Urgency Check}
    
    Urgent -- "Urgent" --> Tight[Assign Tight Deadline<br>Minimal Slack]
    Urgent -- "Flexible" --> Loose[Assign Loose Deadline<br>Large Slack]
    
    Tight --> Build[Construct Task Object]
    Loose --> Build
    Build --> Ret([Return Task to Main])
```

### B. `daa_engine.cpp`
This is the traffic cop that receives tasks and routes them to the correct algorithm.

```mermaid
flowchart TD
    Start([schedule task]) --> Filter[Call feasibility_filter.cpp]
    Filter --> IsFeasible{Are there nodes<br>with enough space?}
    
    IsFeasible -- No --> Fail([Return FAILED Decision])
    IsFeasible -- Yes --> Urgency{Is Task Urgent?}
    
    Urgency -- Yes --> Greedy[Send to greedy_scheduler.cpp]
    Greedy --> Succ1([Return Greedy Decision])
    
    Urgency -- No --> Batch[Hold Task in memory<br>(flexible_batch)]
    Batch --> Full{Is Batch Full?}
    
    Full -- Yes --> MCMF[Send entire batch to<br>mcmf_scheduler.cpp]
    MCMF --> Succ2([Return Batch Decisions])
    
    Full -- No --> Defer([Return DEFERRED Decision<br>Wait for more tasks])
```

### C. `execution_engine.cpp`
This file manages the lifecycle of a task once it has been assigned a server.

```mermaid
flowchart TD
    subgraph Assigning Task [schedule_task]
        Sch([Receive Node ID]) --> Alloc[resource_allocator.cpp<br>Subtract CPU/RAM from Node]
        Alloc --> Run[Mark Task RUNNING<br>Increment active count]
    end
    
    subgraph Simulation Ticking [update]
        Upd([Simulation Clock Ticks]) --> Loop[For each Running Task]
        Loop --> CheckTime{Current Time >=<br>Start Time + Duration?}
        
        CheckTime -- Yes --> Rel[resource_allocator.cpp<br>Restore CPU/RAM to Node]
        Rel --> Done[Mark Task COMPLETED]
        
        CheckTime -- No --> Next[Keep Running]
    end
```

### D. `metrics_engine.cpp` & `server.cpp`
These files work together to generate the JSON that powers the dashboard.

```mermaid
flowchart TD
    Start([Take Snapshot]) --> ME[Gather Data<br>CPU Usage, Running Tasks,<br>Carbon Prevented]
    ME --> JSON[Construct JSON String]
    JSON --> Srv[Send to server.cpp]
    Srv --> File[(Write to metrics.json<br>Overwrite existing)]
    File -.-> |Python Reads| Dashboard([Frontend UI Updates])
```

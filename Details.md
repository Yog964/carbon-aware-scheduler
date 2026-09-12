# Final Conclusion

**1. Final Terminology**

Use these terms consistently throughout your PPT, report, diagrams, and viva:

|**Term**|**Meaning in CarbonGrid**|
|---|---|
|**Cloud Workload**|Computational work submitted by applications|
|**Task / Job**|Individual unit of computational work in our simulation|
|**Pod**|Kubernetes execution unit|
|**Scheduling Event**|Event where a newly created/pending Pod needs a Node|
|**Kubernetes Scheduler**|Normal Kubernetes component responsible for Pod → Node placement|
|**CarbonGrid**|Our carbon-aware scheduling/optimization policy|
|**Node**|Cloud compute machine/resource|
|**Region**|Geographic/cloud location containing resources|
|**Resource Request**|CPU/RAM required by a Pod|
|**Pending Queue**|Pods waiting for scheduling|
|**Running**|Pods currently executing|
|**Carbon Intensity**|CO₂ emitted per unit of electricity, e.g. gCO₂/kWh|
|**Workload Generator**|Generates controlled workloads for simulation/testing|
|**Operator**|Cloud/DevOps person monitoring the infrastructure|
|**DAA Engine**|Core algorithmic optimization component of CarbonGrid|

**Don't say:**

"User request goes to Kubernetes Scheduler."

**Say:**

Application

    ↓

Cloud Workload

    ↓

Pod Creation

    ↓

Scheduling Event

    ↓

CarbonGrid / Kubernetes Scheduling Policy

    ↓

Node Selection

---

**2. FINAL COMPLETE ARCHITECTURE**

This is the main architecture I recommend putting in your report.

                           CARBONGRID

          CARBON-AWARE CLOUD RESOURCE SCHEDULING

══════════════════════════════════════════════════════════════════

 ┌──────────────────────────────────────────────────────────────┐

 │                     1. WORKLOAD SOURCES                      │

 │                                                              │

 │  Real Workload Trace       Synthetic Workload                │

 │  (Alibaba Trace)           (Controlled Load)                 │

 │          │                         │                          │

 │          └─────────────┬───────────┘                          │

 └────────────────────────┼─────────────────────────────────────┘

                          ▼

 ┌──────────────────────────────────────────────────────────────┐

 │                  2. WORKLOAD GENERATOR                       │

 │                                                              │

 │  • Task arrival rate                                         │

 │  • CPU requirement                                           │

 │  • RAM requirement                                           │

 │  • Execution time                                            │

 │  • Priority                                                  │

 │  • Deadline                                                  │

 │                                                              │

 │  Example: 10 → 100 → 500 tasks/sec                           │

 └────────────────────────┬─────────────────────────────────────┘

                          ▼

 ┌──────────────────────────────────────────────────────────────┐

 │                    3. CLOUD STATE MODEL                      │

 │                                                              │

 │      Region A              Region B              Region C     │

 │   ┌────────────┐        ┌────────────┐        ┌────────────┐ │

 │   │ Node A1    │        │ Node B1    │        │ Node C1    │ │

 │   │ CPU / RAM  │        │ CPU / RAM  │        │ CPU / RAM  │ │

 │   └────────────┘        └────────────┘        └────────────┘ │

 │                                                              │

 │  • Capacity   • Current Load   • Availability                │

 └────────────────────────┬─────────────────────────────────────┘

                          │

                          ▼

 ┌──────────────────────────────────────────────────────────────┐

 │                    4. CARBON / COST ENGINE                   │

 │                                                              │

 │  Carbon Intensity     Renewable Energy      Electricity Cost │

 │       │                     │                       │         │

 │       └─────────────────────┼───────────────────────┘         │

 │                             ▼                                │

 │                     Resource Carbon Score                    │

 └────────────────────────┬─────────────────────────────────────┘

                          │

                          ▼

 ┌──────────────────────────────────────────────────────────────┐

 │                    5. PREDICTION LAYER                       │

 │                                                              │

 │              Historical Carbon Data                          │

 │                       │                                      │

 │                       ▼                                      │

 │                  Python ML Model                              │

 │                       │                                      │

 │                       ▼                                      │

 │              Future Carbon Prediction                        │

 │                                                              │

 │          (Optional / Secondary Component)                     │

 └────────────────────────┬─────────────────────────────────────┘

                          │

                          ▼

 ┌──────────────────────────────────────────────────────────────┐

 │                    6. CARBONGRID DAA ENGINE                  │

 │                                                              │

 │                  Scheduling Event                            │

 │                         │                                    │

 │                         ▼                                    │

 │                 Feasibility Check                            │

 │                         │                                    │

 │                  ┌──────┴──────┐                             │

 │                  │             │                             │

 │              Urgent         Flexible                         │

 │                  │             │                             │

 │                  ▼             ▼                             │

 │          FAST PATH       BATCH PATH                          │

 │                  │             │                             │

 │          Greedy + Heap   Graph + MCMF                        │

 │                  │        + Dijkstra                         │

 │                  └──────┬──────┘                             │

 │                         ▼                                    │

 │                  Best Resource                               │

 │                  Selection                                   │

 └────────────────────────┬─────────────────────────────────────┘

                          ▼

 ┌──────────────────────────────────────────────────────────────┐

 │                    7. RESOURCE ALLOCATION                    │

 │                                                              │

 │             Pod → Region → Node → Time Slot                  │

 │                                                              │

 │                    Capacity Check                            │

 └────────────────────────┬─────────────────────────────────────┘

                          ▼

 ┌──────────────────────────────────────────────────────────────┐

 │                    8. EXECUTION / STATE                      │

 │                                                              │

 │   QUEUED → SCHEDULED → RUNNING → COMPLETED                  │

 │                         │                                    │

 │                         └──→ FAILED                          │

 │                                                              │

 │   Update CPU / RAM / Queue / Node State                      │

 └────────────────────────┬─────────────────────────────────────┘

                          ▼

 ┌──────────────────────────────────────────────────────────────┐

 │                     9. METRICS ENGINE                        │

 │                                                              │

 │  Carbon Emission    Cost       Latency                       │

 │  CPU Utilization    Throughput Deadline Miss                 │

 │  Queue Length       Scheduling Time                          │

 │                                                              │

 │              Baseline vs CarbonGrid                           │

 └────────────────────────┬─────────────────────────────────────┘

                          ▼

 ┌──────────────────────────────────────────────────────────────┐

 │                   10. OPERATOR DASHBOARD                     │

 │                                                              │

 │  • Cluster Status                                            │

 │  • Pending / Running / Completed Pods                        │

 │  • Scheduling Events                                         │

 │  • Node Load                                                 │

 │  • Carbon Intensity                                          │

 │  • Queue                                                     │

 │  • Algorithm Decisions                                       │

 │  • Carbon / Cost Savings                                     │

 │  • Live Workload Rate                                        │

 └────────────────────────┬─────────────────────────────────────┘

                          ▼

                   CLOUD / DEVOPS OPERATOR

---

**3. LAYER-WISE ARCHITECTURE**

For your PPT, you can show the system as **10 layers**.

┌──────────────────────────────────────────────┐

│ LAYER 10: OPERATOR DASHBOARD                │

│ Live Monitoring / Metrics / Decisions        │

├──────────────────────────────────────────────┤

│ LAYER 9: METRICS ENGINE                     │

│ Carbon / Cost / Latency / Utilization       │

├──────────────────────────────────────────────┤

│ LAYER 8: EXECUTION & STATE                  │

│ Queue / Running / Completed / Failed        │

├──────────────────────────────────────────────┤

│ LAYER 7: RESOURCE ALLOCATION                │

│ Pod → Region → Node → Time Slot             │

├──────────────────────────────────────────────┤

│ LAYER 6: DAA SCHEDULING ENGINE              │

│ Greedy + Min Heap / MCMF + Dijkstra         │

├──────────────────────────────────────────────┤

│ LAYER 5: CARBON PREDICTION                  │

│ Python ML → Future Carbon                   │

├──────────────────────────────────────────────┤

│ LAYER 4: CARBON & COST ENGINE               │

│ Carbon / Renewable / Electricity Cost       │

├──────────────────────────────────────────────┤

│ LAYER 3: CLOUD STATE MODEL                  │

│ Nodes / CPU / RAM / Capacity / Load         │

├──────────────────────────────────────────────┤

│ LAYER 2: WORKLOAD GENERATOR                 │

│ Real Trace / Synthetic / Load Control       │

├──────────────────────────────────────────────┤

│ LAYER 1: DATA SOURCES                       │

│ Workload / Carbon / Node Profiles           │

└──────────────────────────────────────────────┘

---

**4. WHERE KUBERNETES FITS**

This is important for your viva.

                 KUBERNETES

 Application

      │

      ▼

 Pod Created

      │

      ▼

 API Server

      │

      ▼

 Pending Pod

      │

      ▼

 kube-scheduler

      │

      ├───────────────┐

      │               │

      ▼               ▼

  FILTERING        SCORING

      │               │

      │               │

      └───────┬───────┘

              ▼

         Node Selected

              │

              ▼

          Pod Binding

              │

              ▼

          Kubelet

              │

              ▼

          Pod Running

Your project concept:

             KUBERNETES SCHEDULING

                 Pending Pod

                      │

                      ▼

               Feasible Nodes

                      │

                      ▼

                 CARBONGRID

                      │

        ┌─────────────┼──────────────┐

        │             │              │

       Load         Carbon          Cost

        │             │              │

        └─────────────┼──────────────┘

                      ▼

               DAA Optimization

                      │

                      ▼

                Best Node

**Important:** In the final implementation, be precise about whether CarbonGrid is _replacing Kubernetes' scoring policy_, _simulating a scheduling policy_, or _integrating as a scheduler/plugin_. Don't claim direct Kubernetes integration unless you actually implement it.

---

**5. USE CASE DIAGRAM**

**Actors**

                     ┌──────────────────────┐

                     │ Cloud / DevOps       │

                     │ Operator             │

                     └──────────┬───────────┘

                                │

                                │

                     ┌──────────▼───────────┐

                     │     CARBONGRID       │

                     └──────────────────────┘

Full use-case diagram:

                 ┌─────────────────────────────────────────┐

                 │              CARBONGRID                 │

                 │                                         │

                 │   (Configure Simulation)                │

                 │             │                           │

                 │             ▼                           │

                 │   (Generate Workload)                   │

                 │             │                           │

                 │             ▼                           │

                 │   (Monitor Scheduling)                  │

                 │             │                           │

                 │             ▼                           │

                 │   (View Pending Queue)                  │

                 │                                         │

                 │   (View Node Utilization)               │

                 │                                         │

                 │   (View Carbon Intensity)                │

                 │                                         │

                 │   (View Scheduling Decisions)            │

                 │                                         │

                 │   (Compare Algorithms)                   │

                 │                                         │

                 │   (View Carbon Savings)                  │

                 │                                         │

                 │   (View Cost Optimization)               │

                 │                                         │

                 │   (Control Workload Rate)                │

                 │                                         │

                 │   (Start / Stop Simulation)              │

                 └───────────────────┬─────────────────────┘

                                     │

                                     │

                         ┌───────────▼──────────┐

                         │ Cloud / DevOps       │

                         │ Operator             │

                         └──────────────────────┘

   DATA / SYSTEM ACTORS

   ─────────────────────────────────────────────────────

 ┌─────────────────┐

 │ Workload Trace  │──────────► Generate Workload

 └─────────────────┘

 ┌─────────────────┐

 │ Carbon Dataset  │──────────► Carbon / Cost Engine

 └─────────────────┘

 ┌─────────────────┐

 │ Python Predictor│──────────► Future Carbon

 └─────────────────┘

 ┌─────────────────┐

 │ Kubernetes      │──────────► Scheduling Events /

 │ Cluster State   │            Node / Pod State

 └─────────────────┘

---

**6. MAIN SEQUENCE DIAGRAM**

This is the sequence when a Pod needs scheduling.

Operator     Workload       Kubernetes      CarbonGrid      Carbon Engine

   │            │               │               │                │

   │            │               │               │                │

   │            │  Workload     │               │                │

   │            │  generated    │               │                │

   │            │──────────────►│               │                │

   │            │               │               │                │

   │            │               │ Pod Created   │                │

   │            │               │──────────────►│                │

   │            │               │ Scheduling    │                │

   │            │               │ Event         │                │

   │            │               │──────────────►│                │

   │            │               │               │                │

   │            │               │               │ Get Node State │

   │            │               │               │───────────────►│

   │            │               │               │                │

   │            │               │               │ Carbon + Cost  │

   │            │               │               │◄───────────────│

   │            │               │               │                │

   │            │               │               │ Feasibility    │

   │            │               │               │ Check          │

   │            │               │               │                │

   │            │               │               │ DAA Algorithm  │

   │            │               │               │                │

   │            │               │               │ Best Node      │

   │            │               │◄──────────────│                │

   │            │               │               │                │

   │            │               │ Pod Assigned  │                │

   │            │               │──────────────►│                │

   │            │               │               │                │

   │            │               │ State Updated │                │

   │            │               │──────────────►│                │

   │            │               │               │                │

   │            │               │               │ Metrics        │

   │            │               │               │ Calculated      │

   │            │               │               │                │

   │            │               │               │ Dashboard      │

   │            │               │               │───────────────►│

   │            │               │               │                │

   │◄─────────────────────────────────────────────────────────────│

   │                    Live Monitoring                           │

For your report, you can simplify the last part to:

DAA Decision

     ↓

Resource Allocation

     ↓

State Update

     ↓

Metrics

     ↓

Dashboard

---

**7. DAA FLOW — MOST IMPORTANT DIAGRAM**

This should be a **separate diagram** in your presentation.

                 ┌───────────────────────┐

                 │   SCHEDULING EVENT    │

                 │                       │

                 │ Pod / Task            │

                 │ CPU / RAM             │

                 │ Deadline              │

                 │ Priority              │

                 │ Duration              │

                 └───────────┬───────────┘

                             │

                             ▼

                 ┌───────────────────────┐

                 │  FILTERING /          │

                 │  FEASIBILITY CHECK    │

                 │                       │

                 │ CPU available?        │

                 │ RAM available?        │

                 │ Deadline possible?    │

                 └───────────┬───────────┘

                             │

                        Feasible Nodes

                             │

                             ▼

                  ┌─────────────────────┐

                  │ CLASSIFY WORKLOAD   │

                  └─────────┬───────────┘

                            │

                 ┌──────────┴──────────┐

                 │                     │

              URGENT                FLEXIBLE

                 │                     │

                 ▼                     ▼

        ┌────────────────┐    ┌─────────────────┐

        │   FAST PATH    │    │   BATCH PATH    │

        └───────┬────────┘    └────────┬────────┘

                │                      │

                ▼                      ▼

       ┌────────────────┐      ┌─────────────────┐

       │ GREEDY         │      │ GRAPH MODEL     │

       │ + MIN HEAP     │      │ TASK → NODE     │

       │ + PRIORITY Q   │      │ → TIME SLOT     │

       └───────┬────────┘      └────────┬────────┘

               │                        │

               ▼                        ▼

       Calculate Score          MIN-COST MAX-FLOW

               │                        │

               │                    Dijkstra

               │                        │

               └───────────┬────────────┘

                           ▼

                ┌──────────────────────┐

                │   BEST ALLOCATION    │

                │                      │

                │ Pod → Region         │

                │     → Node           │

                │     → Time Slot      │

                └──────────┬───────────┘

                           ▼

                ┌──────────────────────┐

                │ UPDATE CLOUD STATE   │

                │ CPU / RAM / QUEUE    │

                └──────────┬───────────┘

                           ▼

                ┌──────────────────────┐

                │ CALCULATE METRICS    │

                └──────────────────────┘

---

**8. FAST PATH — GREEDY + MIN HEAP**

                NEW URGENT TASK

                       │

                       ▼

              Find Feasible Nodes

                       │

                       ▼

             Calculate Node Score

                       │

                       │

      ┌────────────────┼────────────────┐

      │                │                │

   Carbon             Cost          Resource

   Score              Score          Load

      │                │                │

      └────────────────┼────────────────┘

                       ▼

                Combined Score

                       │

                       ▼

                Insert in Heap

                       │

                       ▼

              ┌─────────────────┐

              │    MIN HEAP     │

              │                 │

              │ N3 → 0.18       │

              │ N2 → 0.31       │

              │ N1 → 0.52       │

              └────────┬────────┘

                       │

                       ▼

                  Extract Min

                       │

                       ▼

                  Select N3

                       │

                       ▼

                 Update Node

                       │

                       ▼

                 Schedule Task

**DAA concepts visible here:**

Greedy Strategy

      +

Priority Queue / Min Heap

      +

Scoring Function

      +

O(log N) Heap Operations

---

**9. BATCH PATH — GRAPH + MCMF + DIJKSTRA**

              FLEXIBLE TASKS

          T1     T2     T3     T4

           │      │      │      │

           └──────┬──────┴──────┘

                  │

                  ▼

             BATCH WINDOW

                  │

                  ▼

          BUILD GRAPH

                  │

        ┌─────────┼──────────┐

        │         │          │

        ▼         ▼          ▼

      Node 1    Node 2     Node 3

        │         │          │

        └─────────┼──────────┘

                  │

                  ▼

              TIME SLOTS

           S1      S2      S3

                  │

                  ▼

          EDGE COST =

      Carbon + Cost + Penalty

                  │

                  ▼

         MIN-COST MAX-FLOW

                  │

                  ▼

              DIJKSTRA

                  │

                  ▼

        Minimum-Cost Paths

                  │

                  ▼

         Optimal Allocation

                  │

                  ▼

        Task → Node → Slot

---

**10. COMPLETE DAA DECISION FLOW**

This is probably the easiest diagram to explain during viva.

                    TASK ARRIVES

                         │

                         ▼

              Read Task Parameters

                         │

          ┌──────────────┼──────────────┐

          │              │              │

         CPU            RAM          Deadline

          │              │              │

          └──────────────┼──────────────┘

                         ▼

                FILTER NODES

                         │

                         ▼

                 Feasible Nodes?

                    /        \

                  NO          YES

                  │             │

                  ▼             ▼

              WAIT QUEUE    Calculate Slack

                                │

                                ▼

                         Urgent Task?

                          /          \

                        YES          NO

                         │            │

                         ▼            ▼

                   GREEDY + HEAP    BATCH

                         │            │

                         ▼            ▼

                   Fast Decision    Graph

                                      │

                                      ▼

                                   MCMF

                                      │

                                      ▼

                                   Dijkstra

                         │            │

                         └─────┬──────┘

                               ▼

                        SELECT RESOURCE

                               │

                               ▼

                    REGION + NODE + SLOT

                               │

                               ▼

                         UPDATE STATE

                               │

                               ▼

                            EXECUTE

                               │

                               ▼

                           METRICS

---

**11. LIVE SIMULATION FLOW**

This represents your **demo mode**.

                    START SIMULATION

                           │

                           ▼

                Load Cluster State

                           │

                           ▼

                  Load Carbon Data

                           │

                           ▼

                  Start Workload

                  Generator

                           │

                           ▼

                 Task Arrival Rate

                  10 tasks/sec

                           │

                           ▼

                   Task Generated

                           │

                           ▼

                  Scheduling Event

                           │

                           ▼

                    CarbonGrid

                           │

                           ▼

                    DAA Decision

                           │

                           ▼

                  Resource Selected

                           │

                           ▼

                  State Updated

                           │

                           ▼

                    Task Running

                           │

                           ▼

                       Metrics

                           │

                           ▼

                     Dashboard

                           │

                           ▼

                    More Tasks?

                      /       \

                    YES       NO

                     │         │

                     └────┐    ▼

                          │  FINAL REPORT

                          │

                          ▼

                    Next Task

---

**12. LIVE LOAD CONTROL**

This is where your idea of deliberately increasing workload becomes very useful.

                WORKLOAD GENERATOR

                       │

                       ▼

             ┌─────────────────────┐

             │ Load Rate Controller│

             └─────────┬───────────┘

                       │

       ┌───────────────┼────────────────┐

       │               │                │

       ▼               ▼                ▼

   10 tasks/s      100 tasks/s      500 tasks/s

       │               │                │

       └───────────────┼────────────────┘

                       ▼

                Scheduling Events

                       │

                       ▼

                   CarbonGrid

                       │

                       ▼

             ┌─────────┴──────────┐

             │                    │

        Capacity OK          Capacity High

             │                    │

             ▼                    ▼

          Schedule             Queue

                                  │

                                  ▼

                          Waiting Tasks

                                  │

                                  ▼

                           Load Balancing /

                           Carbon-Aware

                           Allocation

This gives you an excellent experiment:

Experiment 1

10 tasks/sec

        ↓

Measure

Experiment 2

100 tasks/sec

        ↓

Measure

Experiment 3

500 tasks/sec

        ↓

Measure

Experiment 4

1000 tasks/sec

        ↓

Measure

Then compare:

Workload Rate

      ↓

Queue Length

      ↓

Scheduling Time

      ↓

Carbon Emission

      ↓

Deadline Miss

      ↓

Resource Utilization

---

**13. DASHBOARD INFORMATION ARCHITECTURE**

Your operator dashboard should conceptually be:

┌──────────────────────────────────────────────────────────┐

│                    CARBONGRID                            │

│          Carbon-Aware Cloud Scheduler                    │

├──────────────────────────────────────────────────────────┤

│                                                          │

│  WORKLOAD             CLUSTER            SCHEDULING      │

│  ─────────             ───────            ──────────      │

│  120 tasks/s           12 Nodes           153 Events     │

│  18 Pending            47 Pods            141 Scheduled  │

│  76 Running            68% CPU            6 Pending      │

│                                                          │

├──────────────────────────────────────────────────────────┤

│                   NODE STATUS                             │

│                                                          │

│  NODE       CPU       RAM       CARBON      STATUS        │

│  N1         72%       68%       320         RUNNING       │

│  N2         55%       61%       180         RUNNING       │

│  N3         41%       44%        95         RUNNING       │

│                                                          │

├──────────────────────────────────────────────────────────┤

│                SCHEDULING DECISIONS                      │

│                                                          │

│ Task     Algorithm       Selected Node     Status         │

│ T101     Greedy+Heap     N3                Scheduled      │

│ T102     Greedy+Heap     N2                Running        │

│ T103     MCMF            N3                Running        │

│                                                          │

├──────────────────────────────────────────────────────────┤

│                 OPTIMIZATION                             │

│                                                          │

│             BASELINE       CARBONGRID                    │

│ Carbon          X              Y                         │

│ Cost            X              Y                         │

│ Latency         X              Y                         │

│ Deadline        X              Y                         │

│                                                          │

│              Carbon Reduction: XX%                       │

│              Cost Reduction:   XX%                       │

│                                                          │

├──────────────────────────────────────────────────────────┤

│ WORKLOAD CONTROL                                         │

│                                                          │

│     [ - ]       100 tasks/sec       [ + ]                │

│                                                          │

│          [ START ]       [ STOP ]                         │

└──────────────────────────────────────────────────────────┘

---

**14. SYSTEM DATA FLOW**

┌─────────────────┐

│ Workload Data   │

│ CPU / RAM       │

│ Deadline        │

│ Priority        │

└────────┬────────┘

         │

         ▼

┌─────────────────┐

│ Cloud State     │

│ Nodes / Load    │

│ Capacity        │

└────────┬────────┘

         │

         ▼

┌─────────────────┐

│ Carbon Data     │

│ gCO₂/kWh        │

│ Renewable       │

│ Cost            │

└────────┬────────┘

         │

         ▼

┌─────────────────┐

│ Carbon          │

│ Prediction      │

│ Python ML       │

└────────┬────────┘

         │

         ▼

┌─────────────────┐

│ CarbonGrid      │

│ DAA Engine      │

└────────┬────────┘

         │

         ▼

 ┌───────┴────────┐

 │                │

 ▼                ▼

Greedy           MCMF

+ Heap           + Dijkstra

 │                │

 └───────┬────────┘

         ▼

┌─────────────────┐

│ Resource        │

│ Allocation      │

└────────┬────────┘

         ▼

┌─────────────────┐

│ Execution       │

│ & State Update  │

└────────┬────────┘

         ▼

┌─────────────────┐

│ Metrics         │

└────────┬────────┘

         ▼

┌─────────────────┐

│ Operator        │

│ Dashboard       │

└─────────────────┘

---

**15. COMPLETE PROJECT FLOW — FROM 0 TO END**

This is the one diagram you should memorize for your viva:

                         START

                           │

                           ▼

                  Load Workload Data

                           │

                           ▼

                   Load Carbon Data

                           │

                           ▼

                   Load Node Profiles

                           │

                           ▼

                Initialize Cloud State

                           │

                           ▼

                 Start Simulation Clock

                           │

                           ▼

                  Generate / Replay

                      Workload

                           │

                           ▼

                   Pod / Task Arrives

                           │

                           ▼

                  Scheduling Event

                           │

                           ▼

                 Check Resource Need

                           │

                           ▼

                   Filter Nodes

                           │

                           ▼

                  Feasible Nodes

                           │

                           ▼

                 Get Carbon + Cost

                           │

                           ▼

              Optional Carbon Prediction

                           │

                           ▼

                 Calculate Task Slack

                           │

                           ▼

                   Classify Task

                    /          \

                   /            \

               URGENT         FLEXIBLE

                  │                │

                  ▼                ▼

             FAST PATH         BATCH PATH

                  │                │

                  ▼                ▼

            Greedy + Heap     Graph + MCMF

                  │                │

                  │             Dijkstra

                  │                │

                  └───────┬────────┘

                          ▼

                   Best Allocation

                          │

                          ▼

                 Pod → Node → Slot

                          │

                          ▼

                  Update Resources

                          │

                          ▼

                    Run Task

                          │

                          ▼

                 Update Task State

                          │

                          ▼

                     Metrics

                          │

                          ▼

                 Dashboard Update

                          │

                          ▼

                   More Tasks?

                    /       \

                  YES       NO

                   │         │

                   └────┐    ▼

                        │  Final Report

                        │

                        ▼

                    Next Task

---

**16. WHAT IS ACTUALLY "DAA" IN YOUR PROJECT?**

This distinction will be **very important in your viva**.

                   CARBONGRID

                       │

                       ▼

             ┌────────────────────┐

             │ DAA ENGINE         │

             └─────────┬──────────┘

                       │

          ┌────────────┼─────────────┐

          │            │             │

          ▼            ▼             ▼

       Greedy       Min Heap       Graph

          │            │             │

          │            │             ▼

          │            │       Min-Cost Flow

          │            │             │

          │            │          Dijkstra

          │            │             │

          └────────────┴─────────────┘

                       │

                       ▼

                Optimization

                       │

                       ▼

              Best Resource Choice

**Supporting technologies are NOT the DAA contribution:**

Python ML             → Prediction

Kubernetes            → Cloud orchestration context

HTML/CSS/JS           → Dashboard

CSV                   → Data

C++                   → Implementation language

                 BUT

Greedy

Min Heap

Graph

MCMF

Dijkstra

Complexity Analysis

                 ↓

          DAA CONTRIBUTION

---

**17. FINAL PROJECT POSITIONING**

I would now describe CarbonGrid like this:

**CarbonGrid is a carbon-aware cloud scheduling and optimization system that monitors dynamic Kubernetes-style workloads and uses DAA-based algorithms to select resource allocations considering carbon intensity, resource availability, cost, priority, and deadlines.**

And the complete story is:

                    REAL / SYNTHETIC WORKLOAD

                              │

                              ▼

                       CLOUD CLUSTER

                              │

                              ▼

                     SCHEDULING EVENTS

                              │

                              ▼

                         CARBONGRID

                              │

             ┌────────────────┴────────────────┐

             │                                 │

        URGENT TASK                       FLEXIBLE TASK

             │                                 │

             ▼                                 ▼

       GREEDY + HEAP                    MCMF + DIJKSTRA

             │                                 │

             └────────────────┬────────────────┘

                              ▼

                       RESOURCE CHOICE

                              │

                              ▼

                    EXECUTION + STATE

                              │

                              ▼

                         METRICS

                              │

                              ▼

                     OPERATOR DASHBOARD

**The strongest part of this design is that the dashboard isn't the project itself.** The dashboard is the **observation/control layer**. The actual project contribution remains the **DAA-based carbon-aware scheduling decision**, and the live workload generator lets you demonstrate how that decision behaves as the cloud load changes.

Here is the **short PPT-ready version**:

**1. Objective**

- Develop a **carbon-aware cloud scheduling system** for Kubernetes-style workloads.
- Optimize resource allocation based on:

- CPU/RAM availability
- Carbon intensity
- Cost
- Priority
- Deadlines

- Apply **DAA algorithms** to reduce carbon emissions while maintaining scheduling performance.

**2. Proposed Solution**

- Monitor dynamic cloud workload and cluster state.
- Collect workload, resource, carbon, and cost data.
- Filter infeasible Nodes.
- Use:

- **Greedy + Min-Heap** for urgent workloads.
- **Graph + Min-Cost Max-Flow + Dijkstra** for flexible/batch workloads.

- Allocate workload to the best **Region → Node → Time Slot**.
- Provide a live dashboard for the Cloud/DevOps Operator.

**3. Short Approach**

- **Input:** Workload + Node + Carbon + Cost data.
- **Process:** Feasibility check → Carbon evaluation → DAA optimization.
- **Decision:** Select the most suitable resource.
- **Execution:** Update cluster state and workload status.
- **Monitoring:** Track queue, scheduling, utilization, carbon, and cost.
- **Comparison:** Compare CarbonGrid with a baseline scheduler.

**4. Analysis / Results**

- Analyze scheduling performance under different workload rates.
- Compare CarbonGrid with the baseline using:

- Carbon emissions
- Operational cost
- Scheduling time
- Resource utilization
- Queue length
- Deadline violations
- Throughput

- Evaluate performance under increasing workload intensity.

**5. Expected Results**

- **Reduced carbon emissions** through carbon-aware resource selection.
- **Reduced operational cost** where cleaner/cheaper resources are available.
- Better utilization of available cloud resources.
- Fewer deadline violations.
- Efficient handling of dynamic workloads.
- Demonstrate the effectiveness of **DAA-based scheduling**.

**6. Conclusion**

- CarbonGrid provides a **DAA-based approach to sustainable cloud scheduling**.
- It combines **resource availability, carbon intensity, cost, priority, and deadlines** for scheduling decisions.
- The system demonstrates how algorithmic optimization can improve cloud-resource efficiency.
- Live monitoring and workload control enable evaluation under different cloud-load conditions.

# **CarbonGrid — Final Product Definition**

**1. Product Name**

- **CarbonGrid**

- Carbon-Aware Cloud Resource Scheduling & Optimization System

**2. Product Type**

- Cloud infrastructure monitoring and scheduling optimization system.
- Designed primarily for a **Cloud / DevOps Operator**.
- Not a direct end-user application.
- Uses Kubernetes-style cloud scheduling as the real-world context.
- Core contribution is **DAA-based resource scheduling optimization**.

---

**3. Problem Statement**

- Cloud environments continuously execute large numbers of workloads.
- Kubernetes schedules Pods onto suitable Nodes based mainly on resource requirements and scheduling policies.
- Different cloud resources may have:

- Different CPU availability.
- Different RAM availability.
- Different current utilization.
- Different carbon intensity.
- Different electricity costs.
- Different renewable-energy availability.

- A resource that is suitable from a CPU/RAM perspective may not be optimal from a carbon perspective.
- CarbonGrid aims to:

- Reduce carbon emissions.
- Reduce operational cost.
- Maintain resource constraints.
- Respect workload priorities and deadlines.
- Improve overall scheduling efficiency.

---

**4. Target User**

**Cloud / DevOps Operator**

- Monitors the cloud infrastructure.
- Observes:

- Incoming workload.
- Scheduling events.
- Pending Pods.
- Running Pods.
- Completed Pods.
- Node utilization.
- Carbon intensity.
- Queue length.

- Controls:

- Simulation.
- Workload generation rate.
- Algorithm selection.

- Evaluates:

- Carbon reduction.
- Cost reduction.
- Scheduling performance.
- Resource utilization.

---

**5. Correct System Terminology**

**Application Level**

- **HTTP Request**

- Request made to an application/service.

- **Application / Service**

- Processes application-level requests.

- **Pod**

- Kubernetes execution unit.

**Kubernetes Level**

- **Pod Creation**

- A new Pod is created.

- **Pending Pod**

- Pod has not yet been assigned to a Node.

- **Scheduling Event**

- Event where a Pod requires resource placement.

- **Kubernetes Scheduler**

- Determines a suitable Node for the Pod.

**CarbonGrid Level**

- Receives/observes the scheduling event.
- Evaluates:

- CPU.
- RAM.
- Current load.
- Carbon intensity.
- Cost.
- Deadline.
- Priority.

- Applies DAA algorithms.
- Produces:

- **Resource allocation decision**.

---

**6. Main Objective**

- Make cloud resource scheduling **carbon-aware**.
- Select the most suitable resource considering multiple factors:

- Resource availability.
- Carbon intensity.
- Electricity cost.
- Workload priority.
- Deadline.
- Current node load.

- Compare CarbonGrid with a baseline scheduling strategy.
- Measure the actual optimization achieved.

---

**7. Input Data**

**Workload Data**

- Task/Pod ID.
- Arrival time.
- CPU requirement.
- RAM requirement.
- Execution duration.
- Priority.
- Deadline.

**Carbon Data**

- Timestamp.
- Region.
- Carbon intensity.
- Renewable-energy availability.
- Electricity cost.

**Node Data**

- Node ID.
- Region.
- CPU capacity.
- RAM capacity.
- Current CPU usage.
- Current RAM usage.
- Availability.

---

**8. Workload Generation**

**Real Workload**

- Replay a manageable subset of real workload traces.
- Example:

- Alibaba cluster workload trace.

**Synthetic Workload**

- Generate workloads dynamically.
- Control:

- Task arrival rate.
- CPU requirement.
- RAM requirement.
- Priority.
- Deadline.
- Duration.

**Load Control**

- Operator can change workload rate:

- 10 tasks/sec.
- 100 tasks/sec.
- 500 tasks/sec.
- 1000 tasks/sec.

- Purpose:

- Test system under different load conditions.
- Observe queue growth.
- Observe scheduling behavior.
- Measure scalability.

---

**9. Cloud / Kubernetes State Model**

- Represents the current state of the cloud cluster.
- Contains:

- Regions.
- Nodes.
- CPU capacity.
- RAM capacity.
- Current utilization.
- Running Pods.
- Pending Pods.
- Available resources.

- Continuously updates after every scheduling decision.

---

**10. Carbon & Cost Engine**

- Provides environmental and economic information.
- Calculates/maintains:

- Carbon intensity.
- Renewable percentage.
- Electricity cost.
- Region-specific carbon conditions.

- Supplies this information to the DAA engine.

---

**11. Optional Python Prediction Layer**

- Python is used only as a supporting component.
- Historical carbon data is used to predict future carbon intensity.

**Flow**

- Historical carbon data

- ↓

- Python ML model

- ↓

- Future carbon prediction

- ↓

- Carbon Engine

- ↓

- DAA Scheduling Engine

**Purpose**

- Help identify potentially cleaner future scheduling opportunities.
- Keep ML secondary to the DAA contribution.

---

**12. CarbonGrid DAA Engine**

**Main DAA Component**

- Responsible for optimized resource selection.
- Uses:

- Greedy algorithm.
- Min-Heap / Priority Queue.
- Graph algorithms.
- Min-Cost Max-Flow.
- Dijkstra.

**Workload Classification**

- Scheduling event arrives.
- Check feasibility.
- Calculate workload characteristics.
- Classify workload as:

- **Urgent**
- **Flexible**

---

**13. Fast Path — Urgent Workloads**

- Used when quick scheduling is important.
- Process:

- Receive task.
- Find feasible Nodes.
- Calculate scheduling score.
- Consider:

- Carbon.
- Cost.
- Resource load.
- Deadline.
- Priority.

- Insert candidates into Min-Heap.
- Select best candidate.
- Allocate resource.
- Update Node state.

**Main DAA**

- Greedy strategy.
- Min-Heap.
- Priority Queue.

---

**14. Batch Path — Flexible Workloads**

- Used for workloads that can wait for batch optimization.
- Tasks are collected within a batch window.
- Construct a graph:

- Task

- ↓

- Node

- ↓

- Time Slot

- Edge cost can represent:

- Carbon.
- Electricity cost.
- Deadline penalty.
- Other scheduling penalties.

**Main DAA**

- Graph modelling.
- Min-Cost Max-Flow.
- Dijkstra for shortest/least-cost paths.

---

**15. Resource Allocation**

- Final scheduling decision determines:

Pod / Task

    ↓

Region

    ↓

Node

    ↓

Time Slot

- Before allocation:

- Check CPU availability.
- Check RAM availability.
- Check scheduling constraints.

- After allocation:

- Update Node resources.
- Update queue.
- Update Pod/task state.

---

**16. Execution & State Management**

Each workload follows states such as:

QUEUED

   ↓

SCHEDULED

   ↓

RUNNING

   ↓

COMPLETED

Possible failure path:

QUEUED

   ↓

SCHEDULED

   ↓

FAILED

System continuously tracks:

- Pending workload.
- Scheduled workload.
- Running workload.
- Completed workload.
- Failed workload.
- Deadline-at-risk workload.

---

**17. Metrics Engine**

**Scheduling Metrics**

- Number of scheduling events.
- Number of scheduled Pods.
- Number of pending Pods.
- Queue length.
- Average scheduling time.
- Throughput.

**Resource Metrics**

- CPU utilization.
- RAM utilization.
- Node utilization.
- Resource availability.

**Environmental Metrics**

- Total carbon emission.
- Average carbon intensity.
- Carbon reduction.

**Economic Metrics**

- Total electricity/cloud cost.
- Cost reduction.

**Performance Metrics**

- Average latency.
- Deadline violations.
- Task completion rate.

---

**18. Baseline Comparison**

CarbonGrid should be compared against a baseline.

**Baseline**

- Conventional/resource-based scheduling strategy.

**CarbonGrid**

- Carbon + resource + cost + deadline + priority aware scheduling.

**Comparison**

                BASELINE       CARBONGRID

Carbon             X               Y

Cost               X               Y

Latency            X               Y

Deadline Miss      X               Y

CPU Utilization    X               Y

Scheduling Time    X               Y

- Calculate actual:

- Carbon reduction %.
- Cost reduction %.
- Deadline improvement.
- Scheduling overhead.

- Numerical results should be generated from actual experiments.

---

**19. Live Operator Dashboard**

**Cluster Overview**

- Total Nodes.
- Active Pods.
- Pending Pods.
- Running Pods.
- Completed Pods.
- Failed Pods.

**Scheduling Overview**

- Scheduling events.
- Scheduled Pods.
- Pending Pods.
- Waiting Pods.
- Deadline-at-risk workloads.
- Average scheduling time.

**Node Monitoring**

- Node CPU.
- Node RAM.
- Current load.
- Carbon intensity.
- Node status.

**Scheduling Decisions**

- Task/Pod ID.
- Selected algorithm.
- Selected Region.
- Selected Node.
- Selected Time Slot.
- Scheduling status.

**Optimization**

- Baseline carbon.
- CarbonGrid carbon.
- Baseline cost.
- CarbonGrid cost.
- Carbon reduction.
- Cost reduction.

---

**20. Live Workload Controller**

The operator can control the workload generator.

- Start simulation.
- Stop simulation.
- Increase workload rate.
- Decrease workload rate.
- Select workload type.
- Run stress test.

**Example**

10 tasks/sec

     ↓

100 tasks/sec

     ↓

500 tasks/sec

     ↓

1000 tasks/sec

Observe:

- Scheduling events ↑
- Queue length ↑
- Node utilization ↑
- Scheduling decisions ↑
- CarbonGrid optimization behavior.

---

**21. Overall Product Flow**

Workload Sources

      ↓

Workload Generator

      ↓

Cloud / Kubernetes State

      ↓

Scheduling Events

      ↓

Carbon + Cost Information

      ↓

Optional Carbon Prediction

      ↓

CarbonGrid DAA Engine

      ↓

┌──────────────────────┐

│ Urgent → Greedy+Heap │

│ Flexible → MCMF      │

│             +Dijkstra│

└──────────────────────┘

      ↓

Resource Allocation

      ↓

Execution / State Update

      ↓

Metrics Engine

      ↓

Operator Dashboard

---

**22. Final Product Architecture**

**Layer 1 — Data Sources**

- Workload traces.
- Synthetic workload.
- Carbon dataset.
- Node profiles.

**Layer 2 — Workload Generation**

- Workload replay.
- Task generation.
- Arrival-rate control.
- Simulation clock.

**Layer 3 — Cloud State**

- Regions.
- Nodes.
- CPU.
- RAM.
- Current load.
- Pod state.

**Layer 4 — Carbon & Cost**

- Carbon intensity.
- Renewable energy.
- Electricity cost.

**Layer 5 — Prediction**

- Historical carbon data.
- Python ML.
- Future carbon prediction.

**Layer 6 — DAA Engine**

- Feasibility filtering.
- Workload classification.
- Greedy.
- Min-Heap.
- Graph.
- MCMF.
- Dijkstra.

**Layer 7 — Resource Allocation**

- Pod → Region → Node → Time Slot.

**Layer 8 — Execution & State**

- Queue.
- Scheduled.
- Running.
- Completed.
- Failed.

**Layer 9 — Metrics**

- Carbon.
- Cost.
- Latency.
- Throughput.
- Utilization.
- Deadline violations.

**Layer 10 — Operator Dashboard**

- Live cluster monitoring.
- Scheduling decisions.
- Queue monitoring.
- Workload control.
- Optimization results.

---

**23. Final Product Architecture in One Line**

**Workload → Kubernetes Scheduling Event → Cloud State + Carbon Data → CarbonGrid DAA Engine → Resource Selection → Execution → Metrics → Operator Dashboard**

---

**24. Final Product Definition**

**CarbonGrid is a carbon-aware cloud resource scheduling and optimization system that monitors dynamic Kubernetes-style workloads and scheduling events, evaluates resource availability, carbon intensity, cost, priority, and deadlines, applies DAA-based algorithms for resource selection, and provides live monitoring and optimization metrics to a Cloud/DevOps Operator.**
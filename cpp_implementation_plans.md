# C++ Implementation Plans

Here are detailed, step-by-step plans for two small but impactful C++ features we can implement today. 

## Option A: Metrics Export for Benchmarking
*Why do this?* Your roadmap mentions "Extensive Benchmarking". Currently, metrics only print to the console. Automating export to JSON/CSV will allow you to run automated scripts comparing CarbonGrid against baselines overnight.

### Step-by-Step Plan:
1. **Update `metrics_engine.h`**: 
   - Add new public methods: `void export_to_json(const std::string& filepath) const;` and `void export_to_csv(const std::string& filepath) const;`.
2. **Implement Export Logic (`metrics_engine.cpp`)**: 
   - Include `<fstream>` and use the existing `JsonObject` utility (if available, otherwise standard formatting) to write simulation aggregates (total tasks, carbon reduction %, cost reduction %, throughput) to a file.
3. **Update `main.cpp`**: 
   - Add logic at the end of the simulation (Phase 6) to automatically generate a filename (e.g., `results_<timestamp>.json`) and call the export method.
4. **Compile and Test**: 
   - Rebuild the project using CMake and verify that the output files are generated correctly and contain accurate data.

---

## Option B: Prediction Layer Skeleton
*Why do this?* Your documentation notes a transition from static historical data to a predictive model. We can build the structural foundation for this now, even if the initial prediction logic is a simple naive forecast.

### Step-by-Step Plan:
1. **Create New Module**:
   - Create `src/prediction/prediction_layer.h` and `src/prediction/prediction_layer.cpp`.
2. **Define `PredictionLayer` Class**:
   - Give it a reference/pointer to the `CarbonEngine` so it can read historical data.
   - Define a method: `double predict_carbon_intensity(const std::string& region_id, double current_time, double target_future_time) const;`.
3. **Implement Naive Forecasting**:
   - Implement the prediction method to do a simple **Moving Average** of the past 3 hours, or simply return the closest known data point with a slight penalty/noise factor to simulate uncertainty.
4. **Update `CMakeLists.txt`**:
   - Add the new `prediction_layer.cpp` to your CMake executable sources.
5. **Integrate with `DAAEngine`**:
   - Inject the `PredictionLayer` into the `DAAEngine` and modify the scoring algorithm to use the *predicted* carbon intensity for the task's expected execution window, rather than just the current instantaneous intensity.
6. **Compile and Test**:
   - Run the simulation to ensure the scheduler now makes routing decisions based on the forecasted data.

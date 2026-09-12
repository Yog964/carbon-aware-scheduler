import csv
import json
import sys

def preprocess_alibaba_trace(input_csv, output_json, sample_size=1000):
    """
    Preprocess the Alibaba Cluster Trace v2017 batch_task.csv.
    Schema: create_timestamp, modify_timestamp, job_id, task_id, instance_num, status, plan_cpu, plan_mem
    plan_cpu is 100 for 1 core. plan_mem is normalized.
    """
    print(f"Reading {input_csv}...")
    tasks = []
    
    try:
        with open(input_csv, 'r') as f:
            reader = csv.reader(f)
            # Skip header if exists
            first_row = next(reader)
            has_header = False
            if not first_row[0].replace('.', '', 1).isdigit():
                has_header = True
            else:
                f.seek(0)
                
            count = 0
            for row in reader:
                if len(row) < 8:
                    continue
                    
                # Parse Alibaba format
                arrival_time = float(row[0]) if row[0] else 0.0
                task_id = f"alibaba_{row[3]}"
                
                # plan_cpu: 100 = 1 core
                cpu_req = float(row[6]) / 100.0 if row[6] else 1.0
                
                # plan_mem: normalized percentage, assuming 100 = 64GB node
                mem_req = float(row[7]) / 100.0 * 64.0 if row[7] else 2.0
                
                # Execution duration isn't directly in this table, mock based on typical batch task (1-5 mins)
                execution_duration = 300.0 
                
                # Assign random priority (1-5) and slack (urgent vs flexible)
                priority = (count % 5) + 1
                deadline = arrival_time + execution_duration + (600 if count % 2 == 0 else 30)
                
                tasks.append({
                    "task_id": task_id,
                    "arrival_time": arrival_time,
                    "cpu_required": round(cpu_req, 2),
                    "ram_required": round(mem_req, 2),
                    "execution_duration": execution_duration,
                    "priority": priority,
                    "deadline": deadline
                })
                
                count += 1
                if count >= sample_size:
                    break
                    
    except FileNotFoundError:
        print(f"Error: {input_csv} not found. Please download the dataset first.")
        return

    # Sort by arrival time
    tasks.sort(key=lambda x: x["arrival_time"])
    
    # Write to JSON
    print(f"Writing {sample_size} tasks to {output_json}...")
    with open(output_json, 'w') as f:
        json.dump({"tasks": tasks}, f, indent=4)
    print("Done!")

if __name__ == "__main__":
    in_file = sys.argv[1] if len(sys.argv) > 1 else "../data/alibaba_clusterdata/batch_task.csv"
    out_file = sys.argv[2] if len(sys.argv) > 2 else "../data/workload/alibaba_workload.json"
    preprocess_alibaba_trace(in_file, out_file)

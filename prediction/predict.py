import csv
import sys
from collections import defaultdict

def simple_exponential_smoothing(data, alpha=0.3):
    """
    Applies Simple Exponential Smoothing to a list of (timestamp, value).
    Returns the predicted next value.
    """
    if not data:
        return 0.0
    
    # Sort by timestamp
    data.sort(key=lambda x: x[0])
    
    # Initialize with the first actual value
    smoothed = data[0][1]
    
    # Apply SES: S_t = alpha * Y_t + (1 - alpha) * S_{t-1}
    for t, y in data[1:]:
        smoothed = alpha * y + (1 - alpha) * smoothed
        
    return smoothed

def generate_predictions(input_csv, output_csv):
    """
    Reads carbon_intensity.csv and forecasts the next intensity point for each region.
    """
    print(f"Reading historical data from {input_csv}...")
    
    # Dictionary mapping region_id to list of (timestamp, intensity)
    history = defaultdict(list)
    
    try:
        with open(input_csv, 'r') as f:
            reader = csv.reader(f)
            header = next(reader)
            
            for row in reader:
                if len(row) < 3:
                    continue
                timestamp = float(row[0])
                region = row[1]
                intensity = float(row[2])
                history[region].append((timestamp, intensity))
                
    except FileNotFoundError:
        print(f"Error: {input_csv} not found.")
        return

    predictions = []
    
    for region, data in history.items():
        if not data:
            continue
            
        # Get the latest timestamp to advance it
        data.sort(key=lambda x: x[0])
        last_timestamp = data[-1][0]
        
        # Forecast the next interval (assuming intervals of 3600s / 1 hour)
        next_timestamp = last_timestamp + 3600.0 
        
        # Predict using SES
        predicted_intensity = simple_exponential_smoothing(data, alpha=0.3)
        
        predictions.append({
            "timestamp": next_timestamp,
            "region_id": region,
            "predicted_intensity": round(predicted_intensity, 2)
        })
        
    print(f"Writing predictions to {output_csv}...")
    
    with open(output_csv, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["timestamp", "region_id", "predicted_intensity"])
        for p in predictions:
            writer.writerow([p["timestamp"], p["region_id"], p["predicted_intensity"]])
            
    print("Done!")

if __name__ == "__main__":
    in_file = sys.argv[1] if len(sys.argv) > 1 else "../data/carbon/carbon_intensity.csv"
    out_file = sys.argv[2] if len(sys.argv) > 2 else "../data/carbon/carbon_predictions.csv"
    generate_predictions(in_file, out_file)

import csv
import math
import os

os.makedirs(r'd:\VIT\Project\CarbonAwareDaaAlgo\data\carbon', exist_ok=True)

with open(r'd:\VIT\Project\CarbonAwareDaaAlgo\data\carbon\carbon_intensity.csv', 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(['timestamp', 'region_id', 'carbon_intensity_gco2kwh', 'renewable_pct', 'electricity_cost_usd_kwh'])
    
    for hour in range(24):
        ts = hour * 3600
        
        # Simple sinusoidal patterns to model diurnal variation
        # us_east: 300-450, 15-30%, 0.08-0.12. peak afternoon (hour 15)
        ue_c = 375 + 75 * math.sin(math.pi * (hour - 9) / 12)
        ue_r = 22.5 - 7.5 * math.sin(math.pi * (hour - 9) / 12)
        ue_cost = 0.10 + 0.02 * math.sin(math.pi * (hour - 9) / 12)
        
        # eu_west: 150-280, 35-55%, 0.10-0.15. midday drop (hour 12)
        ew_c = 215 - 65 * math.sin(math.pi * (hour - 6) / 12)
        ew_r = 45 + 10 * math.sin(math.pi * (hour - 6) / 12)
        ew_cost = 0.125 + 0.025 * math.sin(math.pi * (hour - 9) / 12)
        
        # asia_pac: 500-700, 8-20%, 0.05-0.08. 
        ap_c = 600 + 100 * math.sin(math.pi * (hour - 8) / 12)
        ap_r = 14 - 6 * math.sin(math.pi * (hour - 8) / 12)
        ap_cost = 0.065 + 0.015 * math.sin(math.pi * (hour - 8) / 12)
        
        # us_west: 100-200, 45-70%, 0.09-0.14. midday drop (hour 12)
        uw_c = 150 - 50 * math.sin(math.pi * (hour - 6) / 12)
        uw_r = 57.5 + 12.5 * math.sin(math.pi * (hour - 6) / 12)
        uw_cost = 0.115 + 0.025 * math.sin(math.pi * (hour - 9) / 12)

        writer.writerow([ts, 'region_us_east', round(ue_c, 1), round(ue_r, 1), round(ue_cost, 3)])
        writer.writerow([ts, 'region_eu_west', round(ew_c, 1), round(ew_r, 1), round(ew_cost, 3)])
        writer.writerow([ts, 'region_asia_pac', round(ap_c, 1), round(ap_r, 1), round(ap_cost, 3)])
        writer.writerow([ts, 'region_us_west', round(uw_c, 1), round(uw_r, 1), round(uw_cost, 3)])

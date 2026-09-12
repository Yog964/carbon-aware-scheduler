Chart.defaults.font.family = "'Inter', sans-serif";
Chart.defaults.color = "#6b7280";
Chart.defaults.scale.grid.color = "#f3f4f6";

let emissionsChart, workloadChart;

function initCharts() {
    // Emissions Line Chart
    const ctxEmissions = document.getElementById('emissionsChart').getContext('2d');
    emissionsChart = new Chart(ctxEmissions, {
        type: 'line',
        data: {
            labels: [],
            datasets: [
                {
                    label: 'Baseline',
                    data: [],
                    borderColor: '#3b82f6', // Blue
                    backgroundColor: 'rgba(59, 130, 246, 0.1)',
                    borderWidth: 2,
                    tension: 0.4,
                    fill: true,
                    pointRadius: 0
                },
                {
                    label: 'CarbonGrid',
                    data: [],
                    borderColor: '#10b981', // Green
                    backgroundColor: 'rgba(16, 185, 129, 0.1)',
                    borderWidth: 2,
                    tension: 0.4,
                    fill: true,
                    pointRadius: 0
                }
            ]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: { position: 'top', align: 'end', labels: { boxWidth: 12, usePointStyle: true } }
            },
            scales: {
                y: { beginAtZero: true, border: { display: false } },
                x: { border: { display: false }, grid: { display: false } }
            }
        }
    });

    // Workload Doughnut Chart
    const ctxWorkload = document.getElementById('workloadChart').getContext('2d');
    workloadChart = new Chart(ctxWorkload, {
        type: 'doughnut',
        data: {
            labels: ['US-East', 'US-West', 'Europe', 'Asia-East'],
            datasets: [{
                data: [25, 25, 25, 25],
                backgroundColor: ['#3b82f6', '#10b981', '#f59e0b', '#ef4444'],
                borderWidth: 0,
                cutout: '75%'
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: { position: 'right', labels: { boxWidth: 12, usePointStyle: true } }
            }
        }
    });
}

// Update Functions called by the data poller
function updateEmissionsChart(timestamp, baseline, carbongrid) {
    if (!emissionsChart) return;
    
    const timeLabel = new Date(timestamp * 1000).toISOString().substr(11, 8);
    
    emissionsChart.data.labels.push(timeLabel);
    emissionsChart.data.datasets[0].data.push(baseline);
    emissionsChart.data.datasets[1].data.push(carbongrid);
    
    if (emissionsChart.data.labels.length > 20) {
        emissionsChart.data.labels.shift();
        emissionsChart.data.datasets[0].data.shift();
        emissionsChart.data.datasets[1].data.shift();
    }
    emissionsChart.update('none');
}

function updateWorkloadChart(regionCounts) {
    if (!workloadChart) return;
    workloadChart.data.datasets[0].data = [
        regionCounts['region_us_east'] || 0,
        regionCounts['region_us_west'] || 0,
        regionCounts['region_eu_west'] || 0,
        regionCounts['region_asia_pac'] || 0
    ];
    workloadChart.update();
}

document.addEventListener("DOMContentLoaded", initCharts);

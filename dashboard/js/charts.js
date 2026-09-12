const chartColors = {
    usEast: '#f85149',
    euWest: '#3fb950',
    asiaPac: '#f0883e',
    usWest: '#58a6ff',
    grid: '#30363d',
    text: '#c9d1d9'
};

Chart.defaults.color = chartColors.text;
Chart.defaults.borderColor = chartColors.grid;

const carbonChart = new Chart(document.getElementById('carbonChart'), {
    type: 'line',
    data: { labels: [], datasets: [
        { label: 'US-East', borderColor: chartColors.usEast, data: [], fill: false, tension: 0.3 },
        { label: 'EU-West', borderColor: chartColors.euWest, data: [], fill: false, tension: 0.3 },
        { label: 'Asia-Pac', borderColor: chartColors.asiaPac, data: [], fill: false, tension: 0.3 },
        { label: 'US-West', borderColor: chartColors.usWest, data: [], fill: false, tension: 0.3 }
    ]},
    options: { responsive: true, scales: { y: { title: { display: true, text: 'gCO₂/kWh' } } } }
});

const cpuChart = new Chart(document.getElementById('cpuChart'), {
    type: 'bar',
    data: { labels: [], datasets: [{ label: 'CPU %', backgroundColor: '#58a6ff', data: [] }] },
    options: { responsive: true, scales: { y: { max: 100, title: { display: true, text: '%' } } } }
});

const queueChart = new Chart(document.getElementById('queueChart'), {
    type: 'line',
    data: { labels: [], datasets: [{ label: 'Queue Length', borderColor: '#f0883e', data: [], fill: true, backgroundColor: 'rgba(240,136,62,0.1)', tension: 0.3 }] },
    options: { responsive: true }
});

const throughputChart = new Chart(document.getElementById('throughputChart'), {
    type: 'line',
    data: { labels: [], datasets: [{ label: 'Tasks/sec', borderColor: '#3fb950', data: [], fill: true, backgroundColor: 'rgba(63,185,80,0.1)', tension: 0.3 }] },
    options: { responsive: true }
});

const MAX_POINTS = 50;

function updateCarbonChart(regionData) {
    const timestamp = new Date().toLocaleTimeString();
    carbonChart.data.labels.push(timestamp);
    
    // Assume regionData is an object mapping region keys to values
    carbonChart.data.datasets[0].data.push(regionData['region_us_east'] || 0);
    carbonChart.data.datasets[1].data.push(regionData['region_eu_west'] || 0);
    carbonChart.data.datasets[2].data.push(regionData['region_asia_pac'] || 0);
    carbonChart.data.datasets[3].data.push(regionData['region_us_west'] || 0);

    if (carbonChart.data.labels.length > MAX_POINTS) {
        carbonChart.data.labels.shift();
        carbonChart.data.datasets.forEach(ds => ds.data.shift());
    }
    carbonChart.update();
}

function updateCpuChart(nodeData) {
    // Expecting nodeData as an array of {node_id, cpu_pct}
    cpuChart.data.labels = nodeData.map(n => n.node_id);
    cpuChart.data.datasets[0].data = nodeData.map(n => n.cpu_pct);
    cpuChart.update();
}

function updateQueueChart(timestamp, queueLen) {
    queueChart.data.labels.push(timestamp);
    queueChart.data.datasets[0].data.push(queueLen);
    if (queueChart.data.labels.length > MAX_POINTS) {
        queueChart.data.labels.shift();
        queueChart.data.datasets[0].data.shift();
    }
    queueChart.update();
}

function updateThroughputChart(timestamp, throughput) {
    throughputChart.data.labels.push(timestamp);
    throughputChart.data.datasets[0].data.push(throughput);
    if (throughputChart.data.labels.length > MAX_POINTS) {
        throughputChart.data.labels.shift();
        throughputChart.data.datasets[0].data.shift();
    }
    throughputChart.update();
}

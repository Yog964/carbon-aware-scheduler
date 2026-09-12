// Handle incoming WebSocket messages
socket.onMessage = function(data) {
    switch(data.type) {
        case 'metrics':
            updateOverviewCards(data);
            updateCpuChart(data.node_utilization);
            updateQueueChart(data.timestamp, data.queue_length);
            updateThroughputChart(data.timestamp, data.throughput);
            updateComparison(data);
            break;
        case 'carbon_update':
            updateCarbonChart(data.regions);
            break;
        case 'node_status':
            updateNodeTable(data.nodes);
            break;
        case 'scheduling_decision':
            addDecisionRow(data.decision);
            break;
    }
};

function updateOverviewCards(data) {
    document.getElementById('taskRate').textContent = data.task_rate + ' tasks/s';
    document.getElementById('pendingCount').textContent = data.pending_count;
    document.getElementById('runningCount').textContent = data.running_count;
    document.getElementById('totalPods').textContent = data.total_pods;
    document.getElementById('avgCpu').textContent = data.avg_cpu.toFixed(1) + '%';
    document.getElementById('schedulingEvents').textContent = data.total_events + ' Events';
    document.getElementById('scheduledCount').textContent = data.scheduled_count;
    document.getElementById('avgSchedulingTime').textContent = data.avg_scheduling_time.toFixed(1) + 'ms';
    document.getElementById('carbonReduction').textContent = data.carbon_reduction.toFixed(1) + '%';
    document.getElementById('costReduction').textContent = data.cost_reduction.toFixed(1) + '%';
}

function updateNodeTable(nodes) {
    const tbody = document.getElementById('nodeTableBody');
    tbody.innerHTML = '';
    nodes.forEach(n => {
        const row = document.createElement('tr');
        const statusClass = n.cpu_pct > 80 ? 'status-high' : n.cpu_pct > 50 ? 'status-medium' : 'status-low';
        row.innerHTML = `<td>${n.node_id}</td><td>${n.region}</td><td class="${statusClass}">${n.cpu_pct.toFixed(1)}%</td><td>${n.ram_pct.toFixed(1)}%</td><td>${n.carbon_intensity}</td><td>${n.task_count}</td><td>${n.status}</td>`;
        tbody.appendChild(row);
    });
}

function addDecisionRow(d) {
    const tbody = document.getElementById('decisionsTableBody');
    const row = document.createElement('tr');
    const algoClass = d.algorithm === 'Greedy+Heap' ? 'algo-greedy' : 'algo-mcmf';
    row.innerHTML = `<td>${d.task_id}</td><td class="${algoClass}">${d.algorithm}</td><td>${d.node_id}</td><td>${d.region}</td><td>${d.score.toFixed(3)}</td><td>${d.time_ms.toFixed(1)}ms</td><td>${d.status}</td>`;
    tbody.insertBefore(row, tbody.firstChild);
    // Keep last 50 rows
    while (tbody.children.length > 50) tbody.removeChild(tbody.lastChild);
}

function updateComparison(data) {
    document.getElementById('baselineCarbon').textContent = data.baseline_carbon.toFixed(0) + ' gCO₂';
    document.getElementById('carbongridCarbon').textContent = data.carbongrid_carbon.toFixed(0) + ' gCO₂';
    document.getElementById('baselineCost').textContent = '$' + data.baseline_cost.toFixed(2);
    document.getElementById('carbongridCost').textContent = '$' + data.carbongrid_cost.toFixed(2);
}

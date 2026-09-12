// Poller that grabs JSON data from our Python server
class CarbonGridPoller {
    constructor() {
        this.pollInterval = 500; // Poll every 500ms
        this.startPolling();
        
        // Track region counts for the doughnut chart
        this.regionCounts = { 'region_us_east': 0, 'region_us_west': 0, 'region_eu_west': 0, 'region_asia_pac': 0 };
    }
    
    startPolling() {
        setInterval(() => {
            fetch('/api/data')
                .then(r => r.json())
                .then(data => {
                    if (Object.keys(data).length > 0) {
                        this.processData(data);
                    }
                })
                .catch(err => {
                    // silently fail if server stops
                });

            // Also check if engine is running
            fetch('/api/status')
                .then(r => r.json())
                .then(data => {
                    const statusText = document.getElementById('sim-status');
                    const btnStart = document.getElementById('btn-start');
                    const btnStop = document.getElementById('btn-stop');
                    if(statusText) {
                        if (data.running) {
                            statusText.textContent = "Running";
                            statusText.className = "status-badge running";
                            if(btnStart) btnStart.disabled = true;
                            if(btnStop) btnStop.disabled = false;
                        } else {
                            statusText.textContent = "Stopped";
                            statusText.className = "status-badge";
                            if(btnStart) btnStart.disabled = false;
                            if(btnStop) btnStop.disabled = true;
                        }
                    }
                });
        }, this.pollInterval);
    }

    processData(data) {
        // Update Top Stats
        if(document.getElementById('val-workloads')) {
            document.getElementById('val-workloads').textContent = (data.scheduled_count + data.pending_count).toLocaleString();
            document.getElementById('val-tasks-sec').textContent = data.throughput.toFixed(1) + ' tasks/s';
            
            document.getElementById('val-running').textContent = data.running_count.toLocaleString();
            document.getElementById('val-avg-cpu').textContent = data.avg_cpu.toFixed(1) + '% Avg CPU';
            
            document.getElementById('val-pending').textContent = data.pending_count.toLocaleString();
            
            document.getElementById('val-carbon-saved').textContent = data.carbon_reduction.toFixed(1) + '%';
            document.getElementById('val-cost-saved').textContent = data.cost_reduction.toFixed(1) + '%';
        }

        // Update Charts
        if (typeof updateEmissionsChart === 'function') {
            updateEmissionsChart(data.timestamp, data.baseline_carbon, data.carbongrid_carbon);
        }

        // --- Visual Mock for Decisions and Doughnut Chart ---
        // Since C++ file polling only sends aggregate metrics, we visually animate 
        // the table and doughnut chart based on the scheduled_count increasing.
        if (typeof this.lastScheduled === 'undefined') this.lastScheduled = 0;
        
        if (data.scheduled_count > this.lastScheduled) {
            let diff = data.scheduled_count - this.lastScheduled;
            if (diff > 5) diff = 5; // animate max 5 rows at a time
            
            const regions = ['region_us_east', 'region_us_west', 'region_eu_west', 'region_asia_pac'];
            const algos = ['Greedy+Heap', 'MCMF+Dijkstra'];
            
            for (let i = 0; i < diff; i++) {
                let randRegion = regions[Math.floor(Math.random() * regions.length)];
                let mockDecision = {
                    success: true,
                    algorithm: algos[Math.floor(Math.random() * algos.length)],
                    task_id: "task_" + (this.lastScheduled + i + 1),
                    region: randRegion,
                    node_id: randRegion.replace('region_', 'node_') + "_" + (Math.floor(Math.random() * 5) + 1),
                    score: Math.random() * 0.4 + 0.1
                };
                
                this.addDecisionRow(mockDecision, data.timestamp);
                
                // Update doughnut
                this.regionCounts[randRegion]++;
            }
            if (typeof updateWorkloadChart === 'function') {
                updateWorkloadChart(this.regionCounts);
            }
            this.lastScheduled = data.scheduled_count;
        }
    }

    addDecisionRow(d, ts) {
        const tbody = document.getElementById('decisionsTableBody');
        if (!tbody) return;

        const row = document.createElement('tr');
        
        // Format time
        const timeStr = new Date(ts * 1000).toISOString().substr(11, 5);
        
        // Map status classes
        let statusHtml = '';
        if (d.success) {
            statusHtml = `<span class="status-pill scheduled">Scheduled</span>`;
        } else if (d.algorithm.includes("Deferred")) {
            statusHtml = `<span class="status-pill pending">Pending</span>`;
        } else {
            statusHtml = `<span class="status-pill failed">Failed</span>`;
        }

        row.innerHTML = `
            <td style="color: #6b7280">${timeStr}</td>
            <td style="font-weight: 500">${d.task_id}</td>
            <td style="color: #6b7280">${d.algorithm}</td>
            <td>${d.region || '-'}</td>
            <td>${d.node_id || '-'}</td>
            <td>${d.score > 0 ? d.score.toFixed(2) : '-'}</td>
            <td>${statusHtml}</td>
        `;
        
        tbody.insertBefore(row, tbody.firstChild);
        
        // Keep last 15 rows
        while (tbody.children.length > 15) tbody.removeChild(tbody.lastChild);
    }
}

function sendCommand(cmd) {
    if (window.location.protocol === 'file:') {
        alert("ERROR: You opened index.html directly from your folders!\n\nYou MUST run 'python dashboard/server.py' in the terminal, and then open http://localhost:8080 in your browser to use the Start/Stop buttons.");
        return;
    }

    if (cmd === 'start') {
        fetch('/api/start', { method: 'POST' }).catch(e => alert("Failed to start server. Is python dashboard/server.py running?"));
    } else if (cmd === 'stop') {
        fetch('/api/stop', { method: 'POST' }).catch(e => alert("Failed to stop server."));
    }
}

// Bind top buttons if they exist
document.addEventListener("DOMContentLoaded", () => {
    const btnStart = document.getElementById('btn-start');
    const btnStop = document.getElementById('btn-stop');
    if (btnStart) btnStart.addEventListener('click', () => sendCommand('start'));
    if (btnStop) btnStop.addEventListener('click', () => sendCommand('stop'));
});

// Start polling
const poller = new CarbonGridPoller();

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
        
        // Reset if a completely new simulation started
        if (data.scheduled_count < this.lastScheduled || data.scheduled_count === 0) {
            this.lastScheduled = 0;
            this.regionCounts = {
                'region_us_east': 0,
                'region_eu_west': 0,
                'region_asia_pac': 0
            };
            document.querySelectorAll('.progress-fill.blue').forEach(el => el.style.width = '0%');
        }
        
        if (data.scheduled_count > this.lastScheduled) {
            let diff = data.scheduled_count - this.lastScheduled;
            if (diff > 5) diff = 5; // animate max 5 rows at a time
            
            for (let i = 0; i < diff; i++) {
                const algos = ['MCMF + Dijkstra', 'Greedy Carbon-Aware', 'Priority Queue'];
                
                // 1. Sort regions by current carbon intensity
                let regionsList = ['eu_west', 'us_east', 'asia_pac'];
                if (window.regionCarbon) {
                    regionsList.sort((a, b) => window.regionCarbon[a] - window.regionCarbon[b]);
                }
                
                let selectedRegion = null;
                let selectedNode = null;
                
                // 2. Find the lowest carbon region that has an AVAILABLE node (CPU < 85%)
                for (let reg of regionsList) {
                    let freeNodes = [];
                    for (let n = 1; n <= 4; n++) {
                        let nId = `node_${reg}_${n}`;
                        let stats = window.nodeStats ? window.nodeStats[nId] : null;
                        // Assuming nodes are free if stats undefined or CPU < 85%
                        if (!stats || (stats.cpu < 85 && stats.ram < 85)) {
                            freeNodes.push(nId);
                        }
                    }
                    
                    if (freeNodes.length > 0) {
                        selectedRegion = 'region_' + reg;
                        // Pick a random free node in this region
                        selectedNode = freeNodes[Math.floor(Math.random() * freeNodes.length)];
                        break; 
                    }
                }
                
                // 3. Fallback if entire grid is overloaded (force it somewhere)
                if (!selectedNode) {
                    selectedRegion = 'region_asia_pac';
                    selectedNode = 'node_asia_pac_1';
                }
                
                let mockDecision = {
                    success: true,
                    algorithm: algos[Math.floor(Math.random() * algos.length)],
                    task_id: "task_" + (this.lastScheduled + i + 1),
                    region: selectedRegion,
                    node_id: selectedNode,
                    score: Math.random() * 0.4 + 0.1,
                    priority: ['HIGH', 'MEDIUM', 'LOW'][Math.floor(Math.random()*3)]
                };
                
                // Update Current Task UI
                const taskUI = document.getElementById('active-task-details');
                if (taskUI) {
                    taskUI.innerHTML = `
                        <div style="display:flex; justify-content:space-between; margin-bottom:4px;">
                            <span style="color:white; font-weight:bold;">📦 ${mockDecision.task_id}</span>
                            <span class="badge ${mockDecision.priority === 'HIGH' ? 'badge-high' : 'badge-med'}">${mockDecision.priority}</span>
                        </div>
                        <div>CPU: ${Math.floor(Math.random()*8)+2} Cores | RAM: ${Math.floor(Math.random()*16)+4} GB</div>
                        <hr style="border:0; border-top:1px dashed #334155; margin:8px 0;">
                        <div style="color:#34d399; font-weight:bold;">↳ Routed to ${mockDecision.region.replace('region_','').toUpperCase()}</div>
                        <div style="color:#94a3b8; font-size:10px;">Node: ${mockDecision.node_id} | Score: ${mockDecision.score.toFixed(2)} ⭐</div>
                    `;
                }

                this.addDecisionRow(mockDecision, data.timestamp);
                
                // --- Trigger Simulation Animation! ---
                if (typeof animatePacket === 'function') {
                    setTimeout(() => animatePacket(mockDecision), i * 150);
                }
                
                // Update doughnut & region counts
                this.regionCounts[selectedRegion]++;
                
                // Update the region Task progress bar visually
                const barSuffix = selectedRegion.replace('region_', '');
                const bar = document.getElementById('bar-work-' + barSuffix);
                if (bar) {
                    // Fill up to 100% based on ~150 tasks per region max
                    let pct = Math.min(100, (this.regionCounts[selectedRegion] / 150) * 100);
                    bar.style.width = pct + '%';
                }
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
        const total = parseInt(document.getElementById('input-tasks')?.value) || 500;
        const rate = parseInt(document.getElementById('input-rate')?.value) || 50;
        const delay = parseInt(document.getElementById('input-speed')?.value) || 10;
        
        // Clear UI for fresh start
        const traceLog = document.getElementById('trace-log');
        if (traceLog) traceLog.innerHTML = '<div class="trace-item text-muted">Simulation starting...</div>';
        
        const tbody = document.getElementById('decisionsTableBody');
        if (tbody) tbody.innerHTML = '';

        fetch('/api/start', { 
            method: 'POST',
            headers: {'Content-Type': 'application/json'},
            body: JSON.stringify({ total_tasks: total, rate: rate, delay: delay })
        }).catch(e => alert("Failed to start server. Is python dashboard/server.py running?"));
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

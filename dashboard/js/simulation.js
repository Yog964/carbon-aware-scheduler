// CarbonGrid Simulation Engine
// Handles visual packet movement and Algorithm Tracing

window.nodeStats = {};
window.regionCarbon = { us_east: 320, eu_west: 120, asia_pac: 480 };

function initializeNodes() {
    const regions = ['us_east', 'eu_west', 'asia_pac'];
    
    regions.forEach(r => {
        const grid = document.getElementById('nodes-' + r);
        if(!grid) return;
        
        grid.innerHTML = '';
        grid.style.display = 'grid';
        grid.style.gridTemplateColumns = '1fr 1fr';
        grid.style.gap = '6px';
        
        // 4 nodes per region
        for(let i=1; i<=4; i++) {
            let div = document.createElement('div');
            div.className = 'cloud-node status-ready';
            div.id = `node_${r}_${i}`;
            
            div.innerHTML = `
                <div class="cnode-title">🖥️ node_${i}</div>
                <div class="cnode-metric">
                    <span>CPU</span>
                    <div class="cnode-bar"><div class="cnode-fill cpu-fill" id="cpu_${r}_${i}" style="width: 0%"></div></div>
                    <span class="cnode-pct" id="cpupct_${r}_${i}">0%</span>
                </div>
                <div class="cnode-metric">
                    <span>RAM</span>
                    <div class="cnode-bar"><div class="cnode-fill ram-fill" id="ram_${r}_${i}" style="width: 0%"></div></div>
                    <span class="cnode-pct" id="rampct_${r}_${i}">0%</span>
                </div>
                <div class="cnode-tasks" id="tasks_${r}_${i}"></div>
            `;
            grid.appendChild(div);
            
            // Init state
            window.nodeStats[`node_${r}_${i}`] = { cpu: 0, ram: 0, tasks: 0 };
        }
    });
}

// Called after packet animation finishes
function updateNodeState(nodeId) {
    if (!window.nodeStats[nodeId]) return;
    
    // Simulate resource usage jump
    window.nodeStats[nodeId].cpu += Math.floor(Math.random() * 15) + 5;
    window.nodeStats[nodeId].ram += Math.floor(Math.random() * 10) + 5;
    window.nodeStats[nodeId].tasks++;
    
    let c = Math.min(100, window.nodeStats[nodeId].cpu);
    let r = Math.min(100, window.nodeStats[nodeId].ram);
    
    let cpuFill = document.getElementById(nodeId.replace('node_', 'cpu_'));
    let ramFill = document.getElementById(nodeId.replace('node_', 'ram_'));
    let cpuPct = document.getElementById(nodeId.replace('node_', 'cpupct_'));
    let ramPct = document.getElementById(nodeId.replace('node_', 'rampct_'));
    let tasksDiv = document.getElementById(nodeId.replace('node_', 'tasks_'));
    let nodeEl = document.getElementById(nodeId);
    
    if(cpuFill) cpuFill.style.width = c + '%';
    if(ramFill) ramFill.style.width = r + '%';
    if(cpuPct) cpuPct.innerText = c + '%';
    if(ramPct) ramPct.innerText = r + '%';
    
    if(tasksDiv && window.nodeStats[nodeId].tasks <= 10) {
        tasksDiv.innerHTML += '<span>📦</span>';
    }
    
    if(nodeEl) {
        nodeEl.classList.remove('status-ready', 'status-busy', 'status-overload');
        if(c > 85 || r > 85) nodeEl.classList.add('status-overload');
        else if(c > 50 || r > 50) nodeEl.classList.add('status-busy');
        else nodeEl.classList.add('status-ready');
    }
}

// Update Carbon UI helper
function updateRegionCarbonUI(reg, val) {
    val = Math.floor(val);
    let indicator = document.getElementById(`carb-${reg}`);
    let container = document.getElementById(`reg-${reg}`);
    let bars = container ? container.querySelectorAll('.reg-stats .progress-fill') : null;
    let cBar = bars && bars.length > 0 ? bars[0] : null;
    
    if(!indicator) return;
    
    let colorClass = 'green';
    let icon = '🟢';
    if (val > 250) { colorClass = 'yellow'; icon = '🟡'; }
    if (val > 400) { colorClass = 'red'; icon = '🔴'; }
    
    indicator.className = `carbon-indicator ${colorClass}`;
    indicator.innerText = `${icon} ${val} gCO₂`;
    
    if (cBar) {
        cBar.className = `progress-fill ${colorClass}`;
        cBar.style.width = Math.min(100, (val / 600) * 100) + '%';
    }
}

// Simulate nodes cooling down over time & Carbon drifting
setInterval(() => {
    // 1. Cool down nodes
    Object.keys(window.nodeStats).forEach(nodeId => {
        if(window.nodeStats[nodeId].cpu > 0) window.nodeStats[nodeId].cpu = Math.max(0, window.nodeStats[nodeId].cpu - 2);
        if(window.nodeStats[nodeId].ram > 0) window.nodeStats[nodeId].ram = Math.max(0, window.nodeStats[nodeId].ram - 1);
        
        let c = Math.min(100, window.nodeStats[nodeId].cpu);
        let r = Math.min(100, window.nodeStats[nodeId].ram);
        
        let cpuFill = document.getElementById(nodeId.replace('node_', 'cpu_'));
        let ramFill = document.getElementById(nodeId.replace('node_', 'ram_'));
        let cpuPct = document.getElementById(nodeId.replace('node_', 'cpupct_'));
        let ramPct = document.getElementById(nodeId.replace('node_', 'rampct_'));
        let nodeEl = document.getElementById(nodeId);
        
        if(cpuFill) cpuFill.style.width = c + '%';
        if(ramFill) ramFill.style.width = r + '%';
        if(cpuPct) cpuPct.innerText = c + '%';
        if(ramPct) ramPct.innerText = r + '%';
        
        if(nodeEl) {
            nodeEl.classList.remove('status-ready', 'status-busy', 'status-overload');
            if(c > 85 || r > 85) nodeEl.classList.add('status-overload');
            else if(c > 50 || r > 50) nodeEl.classList.add('status-busy');
            else nodeEl.classList.add('status-ready');
        }
    });

    // 2. Dynamic Carbon based on regional load
    Object.keys(window.regionCarbon).forEach(reg => {
        let totalCpu = 0;
        for(let i=1; i<=4; i++) {
            let ns = window.nodeStats[`node_${reg}_${i}`];
            if(ns) totalCpu += ns.cpu;
        }
        let avgLoad = totalCpu / 4;
        
        // Carbon fluctuates. If load is high, it drifts upwards.
        let drift = (Math.random() * 8 - 4); 
        if (avgLoad > 60) drift += 3;
        if (avgLoad < 20) drift -= 2;
        
        window.regionCarbon[reg] += drift;
        
        // Enforce rough geographical bounds
        if (reg === 'eu_west') window.regionCarbon[reg] = Math.max(50, Math.min(250, window.regionCarbon[reg]));
        if (reg === 'us_east') window.regionCarbon[reg] = Math.max(200, Math.min(450, window.regionCarbon[reg]));
        if (reg === 'asia_pac') window.regionCarbon[reg] = Math.max(350, Math.min(700, window.regionCarbon[reg]));
        
        updateRegionCarbonUI(reg, window.regionCarbon[reg]);
    });
}, 1000);

function logTrace(message, type = '') {
    const traceLog = document.getElementById('trace-log');
    if(!traceLog) return;
    
    // Clear initial message
    if (traceLog.innerHTML.includes('Waiting for simulation')) {
        traceLog.innerHTML = '';
    }

    const item = document.createElement('div');
    item.className = `trace-item ${type}`;
    item.textContent = message;
    
    traceLog.appendChild(item);
    
    // Auto scroll to bottom
    traceLog.scrollTop = traceLog.scrollHeight;
    
    // Keep max 50 items
    while (traceLog.children.length > 50) {
        traceLog.removeChild(traceLog.firstChild);
    }
}

function animatePacket(decision) {
    // Determine target region and node
    const arena = document.querySelector('.sim-arena');
    const scheduler = document.getElementById('scheduler-node');
    
    let regSuffix = '';
    if (decision.region === 'region_us_east') regSuffix = 'us_east';
    else if (decision.region === 'region_eu_west') regSuffix = 'eu_west';
    else if (decision.region === 'region_asia_pac') regSuffix = 'asia_pac';
    else return;

    // We updated to 4 nodes with underscores: node_eu_west_1
    // The node_id passed in decision is like "node_eu_west_2" (random 1 to 4)
    let finalNodeId = decision.node_id; 
    const targetNode = document.getElementById(finalNodeId);
    
    if (!scheduler || !targetNode || !arena) return;

    // Get positions relative to arena
    const arenaRect = arena.getBoundingClientRect();
    const schedRect = scheduler.getBoundingClientRect();
    const nodeRect = targetNode.getBoundingClientRect();

    const startX = schedRect.left - arenaRect.left + (schedRect.width / 2) - 10;
    const startY = schedRect.bottom - arenaRect.top + 10;
    
    const endX = nodeRect.left - arenaRect.left + (nodeRect.width / 2) - 10;
    const endY = nodeRect.top - arenaRect.top - 10;

    // Create packet
    const packetTypes = ['📦', '🔴', '🔵', '🟠'];
    const pIcon = packetTypes[Math.floor(Math.random() * packetTypes.length)];
    
    const packet = document.createElement('div');
    packet.className = 'packet';
    packet.textContent = pIcon;
    packet.style.left = startX + 'px';
    packet.style.top = startY + 'px';
    
    arena.appendChild(packet);

    // Sequence the animation and trace logging
    setTimeout(() => {
        logTrace(`✓ Task received [${decision.task_id}]`, 'info');
    }, 50);

    setTimeout(() => {
        logTrace(`→ Evaluating ${decision.algorithm}...`);
        logTrace(`→ Carbon scores compared.`, 'eval');
        packet.style.transform = `translate(${endX - startX}px, ${endY - startY}px)`;
        
        // Highlight decision visually
        highlightDecision(decision.region);
        
    }, 150);

    setTimeout(() => {
        logTrace(`✓ Assigned to ${decision.region}`, 'success');
        
        // Update Node Visuals
        updateNodeState(finalNodeId);
        
        // Remove packet
        packet.style.opacity = '0';
        setTimeout(() => {
            if(packet.parentNode) packet.parentNode.removeChild(packet);
        }, 300);
    }, 650); // matches the CSS transition of 0.5s + 0.15s start
}

function highlightDecision(winnerRegion) {
    // Flash all briefly
    document.querySelectorAll('.region-box').forEach(el => {
        el.style.boxShadow = '0 0 15px rgba(255,255,255,0.05)';
        el.style.borderColor = '#475569';
    });
    
    setTimeout(() => {
        document.querySelectorAll('.region-box').forEach(el => {
            el.style.boxShadow = '';
            el.style.borderColor = '';
        });
        const winner = document.getElementById(winnerRegion.replace('region_', 'reg-'));
        if(winner) {
            winner.style.boxShadow = '0 0 20px rgba(52, 211, 153, 0.4)';
            winner.style.borderColor = '#34d399';
            setTimeout(() => {
                winner.style.boxShadow = '';
                winner.style.borderColor = '';
            }, 800);
        }
    }, 200);
}

document.addEventListener("DOMContentLoaded", initializeNodes);

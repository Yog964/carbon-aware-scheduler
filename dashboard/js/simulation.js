// CarbonGrid Simulation Engine
// Handles visual packet movement and Algorithm Tracing

function initNodes() {
    const regions = ['us_east', 'eu_west', 'asia_pac'];
    regions.forEach(reg => {
        const grid = document.getElementById('nodes-' + reg);
        if(!grid) return;
        grid.innerHTML = '';
        for (let i = 1; i <= 6; i++) {
            const node = document.createElement('div');
            node.className = 'node';
            node.id = `node-${reg}-${i}`;
            grid.appendChild(node);
        }
    });
}

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

    // Pick a random node 1-6 in that region
    const nodeIdNum = Math.floor(Math.random() * 6) + 1;
    const targetNode = document.getElementById(`node-${regSuffix}-${nodeIdNum}`);
    
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
    const packet = document.createElement('div');
    packet.className = 'packet';
    packet.textContent = '📦';
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
    }, 150);

    setTimeout(() => {
        logTrace(`✓ Assigned to ${decision.region}`, 'success');
        
        // Flash node
        targetNode.classList.add('active');
        setTimeout(() => targetNode.classList.remove('active'), 500);
        
        // Remove packet
        packet.style.opacity = '0';
        setTimeout(() => {
            if(packet.parentNode) packet.parentNode.removeChild(packet);
        }, 300);
    }, 650); // matches the CSS transition of 0.5s + 0.15s start
}

document.addEventListener("DOMContentLoaded", initNodes);

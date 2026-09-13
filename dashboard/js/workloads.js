function generateMockWorkloads() {
    const tbody = document.getElementById('workloads-tbody');
    if (!tbody) return;
    tbody.innerHTML = '';
    
    const priorities = [
        { label: 'HIGH', class: 'badge-high' },
        { label: 'HIGH', class: 'badge-high' }, // Weighted
        { label: 'MEDIUM', class: 'badge-med' },
        { label: 'LOW', class: 'badge-low' }
    ];
    
    const statuses = [
        { label: 'Running', class: 'badge-running' },
        { label: 'Scheduled', class: 'badge-scheduled' },
        { label: 'Pending', class: 'badge-pending' }
    ];
    
    // Generate 25 dummy tasks to fill the table beautifully
    for(let i = 101; i <= 125; i++) {
        const p = priorities[Math.floor(Math.random() * priorities.length)];
        
        // Logic so earlier tasks are running/scheduled, later are pending
        let s = statuses[0]; 
        if (i > 105) s = statuses[1];
        if (i > 115) s = statuses[2]; 
        
        const cpu = [2, 4, 8, 16][Math.floor(Math.random() * 4)];
        const ram = cpu * 2;
        const deadline = Math.floor(Math.random() * 45) + 5;
        
        const tr = document.createElement('tr');
        tr.innerHTML = `
            <td style="font-family:monospace; font-weight:600;">T-${i}</td>
            <td>${cpu} Cores</td>
            <td>${ram} GB</td>
            <td><span class="badge ${p.class}">${p.label}</span></td>
            <td>${deadline} sec</td>
            <td><span class="badge ${s.class}">${s.label}</span></td>
            <td><a class="action-link" onclick="switchView('simulation')">Locate →</a></td>
        `;
        tbody.appendChild(tr);
    }
}

document.addEventListener('DOMContentLoaded', generateMockWorkloads);

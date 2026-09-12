class CarbonGridSocket {
    constructor(url = 'ws://localhost:9002') {
        this.url = url;
        this.ws = null;
        this.onMessage = null;
        this.reconnectInterval = 3000;
        this.connect();
    }
    
    connect() {
        this.ws = new WebSocket(this.url);
        this.ws.onopen = () => { updateConnectionStatus(true); };
        this.ws.onclose = () => { updateConnectionStatus(false); setTimeout(() => this.connect(), this.reconnectInterval); };
        this.ws.onmessage = (event) => { if (this.onMessage) this.onMessage(JSON.parse(event.data)); };
        this.ws.onerror = (err) => { console.error('WebSocket error:', err); };
    }
    
    send(data) {
        if (this.ws && this.ws.readyState === WebSocket.OPEN) {
            this.ws.send(JSON.stringify(data));
        }
    }
}

function updateConnectionStatus(connected) {
    const el = document.getElementById('connectionStatus');
    el.textContent = connected ? '● Connected' : '● Disconnected';
    el.className = 'connection-status ' + (connected ? 'connected' : 'disconnected');
}

function sendCommand(cmd, value) {
    if (socket) socket.send({ command: cmd, value: value });
}

function changeRate(delta) {
    const el = document.getElementById('currentRate');
    let rate = parseInt(el.textContent) + delta;
    rate = Math.max(1, Math.min(1000, rate));
    el.textContent = rate;
    sendCommand('set_rate', rate);
}

const socket = new CarbonGridSocket();

const connect_btn = document.getElementById("_connect_button");
const disconnect_btn = document.getElementById("_disconnect_button");
const send_btn = document.getElementById("_send_button");
const url_input = document.getElementById("_url_input");
const name_input = document.getElementById("_name_input");
const message_input = document.getElementById("_message_input");
const messages_container = document.getElementById("_messages");

var ws = null;

function addMessage(msg) {
    if (!msg) return;
    messages_container.innerText += msg;
}

connect_btn.onclick = function () {
    const url = url_input.value;
    addMessage(`[connecting...]\n`);

    if (ws) {
        addMessage("[error]: Already connected to a server!\n");
        return;
    }

    if (!url) {
        addMessage("[error]: Please provide a URL!\n");
        return;
    }

    ws = new WebSocket(url);

    ws.onopen = function () {
        addMessage("[connected :)]\n");
    }

    ws.onmessage = function (ev) {
        const msg = ev.data;
        addMessage(msg + "\n");
    }

    ws.onerror = function (err) {
        addMessage("[error]: see the console.\n");
        closeConnection();
        console.error(err);
    }

    ws.onclose = function () {
        addMessage("[disconnected :(]\n");
    }
}

send_btn.onclick = function () {
    if (!ws) {
        addMessage("[error]: Please connect to a server first!\n");
        return;
    }
    const name = name_input.value;
    const message = message_input.value;
    if (!name || !message) {
        addMessage("[error]: Please provide a name and a message!\n");
        return;
    }
    const msg = `${name}: ${message}`;
    ws.send(msg);
    message_input.value = "";
}

function closeConnection() {
    if (ws) ws.close();
    ws = null;
}

disconnect_btn.onclick = function () {
    closeConnection();
}
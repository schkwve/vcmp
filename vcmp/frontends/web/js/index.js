const socket = new WebSocket("ws://localhost:44444");

const chatLog = document.getElementById("chatLog");
const messageInput = document.getElementById("messageInput");
const sendButton = document.getElementById("sendButton");
const modeToggle = document.getElementById("modeToggle");


socket.addEventListener("open", (event) => {
    console.log("Connected!!");
});
socket.addEventListener("message", (event) => {
    console.log("Message from server ", event.data);
});
socket.addEventListener("error", (event) => {
    console.error("WebSocket error observed:", event);
});
socket.addEventListener("close", (event) => {
    console.log("Connection closed:", event);
});

sendButton.addEventListener("click", () => {
    const message = messageInput.value;
    if (message.trim()) {
        chatLog.innerHTML += `<div>${message}</div>`;
        messageInput.value = "";  // Clear the input
        chatLog.scrollTop = chatLog.scrollHeight;  // Scroll to the bottom
    }
});

modeToggle.addEventListener("click", () => {
    document.body.classList.toggle("light-mode");
    
    // Update button text based on mode
    if (document.body.classList.contains("light-mode")) {
        modeToggle.textContent = "Dark Mode";
    } else {
        modeToggle.textContent = "Light Mode";
    }
});

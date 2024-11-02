const socket = new WebSocket("ws://localhost:44444");

const chatLog = document.getElementById("chatLog");
const messageInput = document.getElementById("messageInput");
const sendButton = document.getElementById("sendButton");
const modeToggle = document.getElementById("modeToggle");
const membersList = document.getElementById("membersList");

function addMember(username) {
    const existingMember = document.querySelector(`#membersList li[data-username="${username}"]`);
    
    if (!existingMember) {
        const listItem = document.createElement("li");
        listItem.textContent = username;
        listItem.dataset.username = username;
        membersList.appendChild(listItem);
    }
}

function removeMember(username) {
    const existingMember = document.querySelector(`#membersList li[data-username="${username}"]`);
    
    if (existingMember) {
        existingMember.remove();
    }
}

function parseData(data) {
    try {
        const pdata = JSON.parse(data);  // Parse JSON data
        console.log("Message from server:", data);
        console.log(pdata.username)

        if (!pdata.event) {
            console.error("Data is not found in the Message!");
        }
        
        if (pdata.event == "user_join") {
            addMember(pdata.username);
        } else if (pdata.event == "user_leave") {
            removeMember(pdata.username);
        }

    } catch (error) {
        console.error("Error parsing JSON:", error);
    }
}

socket.addEventListener("open", (event) => {
    addMember("You");
    console.log("Websockets opened");
});
socket.addEventListener("message", (event) => {
    console.log("Message from server ", event.data);
    parseData(event.data)
});
socket.addEventListener("error", (event) => {
    console.error("WebSocket error observed:", event);
});
socket.addEventListener("close", (event) => {
    removeMember("You");
    console.log("Websockets closed");
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


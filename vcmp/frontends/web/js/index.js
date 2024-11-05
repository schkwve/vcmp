const socket = new WebSocket("ws://localhost:44444");

const chatLog = document.getElementById("chatLog");
const messageInput = document.getElementById("messageInput");
const sendButton = document.getElementById("sendButton");
const modeToggle = document.getElementById("modeToggle");
const membersList = document.getElementById("membersList");

let username = "";

function addMember(user_name, fmt_user) {
    const existingMember = document.querySelector(`#membersList li[data-username="${user_name}"]`);
    
    if (!existingMember) {
        const listItem = document.createElement("li");
        listItem.innerHTML = fmt_user.replace("${username}", user_name);;
        listItem.dataset.username = user_name;
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
            addMember(pdata.username, pdata.username);
        } else if (pdata.event == "user_leave") {
            removeMember(pdata.username);
        } else if (pdata.event == "user_message") {
            const message = pdata.message;
            if (message.trim()) {
                chatLog.innerHTML += `<div><b>${pdata.username}</b>: ${message}</div>`;
                messageInput.value = "";  // Clear the input
                chatLog.scrollTop = chatLog.scrollHeight;  // Scroll to the bottom
            }
        }

    } catch (error) {
        console.error("Error parsing JSON:", error);
    }
}

socket.addEventListener("open", (event) => {
    console.log("Websockets opened");
	document.getElementById("messageInput").disabled = false;

    // test
    parseData('{"event": "user_message", "username": "Example", "timestamp": 1730736486, "message": "Hello, world!", "room": "fc3667cb-fe5b-4685-8ef7-f780deaa322e"}');
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
	document.getElementById("messageInput").disabled = true;
});

sendButton.addEventListener("click", () => {
    const message = messageInput.value;
    if (message.trim()) {
        chatLog.innerHTML += `<div><b>${username}</b>: ${message}</div>`;
        messageInput.value = "";  // Clear the input
        chatLog.scrollTop = chatLog.scrollHeight;  // Scroll to the bottom
    }
});

messageInput.addEventListener("keydown", event => {
    if (event.key === "Enter") sendButton.click();
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

// Show the popup on load
window.onload = () => {
    document.getElementById("overlay").style.display = "block";
    document.getElementById("usernamePopup").style.display = "block";
};

// Handle username submission
document.getElementById("submitUsername").addEventListener("click", () => {
    const usernameInput = document.getElementById("usernameInput").value.trim();
    if (usernameInput) {
        username = usernameInput;
        document.getElementById("overlay").style.display = "none";
        document.getElementById("usernamePopup").style.display = "none";
        addMember(username, "<b>${username}</b>");
    }
});

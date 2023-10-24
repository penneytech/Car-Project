const express = require("express");
const http = require("http");
const WebSocket = require("ws");
const path = require("path");

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

const clients = {}; // To store clients by their ID
const lastMessages = {}; // To store the last message from each client

const bonjour = require("bonjour")();

// Serve static files from the 'public' directory
app.use(express.static(path.join(__dirname, 'public')));

wss.on("connection", (ws) => {
  console.log("Client connected");

  ws.on("error", (error) => {
    console.error(`WebSocket Error: ${error}`);
  });

  ws.on("message", (message) => {
    const messageStr = message.toString();

    if (messageStr.startsWith("ID:")) {
      let id = messageStr.split("ID:")[1].trim();
      clients[id] = ws;
      ws.clientID = id;
      console.log(`Registered client with ID: ${id}`);
    } else if (messageStr.startsWith("POS:")) {
      const webClient = clients["WebClient"];
      if (webClient && webClient.readyState === WebSocket.OPEN) {
        webClient.send(messageStr);
      }
    } else {
      if (ws.clientID) {
        console.log(`Message from client ID: ${ws.clientID}, ${messageStr}`);
        lastMessages[ws.clientID] = messageStr;
        const webClient = clients["WebClient"];
        if (webClient && webClient.readyState === WebSocket.OPEN) {
          webClient.send(JSON.stringify({ clientId: ws.clientID, message: messageStr }));
        }
      }
      ws.send("1");
    }
  });

  if (!ws.clientID) {
    ws.clientID = "WebClient";
    clients[ws.clientID] = ws;
  }
});

server.listen(8080, () => {
  console.log("Server started");
  bonjour.publish({ name: "MyWebSocketServer", type: "http", port: 8080 });
});

app.get("/client-data", (req, res) => {
  res.json(lastMessages);
});

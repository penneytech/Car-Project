const express = require("express");
const http = require("http");
const WebSocket = require("ws");

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });
const clients = {}; // To store clients by their ID
const lastMessages = {}; // To store the last message from each client

const bonjour = require("bonjour")();

wss.on("connection", (ws) => {
  console.log("Client connected");

  ws.on("error", (error) => {
    console.error(`WebSocket Error: ${error}`);
  });

  ws.on("message", (message) => {
    const messageStr = message.toString();
    // console.log(`Received message => ${messageStr}`);

    if (messageStr.startsWith("ID:")) {
      let id = messageStr.split("ID:")[1].trim();
      clients[id] = ws;
      ws.clientID = id;
      console.log(`Registered client with ID: ${id}`);
    } else {
      if (ws.clientID) {
        console.log(`Message from client ID: ${ws.clientID}, ${messageStr}`);
        lastMessages[ws.clientID] = messageStr;

        // Send update to WebClient
        const webClient = clients["WebClient"];
        if (webClient && webClient.readyState === WebSocket.OPEN) {
          webClient.send(JSON.stringify({ clientId: ws.clientID, message: messageStr }));
        }
      }
      ws.send("1");
    }
  });

  // If it's a web client without a clientID, assign it "WebClient"
  if (!ws.clientID) {
    ws.clientID = "WebClient";
    clients[ws.clientID] = ws;
  }

  //  ws.send({data: "Welcome to the WebSocket server!"});
});

server.listen(8080, () => {
  console.log("Server started");

  bonjour.publish({ name: "MyWebSocketServer", type: "http", port: 8080 });
});

app.get("/", (req, res) => {
  res.sendFile(__dirname + "/server.html");
});

app.get("/client-data", (req, res) => {
  // Endpoint to fetch data
  res.json(lastMessages);
});

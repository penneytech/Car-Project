const clients = [];

// Initialize the robot data array
const initialRobotData = [
  { x: 5, y: 5, direction: "North", color: "red" },
  { x: 6, y: 6, direction: "South", color: "blue" },
  { x: 7, y: 7, direction: "East", color: "yellow" },
  { x: 8, y: 8, direction: "West", color: "green" },
];

// Call updateGrid and updateRobotInfo with initial data
updateGrid(initialRobotData);
updateRobotInfo(initialRobotData);

function updateGrid(robotData) {
  const grid = document.querySelector(".grid");
  grid.innerHTML = "";

  for (let y = 0; y < 10; y++) {
    for (let x = 0; x < 10; x++) {
      const cell = document.createElement("div");
      cell.classList.add("grid-cell");
      const robot = robotData.find((r) => r.x === x && r.y === y);

      if (robot) {
        const robotElement = document.createElement("div");
        robotElement.classList.add("robot");
        robotElement.classList.add(robot.color.toLowerCase());
        robotElement.classList.add(robot.direction.toLowerCase());

        const carName = document.createElement("div");
        carName.classList.add("car-name");
        carName.textContent = `Robot ${robot.color}`;
        carName.style.transform = "rotate(180deg) scaleX(-1)";
        carName.style.position = "relative";
        carName.style.top = "30px";
        carName.style.left = "-13px";
        carName.style.width = "fit-content";
        carName.style.backgroundColor = "rgba(0,0,0,0.5)";
        carName.style.color = "white";
        carName.style.padding = "2px 5px";
        carName.style.borderRadius = "5px";
        carName.style.whiteSpace = "nowrap";

        cell.appendChild(carName);
        cell.appendChild(robotElement);
      }

      grid.appendChild(cell);
    }
  }
}

const socket = new WebSocket("ws://localhost:8080");

socket.addEventListener("message", (event) => {
  const data = event.data.split(",");
  const clientId = data[0].split(":")[1];
  const x = parseInt(data[1]);
  const y = parseInt(data[2]);
  const direction = data[3]?.trim() || "";  // Use optional chaining and default to empty string if undefined

  let client = clients.find((c) => c.clientId === clientId);

  if (!client) {
    client = { clientId, robotData: {} };
    clients.push(client);
  }

  client.robotData = {
    x,
    y,
    direction,
    color: colors[clients.indexOf(client) % colors.length],
  };

  const updatedRobotData = clients.map((client) => client.robotData);
  updateGrid(updatedRobotData);
  updateRobotInfo(updatedRobotData);
});

socket.addEventListener("open", () => {
  socket.send("get-robot-data");
});

const colors = ["red", "blue", "yellow", "green"];

function updateRobotInfo(robotData) {
  const infoDiv = document.querySelector(".robot-info");
  infoDiv.innerHTML = "";
  robotData.forEach((robot, index) => {
    const infoItem = document.createElement("div");
    infoItem.classList.add("robot-info-item");
    infoItem.id = `robot-${index + 1}-info`;

    const infoContent = `${robot.color} (color: ${robot.color}): 
    <span class="robot-data ${robot.color}-data">${robot.direction} (${robot.x}, ${robot.y})</span>`;
    infoItem.innerHTML = infoContent;

    infoDiv.appendChild(infoItem);
  });
}

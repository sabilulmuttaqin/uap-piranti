const express = require("express");
const cors = require("cors");
const bodyParser = require("body-parser");

const app = express();
app.use(cors());
app.use(bodyParser.json());

let soilMoisture = 0;
let pumpStatus = false;

app.get("/moisture", (req, res) => {
  try {
    res.send(soilMoisture.toString());
  } catch (error) {
    console.error("Error fetching moisture data:", error);
    res.status(500).send("Error fetching moisture data");
  }
});

app.post("/moisture", (req, res) => {
  try {
    soilMoisture = req.body.moisture;
    res.send("Moisture data received");
  } catch (error) {
    console.error("Error receiving moisture data:", error);
    res.status(500).send("Error receiving moisture data");
  }
});

app.get("/pump/status", (req, res) => {
  try {
    res.send(pumpStatus ? "ON" : "OFF");
  } catch (error) {
    console.error("Error fetching pump status:", error);
    res.status(500).send("Error fetching pump status");
  }
});

app.get("/pump/on", (req, res) => {
  try {
    pumpStatus = true;
    res.send("Pump ON");
  } catch (error) {
    console.error("Error turning on the pump:", error);
    res.status(500).send("Error turning on the pump");
  }
});

app.get("/pump/off", (req, res) => {
  try {
    pumpStatus = false;
    res.send("Pump OFF");
  } catch (error) {
    console.error("Error turning off the pump:", error);
    res.status(500).send("Error turning off the pump");
  }
});

const PORT = 3000;
app.listen(PORT, () => {
  console.log(`Server running at http://localhost:${PORT}`);
});

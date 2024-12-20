const express = require("express");
const app = express();
const fetch = require("node-fetch");

app.use(express.json());

app.get("/proxy/:endpoint", async (req, res) => {
  const endpoint = req.params.endpoint;
  const url = `http://192.168.244.26/${endpoint}`;
  try {
    const response = await fetch(url);
    const data = await response.text();
    res.send(data);
  } catch (error) {
    res.status(500).send("Error fetching data from ESP32.");
  }
});

app.listen(3000, () => console.log("Proxy server running on port 3000"));

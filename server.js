const express = require("express");
const cors = require("cors");
const path = require("path");

const app = express();
app.use(cors()); // Mengizinkan CORS untuk frontend

// Simulasi data kelembapan tanah
let soilMoisture = 0;

// Simulasi status pompa (true untuk ON, false untuk OFF)
let pumpStatus = false;

// Endpoint untuk root
app.get("/", (req, res) => {
  res.send("Server is running! Use /moisture, /pump/on, or /pump/off");
});

// Endpoint untuk mendapatkan data kelembapan tanah
app.get("/moisture", (req, res) => {
  // Data simulasi kelembapan tanah (dapat dihubungkan dengan ESP32 melalui API sebenarnya)
  soilMoisture = Math.floor(Math.random() * 101); // Simulasi nilai 0-100%
  res.send(soilMoisture.toString());
});

// Endpoint untuk menyalakan pompa
app.get("/pump/on", (req, res) => {
  pumpStatus = true; // Menyalakan pompa
  res.send("Pump ON");
});

// Endpoint untuk mematikan pompa
app.get("/pump/off", (req, res) => {
  pumpStatus = false; // Mematikan pompa
  res.send("Pump OFF");
});

// Menjalankan server di port 3000
const PORT = 3000;
app.listen(PORT, () => {
  console.log(`Server running at http://localhost:${PORT}`);
});

const express = require("express");
const cors = require("cors");
const path = require("path");

const app = express();
app.use(cors()); // Mengizinkan CORS untuk frontend

// Simulasi data kelembapan tanah
let soilMoisture = 0;

// Simulasi status pompa (true untuk ON, false untuk OFF)
let pumpStatus = false;

// Simulasi status auto mode (true untuk mode otomatis, false untuk mode manual)
let autoMode = true;

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
  autoMode = false; // Beralih ke mode manual
  res.send("Pump ON in Manual Mode");
});

// Endpoint untuk mematikan pompa
app.get("/pump/off", (req, res) => {
  pumpStatus = false; // Mematikan pompa
  autoMode = false; // Beralih ke mode manual
  res.send("Pump OFF in Manual Mode");
});

// Endpoint untuk mendapatkan status pompa
app.get("/pump/status", (req, res) => {
  const status = pumpStatus ? "ON" : "OFF";
  res.send(`Pump is ${status}`);
});

// Endpoint untuk mendapatkan status auto mode
app.get("/pump/auto/status", (req, res) => {
  const mode = autoMode ? "Automatic" : "Manual";
  res.send(`Mode is ${mode}`);
});

// Endpoint untuk mengaktifkan auto mode
app.get("/pump/auto/on", (req, res) => {
  autoMode = true; // Aktifkan mode otomatis
  res.send("Auto mode is ON");
});

// Endpoint untuk menonaktifkan auto mode
app.get("/pump/auto/off", (req, res) => {
  autoMode = false; // Nonaktifkan mode otomatis
  res.send("Auto mode is OFF");
});

// Menjalankan server di port 3000
const PORT = 3000;
app.listen(PORT, () => {
  console.log(`Server running at http://localhost:${PORT}`);
});

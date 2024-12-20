// Fungsi untuk mendapatkan nilai kelembapan tanah
function updateMoisture() {
  fetch("http://192.168.244.107/moisture") // IP ESP32, ganti dengan IP ESP32 Anda
    .then((response) => response.text())
    .then((data) => {
      document.getElementById("moisture").innerText = data + "%";
    })
    .catch((error) => {
      console.error("Error fetching moisture data:", error);
      document.getElementById("moisture").innerText = "Error retrieving data";
    });
}

// Fungsi untuk menghidupkan atau mematikan pompa
function togglePump(action) {
  const url = action === "on" ? "/pump/on" : "/pump/off";
  fetch("http://192.168.244.107" + url) // IP ESP32, ganti dengan IP ESP32 Anda
    .then((response) => response.text())
    .then((data) => {
      alert(action === "on" ? "Pump ON" : "Pump OFF"); // Status pompa
      updatePumpStatus(); // Menampilkan status pompa terbaru
    })
    .catch((error) => {
      console.error("Error toggling pump:", error);
      alert("Failed to toggle pump. Check connection.");
    });
}

// Fungsi untuk mendapatkan status pompa
function updatePumpStatus() {
  fetch("http://192.168.244.107/pump/status") // IP ESP32, ganti dengan IP ESP32 Anda
    .then((response) => response.text())
    .then((data) => {
      document.getElementById("pumpStatus").innerText = "Pump Status: " + data;
    })
    .catch((error) => {
      console.error("Error fetching pump status:", error);
      document.getElementById("pumpStatus").innerText =
        "Error retrieving pump status";
    });
}

// Panggil fungsi ini saat halaman selesai dimuat
window.onload = function () {
  updateMoisture();
  updatePumpStatus();
  setInterval(updateMoisture, 2000); // Perbarui setiap 2 detik
  setInterval(updatePumpStatus, 2000); // Perbarui setiap 2 detik
};

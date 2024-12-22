// Initialize Chart.js
const ctx = document.getElementById("moistureChart").getContext("2d");
const moistureData = {
  labels: [],
  datasets: [
    {
      label: "Soil Moisture (%)",
      data: [],
      borderColor: "rgb(59, 130, 246)",
      backgroundColor: "rgba(59, 130, 246, 0.1)",
      fill: true,
      tension: 0.4,
    },
  ],
};

const moistureChart = new Chart(ctx, {
  type: "line",
  data: moistureData,
  options: {
    responsive: true,
    maintainAspectRatio: false,
    scales: {
      y: {
        beginAtZero: true,
        max: 100,
      },
    },
    plugins: {
      legend: {
        display: false,
      },
    },
  },
});

// Function to update moisture with chart
function updateMoisture() {
  fetch("http://192.168.254.26/moisture")
    .then((response) => {
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      return response.text();
    })
    .then((data) => {
      const moistureValue = parseInt(data, 10);
      if (!isNaN(moistureValue)) {
        document.getElementById("moisture").innerText = moistureValue + "%";

        // Update chart
        const now = new Date();
        const timeLabel = now.toLocaleTimeString();

        moistureData.labels.push(timeLabel);
        moistureData.datasets[0].data.push(moistureValue);

        // Keep only last 10 data points
        if (moistureData.labels.length > 10) {
          moistureData.labels.shift();
          moistureData.datasets[0].data.shift();
        }

        moistureChart.update("none"); // Menambahkan 'none' untuk menghindari animasi
      } else {
        document.getElementById("moisture").innerText = "Invalid Data";
      }
    })
    .catch((error) => {
      console.error("Error fetching moisture data:", error);
      document.getElementById("moisture").innerText = "Error";
    });
}
// Function to show custom notification
function showNotification(message, duration = 3000) {
  const notification = document.getElementById("notification");
  const notificationMessage = document.getElementById("notificationMessage");

  // Set message
  notificationMessage.innerText = message;

  // Show notification
  notification.classList.remove("hidden");
  notification.classList.add("translate-y-0");

  // Hide after duration
  setTimeout(() => {
    notification.classList.add("hidden");
    notification.classList.remove("translate-y-0");
  }, duration);
}

// Existing pump control functions
function togglePump(action) {
  const url = action === "on" ? "/pump/on" : "/pump/off";
  fetch("http://192.168.254.26" + url)
    .then((response) => response.text())
    .then((data) => {
      showNotification(data);
      updatePumpStatus();
    })
    .catch((error) => {
      console.error("Error toggling pump:", error);
      alert("Failed to toggle pump. Check connection.");
    });
}

function updatePumpStatus() {
  fetch("http://192.168.254.26/pump/status")
    .then((response) => response.text())
    .then((status) => {
      document.getElementById("pumpStatus").innerText = "Pump is " + status;
    })
    .catch((error) => {
      console.error("Error fetching pump status:", error);
      document.getElementById("pumpStatus").innerText = "Error";
    });
}

function toggleAutoMode(action) {
  const url = action === "on" ? "/pump/auto/on" : "/pump/auto/off";
  fetch("http://192.168.254.26" + url)
    .then((response) => response.text())
    .then((data) => {
      showNotification(data);
      updateAutoModeStatus();
    })
    .catch((error) => {
      console.error("Error toggling auto mode:", error);
      alert("Failed to toggle auto mode. Check connection.");
    });
}

function updateAutoModeStatus() {
  fetch("http://192.168.254.26/pump/auto/status")
    .then((response) => response.text())
    .then((mode) => {
      document.getElementById("autoModeStatus").innerText = "Mode: " + mode;
    })
    .catch((error) => {
      console.error("Error fetching auto mode status:", error);
      document.getElementById("autoModeStatus").innerText = "Error";
    });
}

// Update intervals
setInterval(updateMoisture, 1000);
setInterval(updatePumpStatus, 1000);
setInterval(updateAutoModeStatus, 1000);

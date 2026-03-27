/**
 * @file ebikeClient.cpp
 * @brief Main application for the eBike on-board computer (Worksheet 1).
 *
 * Connects a simulated GPS sensor to the Hardware Abstraction Layer (HAL),
 * reads GPS data, and prints formatted readings to stdout.
 *
 * Usage: ./ebikeClient <client_ip> <ebike_id> <csv_file> <num_ports>
 */

#include "hal/CSVHALManager.h"
#include "sensor/GPSSensor.h"
#include "util/MiscUtils.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

// --- Constants ---
namespace ebikeConstants {
/** Path to the client YAML configuration file. */
const std::string CONFIG_PATH = "config/client-config.yaml";
} // namespace ebikeConstants

int main(int argc, char *argv[]) {
  // Require exactly 4 arguments after the program name
  if (argc < 5) {
    std::cerr << "Usage: " << argv[0]
              << " <client_ip> <ebike_id> <csv_file> <num_ports>" << std::endl;
    return 1;
  }

  //***IMPORTANT: DO NOT CHANGE THE ORDER, QUANTITY OR TYPE OF THE COMMAND LINE
  // ARGUMENTS*/

  std::string clientIp =
      argv[1]; // IP address of this eBike client (used in WS2)
  std::string ebikeId = argv[2]; // Unique eBike identifier (used in WS2)
  std::string csvFile =
      argv[3]; // CSV file containing simulated sensor readings
  int numPorts = std::atoi(argv[4]); // Number of HAL ports to initialise

  // Initialise the CSV-backed Hardware Abstraction Layer
  CSVHALManager halManager(numPorts);
  halManager.initialise(csvFile);

  // Read per-sensor configuration from YAML
  int maxReadings = std::stoi(
      readConfigValue(ebikeConstants::CONFIG_PATH, "client", "max_readings"));
  int gpsSensorId = std::stoi(
      readConfigValue(ebikeConstants::CONFIG_PATH, "client", "gps_sensor_id"));
  int gpsHalPort = std::stoi(
      readConfigValue(ebikeConstants::CONFIG_PATH, "client", "gps_hal_port"));

  // Create the GPS sensor (id, dimension=2 for lat/lon) and attach it to the
  // HAL
  auto gpsSensor = std::make_shared<GPSSensor>(gpsSensorId, 2);
  halManager.attachDevice(gpsHalPort, gpsSensor);

  // Read and print up to maxReadings GPS observations
  for (int i = 0; i < maxReadings; i++) {
    auto reading = halManager.read(gpsHalPort);
    if (!reading.empty()) {
      // Raw data format from CSV: "lat;lon"
      std::string rawData(reading.begin(), reading.end());
      size_t pos = rawData.find(';');
      std::string latStr = rawData.substr(0, pos);
      std::string lonStr = rawData.substr(pos + 1);

      // Print in the expected format:
      // [EBCLIENT] <timestamp> gps: {"lat":<lat>,"lon":<lon>}(unlocked)
      std::cout << "[EBCLIENT] " << getFormattedTime()
                << " gps: {\"lat\":" << latStr << ",\"lon\":" << lonStr
                << "}(unlocked)" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }

  halManager.releaseDevice(gpsHalPort);
  std::cout << "[EBCLIENT] Shutting down." << std::endl;
  return 0;
}
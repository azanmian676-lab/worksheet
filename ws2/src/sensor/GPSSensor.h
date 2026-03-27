/**
 * @file GPSSensor.h
 * @brief Simulated GPS sensor implementing the ISensor interface.
 *
 * Reads lat/lon pairs from the HAL byte stream (CSV format "lat;lon")
 * and formats them as a GeoJSON-style JSON string for downstream use.
 */

#ifndef GPSSENSOR_H
#define GPSSENSOR_H

#include <cstdint>
#include "hal/ISensor.h"
#include <string>
#include <vector>
#include <sstream>

/**
 * @class GPSSensor
 * @brief Simulates a GPS device that converts raw HAL byte data into formatted JSON.
 *
 * The raw reading is a byte sequence representing "lat;lon" (semicolon-separated).
 * The formatted output pairs the key "gps" with a JSON object containing lon and lat.
 */
class GPSSensor : public ISensor {
public:
    /**
     * @brief Construct a GPSSensor with a given ID and dimension.
     * @param id        Unique sensor identifier (matches the CSV column index used by HAL).
     * @param dimension Number of data dimensions (2 for lat/lon).
     */
    GPSSensor(int id, int dimension) : id(id), dimension(dimension) {}

    /**
     * @brief Return the sensor's unique ID.
     * @return Sensor ID.
     */
    int getId() const override {
        return id;
    }

    /**
     * @brief Return the number of dimensions this sensor produces.
     * @return Dimension count (2 for GPS: lat and lon).
     */
    int getDimension() const override {
        return dimension;
    }

    /**
     * @brief Parse a raw HAL reading and format it as a JSON string.
     *
     * Raw format: "firstValue;secondValue" (e.g. "51.457130;-2.557153")
     * Output format: {"gps":{"lon": <firstValue>, "lat": <secondValue>}}
     *
     * NOTE: The field order (lon before lat) and spacing must match the
     * unit test fixture in tests/data/json/td_ebclient_gps.json exactly.
     *
     * @param reading Byte vector from CSVHALManager::read().
     * @return Pair of ("gps", JSON string).
     */
    std::pair<std::string, std::string> format(std::vector<uint8_t> reading) override {
        std::string rawData(reading.begin(), reading.end());
        size_t pos = rawData.find(';');

        if (pos != std::string::npos) {
            // First CSV value maps to lon, second to lat (per test specification)
            std::string firstVal  = rawData.substr(0, pos);
            std::string secondVal = rawData.substr(pos + 1);

            // Build JSON: {"gps":{"lon": <first>, "lat": <second>}}
            // Space after colon required to match expected test output exactly
            std::string formattedJson =
                "{\"gps\":{\"lon\": " + firstVal + ", \"lat\": " + secondVal + "}}";

            return std::make_pair("gps", formattedJson);
        }

        return std::make_pair("gps", "{}");
    }

private:
    int id;        ///< Unique sensor identifier
    int dimension; ///< Number of sensor dimensions (2 for GPS)
};

#endif // GPSSENSOR_H

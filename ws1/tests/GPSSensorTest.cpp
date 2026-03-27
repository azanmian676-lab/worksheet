#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "TestUtils.h"
#include "sensor/GPSSensor.h"

#include <memory>
#include <iostream>
#include <vector>
#include <string>

std::vector<TestUtils::TestCase> testCases = TestUtils::getTestCasesForSuite("tests/config/unit_tests.yaml", "GPSSensorTest" ,"tests/data/json/");

TEST_CASE("Test GPSSensor is built correctly (id and dimensions)", "[GPSSensor]") {
    std::string testName = Catch::getResultCapture().getCurrentTestName();
    TestUtils::TestCaseData testData = TestUtils::getTestCaseDataByName(testCases, testName);

    int gps_id = TestUtils::getInt(testData.at("gps_id"));
    int gps_dim = TestUtils::getInt(testData.at("gps_dim"));

    GPSSensor sensor(gps_id, gps_dim);

    REQUIRE(sensor.getId() == gps_id);
    REQUIRE(sensor.getDimension() == gps_dim);
}

TEST_CASE("Test GPSSensor formats correctly returning a valid pair of latitude and longitude in JSON format", "[GPSSensor]") {
    std::string testName = Catch::getResultCapture().getCurrentTestName();
    TestUtils::TestCaseData testData = TestUtils::getTestCaseDataByName(testCases, testName);

    int gps_id = TestUtils::getInt(testData.at("gps_id"));
    int gps_dim = TestUtils::getInt(testData.at("gps_dim"));
    std::string gps_raw_reading = TestUtils::getString(testData.at("gps_raw_reading"));
    std::string expected_formatted = TestUtils::getString(testData.at("gps_formatted_reading"));

    GPSSensor sensor(gps_id, gps_dim);

    std::vector<uint8_t> rawVec(gps_raw_reading.begin(), gps_raw_reading.end());
    auto formatted = sensor.format(rawVec);

    REQUIRE(formatted.first == "gps");
    // Depending on whitespaces in expected format, let's just make sure it serializes accurately.
    // The test framework may do exact string comparison:
    REQUIRE(formatted.second == expected_formatted);
}

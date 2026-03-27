#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "TestUtils.h"
#include "hal/CSVHALManager.h"

#include <memory>
#include <iostream>
#include <vector>
#include <string>

std::vector<TestUtils::TestCase> testCases = TestUtils::getTestCasesForSuite("tests/config/unit_tests.yaml", "ebikeClientTest", "tests/data/json/");

TEST_CASE("Test HALManager is initialised and manages ports (attach and release) correctly", "[ebikeClient]") {
    std::string testName = Catch::getResultCapture().getCurrentTestName();
    TestUtils::TestCaseData testData = TestUtils::getTestCaseDataByName(testCases, testName);

    int hal_ports = TestUtils::getInt(testData.at("hal_ports"));
    std::string csv_file = TestUtils::getString(testData.at("csv_file"));
    int port_attach = TestUtils::getInt(testData.at("port_attach"));

    CSVHALManager halManager(hal_ports);
    halManager.initialise(csv_file);

    // Create a mock device to attach
    class MockDevice : public IDevice {
    public:
        int getId() const override { return 999; }
    };

    auto device = std::make_shared<MockDevice>();
    
    REQUIRE_FALSE(halManager.isBusy(port_attach));
    halManager.attachDevice(port_attach, device);
    REQUIRE(halManager.isBusy(port_attach));
    
    halManager.releaseDevice(port_attach);
    REQUIRE_FALSE(halManager.isBusy(port_attach));
}

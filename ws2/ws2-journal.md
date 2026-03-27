# UFCFVK-15-2 Internet of Things  - Worksheet 2 Journal

## 2026-03-23

* **What you worked on:** Initialised the `SocketServer` header leveraging `Poco::Net::DatagramSocket`. Built the initial message handling class `MessageHandler` utilising `Poco::JSON::Parser` to deserialise incoming byte payloads and map them into GeoJSON Feature models.
* **Issues faced and solutions:** Dealing with concurrent networking blocking the main thread. `Poco::Thread` was used instead of blocking the main event loop, and `while(true)` loops were used safely on background processes. Added exception catching to avoid terminating the gateway on bad packets. Referenced the [POCO DatagramSocket documentation](https://pocoproject.org/docs/Poco.Net.DatagramSocket.html) for bind/receiveFrom patterns.
* **What I learned:** Working with `Poco::Dynamic::Var` → `Poco::JSON::Object::Ptr` provides memory-safe and intuitive JSON conversions in C++.
* **Reflection:** Initially I tried a single-threaded approach for both the socket server and web server, which caused deadlocks. Switching to `Poco::Thread`-based concurrency from the start resolved this. In future I would plan the threading model before writing any socket code.

## 2026-03-25

* **What you worked on:** Developed the client-side `ebikeClient` to send `JOIN`, parse `JACK`, and periodically emit `DATA` payloads formatted correctly over UDP to the central gateway.
* **Issues faced and solutions:** A blocking socket in the client prevented sensor emulation loops if no config message arrived in time. Used `clientSock.setBlocking(false)` and short timeouts `clientSock.setReceiveTimeout()` allowing fallbacks. Also had to ensure the `COMMAND` message forwarded by the gateway correctly matched the `"action"` key the client expected, rather than the raw management JSON structure.
* **What I learned:** Networking in IoT nodes must remain asynchronous or non-blocking so the embedded system can continue hardware readings while awaiting incoming network events.

## 2026-03-27

* **What you worked on:** Created the `managementClient` using `std::ifstream` and `std::stringstream` to push static `.json` configuration files up to the gateway management port 8085. Also implemented the eBike client address registry in `MessageHandler` so COMMAND directives can be forwarded to the correct eBike.
* **Issues faced and solutions:** The initial management client JSON files used a `"directive"` key while the gateway expected `"type"`. Aligned the test data files to use `"type"` to match the protocol specification. Also removed newline artefacts from `std::string` parsing before transmission to avoid garbled log output.
* **What I learned:** Protocol consistency between producers and consumers is critical. A single field name mismatch (`"directive"` vs `"type"`) silently breaks entire message routing chains. Defining the protocol schema first and validating it in both sender and receiver is essential.
* **Reflection:** This worksheet reinforced applying event-driven design from worksheet 1. Next time I would define the message schemas in a shared header file to prevent key-name drift between management client, gateway, and eBike client.
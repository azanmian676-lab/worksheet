# UFCFVK-15-2 Internet of Things - Worksheet 1 Journal

## 2026-03-22

* **What you worked on:** Initial understanding of the HAL architecture, reading how `CSVHALManager` controls `IDevice` instances, and defining the `GPSSensor` class according to the `ISensor` specifications.
* **Issues faced and solutions:** A little challenge understanding how the `format` function reads `std::vector<uint8_t>` byte stream to string data. Investigated the ASCII standard conversion and used `std::string` range iterators to seamlessly transform memory blocks into text. Referenced [cppreference std::string constructor](https://en.cppreference.com/w/cpp/string/basic_string/basic_string) to confirm the iterator-range constructor approach.
* **What I learned:** IoT embedded constraints force developers to handle byte data safely instead of higher-level types to conserve memory.
* **Reflection:** My initial approach was to immediately start writing code, which led to confusion around the HAL interface contracts. Next time I would read all interface headers (ISensor, IDevice) first and sketch the design on paper before touching the implementation, making the work more structured from the start.

## 2026-03-24

* **What you worked on:** Updating `ebikeClient` to fetch readings via the HAL framework and ensuring all loops effectively output JSON in the correct format matching expected test output.
* **Issues faced and solutions:** Getting the exact JSON schema to match the unit tests — specifically the field ordering (`lon` before `lat` in `format()`) and spacing conventions. Double-checked the test fixture in `td_ebclient_gps.json` and adjusted the string concatenation accordingly. Initially tried using POCO JSON serialisation but standard string concatenation proved simpler for our small payload without adding dependency complexity.
* **What I learned:** The importance of correctly structuring data representations to match consumer expectations exactly, rather than assuming a "standard" ordering.

## 2026-03-26

* **What you worked on:** Completing the unit testing structure utilising Catch2 to validate `GPSSensor` features and `ebikeClient` HAL attachment/release behaviour.
* **Issues faced and solutions:** Creating stub devices to ensure manager ports behave appropriately. Investigated Mock objects in C++ and devised a custom mock deriving `IDevice` inside the test case. Also debugged the `argc` check (needed `< 5` not `< 4` since 4 arguments are used). Consulted the Catch2 documentation at [https://github.com/catchorg/Catch2](https://github.com/catchorg/Catch2) for the `REQUIRE_FALSE` macro.
* **What I learned:** Good unit testing isolates independent components correctly instead of relying on external system states. Designing tests first clarifies what the implementation needs to provide.
* **Reflection:** Throughout Worksheet 1, I noticed my approach shifted from trial-and-error to reading the interface contracts formally and designing top-down. Next time I would outline the specific methods needed to fulfil interfaces before trying to write any implementation code, saving significant debugging time.
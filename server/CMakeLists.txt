cmake_minimum_required(VERSION 3.5)
project(server)

find_package(nlohmann_json CONFIG REQUIRED)

include_directories(
    include

)

add_executable(
    server
    src/main.cpp
    src/tcp_server.cpp
    src/utils/packing.cpp
    src/utils/person.cpp
    src/utils/utility.cpp
)

target_link_libraries(
    server
    nlohmann_json nlohmann_json::nlohmann_json
)

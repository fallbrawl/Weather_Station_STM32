cmake_minimum_required(VERSION 3.16)
project(qt_hello_world VERSION 0.1 LANGUAGES CXX)

# --- ADD THESE TWO LINES ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON) 
# ---------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure CMake knows where Qt is (if not already set in VS Code)
set(CMAKE_PREFIX_PATH "/Users/paulshevsky/Qt/6.11.0/macos")

find_package(Qt6 REQUIRED COMPONENTS Gui Quick Network Graphs)

qt_add_executable(appqt_hello_world
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/weatherclient.cpp
)

qt_add_qml_module(appqt_hello_world
    URI MainModule             # Use a letter here to keep Qt happy
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"  # This is the secret sauce
    NO_RESOURCE_TARGET_PATH    # This tells Qt: "Don't append the URI to the path"
    QML_FILES main.qml
)

target_link_libraries(appqt_hello_world PRIVATE Qt6::Gui Qt6::Quick Qt6::Network 
    Qt6::Graphs)
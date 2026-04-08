cmake_minimum_required(VERSION 3.16)
project(qt_ui VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON) 

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Gui Quick Network Graphs)

qt_add_executable(appqt_ui
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/weatherclient.cpp
)

qt_add_qml_module(appqt_ui
    URI MainModule             
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"  
    NO_RESOURCE_TARGET_PATH    
    QML_FILES main.qml
)

target_link_libraries(appqt_ui PRIVATE Qt6::Gui Qt6::Quick Qt6::Network 
    Qt6::Graphs)
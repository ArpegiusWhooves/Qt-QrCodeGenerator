cmake_minimum_required(VERSION 3.23)

project(QrCodeGenerator LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui)

qt_standard_project_setup()


qt_add_library(QrCodeGenerator STATIC
    src/QrCodeGenerator.cpp
    src/qrcodegen/qrcodegen.cpp
)

target_include_directories(QrCodeGenerator INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/)
target_link_libraries(QrCodeGenerator PRIVATE Qt6::Core Qt6::Gui)

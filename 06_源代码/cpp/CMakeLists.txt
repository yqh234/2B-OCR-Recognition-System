cmake_minimum_required(VERSION 3.16)
project(ocr_reader_2b)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

pkg_check_modules(TESSERACT REQUIRED tesseract lept)
pkg_check_modules(ZBAR REQUIRED zbar)

add_library(ocr_core
    src/ocr_core.cpp
)

target_include_directories(ocr_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
    ${TESSERACT_INCLUDE_DIRS}
)

target_link_libraries(ocr_core PUBLIC
    ${OpenCV_LIBS}
    ${TESSERACT_LIBRARIES}
)

add_executable(ocr_cli
    src/cli_main.cpp
)
target_link_libraries(ocr_cli PRIVATE ocr_core)

add_executable(ocr_qt
    src/qt_main.cpp
)
target_link_libraries(ocr_qt PRIVATE ocr_core Qt6::Widgets)

add_executable(ocr_core_test
    tests/test_core.cpp
)
target_link_libraries(ocr_core_test PRIVATE ocr_core)

add_executable(comprehensive_demo
    src/comprehensive_demo.cpp
)
target_include_directories(comprehensive_demo PRIVATE
    ${ZBAR_INCLUDE_DIRS}
)
target_link_libraries(comprehensive_demo PRIVATE
    ocr_core
    ${ZBAR_LIBRARIES}
)

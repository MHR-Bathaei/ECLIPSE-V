cmake_minimum_required(VERSION 3.15)
project(ECLIPSE_V VERSION 1.0)

# Enforce Modern C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set dependency paths
set(ONNXRUNTIME_DIR "${CMAKE_SOURCE_DIR}/../deps/onnxruntime")
set(STB_DIR "${CMAKE_SOURCE_DIR}/../deps/stb")

# 1. Include the STB Header directory
include_directories("${STB_DIR}")

# 2. Setup ONNX Runtime
add_library(onnxruntime SHARED IMPORTED)
set_target_properties(onnxruntime PROPERTIES
    IMPORTED_LOCATION "${ONNXRUNTIME_DIR}/lib/onnxruntime.dll"
    IMPORTED_IMPLIB "${ONNXRUNTIME_DIR}/lib/onnxruntime.lib"
)
include_directories("${ONNXRUNTIME_DIR}/include")

# 3. Build the Engine
add_executable(eclipse_engine main.cpp)

# 4. Link ONNX Runtime (STB is header-only, no linking required!)
target_link_libraries(eclipse_engine PRIVATE onnxruntime)

# 5. Auto-copy DLLs to the build folder so the .exe runs cleanly
add_custom_command(TARGET eclipse_engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${ONNXRUNTIME_DIR}/lib/onnxruntime.dll"
    $<TARGET_FILE_DIR:eclipse_engine>
)
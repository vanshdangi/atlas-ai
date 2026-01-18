cmake_minimum_required(VERSION 3.26)
project(llama_app LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force static libraries
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Enable CUDA for llama.cpp
set(GGML_CUDA ON CACHE BOOL "" FORCE)

# Add llama.cpp as a subproject
add_subdirectory(external/llama.cpp)

# Your executable
add_executable(llama_app
    core/main.cpp
)

# Link against llama
target_link_libraries(llama_app PRIVATE llama)

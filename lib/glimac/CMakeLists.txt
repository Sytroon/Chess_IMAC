cmake_minimum_required(VERSION 3.10)

add_library(glimac)

file(GLOB_RECURSE GLIMAC_SOURCES CONFIGURE_DEPENDS src/*)
if (MSVC)
target_compile_options(glimac PRIVATE /W3)
else()
target_compile_options(glimac PRIVATE -std=c++17)
endif()
target_sources(glimac PRIVATE ${GLIMAC_SOURCES})
target_include_directories(glimac PUBLIC ../glimac)
target_include_directories(glimac PUBLIC ../quick_imgui/lib/glfw/include)
target_include_directories(glimac PUBLIC ../quick_imgui/lib/glad/include)

# ---Add GLFW---
# GLFW is already added by quick_imgui
# ---Add glad---
# glad is already added by quick_imgui
# ---Add glm---
add_subdirectory(third-party/glm)
target_link_libraries(glimac PUBLIC glm)
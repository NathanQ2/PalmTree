cmake_minimum_required(VERSION 3.31)
project(PalmTree)

set(CMAKE_CXX_STANDARD 23)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

file(GLOB_RECURSE SOURCES src/**.cpp src/**.h)

# ImGui
file(GLOB IMGUI_SOURCES 
    vendor/imgui/*.cpp 
    vendor/imgui/*.h
    vendor/imgui/backends/imgui_impl_glfw.cpp
    vendor/imgui/backends/imgui_impl_glfw.h
    vendor/imgui/backends/imgui_impl_vulkan.cpp
    vendor/imgui/backends/imgui_impl_vulkan.h
)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(PalmTree SHARED ${SOURCES} ${IMGUI_SOURCES})

target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} Vulkan::Vulkan glm::glm)

target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
target_include_directories(${PROJECT_NAME} PRIVATE vendor/tinyobjloader)
target_include_directories(${PROJECT_NAME} PRIVATE IMGUI_INCLUDE)
target_include_directories(${PROJECT_NAME} PUBLIC vendor/spdlog/include)
target_include_directories(${PROJECT_NAME} PUBLIC vendor/imgui/ vendor/imgui/backends)

target_compile_definitions(PalmTree PRIVATE $<$<CONFIG:Debug>:PT_ENABLE_ASSERTS>)
target_compile_definitions(PalmTree PUBLIC $<$<CONFIG:Debug>:PT_DEBUG>)
target_compile_definitions(PalmTree PUBLIC $<$<CONFIG:Release>:PT_RELEASE>)
target_compile_definitions(PalmTree PUBLIC VK_ENABLE_BETA_EXTENSIONS)

target_precompile_headers(PalmTree PUBLIC src/ptpch.h)

file(GLOB shaders shaders/*.vert shaders/*.frag)

foreach (SHADER IN LISTS shaders)
    get_filename_component(FILENAME ${SHADER} NAME)
    add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.spv
            COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} -o ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.spv
            DEPENDS ${SHADER}
            COMMENT "Compiling Shader: ${FILENAME}"
    )
    list(APPEND SPV_SHADERS ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.spv)
endforeach ()

add_custom_target(CompileShaders ALL DEPENDS ${SPV_SHADERS})
add_dependencies(${PROJECT_NAME} CompileShaders)

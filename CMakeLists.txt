cmake_minimum_required(VERSION 3.16)
project(chip-8)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(vendor/SDL3 EXCLUDE_FROM_ALL)

set(MACOSX_BUNDLE_ICON_FILE icon.icns)
set(APP_RESOURCES icon.icns)
set_source_files_properties(${APP_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

add_executable(${PROJECT_NAME} MACOSX_BUNDLE cpu.c instructions.c decode.c chip-8.c ${APP_RESOURCES})

target_sources(${PROJECT_NAME} PUBLIC include/cpu.h PUBLIC include/instructions.h PUBLIC include/decode.h)

#PRIVATE -Wextra
target_compile_options(${PROJECT_NAME} PRIVATE -Wall PRIVATE -Werror)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
cmake_minimum_required(VERSION 3.16)
project(chip-8 LANGUAGES C)

include(FetchContent)
set(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor")

if(APPLE)
    enable_language(Swift)
    add_library(file_dialog SHARED
        src/platform/file_dialog_macos.swift
    )

    find_library(APPKIT_FRAMEWORK AppKit REQUIRED)
    find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)

    set(MACOSX_BUNDLE_ICON_FILE icon.icns)
    set(APP_RESOURCES resources/icon.icns)
    set_source_files_properties(${APP_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL
  GIT_TAG        release-3.4.x
)

FetchContent_MakeAvailable(SDL3)

#add_subdirectory(vendor/sdl3-src EXCLUDE_FROM_ALL)

add_executable(${PROJECT_NAME} MACOSX_BUNDLE src/cpu.c src/instructions.c src/decode.c src/chip-8.c ${APP_RESOURCES})

target_sources(${PROJECT_NAME} PUBLIC include/cpu.h PUBLIC include/instructions.h PUBLIC include/decode.h PUBLIC include/file_dialog.h)

#PRIVATE -Wextra
target_compile_options(${PROJECT_NAME} PRIVATE -Wall PRIVATE -Werror)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 file_dialog ${APPKIT_FRAMEWORK} ${FOUNDATION_FRAMEWORK})
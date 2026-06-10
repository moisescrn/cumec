cmake_minimum_required(VERSION 4.3.3)
project(cumec)


######### EXECUTABLE #########
add_executable(cumec) # name of executable target

target_sources(cumec
    PRIVATE
      src/main.c
)

set_target_properties(cumec 
    PROPERTIES 
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)


######### LIBRARY #########
add_library(inclHead) # name of library target

target_include_directories(inclHead
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_sources(inclHead
    PRIVATE
        src/metronome.c
        src/display.c

    PUBLIC
      FILE_SET HEADERS
      BASE_DIRS
        src
      FILES
        src/metronome.h
        src/display.h
)

find_package(SDL3 REQUIRED)

target_link_libraries (cumec
    PRIVATE
      SDL3::SDL3
      inclHead
)


target_compile_options(cumec PRIVATE -g)
target_compile_options(inclHead PRIVATE -g)

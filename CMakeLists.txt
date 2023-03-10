cmake_minimum_required(VERSION 3.10)
project(video_search_engine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

find_package(OpenCV REQUIRED)
if (NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV not found!")
else ()
    message(STATUS "OpenCV library status:")
    message(STATUS "    version: ${OpenCV_VERSION}")
    message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
    message(STATUS "    libraries: ${OpenCV_LIBS}")
endif ()

find_package(libpqxx CONFIG REQUIRED)

file(GLOB_RECURSE SRC_LIST
    ./src/*.cpp
)

INCLUDE_DIRECTORIES(./header)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
link_libraries(${OpenCV_LIBS})
link_libraries(libpqxx::pqxx)

file(GLOB files "targets/*.cpp")
foreach(file ${files})
    get_filename_component(target ${file} NAME_WE)
    message(STATUS "Adding target: ${target}")
    add_executable(${target} ${SRC_LIST} ./targets/${target}.cpp)
endforeach()
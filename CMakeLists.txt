cmake_minimum_required(VERSION 2.8)

project(five-colors)

add_library(${PROJECT_NAME} SHARED
puzzle.h
puzzle.cpp
)

add_executable("solve-five-colors" "main.cpp")
target_link_libraries("solve-five-colors" ${PROJECT_NAME})

add_executable("test-five-colors" "unit_tests.cpp")
target_link_libraries("test-five-colors" ${PROJECT_NAME})

cmake_minimum_required(VERSION 3.8.2)
project(tree_sample)

add_executable(tree_sample src/tree.cpp src/main.cpp)
target_compile_features(tree_sample PRIVATE cxx_std_17)
target_compile_options(tree_sample PRIVATE -Wpedantic -Wall)

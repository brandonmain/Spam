cmake_minimum_required (VERSION 2.8.9)
project (Spam)

add_executable(training src/training.cc)
add_executable(classify src/classify.cc)
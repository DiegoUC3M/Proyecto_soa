cmake_minimum_required(VERSION 3.17)
project(Proyecto_soa)

set(CMAKE:CXX_STANDARD 20)
add_compile_options(-Wall -Wextra -Wno-deprecated -Werror -pedantic -pedantic-errors)

add_executable(Proyecto_soa sim-soa.hpp sim-soa.cpp calculations.cpp calculations.hpp)

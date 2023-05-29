cmake_minimum_required(VERSION 3.25)
project(archivator2semestr)

set(CMAKE_CXX_STANDARD 17)

configure_file("TextOrig.txt" "TextOrig.txt" COPYONLY)

add_executable(archivator2semestr main.cpp Statistics/Statistics.h Read_dict/Read_dict.h Pair/Pair.h Node/Node.h)

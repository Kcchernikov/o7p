# set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# Установка переменной со списком исходников
set(SOURCE_LIB 
    # headers
    generator.h
    generator_c.h
    
    #sources
    generator.cpp
    generator_c.cpp
)

add_library(generator STATIC ${SOURCE_LIB})   # Создание статической библиотеки

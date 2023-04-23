# set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# Установка переменной со списком исходников
set(SOURCE_LIB 
    # headers
    tlt.h
    #error.h
    parser.h
    
    #sources
    #error.cpp
    parser.cpp
)

add_library(compiler STATIC ${SOURCE_LIB})   # Создание статической библиотеки

target_link_libraries(compiler smodel object_model generator)     # Линковка программы с библиотекой

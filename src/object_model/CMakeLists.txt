# set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# Установка переменной со списком исходников
set(SOURCE_LIB 
    # headers
    declaration.h
    designator.h
    operator.h
    procedure.h
    statement.h
    expression.h
    validate.h
    
    #sources
    declaration.cpp
    designator.cpp
    operator.cpp
    procedure.cpp
    statement.cpp
    expression.cpp
    validate.cpp
)

add_library(object_model STATIC ${SOURCE_LIB})   # Создание статической библиотеки

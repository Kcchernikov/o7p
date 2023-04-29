#pragma once

#ifndef BASELIB_H
#define BASELIB_H

#include <assert.h>
// #include <stdio.h>
#include <stddef.h>
#include <string.h>

unsigned int set_difference(const unsigned int st1, const unsigned int st2) {
    return (st1 ^ st2) & st1;
}

typedef struct {const char* __record_type;} BASE_REC;

BASE_REC* ConvertPtr(BASE_REC* ptr, const char* type) {
    assert(strcmp(ptr->__record_type, type) == 0);
    return ptr;
}

typedef struct BASE_ARRAY {
    size_t lenght;
} BASE_ARRAY;

#endif // BASELIB_H
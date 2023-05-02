#pragma once

#ifndef BASELIB_H
#define BASELIB_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
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

void INC(long long* v) {
    (*v)++;
}

void DEC(long long* v) {
    (*v)--;
}

void INCL(unsigned* v, long long x) {
    (*v) |= (1ll << x);
}

void EXCL(unsigned* v, long long x) {
    (*v) &= ~(1ll << x);
}

void ASSERT(short int b) {
    assert(b);
}

long long LEN(BASE_ARRAY* v) {
    return v->lenght;
}

#endif // BASELIB_H
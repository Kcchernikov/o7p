#pragma once

#ifndef BASELIB_H
#define BASELIB_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

unsigned int set_difference(const unsigned int st1, const unsigned int st2);

typedef struct {const char* __record_type;} BASE_REC;

BASE_REC* ConvertPtr(BASE_REC* ptr, const char* type);

typedef struct BASE_ARRAY {
    size_t lenght;
} BASE_ARRAY;

void INC(long long* v);

void INC_N(long long* v, long long n);

void DEC(long long* v);

void DEC_N(long long* v, long long n);

short int ODD(long long x);

void INCL(unsigned* v, long long x);

void EXCL(unsigned* v, long long x);

void ASSERT(short int b);

long long LEN(BASE_ARRAY* v);

void PACK(float* x, long long n);

void UNPACK(float* x, long long n);

#endif // BASELIB_H
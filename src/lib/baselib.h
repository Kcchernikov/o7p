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
    size_t length;
} BASE_ARRAY;

void INC(long long* v);

void INC_N(long long* v, long long n);

void DEC(long long* v);

void DEC_N(long long* v, long long n);

short int ODD(long long x);

char CHR(long long x);

long long FLOOR(float x);

float FLT(long long x);

long long ABS_INTEGER(long long x);

float ABS_REAL(float x);

long long LSL(long long x, long long n);

long long ASR(long long x, long long n);

long long ROR(long long x, long long n);

long long ORD_CHAR(char x);

long long ORD_BOOL(short int x);

long long ORD_SET(unsigned x);

void INCL(unsigned* v, long long x);

void EXCL(unsigned* v, long long x);

void ASSERT(short int b);

long long LEN(BASE_ARRAY* v);

void PACK(float* x, long long n);

void UNPACK(float* x, long long n);

void PRINT_BOOL(short int x);

void PRINT_CHAR(char x);

void PRINT_INTEGER(long long x);

void PRINT_STRING(char* x);

void PRINT_REAL(float x);

void PRINT_SET(unsigned x);

#endif // BASELIB_H
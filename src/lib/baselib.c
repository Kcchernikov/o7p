#include "baselib.h"

unsigned int set_difference(const unsigned int st1, const unsigned int st2) {
    return (st1 ^ st2) & st1;
}

BASE_REC* ConvertPtr(BASE_REC* ptr, const char* type) {
    assert(strcmp(ptr->__record_type, type) == 0);
    return ptr;
}

void INC(long long* v) {
    (*v)++;
}

void INC_N(long long* v, long long n) {
    (*v) += n;
}

void DEC(long long* v) {
    (*v)--;
}

void DEC_N(long long* v, long long n) {
    (*v) -= n;
}

short int ODD(long long x) {
    return (x % 2) == 1;
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

void PACK(float* x, long long n) {
    (*x) *= pow(2, n);
}

void UNPACK(float* x, long long n) {
    (*x) /= pow(2, n);
}

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

char CHR(long long x) {
    return (char)(x);
}

long long FLOOR(float x) {
    if (x < 0) {
        x = x - 1 + 0.0000001;
    }
    return x;
}

float FLT(long long x) {
    return x;
}

long long ABS_INTEGER(long long x) {
    return llabs(x);
}

float ABS_REAL(float x) {
    return fabsf(x);
}

long long LSL(long long x, long long n) {
    return x << n;
}

long long ASR(long long x, long long n) {
    return x >> n;
}

long long ROR(long long x, long long n) {
    size_t bit_cnt = (sizeof(long long) * 8);
    size_t n1 = n % bit_cnt;
    return (x >> n) + ((((1ll << n) - 1) & x) << (bit_cnt - n));
}

long long ORD_CHAR(char x) {
    return x;
}

long long ORD_BOOL(short int x) {
    return x;
}

long long ORD_SET(unsigned x) {
    return x;
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
    return v->length;
}

void PACK(float* x, long long n) {
    (*x) *= pow(2, n);
}

void UNPACK(float* x, long long n) {
    (*x) /= pow(2, n);
}

void PRINT_BOOL(short int x) {
    if (x == 0) {
        printf("false");
    } else {
        printf("true");
    }
}

void PRINT_CHAR(char x) {
    printf("%c", x);
}

void PRINT_INTEGER(long long x) {
    printf("%lld", x);
}

void PRINT_STRING(char* x) {
    printf("%s", x);
}

void PRINT_REAL(float x) {
    printf("%f", x);
}

void PRINT_SET(unsigned x) {
    printf("{");
    short int start = 1;
    for (int i = 0; i < sizeof(x) * 8; ++i) {
        if (x & (1ll << i)) {
            if (start == 0) {
                printf(", ");
            } else {
                start = 0;
            }
            printf("%d", i);
        }
    }
    printf("}");
}

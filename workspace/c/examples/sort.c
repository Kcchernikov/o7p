#include "/Users/akchernikov/hse/Project/o7p/workspace/h/examples/sort.h"

const long long len = 20;
struct {
    size_t length;
    long long* value;
} arr;
long long arr_base_ref0[len];
long long i;
void BubleSort(BASE_ARRAY* __base_ptr_arr) {
    long long i;
    long long j;
    long long tmp;
    long long len;
    struct {
        size_t length;
        long long* value;
    }* __ptr_arr = (void *) __base_ptr_arr;
    len = LEN((BASE_ARRAY*) &(*__ptr_arr));
    // For loop
    i = 0;
_0:
    if (i <= len - 1) {
        // For loop
        j = 1;
_1:
        if (j <= len - i - 1) {
            if ((*__ptr_arr).value[j] < (*__ptr_arr).value[j - 1]) {
                tmp = (*__ptr_arr).value[j];
                (*__ptr_arr).value[j] = (*__ptr_arr).value[j - 1];
                (*__ptr_arr).value[j - 1] = tmp;
            }
            j += 1;
            goto _1;
        }
        i += 1;
        goto _0;
    }
}

void MergeSort(BASE_ARRAY* __base_ptr_arr, long long l, long long r) {
    long long i;
    long long a;
    long long b;
    long long cur;
    long long m;
    struct {
        size_t length;
        long long* value;
    } newArr;
    long long newArr_base_ref0[r - l];
    newArr.length = r - l;
    newArr.value = (void*) newArr_base_ref0;
    struct {
        size_t length;
        long long* value;
    }* __ptr_arr = (void *) __base_ptr_arr;
    if (r - l > 1) {
        m = (r + l) / 2;
        MergeSort((BASE_ARRAY*) &(*__ptr_arr), l, m);
        MergeSort((BASE_ARRAY*) &(*__ptr_arr), m, r);
        a = l;
        b = m;
        cur = 0;
        // For loop
        i = 0;
_2:
        if (i <= r - l - 1) {
            if (a == m) {
                newArr.value[i] = (*__ptr_arr).value[b];
                INC(&b);
            } else if (b == r) {
                newArr.value[i] = (*__ptr_arr).value[a];
                INC(&a);
            } else if ((*__ptr_arr).value[a] < (*__ptr_arr).value[b]) {
                newArr.value[i] = (*__ptr_arr).value[a];
                INC(&a);
            } else {
                newArr.value[i] = (*__ptr_arr).value[b];
                INC(&b);
            }
            i += 1;
            goto _2;
        }
        // For loop
        i = 0;
_3:
        if (i <= r - l - 1) {
            (*__ptr_arr).value[l + i] = newArr.value[i];
            i += 1;
            goto _3;
        }
    }
}

void InitModuleSort() {
    arr.length = len;
    arr.value = (void*) arr_base_ref0;
    // For loop
    i = 0;
_4:
    if (i <= len - 1) {
        arr.value[len - i - 1] = i;
        i += 1;
        goto _4;
    }
    MergeSort((BASE_ARRAY*) &arr, 0, LEN((BASE_ARRAY*) &arr));
    // For loop
    i = 0;
_5:
    if (i <= len - 1) {
        PRINT_INTEGER(arr.value[i]);
        PRINT_STRING(" ");
        i += 1;
        goto _5;
    }
    PRINT_STRING("\n");
}


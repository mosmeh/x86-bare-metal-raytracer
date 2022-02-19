#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void* memset(void* s, int c, size_t n) {
    uintptr_t dest = (uintptr_t)s;
    __asm__ volatile("rep stosb\n"
                     : "=D"(dest), "=c"(n)
                     : "0"(dest), "1"(n), "a"(c)
                     : "memory");
    return s;
}

void* memcpy(void* dest_ptr, const void* src_ptr, size_t n) {
    size_t dest = (size_t)dest_ptr;
    size_t src = (size_t)src_ptr;
    __asm__ volatile("rep movsb\n" ::"S"(src), "D"(dest), "c"(n) : "memory");
    return dest_ptr;
}

double sqrt(double x) {
    double res;
    __asm__("fsqrt" : "=t"(res) : "0"(x));
    return res;
}

double fmin(double x, double y) {
    if (isnan(x))
        return y;
    if (isnan(y))
        return x;

    return x < y ? x : y;
}

double fabs(double x) {
    __asm__("fabs" : "+t"(x));
    return x;
}

double log2(double x) {
    double ret;
    __asm__("fld1\n"
            "fxch %%st(1)\n"
            "fyl2x\n"
            : "=t"(ret)
            : "0"(x));
    return ret;
}

double exp2(double exponent) {
    double res;
    __asm__("fld1\n"
            "fld %%st(1)\n"
            "fprem\n"
            "f2xm1\n"
            "faddp\n"
            "fscale\n"
            "fstp %%st(1)"
            : "=t"(res)
            : "0"(exponent));
    return res;
}

double pow(double x, double y) {
    if (isnan(y))
        return y;
    if (y == 0)
        return 1;
    if (x == 0)
        return 0;
    if (y == 1)
        return x;

    int y_as_int = (int)y;
    if (y == (double)y_as_int) {
        double result = x;
        for (int i = 0; i < fabs(y) - 1; ++i)
            result *= x;
        if (y < 0)
            result = 1.0l / result;
        return result;
    }

    return exp2(y * log2(x));
}

double tan(double angle) {
    double ret, one;
    __asm__("fptan" : "=t"(one), "=u"(ret) : "0"(angle));
    return ret;
}

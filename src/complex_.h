#pragma once
#include "ext.h"

typedef struct complex_t {
  float real;
  float imag;
} complex_t;

#if !defined(_COMPLEX_H_) && !defined(complex) && !defined(__cplusplus)
typedef complex_t complex;
#endif

float real(complex_t num);
float imag(complex_t num);
complex_t cvalue(float real, float imag);

complex_t cadd(complex_t num1, complex_t num2);
complex_t cdiff(complex_t num1, complex_t num2);
complex_t cmultiply(complex_t num1, complex_t num2);
float ccabs(complex_t num);
complex_t cexp(complex_t num);

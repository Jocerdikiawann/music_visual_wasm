#pragma once

#include "complex_.h"

#ifndef M_PI
#define M_PI 3.141592653589793
#endif
#ifndef M_E
#define M_E 2.718281828459045
#endif

#define SHORT_CTOA 1

float real(complex_t num) { return num.real; }

float imag(complex_t num) { return num.imag; }

complex_t cvalue(float real,
                 float imag) // Change values of a complex function
{
  complex_t result = {real, imag};
  return result;
}

complex_t cadd(complex_t num1, complex_t num2) {
  complex_t result;
  result.real = num1.real + num2.real;
  result.imag = num1.imag + num2.imag;
  return result;
}

complex_t cdiff(complex_t num1, complex_t num2) {
  complex_t result;
  result.real = num1.real - num2.real;
  result.imag = num1.imag - num2.imag;
  return result;
}

complex_t cmultiply(complex_t num1, complex_t num2) {
  complex_t result;
  result.real = num1.real * num2.real - num1.imag * num2.imag;
  result.imag = num1.real * num2.imag + num1.imag * num2.real;
  return result;
}

complex_t cexp(complex_t num) {
  complex_t result;
  complex_t tmp = cvalue(num.real, num.imag);
  result.real = pow(M_E, num.real) * cos(num.imag);
  result.imag = pow(M_E, num.real) * sin(num.imag);
  return result;
}

float ccabs(complex_t num) {
  return sqrt(num.real * num.real + num.imag * num.imag);
}

#ifndef j
#define j cvalue
#endif

#ifndef BMCURVE_H
#define BMCURVE_H
#include <stdint.h>
#include "bm.h"

typedef struct Field {
  bint p[1], x[1];
} field;

typedef struct Order {
  field f;
  bint N[1];
} order;

typedef struct Prime {
  field f;
  bint P[1];
} prime;

typedef struct Point {
  prime prime_x, prime_y;
} point;

typedef struct Curve {
  bint A[1], B[1], N[1], P[1];
  point G;
} curve;

void field_init(field *f, bint *x, bint *p);
void field_initint(field *f, uint32_t x, bint *p);
void field_add(field *ret, field *a, field *b);
void field_sub(field *ret, field *a, field *b);
void field_mul(field *ret, field *a, field *b);
void field_div(field *ret, field *a, field *b);
void field_neg(field *ret, field *a, field *b);
int8_t field_eq(field *a, field *b);

void order_init(order *o, bint *x, bint *p1);
void order_initint(order *o, uint32_t x, bint *p1);

void prime_init(prime *p, bint *x, bint *p1);
void prime_initint(prime *p, uint32_t x, bint *p1);

void point_init(point *p, prime *a, prime *b, bint *p1);
void point_add(point *ret, point *a, point *b, bint *p1);
void point_sub(point *ret, point *a, point *b, bint *p1);
void point_mul(point *ret, point *a, field *k, bint *p1);
int8_t point_eq(point *a, point *b);

void curve_init(curve *c, char *a, char *b, char *n, char *p, char *g);

void Ftester(void);
void Ftester_sanity(void);
void Ftester_math_sanity(void);
#endif


#ifndef BMCURVE_H
#define BMCURVE_H
#include <stdint.h>
#include "bm.h"

typedef struct Curve Curve;
typedef struct Field Field;
typedef struct Order Order;
typedef struct Point Point;
typedef struct Prime Prime;

struct Field {
  bint p[1], x[1];
  void (*init)(bint *x, Field*);
  void (*initint)(uint32_t x, bint *p, Field*);
  void (*get)(bint *ret, Field*);
  int8_t (*eq)(Field*, Field*);
  void (*add)(Field*, Field*, Field*);
  void (*sub)(Field*, Field*, Field*);
  void (*mul)(Field*, Field*, Field*);
  void (*div)(Field*, Field*, Field*);
};

struct Prime {
  Field field;
  bint P[1];
  void (*init)(bint *x, Prime*);
  void (*initint)(uint32_t x, bint *p1, Prime*);
  void (*get)(bint *ret, Prime*);
};

struct Order {
  Field field;
  bint N[1];
  void (*init)(bint *x, Order*);
  void (*initint)(uint32_t x, bint *p1, Order*);
  void (*get)(bint *ret, Order*);
};

struct Point {
  Prime prime_x, prime_y;
  void (*init)(Prime *x, Prime *y, bint *p1, Point *self);
  uint8_t (*eq)(Point*, Point*);
  void (*add)(Point*, Point*, bint *p1, Point*);
  uint8_t (*sub)(Point*, Point*, Point*);
  void (*mul)(Point*, Point*, Point*, bint *p1);
};

struct Curve {
  bint A[1], B[1], N[1], P[1];
  Point G;
  void (*init)(Curve*, char *a, char *b, char *n, char *p, char *g);
  void (*close)(Curve*);
};

void Ftester(void);
void Ftester_sanity(void);
void Ftester_math_sanity(void);
#endif


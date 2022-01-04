//                                                                            //
#include <stdbool.h>

#ifndef LIGHTBIG_H
#define LIGHTBIG_H 1

#define BIGLEN 2048

typedef struct {
  int *dig;
  int len;
  bool neg;
} bigint_t;

void big_init(bigint_t **a);
void big_set(char *a, bigint_t **b);
char *big_get(bigint_t *a);
void big_add(bigint_t *a, bigint_t *b, bigint_t **c);
void big_mul(bigint_t *a, bigint_t *b, bigint_t **c);
void big_sub(bigint_t *a, bigint_t *b, bigint_t **c);
void big_div(bigint_t *a, bigint_t *b, bigint_t **d);
void big_mod(bigint_t *a, bigint_t *b, bigint_t **e);
void big_assert(bigint_t **b1, bigint_t **b2);
#endif

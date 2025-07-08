// Auth: smurfd, 2025 More reading at the bottom of the file; 2 spacs indent; 150 width                                                             //
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "bm.h"

bint *inverse_mod(bint *ret, const bint *k, const bint *p);
void point_add(bint *rx, bint *ry, bint *p1x, bint *p1y, bint *p2x, bint *p2y, bint *p);
void scalar_mul(bint *rx, bint *ry, bint *k, bint *p1x, bint *p1y, bint *p, bint *n);


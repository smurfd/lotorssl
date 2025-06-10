#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "bmcurve.h"

// ----- Field class -----
void Field_init(bint *x, Field *self) {
  str2bint(self->p, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  bmod(self->x, x, self->p);
}

void Field_initint(uint32_t x, bint *p, Field *self) {
  bint x1[1];
  memcpy(self->p, p, sizeof(bint));
  bmod(self->x, wrd2bint(x1, x), self->p);
}

int8_t Field_eq(Field *data, Field *self) {
  bprint("CMP dx", data->x);
  bprint("CMP sx", self->x);
  bprint("CMP dp", data->p);
  bprint("CMP sp", self->p);
  assert(cmp(data->p, self->p) == 0);
  return cmp(data->x, self->x);
}

void Field_add(Field *ret, Field *data, Field *self) {
  assert(cmp(data->p, self->p) == 0);
  bint tmp;
  tmp.siz = self->x->siz;
  self->x->neg = 0; //
  data->x->neg = 0; //
  badd(&tmp, self->x, data->x);
  bprint("Fadd sx", self->x);
  bprint("Fadd dx", data->x);
  bprint("Fadd sp", self->p);
  bprint("Fadd =", &tmp);
  bmod(ret->x, &tmp, self->p); // (self.x + data.x) % self.p
  bprint("Fadd =", ret->x);
}

void Field_sub(Field *ret, Field *data, Field *self) {
  assert(cmp(data->p, self->p) == 0);
  bint tmp;
  bsub(&tmp, self->x, data->x);
  bmod(ret->x, &tmp, self->p); // (self.x - data.x) % self.p
}

void Field_mul(Field *ret, Field *data, Field *self) {
  assert(cmp(data->p, self->p) == 0);
  bint tmp;
  bmul(&tmp, self->x, data->x);
  bmod(ret->x, &tmp, self->p); // (self.x * data.x) % self.p
}

void Field_div(Field *ret, Field *data, Field *self) {
  Field f, o;
  f.initint = Field_initint;
  o.initint = Field_initint;
  f.initint(0, self->p, &f);
  o.initint(1, self->p, &o);
  bdiv(f.x, o.x, data->x);
  Field_mul(ret, self, &f);
}
//  def __truediv__(self, data): return self * data ** -1
//  def __pow__(self, data): return self.__class__(pow(self.x, data, self.p))

void Field_neg(Field *ret, Field *data, Field *self) {
  bsub(ret->x, self->p, self->x); // self.p - self.x
}


// ----- Order class -----
void Order_init(bint *x, Order *self) {
  bint p1[1];
  str2bint(p1, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  self->field.initint = Field_initint;
  self->field.initint(0, p1, &self->field);
  memcpy(self->field.x, x, sizeof(bint));
  str2bint(self->N, "0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
}

void Order_initint(uint32_t x, bint *p1, Order *self) {
  bint x1[1];
  wrd2bint(x1, x);
  self->field.initint = Field_initint;
  self->field.initint(0, p1, &self->field);
  memcpy(self->field.x, x1, sizeof(bint));
  str2bint(self->N, "0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
}


// ----- Prime class -----
void Prime_init(bint *x, Prime *self) {
  bint p1[1];
  str2bint(p1, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  self->field.initint = Field_initint;
  self->field.initint(0, p1, &self->field);
  memcpy(self->field.x, x, sizeof(bint));
  str2bint(self->P, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
}

void Prime_initint(uint32_t x, bint *p1, Prime *self) {
  bint x1[1];
  wrd2bint(x1, x);
  self->field.initint = Field_initint;
  self->field.initint(0, p1, &self->field);
  memcpy(self->field.x, x1, sizeof(bint));
  str2bint(self->P, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
}


// ----- Point class -----
void Point_init(Prime *x, Prime *y, Point *self) {
  bint p1[1];
  str2bint(p1, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  self->prime_x.initint = Prime_initint;
  self->prime_y.initint = Prime_initint;
  self->prime_x.initint(0, p1, &self->prime_x);
  self->prime_y.initint(0, p1, &self->prime_y);
  bprint("XP", x->P);
  bprint("YP", y->P);
  str2bint(x->P, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(y->P, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  bprint("XP", x->P);
  bprint("YP", y->P);
  bint y2[1], x2[1], x3[1], x37[1], b[1], x3m[1];
  memset(b->wrd, 0, LEN * sizeof(uint32_t));
  memset(y2->wrd, 0, LEN * sizeof(uint32_t));
  memset(x2->wrd, 0, LEN * sizeof(uint32_t));
  memset(x3->wrd, 0, LEN * sizeof(uint32_t));
  memset(x37->wrd, 0, LEN * sizeof(uint32_t));
  memset(x3m->wrd, 0, LEN * sizeof(uint32_t));
  wrd2bint(b, 7);
  bprint("pi: y", y->field.x);
  bmul(y2, y->field.x, y->field.x); // y
  bprint("pi: y**2", y2);
  bprint("pi: x", x->field.x);
  bmul(x2, x->field.x, x->field.x); // x
  printf("bmod pi: ------------v\n");
  bprint("pi: x3", x2);
  bmul(x3, x2, x->field.x); // x
  //x3m->siz = 24;
  printf("bmod point x\n");
  bprint("pi: x3", x3);
  bprint("pi: xp", x->P);
//  exit(0);
  //x3m->siz = 24;
  //bmod(x3, x3, x->P);
//  exit(0);
  printf("bmod pi: ------------M\n");
//  x3m->siz = 24;
  printf("bmod point y\n");
  bprint("pi: xxx", x->P);
  bprint("pi: x3m", x3);
  bprint("pi: b", b);
  x3->neg = 0;
  x37->siz = x3->siz;
  badd(x37, x3, b);
  bprint("pi: x37", x37);
  //exit(0);
//  bmod(x37, x37, p1);
  bprint("pi: 37m", x37);
//  exit(0);

  bprint("pi: x37", x37);
  //exit(0);
  bint y22[1], x377[1];
  y22->siz = y2->siz;
//  x377->siz = x37->siz;
  y22->neg = 0;
//  x377->neg = 0;
  y22->cap = y2->cap;
//  x377->cap = x37->cap;
 // x37->siz = 8;

  bmod(y22, y2, p1);
  //bprint("X37x", x37);
  //bprint("X37p", p1);
  bint xx[1];
  bmod(xx, x37, p1);
  //bprint("Y22", y22);
  //bprint("X37", xx);
  assert(cmp(xx, y22) == 0);
  //exit(0);
  // y ** 2 == x ** 3 + A * x + B // A=0, B=7

//POINT y Prime(0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8) Prime(0x4866d6a5ab41ab2c6bcc57ccd3735da5f16f80a548e5e20a44e4e9b8118c26f2)
//POINT x Prime(0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798) Prime(0x4866d6a5ab41ab2c6bcc57ccd3735da5f16f80a548e5e20a44e4e9b8118c26eb)
//POINT x+B Prime(0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798) Prime(0x4866d6a5ab41ab2c6bcc57ccd3735da5f16f80a548e5e20a44e4e9b8118c26f2)

  printf("p1\n");
  self->prime_x.init = Prime_init;
  self->prime_y.init = Prime_init;
  self->prime_x.init(x->field.x, &self->prime_x);
  self->prime_y.init(y->field.x, &self->prime_y);
  bprint("bpinit", self->prime_x.field.x);
  bprint("bpinit", self->prime_y.field.x);
  bprint("Y2", y22);
  //exit(0);
}

void Point_add(Point *ret, Point *data, Point *self) {
  printf("add here\n");
  bint p1[1];
  str2bint(p1, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");

  Prime x1, x2, y1, y2, p0;
  x1.init = Prime_init;
  x2.init = Prime_init;
  y1.init = Prime_init;
  y2.init = Prime_init;

  x1.init(self->prime_x.field.x, &x1);
  x2.init(data->prime_x.field.x, &x2);
  y1.init(self->prime_x.field.x, &y1);
  y2.init(data->prime_x.field.x, &y2);

  p0.initint = Prime_initint;
  p0.initint(0, p1, &p0);
  printf("here\n");
  if (Field_eq(&x1.field, &p0.field) == 0 && Field_eq(&y1.field, &p0.field) == 0) {printf("MULx1y1eq\n");memcpy(ret, data, sizeof(Point)); bprint("MUL dat", data->prime_x.field.x); bprint("MUL dat", data->prime_y.field.x); return;}
  if (Field_eq(&x2.field, &p0.field) == 0 && Field_eq(&y2.field, &p0.field) == 0) {printf("MULx2y2eq\n");memcpy(ret, self, sizeof(Point)); return;}
  if (Field_eq(&x1.field, &x2.field) == 0 && Field_eq(&y1.field, &y2.field) == 0) { // y2 pos?
    Prime x1x1, xa, yy1, x3, y3, sk;
    printf("adder\n");
    x1x1.field.initint = Field_initint;
    x1x1.field.initint(0, p1, &x1x1.field);
    printf("adder\n");
    x1x1.field.mul = Field_mul;
    x1x1.field.mul(&x1x1.field, &x1.field, &x1.field);
    xa.field.initint = Field_initint;
    xa.field.initint(0, p1, &xa.field);
    printf("adder\n");
    xa.field.add = Field_add;
    xa.field.add(&xa.field, &x1x1.field, &x1x1.field);
    xa.field.add(&xa.field, &xa.field, &x1x1.field);
    yy1.field.initint = Field_initint;
    yy1.field.initint(0, p1, &yy1.field);
    yy1.field.add = Field_add;
    yy1.field.add(&yy1.field, &y1.field, &y1.field);

    sk.field.initint = Field_initint;
    sk.field.initint(0, p1, &sk.field);
    x3.field.initint = Field_initint;
    x3.field.initint(0, p1, &x3.field);
    y3.field.initint = Field_initint;
    y3.field.initint(0, p1, &y3.field);
    printf("adder\n");
    sk.field.div = Field_div;
    x3.field.mul = Field_mul;
    x3.field.sub = Field_sub;
    y3.field.sub = Field_sub;
    y3.field.mul = Field_mul;
    sk.field.div(&sk.field, &xa.field, &yy1.field);
    x3.field.mul(&x3.field, &sk.field, &sk.field);
    x3.field.sub(&x3.field, &x3.field, &x1.field);
    x3.field.sub(&x3.field, &x3.field, &x2.field); // x3 = sk * sk - x1 - x2
    y3.field.sub(&y3.field, &x1.field, &x3.field);
    y3.field.mul(&y3.field, &sk.field, &x3.field);
    y3.field.sub(&y3.field, &y3.field, &y1.field); // y3 = sk * (x1 - x3) - y1
    printf("adder\n");
    Point r;
    r.init = Point_init;
    printf("p1 init\n");
    r.init(&x3, &y3, &r);
    printf("p1 init\n");
    memcpy(ret, &r, sizeof(Point));
    printf("pa end2\n");
    return; // return Point(x3, y3)
  }
  if (Field_eq(&x1.field, &x2.field) == 0 && Field_eq(&y1.field, &y2.field) == 0) { // y2 neg?
    Point pz;
    pz.init = Point_init;
    pz.init(&p0, &p0, &pz);
    memcpy(ret, &pz, sizeof(Point));
    printf("MULinf\n");
    return;
  }
  printf("adder\n");
  Prime sk, x3, y3, x21, y21;
  sk.field.initint = Field_initint;
  x3.field.initint = Field_initint;
  y3.field.initint = Field_initint;
  y21.field.initint = Field_initint;
  x21.field.initint = Field_initint;
  sk.field.initint(0, p1, &sk.field);
  x3.field.initint(0, p1, &x3.field);
  y3.field.initint(0, p1, &y3.field);
  y21.field.initint(0, p1, &y21.field);
  x21.field.initint(0, p1, &x21.field);
  x21.field.sub = Field_sub;
  y21.field.sub = Field_sub;
  sk.field.div = Field_div;
  x3.field.mul = Field_mul;
  x3.field.sub = Field_sub;
  y3.field.sub = Field_sub;
  y3.field.mul = Field_mul;
  y21.field.sub(&y21.field, &y2.field, &y1.field);
  x21.field.sub(&x21.field, &x2.field, &x1.field);
  sk.field.div(&sk.field, &y21.field, &x21.field); // sk = (y2 - y1) / (x2 - x1)
  x3.field.mul(&x3.field, &sk.field, &sk.field);
  x3.field.sub(&x3.field, &x3.field, &x1.field);
  x3.field.sub(&x3.field, &x3.field, &x2.field); // x3 = sk * sk - x1 - x2
  y3.field.sub(&y3.field, &x1.field, &x3.field);
  y3.field.mul(&y3.field, &sk.field, &x3.field);
  y3.field.sub(&y3.field, &y3.field, &y1.field); // y3 = sk * (x1 - x3) - y1
  Point r;
  r.init = Point_init;
  printf("p2 init\n");
  r.init(&x3, &y3, &r);
  printf("p2 init\n");
  memcpy(ret, &r, sizeof(Point));
  printf("pa end3\n");
  return; // return Point(x3, y3)
}

void Point_mul(Point *ret, Point *self, Point *k) { // TODO: this actually work?!
  bint n[1], z[1], o[1], tw[1], b[1];
  bint p1[1];
  str2bint(p1, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  Prime pz; // Point multiplication: Double-and-add https://en.wikipedia.org/wiki/Elliptic_curve_point_multiplication
  Point result, addend, a;
  printf("mult\n");
  wrd2bint(z, 0);
  wrd2bint(o, 1);
  wrd2bint(tw,2);

  n->siz = 30;
  pz.initint = Prime_initint;
  pz.initint(0, p1, &pz);
  result.init = Point_init;
  addend.init = Point_init;
  a.init = Point_init;
  printf("mult\n");
  result.init(&pz, &pz, &result);
  addend.init(&self->prime_x, &self->prime_y, &addend);
  memset(n->wrd, 0, LEN * sizeof(uint32_t));
  memcpy(n->wrd, k->prime_x.field.x->wrd, k->prime_x.field.x->siz * sizeof(uint32_t));
  while (cmp(n, z) == 1 && n->siz > 0) {
    bprint("GNn", n);
    bprint("GNt", tw);
    bprint("GNo", o);
    bmod(b, n, tw);
    bprint("GBbb", b);
    printf("aft mod\n");
    printf("GNcmp %d\n", cmp(b, o));
    if (cmp(b, o) == 0) {Point_add(&result, &result, &addend); printf("Gnnn ODD\n");}
    a.init(&pz, &pz, &a);
    printf("add here -----------------------------------------\n");
    Point_add(&a, &addend, &addend);
    printf("add here -----------------------------------------\n");
    printf("nnn b111 %d\n", cmp(b, o));
    brshift(n, n, 1);
    bprint("NNNNN", n);
    printf("NNNNN %d\n", cmp(n, z));
  }
  bprint("Gpr1x", result.prime_x.field.x);
  bprint("Gpr1y", result.prime_y.field.x);
  memcpy(ret, &result, sizeof(Point));
  bprint("Gpr2x", ret->prime_x.field.x);
  bprint("Gpr2y", ret->prime_y.field.x);
}

void Ftester(void) {
  Field field0, field1, field2, field3;
  bint p1[1];
  wrd2bint(p1, 23);
  printf("--------- Testvalues -------------------------------------------\n");
  field0.initint = Field_initint;
  field1.initint = Field_initint;
  field2.initint = Field_initint;
  field3.initint = Field_initint;
  field0.initint(0, p1, &field0);
  field1.initint(12, p1, &field1);
  field2.initint(20, p1, &field2);
  field3.initint(9, p1, &field3);
  field0.add = Field_add;
  field0.add(&field0, &field1, &field2);
  field0.eq = Field_eq;
  printf("eq: %d\n", field0.eq(&field0, &field3)); //assert Field(12) + Field(20) == Field(9)
  assert(field0.eq(&field0, &field3) == 0);
  wrd2bint(p1, 23);
  field0.initint(0, p1, &field0);
  field1.initint(8, p1, &field1);
  field2.initint(9, p1, &field2);
  field3.initint(3, p1, &field3);
  field0.mul = Field_mul;
  field0.mul(&field0, &field1, &field2);
  printf("eq: %d\n", field0.eq(&field0, &field3)); //assert Field(8) * Field(9) == Field(3)
  assert(field0.eq(&field0, &field3) == 0);

  Order order1, order2;
  order1.initint = Order_initint;
  order2.initint = Order_initint;
  order1.initint(42, p1, &order1);
  order2.initint(24, p1, &order2);
  bint x[1], y[1];
  Prime px, py, pz, po, qo;
  px.init = Prime_init;
  py.init = Prime_init;
  pz.initint = Prime_initint;
  po.init = Prime_init;
  qo.init = Prime_init;
  str2bint(x, "0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"); // point gx
  str2bint(y, "0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"); // point gy
  px.init(x, &px);
  py.init(y, &py);
  pz.initint(0, p1, &pz);
  po.init(order1.field.x, &po);
  qo.init(order2.field.x, &qo);
  Point G, G2, GR, p, q, pop, qop, infinity;
  G.init = Point_init;
  G2.init = Point_init;
  GR.init = Point_init;
  p.init = Point_init;
  q.init = Point_init;
  pop.init = Point_init;
  qop.init = Point_init;
  infinity.init = Point_init;
  G.init(&px, &py, &G);
  exit(0); // TODO: Break here for now
  G2.init(&po, &py, &G2);
  bprint("Gpo", po.field.x);
  bprint("Gqo", qo.field.x);
  bprint("GX", G.prime_x.field.x);
  bprint("GY", G.prime_y.field.x);
  printf("p----\n");
  GR.init(&pz, &pz, &GR);
  p.init(&pz, &pz, &p);
  q.init(&pz, &pz, &q);
  infinity.init(&pz, &pz, &infinity);
  pop.init(&po, &po, &pop);//&pz, &pop);
  qop.init(&qo, &qo, &qop);//&pz, &qop);
  bprint("GpopX", pop.prime_x.field.x);
  bprint("GqopY", qop.prime_y.field.x);
  p.mul = Point_mul;
  q.mul = Point_mul;
  p.mul(&p, &G, &pop); // p = c.G * Order(42)
  q.mul(&q, &G, &qop); // q = c.G * Order(24)
  bprint("Gp", pop.prime_x.field.x);
  bprint("Gq", qop.prime_y.field.x);
  bprint("G", G.prime_x.field.x);
  bprint("G", G.prime_y.field.x);
  bprint("P", p.prime_x.field.x);
  bprint("P", p.prime_y.field.x);
  bprint("Q", q.prime_x.field.x);
  bprint("Q", q.prime_y.field.x);
  bint t1, t2, t3, t4;
  wrd2bint(&t1, 0x2a);
  wrd2bint(&t2, 0x18);
  str2bint(&t3, "0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"); // point gx
  str2bint(&t4, "0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"); // point gy

  Prime t1p, t2p, t3p, t4p;
  t1p.init = Prime_init;
  t2p.init = Prime_init;
  t3p.init = Prime_init;
  t4p.init = Prime_init;
  t1p.init(&t1, &t1p);
  t2p.init(&t2, &t2p);
  t3p.init(&t3, &t3p);
  t4p.init(&t4, &t4p);
  Point pt1, pt2, pt3;
  pt1.init = Point_init;
  pt2.init = Point_init;
  pt1.init(&t1p, &t2p, &pt1);
  pt2.init(&t3p, &t4p, &pt2);
  pt3.init = Point_init;
  pt3.init(&pz, &pz, &pt3);
  pt3.mul = Point_mul;
  pt3.mul(&pt3, &pt1, &pt2);
  bprint("P3", pt3.prime_x.field.x);
  bprint("P3", pt3.prime_y.field.x);
  //r = Point(p.x, -p.y)
  // assert p + q == c.G * Order(66)
  GR.add = Point_add;
  bprint("pp", p.prime_x.field.x);
  bprint("pp", p.prime_y.field.x);
  bprint("qq", q.prime_x.field.x);
  bprint("qq", q.prime_y.field.x);
  GR.add(&GR, &p, &q); // p + q
  bprint("p+q", GR.prime_x.field.x);
  bprint("p+q", GR.prime_y.field.x);
  order1.initint(66, p1, &order1);
  po.init(order1.field.x, &po);
  pop.init(&po, &pz, &pop);
  Point_mul(&G2, &G, &pop);// c.G * Order(66)
  bprint("GRx", GR.prime_x.field.x);
  bprint("G2x", G2.prime_x.field.x);
  bprint("order1", order1.field.x);
  bprint("po", po.field.x);
  bprint("p+q", GR.prime_x.field.x);
  bprint("p+q", GR.prime_y.field.x);
}

void Ftester_sanity(void) {
  bint aaa, bbb, ccc, r1, r2, ddd, hhh, jjj;
  wrd2bint(&aaa, 666);
  wrd2bint(&bbb, 999);
  wrd2bint(&r1, 333);
  bmod(&ccc, &bbb, &aaa);
  assert(cmp(&ccc, &r1) == 0);
  str2bint(&ddd, "0x6666666666666666666666666666666666666666666666666666666666666666");
  str2bint(&jjj, "0x9999999999999999999999999999999999999999999999999999999999999999");
  str2bint(&r2, "0x3333333333333333333333333333333333333333333333333333333333333333");
  bmod(&hhh, &jjj, &ddd);
  assert(cmp(&hhh, &r2) == 0);
}

void Ftester_math_sanity(void) {
  bint a, b, p, r1, r2;
  str2bint(&a, "0x3b6859c358bb6fa30b3f11ff6c29164dc21b2abaf4c2027ea8e6701e99dd5b7c");
  str2bint(&b, "0xd7dab791a8647ac88695e20e29960a6fd41707258cc88cc0480ea7e5bb3f132f");
  Field f1, f2, f3, f4, fr;
  fr.initint = Field_initint;
  f1.init = Field_init;
  f2.init = Field_init;
  f3.init = Field_init;
  f4.init = Field_init;

  str2bint(&p, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  fr.initint(0, &p, &fr);
  f1.init(&a, &f1);
  f2.init(&b, &f2);
  f2.add = Field_add;
  f2.add(&fr, &f1, &f2);
  bprint("FR", fr.x);
  str2bint(&r1, "0x13431155011fea6b91d4f40d95bf20bd963231e0818a8f3ef0f51805551c727c");
  bprint("R1", &r1);
  fr.x->siz = 8;
  bprint("FR", fr.x);
  bprint("R1", &r1);
  assert(cmp((bint*)fr.x, &r1) == 0);

  f2.mul = Field_mul;
  f2.mul(&f3, &f1, &f2);
  str2bint(&r2, "0xa0cc141790812f1cc95dd3da756f3d4930f1adbeef8929265e0a74480f6eb392");
  assert(cmp((bint*)f3.x, &r2) == 0);
}


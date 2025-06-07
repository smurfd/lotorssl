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
  x3m->siz = 24;
  printf("bmod point x\n");
  bprint("pi: x3", x3);
  bprint("pi: xp", x->P);
  bmod(x3m, x3, x->P);
  printf("bmod pi: ------------M\n");
  x3m->siz = 24;
  printf("bmod point y\n");
  bprint("pi: xxx", x->P);
  bprint("pi: x3m", x3m);
  x3->neg = 0;
  badd(x37, x3, b);
  bprint("pi: x37", x37);
  // y ** 2 == x ** 3 + A * x + B // A=0, B=7
  printf("p1\n");
  self->prime_x.init = Prime_init;
  self->prime_y.init = Prime_init;
  self->prime_x.init(x->field.x, &self->prime_x);
  self->prime_y.init(y->field.x, &self->prime_y);
  bprint("bpinit", self->prime_x.field.x);
  bprint("bpinit", self->prime_y.field.x);
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
  exit(0); // TODO: Break here for now

  Order order1, order2;
  order1.initint = Order_initint;
  order2.initint = Order_initint;
  order1.initint(42, p1, &order1);
  //order1.initint(42, &order1);
  //order2.initint(24, &order2);
  order2.initint(24, p1, &order2);
  bint x[1], y[1];
//  uint32_t x[8] = {0}, y[8] = {0};
  Prime px, py, prime_retp, prime_retq, pz, po, qo;
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
  //pz.initint(0, &pz);
  po.init(order1.field.x, &po);
  qo.init(order2.field.x, &qo);

  Point G, G2, GR, p, q, pop, qop, infinity;
  //printuint32("PTXXX", x);
  //printuint32("PTXXX", px.field.x);
  //printuint32("PTXXX", y);
  //printuint32("PTXXX", py.field.x);
  G.init = Point_init;
  G2.init = Point_init;
  GR.init = Point_init;
  p.init = Point_init;
  q.init = Point_init;
  pop.init = Point_init;
  qop.init = Point_init;
  infinity.init = Point_init;
  G.init(&px, &py, &G);
  printf("p----\n");
  //G->init(&px, &py, &G);
  //GR->init(&pz, &pz, GR);
  printf("pp---\n");
  G2.init(&po, &py, &G2);
  bprint("Gpo", po.field.x);
  bprint("Gqo", qo.field.x);
  bprint("GX", G.prime_x.field.x);
  bprint("GY", G.prime_y.field.x);
  //G.init(&po, &py, &G2);
  printf("p----\n");

  //printuint32("PTXXX", G.prime_x.field.x);
  //printuint32("PTXXX", G.prime_y.field.x);

  GR.init(&pz, &pz, &GR);

  p.init(&pz, &pz, &p);
  q.init(&pz, &pz, &q);
  infinity.init(&pz, &pz, &infinity);
  pop.init(&po, &po, &pop);//&pz, &pop);
  qop.init(&qo, &qo, &qop);//&pz, &qop);

  bprint("GpopX", pop.prime_x.field.x);
  bprint("GqopY", qop.prime_y.field.x);
  //G.init(&po, &py, &G2);
  //printuint32("PTXXX", pop.prime_x.field.x);
  //printuint32("PTXXX", qop.prime_x.field.x);
  //G.init(&px, &py, &G);
  printf("xxxxxxxx\n");
  p.mul = Point_mul;
  q.mul = Point_mul;
  p.mul(&p, &G, &pop);
  q.mul(&q, &G, &qop);
  //Point_mul(&p, &G, &pop); // p = c.G * Order(42)
  //exit(0);
  //Point_mul(&q, &G, &qop); // q = c.G * Order(24)
  //exit(0);
  bprint("Gp", pop.prime_x.field.x);
  bprint("Gq", qop.prime_y.field.x);
  bprint("G", G.prime_x.field.x);
  bprint("G", G.prime_y.field.x);
  bprint("P", p.prime_x.field.x);
  bprint("P", p.prime_y.field.x);
  bprint("Q", q.prime_x.field.x);
  bprint("Q", q.prime_y.field.x);


  bint t1, t2, t3, t4, t5, t6;
  wrd2bint(&t1, 0x2a);
  wrd2bint(&t2, 0x18);
  str2bint(&t3, "0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"); // point gx
  str2bint(&t4, "0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"); // point gy

  Point pt1, pt2, pt3;
  pt1.init = Point_init;
  pt2.init = Point_init;
  pt1.init(&t1, &t2, &pt1);
  pt2.init(&t3, &t4, &pt2);
  pt3.init = Point_init;
  pt3.init(&pz, &pz, &pt3);
  pt3.mul = Point_mul;
  pt3.mul(&pt3, &pt1, &pt2);
  bprint("P3", pt3.prime_x.field.x);
  bprint("P3", pt3.prime_y.field.x);
  exit(0);
  //exit(0); ////  -----------------------------------
  //exit(0);
  //G.init(&po, &py, &G2);
  //Point_mul(&p, &G, &pop); // p = c.G * Order(42)
  //Point_mul(&q, &G, &qop); // q = c.G * Order(24)

  //printuint32("PX", p.prime_x.x);
  //printuint32("PY", p.prime_y.x);
  //printuint32("GX", G.prime_x.x);
  //printuint32("GY", G.prime_y.x);

  //printuint32("PTXXX", p.prime_x.field.x);
  //printuint32("PTXXX", p.prime_y.field.x);
  //printuint32("PTXXX", q.prime_x.field.x);
  //printuint32("PTXXX", q.prime_y.field.x);
  //r = Point(p.x, -p.y)

  // assert p + q == c.G * Order(66)
  GR.add = Point_add;
  bprint("pp", p.prime_x.field.x);
  bprint("pp", p.prime_y.field.x);
  bprint("qq", q.prime_x.field.x);
  bprint("qq", q.prime_y.field.x);
  //exit(0);
  GR.add(&GR, &p, &q);
  //exit(0);
//  Point_add(&GR, &p, &q);///&pop, &qop); // p + q
  bprint("p+q", GR.prime_x.field.x);
  bprint("p+q", GR.prime_y.field.x);
  //exit(0);
  order1.initint(66, p1, &order1);
  //order1.initint(66, &order1);
  po.init(order1.field.x, &po);
  pop.init(&po, &pz, &pop);
  Point_mul(&G2, &G, &pop);// c.G * Order(66)
  //printuint32("GRx", GR.prime_x.field.x);
  //printuint32("G2x", G2.prime_x.field.x);
  bprint("GRx", GR.prime_x.field.x);
  bprint("G2x", G2.prime_x.field.x);
  bprint("order1", order1.field.x);
  bprint("po", po.field.x);

  bprint("p+q", GR.prime_x.field.x);
  bprint("p+q", GR.prime_y.field.x);
}

void Ftester_sanity(void) {
  bint aaa, bbb, ccc, r1, ddd, eee, hhh;
  wrd2bint(&aaa, 666);
  wrd2bint(&bbb, 999);
  wrd2bint(&r1, 333);
  bmod(&ccc, &bbb, &aaa);
  assert(cmp(&ccc, &r1) == 0);
  str2bint(&ddd, "0x6666666666666666666666666666666666666666666666666666666666666666");
  str2bint(&eee, "0x3333333333333333333333333333333333333333333333333333333333333333");
  bmod(&hhh, &ddd, &eee);
  assert(cmp(&hhh, &eee) == 0);
}


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

void Field_initint(uint32_t x, Field *self) {
  bint x1[1];
  wrd2bint(x1, x);
  str2bint(self->p, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  bmod(self->x, x1, self->p);
}

int8_t Field_eq(Field *data, Field *self) {
  assert(cmp(data->p, self->p) == 0);
  return cmp(data->p, self->p);
}

void Field_add(Field *ret, Field *data, Field *self) {
  assert(cmp(data->p, self->p) == 0);
  bint tmp;
  self->x->neg = 0; //
  data->x->neg = 0; //
  badd(&tmp, self->x, data->x);
  bmod(ret->x, &tmp, self->p); // (self.x + data.x) % self.p

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
  f.initint(0, &f);
  o.initint = Field_initint;
  o.initint(1, &o);
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
  self->field.initint = Field_initint;
  self->field.initint(0, &self->field);
  memcpy(self->field.x, x, sizeof(bint));
  str2bint(self->N, "0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
}

void Order_initint(uint32_t x, Order *self) {
  bint x1[1];
  wrd2bint(x1, x);
  self->field.initint = Field_initint;
  self->field.initint(0, &self->field);
  memcpy(self->field.x, x1, sizeof(bint));
  str2bint(self->N, "0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
}


// ----- Prime class -----
void Prime_init(bint *x, Prime *self) {
  self->field.initint = Field_initint;
  self->field.initint(0, &self->field);
  memcpy(self->field.x, x, sizeof(bint));
  str2bint(self->P, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
} 

void Prime_initint(uint32_t x, Prime *self) {
  bint x1[1];
  wrd2bint(x1, x);
  self->field.initint = Field_initint;
  self->field.initint(0, &self->field);
  memcpy(self->field.x, x1, sizeof(bint));
  str2bint(self->P, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
}


// ----- Point class -----
void Point_init(Prime *x, Prime *y, Point *self) {
  self->prime_x.initint = Prime_initint;
  self->prime_y.initint = Prime_initint;
  self->prime_x.initint(0, &self->prime_x);
  self->prime_y.initint(0, &self->prime_y);
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
//  memcpy(y1->wrd, y->field.x, LEN * sizeof(uint32_t));
//  memcpy(y1->wrd, y->field.x, LEN * sizeof(uint32_t));
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

/*
  Prime p0;
  printf("p0ini\n");
  p0.initint = Prime_initint;
  printf("init1\n");
  p0.initint(0, &p0);
  printf("init2\n");
//  x->field.initint(0, &x->field);
//  y->field.initint(0, &y->field);
//  str2uint32(self->P, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
//  str2uint32(self->P, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
 // memcpy32(x->field.p, );
//  memcpy32(y->field.p, );
  printf("init3\n");
  //printuint32("PT init1x", x->field.x);
  //printuint32("PT init1y", y->field.x);

  bprint("point xpx", x->field.x);

  bprint("point ypx", y->field.x);
//  str2bint(x->field.x, "0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"); // point gx

//  str2bint(x->field.x, "0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"); // point gx
//  str2bint(y->field.x, "0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"); // point gy

  printf("init4\n");
  //printuint32("PT init1x", x->field.x);
  //printuint32("PT init1y", y->field.x);

//  str2uint32(x, "0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"); // point gx
//  str2uint32(y, "0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"); // point gy
  if (cmp(p0.field.x, x->field.x) || cmp(p0.field.x, y->field.x)) {

    Prime xx, xxx, yy, ax, b;
    xx.initint = Prime_initint;
    xx.initint(0, &xx);
    xxx.initint = Prime_initint;
    xxx.initint(0, &xxx);
    yy.initint = Prime_initint;
    yy.initint(0, &yy);
    ax.initint = Prime_initint;
    ax.initint(0, &ax);
    b.initint = Prime_initint;
    b.initint(7, &b);

    //printuint32("PT XX", xx.field.x);
    Field_mul(&xx.field, &x->field, &x->field);
    //printuint32("PT XX", xx.field.x);

    Field_mul(&xxx.field, &xx.field, &x->field);
    //printuint32("PT XXX", xxx.field.x);

    Field_mul(&yy.field, &y->field, &y->field);
    Field_add(&ax.field, &xxx.field, &b.field);

    printf("point init\n");
    //printuint32("AX", ax.field.x);
    //printuint32("XX", xxx.field.x);
    assert(Field_eq(&ax.field, &xxx.field) == 0);

  }
*/
  printf("p1\n");
  self->prime_x.init = Prime_init;
  self->prime_y.init = Prime_init;
  self->prime_x.init(x->field.x, &self->prime_x);
  self->prime_y.init(y->field.x, &self->prime_y);
  bprint("bpinit", self->prime_x.field.x);
  bprint("bpinit", self->prime_y.field.x);
  //printuint32("PT initfx", x->field.x);
  //printuint32("PT initfy", y->field.x);
  ////printuint32("PT init x", self->prime_x.x);
  ////printuint32("PT init y", self->prime_y.x);
}

void Point_add(Point *ret, Point *data, Point *self) {
  Prime x1, x2, y1, y2;
  x1.init = Prime_init;
  x2.init = Prime_init;
  y1.init = Prime_init;
  y2.init = Prime_init;

  x1.init(self->prime_x.field.x, &x1);
  x2.init(data->prime_x.field.x, &x2);
  y1.init(self->prime_x.field.x, &y1);
  y2.init(data->prime_x.field.x, &y2);
//  memcpy(&x1, &self->prime_x, sizeof(Prime));
//  memcpy(&x2, &data->prime_x, sizeof(Prime));
//  memcpy(&y1, &self->prime_y, sizeof(Prime));
//  memcpy(&y2, &data->prime_y, sizeof(Prime));

  Prime p0;
  p0.initint = Prime_initint;
  p0.initint(0, &p0);
  if (Field_eq(&x1.field, &p0.field) == 0 && Field_eq(&y1.field, &p0.field) == 0) {printf("MULx1y1eq\n");memcpy(ret, data, sizeof(Point)); bprint("MUL dat", data->prime_x.field.x); bprint("MUL dat", data->prime_y.field.x); return;}
  if (Field_eq(&x2.field, &p0.field) == 0 && Field_eq(&y2.field, &p0.field) == 0) {printf("MULx2y2eq\n");memcpy(ret, self, sizeof(Point)); return;}
  if (Field_eq(&x1.field, &x2.field) == 0 && Field_eq(&y1.field, &y2.field) == 0) { // y2 pos?
    Prime x1x1, xa, yy1, x3, y3, sk;
    x1x1.field.initint = Field_initint;
    x1x1.field.initint(0, &x1x1.field);
    x1x1.field.mul(&x1x1.field, &x1.field, &x1.field);
    xa.field.initint = Field_initint;
    xa.field.initint(0, &xa.field);
    xa.field.add(&xa.field, &x1x1.field, &x1x1.field);
    xa.field.add(&xa.field, &xa.field, &x1x1.field);
    yy1.field.initint = Field_initint;
    yy1.field.initint(0, &yy1.field);
    yy1.field.add(&yy1.field, &y1.field, &y1.field);

    sk.field.initint = Field_initint;
    sk.field.initint(0, &sk.field);

    x3.field.initint = Field_initint;
    x3.field.initint(0, &x3.field);

    y3.field.initint = Field_initint;
    y3.field.initint(0, &y3.field);
    sk.field.div(&sk.field, &xa.field, &yy1.field);
    x3.field.mul(&x3.field, &sk.field, &sk.field);
    x3.field.sub(&x3.field, &x3.field, &x1.field);
    x3.field.sub(&x3.field, &x3.field, &x2.field); // x3 = sk * sk - x1 - x2
    y3.field.sub(&y3.field, &x1.field, &x3.field);
    y3.field.mul(&y3.field, &sk.field, &x3.field);
    y3.field.sub(&y3.field, &y3.field, &y1.field); // y3 = sk * (x1 - x3) - y1
  ////printuint32("MULx31", x3.x);
  ////printuint32("MULy31", y3.x);
    Point r;
    r.init = Point_init;
    r.init(&x3, &y3, &r);
    memcpy(&ret, &r, sizeof(Point));
    printf("pa end2\n");
    return; // return Point(x3, y3)
  }
  if (Field_eq(&x1.field, &x2.field) == 0 && Field_eq(&y1.field, &y2.field) == 0) { // y2 neg?
    memcpy(&ret->prime_x, &p0, sizeof(Prime)); // Infinity
    memcpy(&ret->prime_y, &p0, sizeof(Prime));
    printf("MULinf\n");
    return;
  }
  Prime sk, x3, y3, x21, y21;
  sk.field.initint = Field_initint;
  sk.field.initint(0, &sk.field);
  x3.field.initint = Field_initint;
  x3.field.initint(0, &x3.field);
  y3.field.initint = Field_initint;
  y3.field.initint(0, &y3.field);
  y21.field.initint = Field_initint;
  y21.field.initint(0, &y21.field);
  x21.field.initint = Field_initint;
  x21.field.initint(0, &x21.field);
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
  r.init(&x3, &y3, &r);
  //printuint32("MULx3", x3.x);
  //printuint32("MULy3", y3.x);
  memcpy(&ret, &r, sizeof(Point));
  printf("pa end3\n");
  return; // return Point(x3, y3)
}

void Point_mul(Point *ret, Point *self, Point *k) { // TODO: this actually work?!
  //uint32_t n[8] = {0}, b[8] = {0}, tw[8] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000002};
  bint n[1], z[1], o[1], tw[1], b[1];
  Prime pz; // Point multiplication: Double-and-add https://en.wikipedia.org/wiki/Elliptic_curve_point_multiplication
  Point result, addend;
  wrd2bint(z, 0);
  wrd2bint(o, 1);
  wrd2bint(tw,2);
  o->siz = 30;
  n->siz = 30;
  pz.initint = Prime_initint;
  pz.initint(0, &pz);
  result.init = Point_init;
  addend.init = Point_init;
  result.init(&pz, &pz, &result);
  addend.init(&self->prime_x, &self->prime_y, &addend);
//  addend.init(&pz, &pz, &addend);
  memcpy(n, k->prime_x.field.x, sizeof(bint));
  //printuint32("K primex", k->prime_x.x);
  //printuint32("S primex", self->prime_x.x);
  //uint64_t co = 0;
  while (cmp(n, z) == 1) {// && co < 8) { // TODO: no counter ofc
    //printf("pre mod\n");
    //printuint32("mod b", b);
    //printuint32("mod n", n);
    //printuint32("mod 2", tw);
    bprint("GNn", n);
    bprint("GNt", tw);
    bprint("GNo", o);
    bmod(b, n, tw);
    bprint("GBbb", b);
    printf("aft mod\n");
    printf("GNcmp %d\n", cmp(b, o));
    if (cmp(b, o) == 0) {Point_add(&result, &result, &addend); printf("Gnnn ODD\n");}
    Point_add(&addend, &addend, &addend);
    //printuint32("nnn b", b);
    printf("nnn b111 %d\n", cmp(b, o));
    //printuint32("nnn resu", result.prime_x.x);
    //printuint32("nnn adde", addend.prime_x.x);
    //printuint32("Nnnnnn", n);
    brshift(n, n, 1);//1);
    //shift_right(n, 1);
    //printuint32("Nnnnnn", n);
    //co++;
  }
  bprint("Gpr1x", result.prime_x.field.x);
  bprint("Gpr1y", result.prime_y.field.x);

  memcpy(ret, &result, sizeof(Point));
  bprint("Gpr2x", ret->prime_x.field.x);
  bprint("Gpr2y", ret->prime_y.field.x);
}

/*
    n, result, addend = k.x, I, self
    while n:
      b = n & 1
      if b == 1: result += addend
      addend = addend + addend
      n = n >> 1
    return result
*/


/*
void tester(void) {
  Field field0, field1, field2, field3;
  printf("--------- Testvalues -------------------------------------------\n");
  uint32_t tmp[8] = {0,0,0,0,0,0,0,23};
  field0.initint = Field_initint;
  field1.initint = Field_initint;
  field2.initint = Field_initint;
  field3.initint = Field_initint;
  memcpy32(field0.p, tmp);
  memcpy32(field1.p, tmp);
  memcpy32(field2.p, tmp);
  memcpy32(field3.p, tmp);
  field0.initint(0, &field0);
  field1.initint(12, &field1);
  field2.initint(20, &field2);
  field3.initint(9, &field3);
  field0.add = Field_add;
  field0.add(&field0, &field1, &field2);
  field0.eq = Field_eq;
  printf("eq: %d\n", field0.eq(&field0, &field3)); //assert Field(12) + Field(20) == Field(9)
  field0.initint(0, &field0);
  field1.initint(8, &field1);
  field2.initint(9, &field2);
  field3.initint(3, &field3);
  field0.add(&field0, &field1, &field2);
  printf("eq: %d\n", field0.eq(&field0, &field3)); //assert Field(8) * Field(9) == Field(3)

  memcpy32(field0.p, zero);
  memcpy32(field1.p, zero);
  memcpy32(field2.p, zero);
  memcpy32(field3.p, zero);

  Order order1, order2;
  order1.initint = Order_initint;
  order2.initint = Order_initint;
  order1.initint(42, &order1);
  order2.initint(24, &order2);

  uint32_t x[8] = {0}, y[8] = {0};
  Prime px, py, prime_retp, prime_retq, pz, po, qo;
  px.init = Prime_init;
  py.init = Prime_init;
  pz.initint = Prime_initint;
  po.init = Prime_init;
  qo.init = Prime_init;
  str2uint32(x, "0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"); // point gx
  str2uint32(y, "0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"); // point gy
  px.init(x, &px);
  py.init(y, &py);
  pz.initint(0, &pz);
  po.init(order1.field.x, &po);
  qo.init(order2.field.x, &qo);
  Point G, G2, GR, p, q, pop, qop, infinity;
  printuint32("PTXXX", x);
  printuint32("PTXXX", px.field.x);
  printuint32("PTXXX", y);
  printuint32("PTXXX", py.field.x);
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

  //G.init(&po, &py, &G2);
  printf("p----\n");

  printuint32("PTXXX", G.prime_x.field.x);
  printuint32("PTXXX", G.prime_y.field.x);

  GR.init(&pz, &pz, &GR);

  p.init(&pz, &pz, &p);
  q.init(&pz, &pz, &q);
  infinity.init(&pz, &pz, &infinity);
  pop.init(&po, &po, &pop);//&pz, &pop);
  qop.init(&qo, &qo, &qop);//&pz, &qop);


  printuint32("PTXXX", pop.prime_x.field.x);
  printuint32("PTXXX", qop.prime_x.field.x);
  //G.init(&px, &py, &G);
  printf("xxxxxxxx\n");
  Point_mul(&p, &G, &pop); // p = c.G * Order(42)
  Point_mul(&q, &G, &qop); // q = c.G * Order(24)
  //printuint32("PX", p.prime_x.x);
  //printuint32("PY", p.prime_y.x);
  //printuint32("GX", G.prime_x.x);
  //printuint32("GY", G.prime_y.x);

  printuint32("PTXXX", p.prime_x.field.x);
  printuint32("PTXXX", p.prime_y.field.x);
  printuint32("PTXXX", q.prime_x.field.x);
  printuint32("PTXXX", q.prime_y.field.x);
  //r = Point(p.x, -p.y)

  // assert p + q == c.G * Order(66)

  Point_add(&GR, &p, &q);///&pop, &qop); // p + q
  order1.initint(66, &order1);
  po.init(order1.field.x, &po);
  pop.init(&po, &po, &pop);
  Point_mul(&G2, &G, &pop);// c.G * Order(66)
  printuint32("GRx", GR.prime_x.field.x);
  printuint32("G2x", G2.prime_x.field.x);

}

int main(void) {
  uint32_t a[LEN32], b[LEN32], c[LEN32], cm[LEN32*2], d[LEN32];
  str2uint32(a, "0x33333335333333333333363333333333333333333333a3333333333333333333");
  str2uint32(b, "0x6666666666666666666666666666666666666666666666666666666666666666");
  add(c, a, b);
  printuint32("C ", c);
  memset(c, 0, LEN32 * sizeof(uint32_t));
  str2uint32(a, "0x33333335333333333333363333333333333333333333a3333333333333333333");
  str2uint32(b, "0x6666666666666666666666666666666666666666666666666666666666666666");
  sub(c, b, a);
  printuint32("C ", c);
  str2uint32(a, "0x33333335333333333333363333333333333333333333a3333333333333333333");
  str2uint32(b, "0x6666666666666666666666666666666666666666666666666666666666666666");
  mul(cm, a, b);
  printuint32("C1", cm + LEN32);
  printuint32("C2", cm);

  str2uint32(b, "0x6666666666666666666666666666666666666666666666666666666666666666");
  shift_right(b, 1);
  printuint32("B ", b);

  str2uint32(b, "0x6666666666666666666666666666666666666666666666666666666666666666");
  str2uint32(a, "0x3333333333333333333333333333333333333333333333333333333333333333");
  memset(c, 0, LEN32 * sizeof(uint32_t));
  memset(d, 0, LEN32 * sizeof(uint32_t));
  division(b, a, c);
  printuint32("C ", c);

  str2uint32(b, "0x6666666666666666666666666666666666666666666666666666666666666666");
  str2uint32(a, "0x3333333333333333333333333333333333333333333333333333333333333333");
  memset(c, 0, LEN32 * sizeof(uint32_t));
  mod(c, a, b);
  printuint32("C ", c);

  str2uint32(b, "0x6666666666666666666666666666666666666666666666666666666666666666");
  str2uint32(a, "0x3333333333333333333333333333333333333333333333333333333333333333");
  memset(c, 0, LEN32 * sizeof(uint32_t));
  mod(c, b, a);
  printuint32("C ", c);

  printf("--- tester ---\n");
  tester();
  printf("OK\n");
}
*/



void Ftester(void) {
  bint b, a[1], c[1];
  Field fff, ggg;
  str2bint(a, "0x33333335333333333333363333333333333333333333a3333333333333333333");
  str2bint(&b, "0x33333335333333333333363333333333333333333333a3333333333333333333");
  fff.initint = Field_initint;
  ggg.init = Field_init;
  ggg.init(&b, &ggg);
  fff.initint(666, &fff);
 

  Field field0, field1, field2, field3;
  printf("--------- Testvalues -------------------------------------------\n");
  uint32_t tmp[8] = {0,0,0,0,0,0,0,23};
  field0.initint = Field_initint;
  field1.initint = Field_initint;
  field2.initint = Field_initint;
  field3.initint = Field_initint;
  memcpy(field0.p, tmp, 8 * sizeof(uint32_t));
  memcpy(field1.p, tmp, 8 * sizeof(uint32_t));
  memcpy(field2.p, tmp, 8 * sizeof(uint32_t));
  memcpy(field3.p, tmp, 8 * sizeof(uint32_t));
  field0.initint(0, &field0);
  field1.initint(12, &field1);
  field2.initint(20, &field2);
  field3.initint(9, &field3);
  field0.add = Field_add;
  field0.add(&field0, &field1, &field2);
  field0.eq = Field_eq;
  printf("eq: %d\n", field0.eq(&field0, &field3)); //assert Field(12) + Field(20) == Field(9)
  field0.initint(0, &field0);
  field1.initint(8, &field1);
  field2.initint(9, &field2);
  field3.initint(3, &field3);
  field0.add(&field0, &field1, &field2);
  printf("eq: %d\n", field0.eq(&field0, &field3)); //assert Field(8) * Field(9) == Field(3)

  Order order1, order2;
  order1.initint = Order_initint;
  order2.initint = Order_initint;
  order1.initint(42, &order1);
  order2.initint(24, &order2);

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
  pz.initint(0, &pz);
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

  Point_mul(&p, &G, &pop); // p = c.G * Order(42)
  Point_mul(&q, &G, &qop); // q = c.G * Order(24)

  bprint("Gp", pop.prime_x.field.x);
  bprint("Gq", qop.prime_y.field.x);
  //G.init(&po, &py, &G2);
  //Point_mul(&p, &G, &pop); // p = c.G * Order(42)
  //Point_mul(&q, &G, &qop); // q = c.G * Order(24)

/*


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

  Point_add(&GR, &p, &q);///&pop, &qop); // p + q
  bprint("p+q", GR.prime_x.field.x);
  bprint("p+q", GR.prime_y.field.x);

  order1.initint(66, &order1);
  po.init(order1.field.x, &po);
  pop.init(&po, &po, &pop);
  Point_mul(&G2, &G, &pop);// c.G * Order(66)
  //printuint32("GRx", GR.prime_x.field.x);
  //printuint32("G2x", G2.prime_x.field.x);
  bprint("GRx", GR.prime_x.field.x);
  bprint("G2x", G2.prime_x.field.x);
*/
/*
  bint aaa, bbb, ccc;
  wrd2bint(&aaa, 666);
  wrd2bint(&bbb, 999);
  bprint("aaa", &aaa);
  bprint("bbb", &bbb);
  bmod(&ccc, &bbb, &aaa);//&aaa, &bbb);
  bprint("ccc", &ccc);

  bint ddd, eee, hhh;
  hhh.siz = 30;
  eee.siz = 30;
  str2bint(&ddd, "0x6666666666666666666666666666666666666666666666666666666666666666");
  str2bint(&eee, "0x3333333333333333333333333333333333333333333333333333333333333333");
  hhh.siz = 8;
  eee.siz = 8;
  ddd.siz = 8;
  //memset(&hhh, 0, LEN * sizeof(uint32_t));
  bmod(&hhh, &ddd, &eee);
  hhh.siz = 8;
  bprint("hhh", &hhh);
*/
}


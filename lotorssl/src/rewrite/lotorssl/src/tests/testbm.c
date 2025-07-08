// Auth: smurfd, 2025 More reading at the bottom of the file; 2 spacs indent; 150 width                                                             //
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "../bm.h"
#include "../bmec.h"
//#include "../bmcurve.h"

void tester_bint_sanity(void) {
  bint a = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, b = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, c = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint d = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  str2bint(&a, "0x3b6859c358bb6fa30b3f11ff6c29164dc21b2abaf4c2027ea8e6701e99dd5b7c");
  str2bint(&b, "0xd7dab791a8647ac88695e20e29960a6fd41707258cc88cc0480ea7e5bb3f132f");
  bmul(&c, &a, &b);
  // 0x321764d8e538429bc4539ec57462be61fa493fd0c700db4f19c51549d9c6aad6904bf1b900775bdf11da25e74e4d698c386446a85b7f1afaf9062442c4f1ffc4
  uint32_t xxx[LEN] = {0x321764d8, 0xe538429b, 0xc4539ec5, 0x7462be61, 0xfa493fd0, 0xc700db4f, 0x19c51549, 0xd9c6aad6, 0x904bf1b9, 0x00775bdf,
    0x11da25e7, 0x4e4d698c, 0x386446a8, 0x5b7f1afa, 0xf9062442, 0xc4f1ffc4};
  for (int i = 0; i < 15; i++) {assert(c.wrd[i] == xxx[15-i]);}
  bmul(&d, &c, &a);
  // 0xb9fcf4f502f06b563974c8fd64b52750b925ec4138d28632fc84b3f8eedf7c7d471a809102365772e1eefc491b090a624584b32a122142b07a6c3aefc791d81cf527e4053ea119ffa86f73189e82d3ae2650192831a8fc8618fac175b568ef0
  uint32_t yyy[LEN] = {0x0b9fcf4f, 0x502f06b5, 0x63974c8f, 0xd64b5275, 0x0b925ec4, 0x138d2863, 0x2fc84b3f, 0x8eedf7c7, 0xd471a809, 0x10236577,
    0x2e1eefc4, 0x91b090a6, 0x24584b32, 0xa122142b, 0x07a6c3ae, 0xfc791d81, 0xcf527e40, 0x53ea119f, 0xfa86f731, 0x89e82d3a, 0xe2650192, 0x831a8fc8,
    0x618fac17, 0x5b568ef0};
  for (int i = 0; i < 24; i++) assert(yyy[23 - i] == d.wrd[i]);
}

void tester_bint_div_sanity(void) {
  bint a = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, b = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, c = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint tmp = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, one = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  str2bint(&a, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(&b, "0x9075b4ee4d4788cabb49f7f81c221151fa2f68914d0aa833388fa11ff621a970");
  bdiv(&c, &tmp, &a, &b);
  wrd2bint(&one, 1);
  assert(cmp(&c, &one) == 0);
}

void tester_bint_mod_sanity(void) {
  bint a = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, b = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, c = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint tmp = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, ret = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, r1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint tmp2 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, r2 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  str2bint(&a, "0x34ca4d7bd7efe5cfc1462edca66d539c0a77a83d09ce1196c92c514312232a7e");
  str2bint(&c, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(&b, "0x9ce3bc8a331e7860fe957feaab820c680af219522713db054053fdc5b41f424d");
  str2bint(&r1, "0x32339163b1c163c5132aeb93cffbd83fc98b2045f072b3772675ca04684cbf67");
  str2bint(&r2, "0xcdcc6e9c4e3e9c3aecd5146c300427c03674dfba0f8d4c88d98a35fa97b33cc8");
  a.neg = 0;
  bmul(&tmp, &a, &b);
  bmod(&ret, &tmp2, &tmp, &c);
  assert(cmp(&ret, &r1) == 0);
  a.neg = 1;
  bmul(&tmp, &a, &b);
  bmod(&ret, &tmp2, &tmp, &c);
  assert(cmp(&ret, &r2) == 0);
}

void tester_bint_2ways_sanity(void) {
  bint a = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, b = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, c = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint d = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, e = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, t = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  str2bint(&a, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  wrd2bint(&b, 0x33333333);
  wrd2bint(&c, 0x3);
  wrd2bint(&d, 0x33);
  for (int i = 0; i < 20000; i++) bsub(&a, &a, &b);
  for (int i = 0; i < 20000; i++) badd(&a, &a, &b);
  assert(cmp(str2bint(&b, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"), &a) == 0);
  for (int i = 0; i < 200; i++) {
    bmul(&e, &d, &c);
    memcpy(&d.wrd, e.wrd, e.siz * sizeof(uint32_t));
    d.siz = e.siz;
  }
  for (int i = 0; i < 200; i++) {
    bint ee = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, tt = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bdiv(&ee, &tt, &d, &c);
    memcpy(&d, &ee, sizeof(bint));
  }
  assert(cmp(wrd2bint(&e, 0x33), &d) == 0);
  memset(a.wrd, 0, LEN * sizeof(uint32_t));
  memset(d.wrd, 0, LEN * sizeof(uint32_t));
  memset(e.wrd, 0, LEN * sizeof(uint32_t));
  memset(t.wrd, 0, LEN * sizeof(uint32_t));
  a.neg = d.neg = e.neg = t.neg = a.cap = d.cap = e.cap = t.cap = 0;
  a.siz = d.siz = e.siz = t.siz = 1;
  str2bint(&a, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(&d, "0x0000000000000000000000000000000000000000000000000000003333333333");
  bdiv(&e, &t, &a, &d);
  str2bint(&d, "0x000000000500000000050000000005000000000500000000050000000004ffff");
  assert(cmp(&d, &e) == 0);
}
/*

void tester_field(void) {
  bint p1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, p2 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, p3 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint x = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, y = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  prime px, py, pz, po1, po2, po3;
  point G1, G2, G3, infinity;
  field f0, f1, f2, f3;
  order o1, o2, o3;
  wrd2bint(&p1, 23);
  wrd2bint(&p2, 0);
  field_initint(&f0, 0, &p1);
  field_initint(&f1, 12, &p1);
  field_initint(&f2, 20, &p1);
  field_initint(&f3, 9, &p1);
  field_add(&f0, &f1, &f2);
  assert(field_eq(&f0, &f3) == 0); // Field(12) + Field(20) == Field(9)

  field_initint(&f0, 0, &p1);
  field_initint(&f1, 8, &p1);
  field_initint(&f2, 9, &p1);
  field_initint(&f3, 3, &p1);
  field_mul(&f0, &f1, &f2);
  assert(field_eq(&f0, &f3) == 0); // Field(8) * Field(9) == Field(3)

  order_initint(&o1, 42, &p2);
  order_initint(&o2, 24, &p2);
  order_initint(&o3, 66, &p2);

  str2bint(&p3, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  prime_init(&px, str2bint(&x, "0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"), &p2); // point gx
  prime_init(&py, str2bint(&y, "0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"), &p2); // point gy
  prime_initint(&pz, 0, &p2);

  prime_init(&po1, o1.f.x, &p2);
  prime_init(&po2, o2.f.x, &p2);
  prime_init(&po3, o3.f.x, &p2);

  point_init(&infinity, &pz, &pz, &p2);
  point_init(&G1, &pz, &pz, &p3);

  memcpy(G1.prime_x.f.x->wrd, px.f.x->wrd, px.f.x->siz * sizeof(uint32_t));
  memcpy(G1.prime_y.f.x->wrd, py.f.x->wrd, py.f.x->siz * sizeof(uint32_t));

  memcpy(G1.prime_x.f.p->wrd, p2.wrd, p2.siz * sizeof(uint32_t));
  memcpy(G1.prime_y.f.p->wrd, p2.wrd, p2.siz * sizeof(uint32_t));
  G1.prime_x.f.x->siz = px.f.x->siz;
  G1.prime_x.f.x->neg = 0;

  G1.prime_y.f.x->siz = py.f.x->siz;
  G1.prime_y.f.x->neg = 0;

  point_init(&G2, &pz, &pz, &p3);
  point_init(&G3, &pz, &pz, &p3);
  point_add(&G3, &G1, &G1, &p3);
  point_mul(&G2, &G1, &o1.f, &p3);
}

void tester_field_sanity(void) {
  bint aaa = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, bbb = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, ccc = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint ddd = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, hhh = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, jjj = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint r1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, r2 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, tmp = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  wrd2bint(&aaa, 666);
  wrd2bint(&bbb, 999);
  wrd2bint(&r1, 333);
  bmod(&ccc, &tmp, &bbb, &aaa);
  assert(cmp(&ccc, &r1) == 0);
  str2bint(&ddd, "0x6666666666666666666666666666666666666666666666666666666666666666");
  str2bint(&jjj, "0x9999999999999999999999999999999999999999999999999999999999999999");
  str2bint(&r2, "0x3333333333333333333333333333333333333333333333333333333333333333");
  bmod(&hhh, &tmp, &jjj, &ddd);
  assert(cmp(&hhh, &r2) == 0);
}

void tester_field_math_sanity(void) {
  field f1, f2, f3, fr;
  bint a = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, b = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, p = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint r1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, r2 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  str2bint(&a, "0x3b6859c358bb6fa30b3f11ff6c29164dc21b2abaf4c2027ea8e6701e99dd5b7c");
  str2bint(&b, "0xd7dab791a8647ac88695e20e29960a6fd41707258cc88cc0480ea7e5bb3f132f");
  str2bint(&p, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(&r1, "0x13431155011fea6b91d4f40d95bf20bd963231e0818a8f3ef0f51805551c727c");
  str2bint(&r2, "0xa0cc141790812f1cc95dd3da756f3d4930f1adbeef8929265e0a74480f6eb392");
  field_initint(&fr, 0, &p);
  field_init(&f1, &a, &p);
  field_init(&f2, &b, &p);
  field_add(&fr, &f1, &f2);
  assert(cmp((bint*)fr.x, &r1) == 0);
  field_mul(&f3, &f1, &f2);
  assert(cmp((bint*)f3.x, &r2) == 0);
}

void tester_field_PK(void) { // test public & private key creation and verification // TODO this is not working
  bint P = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, tmp = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  prime px, py, pz;
  order priv;
  point publ, G;
  str2bint(&P, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  order_init(&priv, str2bint(&tmp, "0x5f6717883bef25f45a129c11fcac1567d74bda5a9ad4cbffc8203c0da2a1473c"), &P); // priv key, TODO: randomize 
  prime_initint(&pz, 0, &P);
  prime_init(&px, str2bint(&tmp, "0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"), &P); // point gx
  prime_init(&py, str2bint(&tmp, "0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"), &P); // point gy
  point_init(&publ, &pz, &pz, &P);
  point_init(&G, &px, &py, &P);
  point_mul(&publ, &G, &priv.f, &P);
}
*/
void tester_bint_PK(void) {
  // TODO: check if the point is on curve
  bint CA = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, CB = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint CP = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, CN = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint CX = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, CY = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint CH = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  wrd2bint(&CA, 0);
  wrd2bint(&CB, 7);
  wrd2bint(&CH, 1);
  str2bint(&CP, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(&CN, "0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
  str2bint(&CX, "0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"); // point gx
  str2bint(&CY, "0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"); // point gy

  bint tmp1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, tmp2 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, res1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint alsk = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, alpkx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, alpky = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint bosk = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, bopkx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, bopky = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint alshrx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, boshrx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint alshry = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, boshry = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint r1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, r2 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};

  str2bint(&alsk, "0x024cd559cad384fad17951966dc5a25ae64671ae67b4b78c49725d38f35bfddb"); // TODO: randomize
  str2bint(&bosk, "0x702d7c3642f0aeedb279f623724bffd7932e077b3498a02bda95dbc0ee8725be"); // TODO: randomize
  bprint("Alice's private key", &alsk);
  bprint("Bob's private key", &bosk);
  scalar_mul(&alpkx, &alpky, &alsk, &CX, &CY, &CP, &CN);
  scalar_mul(&bopkx, &bopky, &bosk, &CX, &CY, &CP, &CN);
  bprint("Alice's public key x", &alpkx);
  bprint("Alice's public key y", &alpky);
  bprint("Bob's public key x", &bopkx);
  bprint("Bob's public key y", &bopky);

  scalar_mul(&alshrx, &alshry, &bosk, &alpkx, &alpky, &CP, &CN);
  scalar_mul(&boshrx, &boshry, &alsk, &bopkx, &bopky, &CP, &CN);
  bprint("Alice's share x", &alshrx);
  bprint("Alice's share y", &alshry);
  bprint("Bob's share x", &boshrx);
  bprint("Bob's share y", &boshry);
  assert(cmp(&alshrx, &boshrx) == 0 && cmp(&alshry, &boshry) ==0);
  bmul(&res1, &alsk, &bosk);
  bprint("Alice's & Bob's secret", &res1);
  bmod(&tmp1, &tmp2, &res1, &CN);
  scalar_mul(&r1, &r2, &tmp1, &CX, &CY, &CP, &CN);
  assert(cmp(&r1, &alshrx) == 0); // assert alices shared x is same as ((alicesecret * bobsecret) % N) scalar mult by curve G
}

int main(void) {
  tester_bint_sanity();
  tester_bint_2ways_sanity();
  tester_bint_div_sanity();
  tester_bint_mod_sanity();
  //tester_field_sanity();
  //tester_field_math_sanity();
  tester_bint_PK();
  printf("ok\n");
}


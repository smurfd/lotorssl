// Auth: smurfd, 2025 More reading at the bottom of the file; 2 spacs indent; 150 width                                                             //
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "../bm.h"
#include "../bmcurve.h"

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

bint *invmod(bint *ret, const bint *k, const bint *p) {
  bint s = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, ols = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint t = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, olt = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint r = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, olr = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint zero = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  int16_t kz = cmp(k, &zero);
  if (kz == 0) {
    return NULL;
  } else if (kz < 0) {
    bint ktmp = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bint rtmp = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    memcpy(&ktmp, k, sizeof(bint));
    ktmp.neg ^= 1;
    invmod(&rtmp, &ktmp, p);
    bsub(ret, p, &rtmp);
    return ret;
  }
  wrd2bint(&s, 0);
  wrd2bint(&ols, 1);
  wrd2bint(&t, 1);
  wrd2bint(&olt, 0);
  memcpy(&r, p, sizeof(bint));
  memcpy(&olr, k, sizeof(bint));
  while (cmp(&r, &zero) > 0) {
    bint tmp = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bint q = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bdiv(&q, &tmp, &olr, &r);  // q = old_r // r
    q.neg = 0; // TODO : force always positive quotation
    r.neg = 0;
    olr.neg = 0;
    bint tr = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, ts = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, tt = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    memcpy(&tr, &r, sizeof(bint));  // save r to tmp: tr = r
    memcpy(&ts, &s, sizeof(bint));  // save s to tmp: ts = s
    memcpy(&tt, &t, sizeof(bint));  // save t to tmp: tt = t
    tr.siz = r.siz;
    ts.siz = s.siz;
    tt.siz = t.siz;
    bint qr = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, qs = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, qt = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bmul(&qr, &q, &r); // qr = quot * r
    bmul(&qs, &q, &s); // qs = quot * s
    bmul(&qt, &q, &t); // qt = quot * t
    bint rr1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, ss1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, tt1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bsub(&rr1, &olr, &qr); // r = old_r - (quot * r)
    bsub(&ss1, &ols, &qs); // s = old_s - (quot * s)
    bsub(&tt1, &olt, &qt); // t = old_t - (quot * t)
    memset(r.wrd, 0, LEN * sizeof(uint32_t));
    memset(s.wrd, 0, LEN * sizeof(uint32_t));
    memset(t.wrd, 0, LEN * sizeof(uint32_t));
    memcpy(&r, &rr1, sizeof(bint));
    memcpy(&s, &ss1, sizeof(bint));
    memcpy(&t, &tt1, sizeof(bint));
    r.siz = rr1.siz;
    s.siz = ss1.siz;
    t.siz = tt1.siz;
    memset(olr.wrd, 0, LEN * sizeof(uint32_t));
    memset(ols.wrd, 0, LEN * sizeof(uint32_t));
    memset(olt.wrd, 0, LEN * sizeof(uint32_t));
    memcpy(&olr, &tr, sizeof(bint));
    memcpy(&ols, &ts, sizeof(bint));
    memcpy(&olt, &tt, sizeof(bint));
    olr.siz = tr.siz;
    ols.siz = ts.siz;
    olt.siz = tt.siz;
  }
  bint kx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, one = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, tmp = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint tmp3 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, tmp4 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint pn = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  str2bint(&pn, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2e");
  wrd2bint(&one, 1);
  assert(cmp(&olr, &one) == 0); // assert g == 1
  bmul(&kx, k, &ols);
  bmod(&tmp3, &tmp4, &kx, p);
  if (cmp(&tmp3, &pn) == 0) { // TODO: hack, negative sign for kx (ols or k) gets wrong somewhere
    kx.neg ^= 1;
    bmod(&tmp3, &tmp4, &kx, p);
  }
  assert(cmp(&tmp3, &one) == 0); // assert (k * x) % p == 1
  bmod(ret, &tmp, &ols, p);
  return ret;
}

void padd(bint *rx, bint *ry, bint *p1x, bint *p1y, bint *p2x, bint *p2y, bint *p) {
  bint zero = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, m = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  if (cmp(p1x, &zero) == 0 && cmp(p1y, &zero) == 0) {
    memcpy(rx, p2x, sizeof(bint));
    memcpy(ry, p2y, sizeof(bint));
    return;
  }
  if (cmp(p2x, &zero) == 0 && cmp(p2y, &zero) == 0) {
    memcpy(rx, p1x, sizeof(bint));
    memcpy(ry, p1y, sizeof(bint));
    return;
  }
  if (cmp(p1x, p2x) == 0 && cmp(p1y, p2y) != 0) {
    bint zx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, zy = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    memcpy(rx, &zx, sizeof(bint));
    memcpy(ry, &zy, sizeof(bint));
    return;
  }
  if (cmp(p1x, p2x) == 0) {
    bint xx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, xxx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bint xxx2 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, yy = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bint iy = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    badd(&xx, p1x, p1x);
    badd(&xxx, &xx, p1x);
    bmul(&xxx2, &xxx, p2x);
    badd(&yy, p1y, p1y);
    invmod(&iy, &yy, p);
    bmul(&m, &iy, &xxx2);
  } else {
    bint xx1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, yy1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, ix = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bsub(&yy1, p1y, p2y);
    bsub(&xx1, p1x, p2x);
    invmod(&ix, &xx1, p);
    bmul(&m, &ix, &yy1);
  }
  bint x3 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, y3 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint mm = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, mmx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint xx3 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, mmx3 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint tmp = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bmul(&mm, &m, &m);
  bsub(&mmx, &mm, p1x);
  bsub(&x3, &mmx, p2x);

  bsub(&xx3, &x3, p1x);
  bmul(&mmx3, &m, &xx3);
  badd(&y3, p1y, &mmx3);
  bmod(rx, &tmp, &x3, p);
  y3.neg ^= 1;
  bmod(ry, &tmp, &y3, p);
}

void sm(bint *rx, bint *ry, bint *k, bint *p1x, bint *p1y, bint *p, bint *n) {
  bint zero = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, q = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, tmp = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint kk = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  memcpy(&kk, k, sizeof(bint));
  bmod(&q, &tmp, &kk, n);
  if (cmp(&q, &zero) == 0 || (cmp(p1x, &zero) == 0 && cmp(p1y, &zero) == 0)) {
    bint zx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, zy = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    memcpy(rx, &zx, sizeof(bint));
    memcpy(ry, &zy, sizeof(bint));
    return;
  }
  if (cmp(&kk, &zero) < 0) {
    kk.neg ^= 1;
    p1x->neg ^= 1;
    p1y->neg ^= 1;
    sm(rx, ry, &kk, p1x, p1y, p, n);
    kk.neg ^= 1;
    p1x->neg ^= 1;
    p1y->neg ^= 1;
    return;
  }
  bint rsx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, rsy = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint ax = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, ay = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint tw = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, one = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  memcpy(&ax, p1x, sizeof(bint));
  memcpy(&ay, p1y, sizeof(bint));
  wrd2bint(&one, 1);
  wrd2bint(&tw, 2);
  while(cmp(&kk, &zero) != 0) {
    bint tmp1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, tmp2 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bmod(&tmp1, &tmp2, &kk, &tw);
    if (cmp(&tmp2, &one) == 0) {
    //if (cmp(&tmp1, &zero) == 0) {
      bint rsx1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, rsy1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
      padd(&rsx1, &rsy1, &rsx, &rsy, &ax, &ay, p);
      memcpy(&rsx, &rsx1, sizeof(bint));
      memcpy(&rsy, &rsy1, sizeof(bint));
    }
    padd(&ax, &ay, &ax, &ay, &ax, &ay, p);
    brshift(&kk, &kk, 1);
  }
  memcpy(rx, &rsx, sizeof(bint));
  memcpy(ry, &rsy, sizeof(bint));
}

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

  bint alsk = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, bosk = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint alpkx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, alpky = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint bopkx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, bopky = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};

  str2bint(&alsk, "0x024cd559cad384fad17951966dc5a25ae64671ae67b4b78c49725d38f35bfddb"); // TODO: randomize
  str2bint(&bosk, "0x702d7c3642f0aeedb279f623724bffd7932e077b3498a02bda95dbc0ee8725be"); // TODO: randomize

  bprint("alice prik", &alsk);
  bprint("bob prik", &bosk);
  sm(&alpkx, &alpky, &alsk, &CX, &CY, &CP, &CN);
  sm(&bopkx, &bopky, &bosk, &CX, &CY, &CP, &CN);
  bprint("alice pubk", &alpkx);
  bprint("alice pubk", &alpky);
  bprint("bob pubk", &bopkx);
  bprint("bob punk", &bopky);

  bint alshrx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, boshrx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint alshry = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, boshry = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  sm(&alshrx, &alshry, &bosk, &alpkx, &alpky, &CP, &CN);
  sm(&boshrx, &boshry, &alsk, &bopkx, &bopky, &CP, &CN);
  bprint("Alice share x", &alshrx);
  bprint("Alice share y", &alshry);
  bprint("Bob share x", &boshrx);
  bprint("Bob share y", &boshry);

  bint tmp1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, tmp2 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, res1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint r1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, r2 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bmul(&res1, &alsk, &bosk);
  bmod(&tmp1, &tmp2, &res1, &CN);
  bprint("tmp1", &tmp1);
  bprint("tmp2", &tmp2);
  sm(&r1, &r2, &tmp2, &CX, &CY, &CP, &CN); // TODO: this last part does not work, r1 shall be == alice share x
  bprint("R1", &r1);
  bprint("R2", &r2);
}

int main(void) {
  tester_bint_sanity();
  tester_bint_2ways_sanity();
  tester_bint_div_sanity();
  tester_bint_mod_sanity();
  tester_field_sanity();
  tester_field_math_sanity();
  tester_bint_PK();
  printf("ok\n");
}


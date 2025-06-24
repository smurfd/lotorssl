// Auth: smurfd, 2025 More reading at the bottom of the file; 2 spacs indent; 150 width                                                             //
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "../bm.h"
#include "../bmcurve.h"

void Btester_sanity(void) {
  bint a = {.wrd = {0}}, b = {.wrd = {0}}, c = {.wrd = {0}}, d = {.wrd = {0}};
  str2bint(&a, "0x3b6859c358bb6fa30b3f11ff6c29164dc21b2abaf4c2027ea8e6701e99dd5b7c");
  str2bint(&b, "0xd7dab791a8647ac88695e20e29960a6fd41707258cc88cc0480ea7e5bb3f132f");
  bmul(&c, &a, &b);
  // 0x321764d8e538429bc4539ec57462be61fa493fd0c700db4f19c51549d9c6aad6904bf1b900775bdf11da25e74e4d698c386446a85b7f1afaf9062442c4f1ffc4
  uint32_t xxx[LEN] = {0x321764d8, 0xe538429b, 0xc4539ec5, 0x7462be61, 0xfa493fd0, 0xc700db4f, 0x19c51549, 0xd9c6aad6, 0x904bf1b9, 0x00775bdf,
    0x11da25e7, 0x4e4d698c, 0x386446a8, 0x5b7f1afa, 0xf9062442, 0xc4f1ffc4};
  for (int i = 0; i < 15; i++) {assert(c.wrd[i] == xxx[15-i]);}
  printf("ccc %d %d\n", c.siz, a.siz);
  bmul(&d, &c, &a);
  // 0xb9fcf4f502f06b563974c8fd64b52750b925ec4138d28632fc84b3f8eedf7c7d471a809102365772e1eefc491b090a624584b32a122142b07a6c3aefc791d81cf527e4053ea119ffa86f73189e82d3ae2650192831a8fc8618fac175b568ef0
  uint32_t yyy[LEN] = {0x0b9fcf4f, 0x502f06b5, 0x63974c8f, 0xd64b5275, 0x0b925ec4, 0x138d2863, 0x2fc84b3f, 0x8eedf7c7, 0xd471a809, 0x10236577,
    0x2e1eefc4, 0x91b090a6, 0x24584b32, 0xa122142b, 0x07a6c3ae, 0xfc791d81, 0xcf527e40, 0x53ea119f, 0xfa86f731, 0x89e82d3a, 0xe2650192, 0x831a8fc8,
    0x618fac17, 0x5b568ef0};
  for (int i = 0; i < 24; i++) assert(yyy[23-i] == d.wrd[i]);
}

void test_div(void) {
  bint a = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, b = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, c = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  bint tmp = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, one;
  str2bint(&a, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(&b, "0x9075b4ee4d4788cabb49f7f81c221151fa2f68914d0aa833388fa11ff621a970");
  bdiv(&c, &tmp, &a, &b);
  wrd2bint(&one, 1);
  assert(cmp(&c, &one) == 0);
}

#define SWAP(a, b) {bint tmp = a; a = b; b = tmp;}
void gc(void) {
/*
    r, old_r = p, k

    while r != 0:
        quotient = old_r // r
        old_r, r = r, old_r - quotient * r
*/
  bint s = {.wrd = {0}, .siz=0}, os = {.wrd = {0}, .siz=0}, t = {.wrd = {0}, .siz=0}, ot = {.wrd = {0}, .siz=0}, r = {.wrd = {0}, .siz=0}, or = {.wrd = {0}, .siz=0}, quot = {.wrd = {0}, .siz=0};
  bint P = {.wrd = {0}, .siz=0}, tmp = {.wrd = {0}, .siz=0}, qr = {.wrd = {0}, .siz=0, .cap = 0}, oqr = {.wrd = {0}, .siz=0, .cap = 0};
  bint mqr = {.wrd = {0}, .siz=0, .cap = 0}, d = {.wrd = {0}, .siz = 0, .cap = 0}, rtmp = {.wrd = {0}, .siz = 0, .cap = 0}, zero = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  bint qs = {.wrd = {0}, .siz=0, .cap = 0}, qt = {.wrd = {0}, .siz = 0, .cap = 0}, oqs = {.wrd = {0}, .siz = 0, .cap = 0}, oqt = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  wrd2bint(&s, 0);
  wrd2bint(&os, 1);
  wrd2bint(&t, 1);
  wrd2bint(&ot, 0);
  str2bint(&P, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(&r, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(&or, "0x9075b4ee4d4788cabb49f7f81c221151fa2f68914d0aa833388fa11ff621a970");
  wrd2bint(&rtmp, 0);
  wrd2bint(&quot, 0);
  wrd2bint(&tmp, 0);
  wrd2bint(&qr, 0);
  wrd2bint(&mqr, 0);
  wrd2bint(&oqr, 0);
  wrd2bint(&tmp, 0);
  wrd2bint(&zero, 0);
  while (cmp(&r, &zero) != 0 && r.siz != 0) {
    bdiv(&quot, &tmp, &or, &r); // q = or / r
    printf("------------\n");
    bprint("quot", &quot);
    bprint("r   ", &r);
    bprint("or  ", &or);
    bprint("rtmp", &tmp);
    bmul(&qr, &quot, &r); // (q * r)
    bmul(&qs, &quot, &s); // (q * s)
    bmul(&qt, &quot, &t); // (q * t)
    bprint("qr  ", &qr);
    bsub(&oqr, &or, &qr); // or = or - (q * r)
    bsub(&oqs, &os, &qs); // os = os - (q * s)
    bsub(&oqt, &ot, &qt); // ot = ot - (q * t)
    bprint("oqr  ", &oqr);
    SWAP(r, or); // or = r
    SWAP(s, os); // os = s
    SWAP(t, ot); // ot = t
    printf("\n");
    SWAP(r, oqr); // r = or
    SWAP(s, oqs); // s = os
    SWAP(t, oqt); // t = ot
    printf("\n");
  }
  bprint("G", &or);
  assert(cmp(&or, wrd2bint(&tmp, 1)) == 0);
  bint aa, bb, cc;
  wrd2bint(&aa, 0);
  wrd2bint(&bb, 0);
  str2bint(&rtmp, "0x9075b4ee4d4788cabb49f7f81c221151fa2f68914d0aa833388fa11ff621a970");
  bprint("OS", &os);
  bprint("rtmp", &rtmp);
  bmul(&aa, &os, &rtmp);
  bprint("aa", &aa);
  bmod(&cc, &bb, &aa, &P);
  bprint("bb", &bb);
  bprint("cc", &cc);
  // g, x, y = old_r, old_s, old_t
  //  assert g == Prime(1)
  //  assert Prime(Field((k * x).x) % Field(x = self.p)) == Prime(1)
  //  return Field(x.x) % Field(x = self.p)

}

void tester_2ways(void) {
  bint a = {.wrd = {0}, .siz=0, .cap = 0}, b = {.wrd = {0}, .siz=0, .cap = 0}, c = {.wrd = {0}, .siz=0, .cap = 0}, d = {.wrd = {0}, .siz = 0, .cap = 0};
  bint e = {.wrd = {0}, .siz=0, .cap = 0}, t = {.wrd = {0}, .siz=0, .cap = 0};
  str2bint(&a, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  wrd2bint(&b, 0x33333333);
  wrd2bint(&c, 0x3);
  wrd2bint(&d, 0x33);
  wrd2bint(&e, 0x0);
  wrd2bint(&t, 0x0);
  for (int i = 0; i < 20000; i++) {
    bsub(&a, &a, &b);
  }
  for (int i = 0; i < 20000; i++) {
    badd(&a, &a, &b);
  }
  assert(cmp(str2bint(&b, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"), &a) == 0);
  for (int i = 0; i < 200; i++) {
    bmul(&e, &d, &c);
    memcpy(&d.wrd, e.wrd, e.siz * sizeof(uint32_t));
    d.siz = e.siz;
  }
  for (int i = 0; i < 200; i++) {
    bdiv(&e, &t, &d, &c);
    memcpy(&d.wrd, e.wrd, e.siz * sizeof(uint32_t));
    d.siz = e.siz;
  }
  assert(cmp(wrd2bint(&d, 0x33), &e) == 0);
  memset(a.wrd, 0, LEN * sizeof(uint32_t));
  memset(d.wrd, 0, LEN * sizeof(uint32_t));
  memset(e.wrd, 0, LEN * sizeof(uint32_t));
  memset(t.wrd, 0, LEN * sizeof(uint32_t));
  a.neg = d.neg = e.neg = t.neg = a.siz = d.siz = e.siz = t.siz = a.cap = d.cap = e.cap = t.cap = 0;
  str2bint(&a, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(&d, "0x0000000000000000000000000000000000000000000000000000003333333333");
  d.siz = 2;
  bdiv(&e, &t, &a, &d);
  str2bint(&d, "0x000000000500000000050000000005000000000500000000050000000004ffff");
  d.siz = 7;
  assert(cmp(&d, &e) == 0);
}

int main(void) {

  test_div();
  Btester_sanity();
  Ftester_sanity();
  Ftester_math_sanity();

  Ftester();
  Ftester2();
  tester_2ways();

  gc();

  printf("ok\n");
}


// Auth: smurfd, 2025 More reading at the bottom of the file; 2 spacs indent; 150 width                                                             //
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "../bm.h"
#include "../bmcurve.h"

int main(void) {
  bint a = {.wrd = {0}}, b = {.wrd = {0}}, c = {.wrd = {0}}, d = {.wrd = {0}}, r = {.wrd = {0}};
  bprint("A", &a);
  bprint("B", &b);
  bprint("C", &c);
  str2bint(&a, "0x3b6859c358bb6fa30b3f11ff6c29164dc21b2abaf4c2027ea8e6701e99dd5b7c");
  str2bint(&b, "0xd7dab791a8647ac88695e20e29960a6fd41707258cc88cc0480ea7e5bb3f132f");
  bprint("A", &a);
  bprint("B", &b);
  bmul(&c, &a, &b);
  bprint("C", &c); // 0x321764d8e538429bc4539ec57462be61fa493fd0c700db4f19c51549d9c6aad6904bf1b900775bdf11da25e74e4d698c386446a85b7f1afaf9062442c4f1ffc4
  uint32_t xxx[LEN] = {0x321764d8, 0xe538429b, 0xc4539ec5, 0x7462be61, 0xfa493fd0, 0xc700db4f, 0x19c51549, 0xd9c6aad6, 0x904bf1b9, 0x00775bdf, 0x11da25e7, 0x4e4d698c, 0x386446a8, 0x5b7f1afa, 0xf9062442, 0xc4f1ffc4};
  for (int i = 0; i < 15; i++) {printf("%08x : %08x\n", c.wrd[i], xxx[15-i]); assert(c.wrd[i] == xxx[15-i]);}
  printf("ccc %d %d\n", c.siz, a.siz);//, sizeof(uint32_t), CHAR_BIT, sizeof(uint32_t) * CHAR_BIT);
  bmul(&d, &c, &a);
  bprint("D", &d);
  for (int i = 0; i < 400; i++) {
    brshift(&r, &d, 2);
    bprint2("ds", &r);
    memcpy(d.wrd, r.wrd, LEN * sizeof(uint32_t));
  }
  Ftester_sanity();
  Ftester();
  printf("ok\n");
}


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
  // 0xb9fcf4f502f06b563974c8fd64b52750b925ec4138d28632fc84b3f8eedf7c7d471a809102365772e1eefc491b090a624584b32a122142b07a6c3aefc791d81cf527e4053ea119ffa86f73189e82d3ae2650192831a8fc8618fac175b568ef0

// D: 5b568ef0 618fac17 831a8fc8 e2650192 89e82d3a fa86f731 53ea119f cf527e40 fc791d81 7a6c3ae a122142b 24584b32 91b090a6 2e1eefc4 10236577 d471a809 8eedf7c7 2fc84b3f 138d2863 b925ec4
// D: 5b568ef0 618fac17 831a8fc8 e2650192 89e82d3a fa86f731 53ea119f cf527e40 fc791d81 7a6c3ae a122142b 24584b32 91b090a6 2e1eefc4 10236577 d471a809 8eedf7c7 2fc84b3f 138d2863 b925ec4
// D: 5b568ef0 618fac17 831a8fc8 e2650192 89e82d3a fa86f731 53ea119f cf527e40 100000 0 0 e538429b c4539ec5 7462be61 fa493fd0 c700db4f 19c51549 d9c6aad6 904bf1b9 775bdf

  for (int i = 0; i < 400; i++) {
    brshift(&r, &d, 2);
    bprint2("ds", &r);
    memcpy(d.wrd, r.wrd, LEN * sizeof(uint32_t));
  }
/*
  Field field0, field1, field2, field3;
  printf("--------- Testvalues -------------------------------------------\n");
  uint32_t tmp[8] = {0,0,0,0,0,0,0,23};
  memcpy(field0.p->wrd, tmp, 8 * sizeof(uint32_t));
*/
//  field0.init = Field_init;
//  field0.initint(0, &field0);
/*  field0.initint = Field_initint;
  field1.initint = Field_initint;
  field2.initint = Field_initint;
  field3.initint = Field_initint;
  memcpy(field0.p, tmp, sizeof(uint32_t) * 8);
  memcpy(field1.p, tmp, sizeof(uint32_t) * 8);
  memcpy(field2.p, tmp, sizeof(uint32_t) * 8);
  memcpy(field3.p, tmp, sizeof(uint32_t) * 8);
  field0.initint(0, &field0);
  field1.initint(12, &field1);
  field2.initint(20, &field2);
  field3.initint(9, &field3);
*/


  Ftester();

/*
  F fff;
  fff.init = F_init;
  fff.init(666, &fff);
*/
  printf("ok\n");
}




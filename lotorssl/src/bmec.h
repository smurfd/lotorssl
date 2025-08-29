// Auth: smurfd, 2025 More reading at the bottom of the file; 2 spacs indent; 150 width                                                             //
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
#pragma once
#ifndef BMEC_H
#define BMEC_H
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "lotormath/src/lotormath.h"

void sign(bint *sigx, bint *sigy, bint *pri, char *msg);
int16_t verify(bint *pubx, bint *puby, char *msg, bint *sigx, bint *sigy);
void genkeypair(bint *pubx, bint *puby, bint *sec);
void gensharedsecret(bint *shrx, bint *shry, bint *sec, bint *pubx, bint *puby);
void verifysharedsecret(bint *alshrx, bint *alshry, bint *boshrx, bint *boshry, bint *alsk, bint *bosk);
#define GCDMR(rr, qr, q, r, or) {bmul(qr, q, r); bsub(rr, or, qr);}
#define PA(diim, di, r0x, r0y, r1x, r1y, p) {if (diim) point_add(r1x, r1y, r0x, r0y, r1x, r1y, p); else point_add(r0x, r0y, r0x, r0y, r1x, r1y, p);\
if (di) point_add(r1x, r1y, r1x, r1y, r1x, r1y, p); else point_add(r0x, r0y, r0x, r0y, r0x, r0y, p);}
#define B2CPY(r1, r2, a1, a2) {r1 = a1; r2 = a2;}
#define MAM(di, dii, diim, tmp, kt, one1, two2) {bmod(di, tmp, kt, two2); badd(dii, di, one1); bmod(diim, tmp, dii, two2);}
#define MULMOD(tmp, tmp2, hash1, h, h1, curveN) {bmul(tmp, hash1, h); bmod(h1, tmp2, tmp, curveN);}
#define MPAPA(tmp1, tmp2, kt, two2, rsx, rsy, ax, ay, p, tw, ts) {bmod(tmp1, tmp2, kt, two2); if (tw == 1 && ts == 1) point_add(rsx, rsy, rsx, rsy, ax, ay, p);\
point_add(ax, ay, ax, ay, ax, ay, p);}
#define SMSM(h1x, h1y, h2x, h2y, h1, h2, curveX, curveY, pubx, puby, curveP, curveN) {scalar_mul(h1x, h1y, h1, curveX, curveY, curveP, curveN);\
scalar_mul(h2x, h2y, h2, pubx, puby, curveP, curveN);}

#define curveA wrd2bint(&ctmpA, 0)
#define curveB wrd2bint(&ctmpB, 7)
#define curveH wrd2bint(&ctmpH, 1)
#define curveP str2bint(&ctmpP, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f")
#define curveN str2bint(&ctmpN, "0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141")
#define curveX str2bint(&ctmpX, "0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798") // point gx
#define curveY str2bint(&ctmpY, "0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8") // point gy
#endif

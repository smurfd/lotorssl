//                                                                            //
#ifndef LIGHTDEFS_H
#define LIGHTDEFS_H 1

#include <stdint.h>

typedef int8_t i08;
typedef uint8_t u08;
typedef const char cc;
typedef unsigned int ui;
typedef unsigned char b08;
typedef const uint8_t cu8;
typedef const unsigned char cuc;
typedef long long unsigned int u64;

typedef uint64_t bit[571];
typedef uint64_t sig[72];

// VSH
typedef struct keys key;
typedef struct header head;
typedef struct sockaddr sock;
typedef struct sockaddr_in sock_in;

// SSL
#define BLOCK 1024
#define RAND() (rand() & 0x7FFFFFFFFFFFFFFF)
#define RAND64() ((u64)(RAND()) << 48) ^ ((u64)(RAND()) << 35) ^ \
  ((u64)(RAND()) << 22) ^ ((u64)(RAND()) << 9) ^ ((u64)(RAND()) >> 4)

// TLS
#define TLSCIPHER 222
#define TLSVERSION 0x304
#define TLSCIPHERAVAIL 222
#define TLSCOMPRESSION 123

// Hash
#define SHA_CH00(x, y, z) (((x) & ((y) ^ (z))) ^ (z))
#define SHA_MAJ0(x, y, z) (((x) & ((y) | (z))) | ((y) & (z)))
#define SHA_PARI(x, y, z)  ((x) ^  (y) ^ (z))

// Define the SHA shift, rotate left and rotate right macros
#define SHA_SHRI(b, w)  (((u64)(w)) >> (b))
#define SHA_ROTR(b, w) ((((u64)(w)) >> (b)) | (((u64)(w)) << (64-(b))))

// Define the SHA SIGMA and sigma macros
#define SHA_S0(w) (SHA_ROTR(28, w) ^ SHA_ROTR(34, w) ^ SHA_ROTR(39, w))
#define SHA_S1(w) (SHA_ROTR(14, w) ^ SHA_ROTR(18, w) ^ SHA_ROTR(41, w))
#define SHA_s0(w) (SHA_ROTR( 1, w) ^ SHA_ROTR( 8, w) ^ SHA_SHRI( 7, w))
#define SHA_s1(w) (SHA_ROTR(19, w) ^ SHA_ROTR(61, w) ^ SHA_SHRI( 6, w))

// Add "length" to the length. Set Corrupted when overflow has occurred.
#define SHA_L(c) (++c->len_hi == 0) ? sha_itl : (c)->corrupt
#define SHA_T(c, l) c->corrupt = ((c->len_lo += l) < 0)
#define SHA_AddLength(c, l) (SHA_T(c, l) && SHA_L(c))

// VSH Structs
struct header {u64 len, ver, g, p;};
struct keys {u64 publ, priv, shar;};

// Keys struct
typedef struct curves {
  bit ecdh_p;
  bit ecdh_b;
  bit ecdh_x;
  bit ecdh_y;
  bit ecdh_n;
  int ecdh_a;
  int ecdh_h;

  int CURV; // Curve type
  int PRIV; // Private key sisze
  int PUBL; // Publik key size
  int NBYT; // Nbytes
  int NBIT; // Nbits
  int NWOR; // Nwords
  int DEGR; // Degree
  int MARG; // Margin
} cur;

typedef struct points {
  bit x;
  bit y;
} pt;
#endif

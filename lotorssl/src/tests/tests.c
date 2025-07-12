#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "../lotormath/src/lotormath.h"
#include "../definitions.h"
#include "../ciph.h"
#include "../hash.h"
#include "../cryp.h"
#include "../keys.h"
#include "../bmec.h"

uint8_t test_aes(void) {
  uint32_t key[8] = {0x603deb10, 0x15ca71be, 0x2b73aef0, 0x857d7781, 0x1f352c07, 0x3b6108d7, 0x2d9810a3, 0x0914dff4};
  uint32_t plain[4] = {0xf69f2445, 0xdf4f9b17, 0xad2b417b, 0xe66c3710};
  uint32_t expect[4] = {0x23304b7a, 0x39f9f3ff, 0x067d8d8f, 0x9e24ecc7};
  uint32_t resultenc[64] = {0}, resultdec[64] = {0};
  cipher(resultenc, key, plain);
  inv_cipher(resultdec, key, resultenc);
  assert(memcmp(resultenc, expect, 4 * sizeof(uint32_t)) == 0 && memcmp(resultdec, plain, 4 * sizeof(uint32_t)) == 0);
  return 1;
}

uint8_t test_aesloop(void) {
  uint8_t res = 0;
  uint32_t key[8] = {0x603deb10, 0x15ca71be, 0x2b73aef0, 0x857d7781, 0x1f352c07, 0x3b6108d7, 0x2d9810a3, 0x0914dff4};
  uint32_t plain[4] = {0xf69f2445, 0xdf4f9b17, 0xad2b417b, 0xe66c3710};
  uint32_t expect[4] = {0x23304b7a, 0x39f9f3ff, 0x067d8d8f, 0x9e24ecc7};
  uint32_t resultenc[64] = {0}, resultdec[64] = {0};
  clock_t start = clock();
  for (int i = 0; i < 1000000; i++) {
    cipher(resultenc, key, plain);
    inv_cipher(resultdec, key, resultenc);
    res += memcmp(resultenc, expect, 4 * sizeof(uint32_t));
    res += memcmp(resultdec, plain, 4 * sizeof(uint32_t));
  }
  assert(res == 0);
  printf("aesloop: Time %us %ums\n", (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) / 1000, (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) % 1000);
  return 1;
}

uint8_t test_aesgcm(void) {
  uint8_t iv[32] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
  key[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f},
  plain[32] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff},
  cipher[32] = {0}, tag[32] = {0}, tag2[32] = {0}, aad[32] = {0}, plain2[32] = {0}, res = 0;
  gcm_ciphertag(cipher, tag, key, iv, plain, aad,  32);
  gcm_inv_ciphertag(plain2, tag2, key, iv, cipher, aad, tag);
  res += memcmp(plain, plain2, 32 * sizeof(uint8_t));
  assert(res == 0);
  return 1;
}

uint8_t test_aesgcmloop(void) {
  uint8_t iv[32] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
  key[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f},
  plain[32] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff},
  cipher[32] = {0}, tag[32] = {0}, tag2[32] = {0}, aad[32] = {0}, plain2[32] = {0}, res = 0;
  clock_t start = clock();
  for (int i = 0; i < 1000000; i++) {
    gcm_ciphertag(cipher, tag, key, iv, plain, aad,  32);
    gcm_inv_ciphertag(plain2, tag2, key, iv, cipher, aad, tag);
    res += memcmp(plain, plain2, 32 * sizeof(uint8_t));
  }
  assert(res == 0);
  printf("aesgcmloop: Time %us %ums\n", (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) / 1000, (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) % 1000);
  return 1;
}

uint8_t test_aesgcm32bit(void) {
  uint32_t iv[32] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff},
  key[32] = {0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f, 0x10111213, 0x14151617, 0x18191a1b, 0x1c1d1e1f},
  plain[32] = {0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff, 0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff},
  cipher[32] = {0}, tag[32] = {0}, tag2[32] = {0}, aad[32] = {0}, plain2[32] = {0}, res = 0;
  gcm_ciphertag32bit(cipher, tag, key, iv, plain, aad,  32);
  gcm_inv_ciphertag32bit(plain2, tag2, key, iv, cipher, aad, tag);
  res += memcmp(plain, plain2, 8 * sizeof(uint32_t));
  assert(res == 0);
  return 1;
}

uint8_t test_aesgcm32bitloop(void) {
  uint32_t iv[32] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff},
  key[32] = {0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f, 0x10111213, 0x14151617, 0x18191a1b, 0x1c1d1e1f},
  plain[32] = {0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff, 0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff},
  cipher[32] = {0}, tag[32] = {0}, tag2[32] = {0}, aad[32] = {0}, plain2[32] = {0}, res = 0;
  clock_t start = clock();
  for (int i = 0; i < 1000000; i++) {
    gcm_ciphertag32bit(cipher, tag, key, iv, plain, aad, 8);
    gcm_inv_ciphertag32bit(plain2, tag2, key, iv, cipher, aad, tag);
    res += memcmp(plain, plain2, 8 * sizeof(uint32_t));
  }
  assert(res == 0);
  printf("aesgcm32bitloop: Time %us %ums\n", (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) / 1000, (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) % 1000);
  return 1;
}

uint8_t test_certkey(void) {
  char s0[] = "c211cmZkIGFuZCBtb3JlIHN0dWZm", s1[257], data[LEN], c[8192];
  uint8_t s2[] = "smurfd and more stuff", s3[257];
  crypto_handle_cert(data, "../.build/ca.key");
  crypto_handle_asn(c, "../.build/ca256.cms");
  base64dec(s3, s0, strlen(s0));
  base64enc(s1, s2, strlen("smurfd and more stuff"));
  assert(strcmp(s1, s0) == 0);
  return 1;
}

uint8_t test_certcli(void) {
  int s = crypto_init("127.0.0.1", "9998", false);
  if (s >= 0) {
    u64 dat[BLOCK], cd[BLOCK];
    key k1, k2;
    head h;
    crypto_transfer_key(s, false, &h, &k1);
    k2 = crypto_gen_keys(h.g, h.p);
    crypto_transfer_key(s, true, &h, &k2);
    crypto_gen_share(&k1, &k2, h.p, false);
    printf("share : 0x%.16llx\n", k1.shar);
    for (u64 i = 0; i < 12; i++) {
      dat[i] = (u64)i; cryption(dat[i], k1, &cd[i]);
    }
    crypto_transfer_data(s, cd, &h, true, 11);
    crypto_end(s);
  }
  // locally generate two keypairs
  srand(time(0));
  crypto_gen_keys_local();
  return 1;
}

uint8_t test_certsrv(void) {
  int s = crypto_init("127.0.0.1", "9998", true);
  sock *cli = NULL;
  if (crypto_srv_listen(s, cli) < 0) {
    printf("Can't create a Thread\n");
    exit(0);
  }
  crypto_end(s);
  return 1;
}

uint8_t test_hash3(void) {
  uint8_t *smurfd = (uint8_t*)"smurfd";
  char s[256] = {0};
  hash_new(s, smurfd);
  assert(strcmp(s, "0x5c452b35648528cf3a00a42021489011dd455b78fc34190c7680173b2dcdcc7d61e73d4f2c51051e45d26215f9f7729b8\
986549e169dcee3280bed61cda25f20") == 0);
  assert(strcmp(s, "0x5c452b35648528cf3a00a42021489011dd455b78fc34190c7680173b2dcdcc7d61e73d4f2c51051e45d26215f9f7729b8\
986549e169dcee3280bed61cdffffff") != 0); // Assume failure
  return 1;
}

uint8_t test_hash3big(void) {
  uint8_t *plain = (uint8_t*)"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt\
 ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip e\
x ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pa\
riatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
  char s[256] = {0};
  hash_new(s, plain);
  assert(strcmp(s, "0xf32a9423551351df0a07c0b8c20eb972367c398d61066038e16986448ebfbc3d15ede0ed3693e3905e9a8c601d9d002a0\
6853b9797ef9ab10cbde1009c7d0f09") == 0);
  assert(strcmp(s, "0xf32a9423551351df0a07c0b8c20eb972367c398d61066038e16986448ebfbc3d15ede0ed3693e3905e9a8c601d9d002a0\
6853b9797ef9ab10cbde1009c7d0fff") != 0); // Assume failure
  return 1;
}

uint8_t test_hash3bigloop(void) {
  uint8_t *plain = (uint8_t*)"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt\
 ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip e\
x ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pa\
riatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
  res = 0;
  clock_t start = clock();
  for (int i = 0; i < 1000000; i++) {
    char s[256] = {0};
    hash_new(s, plain);
    res += memcmp(s, "0xf32a9423551351df0a07c0b8c20eb972367c398d61066038e16986448ebfbc3d15ede0ed3693e3905e9a8c601d9d002a0\
6853b9797ef9ab10cbde1009c7d0f09", 130);
  }
  assert(res == 0);
  printf("hash3bigloop: Time %us %ums\n", (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) / 1000, (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) % 1000);
  return 1;
}

uint8_t test_hash3shk(void) {
  uint8_t res[] = {0x0d, 0xcf, 0xbc, 0x11, 0xbd, 0xd2, 0x43, 0x82, 0x4b, 0x31, 0xe5, 0x13, 0x5b, 0x8f, 0x83, 0xfa, 0x1c,
       0x11, 0x8d, 0xd7, 0x6a, 0xc0, 0xea, 0xaf, 0xee, 0x19, 0x10, 0x17, 0x0b, 0xa5, 0x61, 0x89, 0xa5, 0x8d, 0x21, 0x2a,
       0xa2, 0xb4, 0x2d, 0xfe, 0xbd, 0x1b, 0x8c, 0xdd, 0x08, 0xa4, 0xc4, 0xd5, 0xae, 0xcb, 0xfa, 0x0c, 0x33, 0x60, 0x0f,
       0x39, 0x78, 0x8b, 0x75, 0x81, 0xb5, 0xbb, 0x4f, 0x42}, in1[1024] = {0}, out1[512] = {0};
  char s[] = "smurfd";
  memcpy(in1, s, 6 * sizeof(uint8_t));
  hash_shake_new(out1, 64, in1, 6);
  assert(memcmp(out1, res, 64 * sizeof(uint8_t)) == 0);
  return 1;
}

uint8_t test_hash3shkbig(void) {
  uint8_t res[] = {0x75, 0x74, 0x60, 0x89, 0x24, 0x0d, 0x9e, 0x39, 0xff, 0xf1, 0xb4, 0xba, 0x58, 0x13, 0x0a, 0xf5, 0xb9,
       0x74, 0x4f, 0x41, 0x2a, 0x9d, 0xff, 0x73, 0x84, 0x70, 0xd1, 0x24, 0x72, 0x53, 0xd3, 0x2c, 0xe7, 0xfe, 0x5a, 0xef,
       0x0d, 0x43, 0xda, 0x15, 0x5f, 0x29, 0x08, 0x58, 0xa4, 0x2e, 0xa0, 0x41, 0xd3, 0x9a, 0x6b, 0xfd, 0x04, 0x21, 0xd4,
       0x49, 0x8e, 0xa4, 0x95, 0xbd, 0x41, 0x3a, 0x9f, 0x58}, in1[1024], out1[512];
  char s[130] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et\
 dolore magna aliqua. Ut eni";
  memcpy(in1, s, 130 * sizeof(uint8_t));
  hash_shake_new(out1, 64, in1, 130);
  assert(memcmp(out1, res, 64 * sizeof(uint8_t)) == 0);
  return 1;
}

uint8_t test_hash3shkref(void) {
  uint8_t res[] = {0xf6, 0x49, 0x68, 0x85, 0x8b, 0x5c, 0xd8, 0xa6, 0x4f, 0xfd, 0xd9, 0x2e, 0x8c, 0x72, 0xda, 0x03, 0x87,
       0xc5, 0x68, 0x9b, 0x56, 0x2e, 0x96, 0x28, 0x86, 0x04, 0xdf, 0x95, 0x31, 0x5f, 0xee, 0xfa, 0x5a, 0xe9, 0xf0, 0x59,
       0x6b, 0x0b, 0x3d, 0x47, 0xcd, 0x61, 0xac, 0x67, 0x6a, 0xd1, 0xfb, 0x20, 0xcf, 0x3d, 0x92, 0xab, 0x2b, 0x68, 0xda,
       0xa4, 0x89, 0x31, 0xcc, 0x58, 0xd6, 0xd7, 0x23, 0xc7}, in1[1024], out1[512];
  char s[] = "\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3";
  memcpy(in1, s, 20 * sizeof(uint8_t));
  hash_shake_new(out1, 64, in1, 20);
  assert(memcmp(out1, res, 64 * sizeof(uint8_t)) == 0);
  return 1;
}

uint8_t test_hash3shkrefloop(void) {
  uint8_t res[] = {0xf6, 0x49, 0x68, 0x85, 0x8b, 0x5c, 0xd8, 0xa6, 0x4f, 0xfd, 0xd9, 0x2e, 0x8c, 0x72, 0xda, 0x03, 0x87,
       0xc5, 0x68, 0x9b, 0x56, 0x2e, 0x96, 0x28, 0x86, 0x04, 0xdf, 0x95, 0x31, 0x5f, 0xee, 0xfa, 0x5a, 0xe9, 0xf0, 0x59,
       0x6b, 0x0b, 0x3d, 0x47, 0xcd, 0x61, 0xac, 0x67, 0x6a, 0xd1, 0xfb, 0x20, 0xcf, 0x3d, 0x92, 0xab, 0x2b, 0x68, 0xda,
       0xa4, 0x89, 0x31, 0xcc, 0x58, 0xd6, 0xd7, 0x23, 0xc7}, in1[1024], out1[512], ret = 0;
  char s[] = "\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3";
  clock_t start = clock();
  for (int i = 0; i < 1000000; i++) {
    memcpy(in1, s, 20 * sizeof(uint8_t));
    hash_shake_new(out1, 64, in1, 20);
    ret += memcmp(out1, res, 64 * sizeof(uint8_t));
  }
  assert(ret == 0);
  printf("hash3shkrefloop: Time %us %ums\n", (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) / 1000, (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) % 1000);
  return 1;
}

uint8_t test_hash3shkrefloop2(void) {
  uint8_t res[] = {0xf6, 0x49, 0x68, 0x85, 0x8b, 0x5c, 0xd8, 0xa6, 0x4f, 0xfd, 0xd9, 0x2e, 0x8c, 0x72, 0xda, 0x03, 0x87,
       0xc5, 0x68, 0x9b, 0x56, 0x2e, 0x96, 0x28, 0x86, 0x04, 0xdf, 0x95, 0x31, 0x5f, 0xee, 0xfa, 0x5a, 0xe9, 0xf0, 0x59,
       0x6b, 0x0b, 0x3d, 0x47, 0xcd, 0x61, 0xac, 0x67, 0x6a, 0xd1, 0xfb, 0x20, 0xcf, 0x3d, 0x92, 0xab, 0x2b, 0x68, 0xda,
       0xa4, 0x89, 0x31, 0xcc, 0x58, 0xd6, 0xd7, 0x23, 0xc7}, in1[1024], out1[512], ret = 0;
  char s[] = "\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3";
  clock_t start = clock();
  for (int i = 0; i < 1000000; i++) {
    memcpy(in1, s, 20 * sizeof(uint8_t));
    hash_shake_new(out1, 64, in1, 20);
    ret += memcmp(out1, res, 64 * sizeof(uint8_t));
  }
  assert(ret == 0);
  printf("hash3shkrefloop2: Time %us %ums\n", (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) / 1000, (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) % 1000);
  return 1;
}

uint8_t test_keysmake(void) {
  uint8_t pubkey[BYTES + 1] = {0}, privkey[BYTES] = {0};
  assert(keys_make(pubkey, privkey));
  return 1;
}

uint8_t test_keyssecr(void) {
  uint8_t pubkey[BYTES + 1] = {0}, sec[BYTES] = {0}, privkey[BYTES] = {0};
  assert(keys_make(pubkey, privkey));
  assert(keys_secr(pubkey, privkey, sec));
  return 1;
}

uint8_t test_keyssign(void) {
  uint8_t sig[BYTES * 2] = {0}, pubkey[BYTES + 1] = {0}, sec[BYTES] = {0}, privkey[BYTES] = {0}, h[BYTES] = {0};
  assert(keys_make(pubkey, privkey));
  assert(keys_secr(pubkey, privkey, sec));
  assert(keys_sign(privkey, h, sig));
  return 1;
}

uint8_t test_keyssvrfy(void) {
  uint8_t sig[BYTES * 2] = {0}, pubkey[BYTES + 1] = {0}, sec[BYTES] = {0}, privkey[BYTES] = {0}, h[BYTES] = {0};
  assert(keys_make(pubkey, privkey));
  assert(keys_secr(pubkey, privkey, sec));
  assert(keys_sign(privkey, h, sig));
  assert(keys_vrfy(pubkey, h, sig));
  assert(!keys_vrfy(privkey, h, sig)); // assert failure
  return 1;
}

uint8_t test_keyswrite(void) {
  uint8_t pubkey[BYTES + 1] = {0}, privkey[BYTES] = {0};
  assert(keys_make(pubkey, privkey));
  keys_write("../.build/ca-own.key", privkey, 2);
  return 1;
}

uint8_t test_keyssvrfyloop(void) {
  uint32_t ret = 0;
  clock_t start = clock();
  uint8_t sig[BYTES * 2] = {0}, pubkey[BYTES + 1] = {0}, sec[BYTES] = {0}, privkey[BYTES] = {0}, h[BYTES] = {0};
  for (int i = 0; i < 1000000; i++) {
    ret += !keys_make(pubkey, privkey);
    ret += !keys_secr(pubkey, privkey, sec);
    ret += !keys_sign(privkey, h, sig);
    ret += !keys_vrfy(pubkey, h, sig);
  }
  assert(ret == 0);
  printf("keyssvrfyloop: Time %us %ums\n", (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) / 1000, (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) % 1000);
  return 1;
}

uint8_t tester_bint_sanity(void) {
  bint a = bcreate(), b = bcreate(), c = bcreate(), d = bcreate();
  str2bint(&a, "0x3b6859c358bb6fa30b3f11ff6c29164dc21b2abaf4c2027ea8e6701e99dd5b7c");
  str2bint(&b, "0xd7dab791a8647ac88695e20e29960a6fd41707258cc88cc0480ea7e5bb3f132f");
  bmul(&c, &a, &b);
  // 0x321764d8e538429bc4539ec57462be61fa493fd0c700db4f19c51549d9c6aad6904bf1b900775bdf11da25e74e4d698c386446a85b7f1afaf9062442c4f1ffc4
  uint32_t xxx[BLEN] = {0x321764d8, 0xe538429b, 0xc4539ec5, 0x7462be61, 0xfa493fd0, 0xc700db4f, 0x19c51549, 0xd9c6aad6, 0x904bf1b9, 0x00775bdf,
    0x11da25e7, 0x4e4d698c, 0x386446a8, 0x5b7f1afa, 0xf9062442, 0xc4f1ffc4};
  for (int i = 0; i < 15; i++) {assert(c.wrd[i] == xxx[15-i]);}
  bmul(&d, &c, &a);
  // 0xb9fcf4f502f06b563974c8fd64b52750b925ec4138d28632fc84b3f8eedf7c7d471a809102365772e1eefc491b090a624584b32a122142b07a6c3aefc791d81cf527e4053ea119ffa86f73189e82d3ae2650192831a8fc8618fac175b568ef0
  uint32_t yyy[BLEN] = {0x0b9fcf4f, 0x502f06b5, 0x63974c8f, 0xd64b5275, 0x0b925ec4, 0x138d2863, 0x2fc84b3f, 0x8eedf7c7, 0xd471a809, 0x10236577,
    0x2e1eefc4, 0x91b090a6, 0x24584b32, 0xa122142b, 0x07a6c3ae, 0xfc791d81, 0xcf527e40, 0x53ea119f, 0xfa86f731, 0x89e82d3a, 0xe2650192, 0x831a8fc8,
    0x618fac17, 0x5b568ef0};
  for (int i = 0; i < 24; i++) assert(yyy[23 - i] == d.wrd[i]);
  return 1;
}

uint8_t tester_bint_div_sanity(void) {
  bint a = bcreate(), b = bcreate(), c = bcreate(), tmp = bcreate(), one = bcreate();
  str2bint(&a, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(&b, "0x9075b4ee4d4788cabb49f7f81c221151fa2f68914d0aa833388fa11ff621a970");
  bdiv(&c, &tmp, &a, &b);
  wrd2bint(&one, 1);
  assert(cmp(&c, &one) == 0);
  return 1;
}

uint8_t tester_bint_mod_sanity(void) {
  bint a = bcreate(), b = bcreate(), c = bcreate(), tmp = bcreate(), tmp2 = bcreate(), ret = bcreate(), r1 = bcreate(), r2 = bcreate();
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
  return 1;
}

uint8_t tester_bint_2ways_sanity(void) {
  bint a = bcreate(), b = bcreate(), c = bcreate(), d = bcreate(), e = bcreate(), t = bcreate();
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
    bint ee = bcreate(), tt = bcreate();
    bdiv(&ee, &tt, &d, &c);
    memcpy(&d, &ee, sizeof(bint));
  }
  assert(cmp(wrd2bint(&e, 0x33), &d) == 0);
  memset(a.wrd, 0, BLEN * sizeof(uint32_t));
  memset(d.wrd, 0, BLEN * sizeof(uint32_t));
  memset(e.wrd, 0, BLEN * sizeof(uint32_t));
  memset(t.wrd, 0, BLEN * sizeof(uint32_t));
  a.neg = d.neg = e.neg = t.neg = a.cap = d.cap = e.cap = t.cap = 0;
  a.siz = d.siz = e.siz = t.siz = 1;
  str2bint(&a, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(&d, "0x0000000000000000000000000000000000000000000000000000003333333333");
  bdiv(&e, &t, &a, &d);
  str2bint(&d, "0x000000000500000000050000000005000000000500000000050000000004ffff");
  assert(cmp(&d, &e) == 0);
  return 1;
}

uint8_t tester_bint_PK(void) {
  // TODO: check if the point is on curve
  bint CA = bcreate(), CB = bcreate(), CP = bcreate(), CN = bcreate(), CX = bcreate(), CY = bcreate(), CH = bcreate(), tmp1 = bcreate(), tmp2 = bcreate();
  bint alsk = bcreate(), alpkx = bcreate(), alpky = bcreate(), bosk = bcreate(), bopkx = bcreate(), bopky = bcreate(), r1 = bcreate(), r2 = bcreate();
  bint alshrx = bcreate(), alshry = bcreate(), boshrx = bcreate(), boshry = bcreate(), res1 = bcreate();
  wrd2bint(&CA, 0); // Curve parameters
  wrd2bint(&CB, 7);
  wrd2bint(&CH, 1);
  str2bint(&CP, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(&CN, "0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
  str2bint(&CX, "0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"); // point gx
  str2bint(&CY, "0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"); // point gy
  str2bint(&alsk, "0x024cd559cad384fad17951966dc5a25ae64671ae67b4b78c49725d38f35bfddb"); // TODO: randomize
  str2bint(&bosk, "0x702d7c3642f0aeedb279f623724bffd7932e077b3498a02bda95dbc0ee8725be"); // TODO: randomize
  scalar_mul(&alpkx, &alpky, &alsk, &CX, &CY, &CP, &CN); // Alice's public key
  scalar_mul(&bopkx, &bopky, &bosk, &CX, &CY, &CP, &CN); // Bob's public key
  scalar_mul(&alshrx, &alshry, &bosk, &alpkx, &alpky, &CP, &CN); // Alice's shared secret
  scalar_mul(&boshrx, &boshry, &alsk, &bopkx, &bopky, &CP, &CN); // Bob's shared secret
  assert(cmp(&alshrx, &boshrx) == 0 && cmp(&alshry, &boshry) == 0); // assert alices shared secret is the same as bobs shared secret
  bmul(&res1, &alsk, &bosk); // alice's and bob's secret
  bmod(&tmp1, &tmp2, &res1, &CN);
  scalar_mul(&r1, &r2, &tmp1, &CX, &CY, &CP, &CN); // scale with curve G
  assert(cmp(&r1, &alshrx) == 0); // assert alices shared x is same as ((alicesecret * bobsecret) % N) scalar mult by curve G
  return 1;
}

int main(int argc, char** argv) {
  int ret = 1;
  if (argc == 1) { // When run without arguments
    ret &= test_aes();
    ret &= test_aesgcm();
    ret &= test_aesgcm32bit();
    ret &= test_hash3();
    ret &= test_hash3big();
    ret &= test_hash3shk();
    ret &= test_hash3shkbig();
    ret &= test_hash3shkref();
    ret &= test_keysmake();
    ret &= test_keyssecr();
    ret &= test_keyssign();
    ret &= test_keyssvrfy();
    ret &= test_keyswrite();
    ret &= tester_bint_sanity();
    ret &= tester_bint_2ways_sanity();
    ret &= tester_bint_div_sanity();
    ret &= tester_bint_mod_sanity();
    clock_t start = clock();
    //for (int i = 0; i < 1000; i++)
    ret &= tester_bint_PK();
    printf("pk loop: Time %us %ums\n", (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) / 1000, (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) % 1000);
  } else {
    if (strcmp(argv[1], "local") == 0) { // When run locally to measure speed
      ret &= test_aes();
      ret &= test_aesloop();
      ret &= test_aesgcmloop();
      ret &= test_aesgcm();
      ret &= test_aesgcmloop();
      ret &= test_aesgcm32bit();
      ret &= test_aesgcm32bitloop();
      ret &= test_hash3();
      ret &= test_hash3big();
      ret &= test_hash3bigloop();
      ret &= test_hash3shk();
      ret &= test_hash3shkbig();
      ret &= test_hash3shkref();
      ret &= test_hash3shkrefloop();
      ret &= test_hash3shkrefloop2();
      ret &= test_keysmake();
      ret &= test_keyssecr();
      ret &= test_keyssign();
      ret &= test_keyssvrfy();
      ret &= test_keyswrite();
      //ret &= test_keyssvrfyloop(); // Slow as fudge
    }
  }
  if (ret) printf("OK\n");
  else printf("Not OK\n");
}

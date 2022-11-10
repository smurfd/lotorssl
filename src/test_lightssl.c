//                                                                            //
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "lighthash.h"
#include "lighthash3.h"
#include "lightcrypto.h"
#include "lightkeys.h"

void print_usage() {
  printf("Usage: ./build/test_lightssl <test>\n");
  printf("  <test> crypto_cli | crypto_srv | hash | hash3 | keys\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    print_usage();
    exit(0);
  }
  if (strcmp(argv[1], "crypto_cli") == 0) {
    int s = lightcrypto_init("127.0.0.1", "9998", false);

    if (s >= 0) {
      u64 dat[BLOCK], cd[BLOCK], i;
      key k1, k2;
      head h;

      lightcrypto_transferkey(s, false, &h, &k1);
      k2 = lightcrypto_genkeys(h.g, h.p);
      lightcrypto_transferkey(s, true, &h, &k2);
      lightcrypto_genshare(&k1, &k2, h.p, false);
      printf("share : 0x%.16llx\n", k1.shar);
      for (i = 0; i < 12; i++) {
        dat[i] = (u64)i; lightcrypto_crypt(dat[i],k1,&cd[i]);
      }
      lightcrypto_transferdata(s, cd, &h, true, 11);
      lightcrypto_end(s);
    }
    // locally generate two keypairs
    srand(time(0));
    lightcrypto_keys();
  } else if (strcmp(argv[1], "crypto_srv") == 0) {
    int s = lightcrypto_init("127.0.0.1", "9998", true);
    sock *cli = NULL;

    if (lightcrypto_listen(s, cli) < 0) {printf("Can't Thread\n"); exit(0);}
    lightcrypto_end(s);
  } else if (strcmp(argv[1], "hash") == 0) {
    char* ra = "555CFC37FC24D4971DE9B091EF13401B8C5CB8B5B55804DA571FB201CBB4F"
      "C5D147AC6F528656456651606546CA42A1070BDFD79D024F3B97DD1BDAC7E70F3D1";
    char *s = malloc(sha_blk_sz);

    lighthash_hash_new("smurfd", s);
    assert(lighthash_hash_test() == 1);
    assert(strcmp(ra, s) == 0);
    free(s);
    printf("OK\n");
  } else if (strcmp(argv[1], "hash3") == 0) {
    char s[128] = {0};
    char hash[] = "5c452b35648528cf3a00a42021489011dd455b78fc34190c7680173b2d"
      "cdcc7d61e73d4f2c51051e45d26215f9f7729b8986549e169dcee3280bed61cda25f20";
    uint8_t *smurfd = (uint8_t*)"smurfd";

    lighthash3_hash_new(smurfd, s);
    assert(strcmp(s, hash) == 0);
    printf("OK\n");
  } else if (strcmp(argv[1], "keys") == 0) {
    cur *c = malloc(sizeof(cur));
    bit a, b;
    pt *p1 = malloc(sizeof(pt)), *p2 = malloc(sizeof(pt));
    lightecdh_curves_init(c);
    lightecdh_bit_copy(&a, (*c).ecdh_x, c);
    lightecdh_bit_copy(&b, (*c).ecdh_y, c);
    print_bit(a, c);
    print_bit(b, c);
    lightecdh_point_copy(p1, *p2, c);
    lightecdh_curves_end(c);
    free(p2); free(p1);
    printf("OK\n");
  } else {print_usage();}
}

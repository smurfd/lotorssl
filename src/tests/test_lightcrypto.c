//                                                                            //
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include "../lightcrypto.h"
#include "../lighttools.h"

int main(int argc, char **argv) {
  char s0[] = "c211cmZkIGFuZCBtb3JlIHN0dWZm", s1[257], data[LEN];
  uint8_t s2[] = "smurfd and more stuff", s3[257], s4[LEN];
  int d = 0;

  if (argc == 1) {
    lchandle_cert("build/debug/ca.key", data);
    lchandle_asn("build/debug/ca256.cms");
  } else if (argc == 3) {
    lchandle_cert(argv[1], data);
    lchandle_asn(argv[2]);
    lcdecode64((char*)data, strlen((char*)data), &d, s4);
  }
  lcdecode64(s0, strlen(s0), &d, s3);
  lcencode64(s2, strlen("smurfd and more stuff"), &d, s1);
  assert(strcmp(s1, s0) == 0);
  printf("OK\n");
  return 0;
}

// Auth: smurfd, 2025 More reading at the bottom of the file; 2 spacs indent; 150 width                                                             //
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "../ec.h"

int main(void) {
  uint32_t private[8] = {0}, public[16] = {0}, hash[8] = {0}, sig[16] = {0};
  const curve *e = getcurve();
  make_keypair(public, private, e); // TODO: does nothing
  memcpy(hash, public, sizeof(hash));
  sign(private, hash, sizeof(hash), sig, e); // TODO: does nothing
  verify(public, hash, sizeof(hash), sig, e); // TODO: does nothing
  debug_printcurve(e);
  printf("OK\n");
}

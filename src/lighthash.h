//                                                                                                                    //
// Code grabbed from https://www.rfc-editor.org/rfc/rfc6234 and massaged
#ifndef LIGHTHASH_H
#define LIGHTHASH_H 1
#include <stdint.h>
#include <stdbool.h>
#include "lightdefs.h"

void hash_new(char *ss, const uint8_t *n);
void shake256(uint8_t *out, uint32_t outlen, const uint8_t *in, uint32_t inlen);
#endif

#ifndef TOOLING_H
#define TOOLING_H 1
#include <stdint.h>
#define LEN 4096
#define u64 unsigned long long int // because linux uint64_t is not same as on mac
void bit_hex_str(char hs[], const uint8_t *d, const int len);
void bit_pack(u64 big[], const uint8_t byte[]);
void bit_unpack(uint8_t byte[], const u64 big[]);
int base64enc(char ed[], const uint8_t *data, int inl);
int base64dec(uint8_t dd[], const char *data, int inl);
u64 u64rnd(void);
void u64rnd_array(uint8_t h[], u64 k[], int len);
#endif

#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include "tooling.h"

// Static variables
static char enc[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
  'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
  's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};
static char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

//
// 0-255 to 0x0 to 0xff
static void to_hex(uint8_t h[], uint8_t d) {
  h[0] = d >> 4;
  h[1] = d & 0xf;
}

static void to_hex_chr(char hs[], uint8_t h[]) {
  hs[0] = hex[h[0]];
  hs[1] = hex[h[1]];
}

static uint32_t oct(int i, int inl, const uint8_t d[]) {
  if (i < inl) return d[i];
  return 0;
}

static uint32_t sex(const char d[], char c[], int i) {
  if (d[i] == '=') return (0 & i++);
  return c[(int)d[i]];
}

//
// Convert a hex bitstring to a string
void bit_hex_str(char hs[], const uint8_t *d, const int len) {
  int co = 2;
  hs[0] = '0';
  hs[1] = 'x';
  for (int i = 0; i < len; i++) {
    uint8_t h[2];
    char hc[2];
    to_hex(h, d[i]);
    to_hex_chr(hc, h);
    hs[co++] = hc[0];
    hs[co++] = hc[1];
  }
  hs[len*2+2] = '\0';
}

// big[i] =
// ((uint64_t)dig[0] << 56) |
// ((uint64_t)dig[1] << 48) |
// ((uint64_t)dig[2] << 40) |
// ((uint64_t)dig[3] << 32) |
// ((uint64_t)dig[4] << 24) |
// ((uint64_t)dig[5] << 16) |
// ((uint64_t)dig[6] << 8) |
// (uint64_t)dig[7];
//
// Bit packing function uint8 to uint64
void bit_pack(u64 big[], const uint8_t byte[]) {
  for(uint32_t i = 0; i < 6; ++i) {
    const uint8_t *dig = byte + 8 * (6 - 1 - i); big[i] = 0;
    for (int j = 7; j >= 0; j--) {
      big[i] |= ((u64)dig[7 - j] << (j * 8));
    }
  }
}

// dig[0] = big[i] >> 56;
// dig[1] = big[i] >> 48;
// dig[2] = big[i] >> 40;
// dig[3] = big[i] >> 32;
// dig[4] = big[i] >> 24;
// dig[5] = big[i] >> 16;
// dig[6] = big[i] >> 8;
// dig[7] = big[i];
//
// Bit unpack uint64 to uint8
void bit_unpack(uint8_t byte[], const u64 big[]) {
  for(uint32_t i = 0; i < 6; ++i) {
    uint8_t *dig = byte + 8 * (6 - 1 - i);
    for (int j = 7; j >= 0; j--) {
      dig[7 - j] = big[i] >> (j * 8);
    }
  }
}

//
// Base64 encoder
int base64enc(char ed[], const uint8_t *data, int inl) {
  int tab[] = {0, 2, 1}, ol = 4 * ((inl + 2) / 3);
  for (int i = 0, j = 0; i < inl;) {
    uint32_t a = oct(i++, inl, data), b = oct(i++, inl, data), c = oct(i++, inl, data),tri = (a << 0x10)+(b << 0x08) + c;
    for (int k = 3; k >=0; k--) {
      ed[j++] = enc[(tri >> k * 6) & 0x3f];
    }
  }
  for (int i = 0; i < tab[inl % 3]; i++) {
    ed[ol - 1 - i] = '=';
  }
  ed[ol] = '\0';
  return ol;
}

//
// Base64 decoder
int base64dec(uint8_t dd[], const char *data, int inl) {
  static char dec[LEN] = {0};
  int ol = inl / 4 * 3;
  for (int i = 1; i <= 2; i++) {if (data[inl - i] == '=') ol--;}
  for (int i = 0; i < 64; i++) dec[(uint8_t)enc[i]] = i;
  for (int i = 0, j = 0; i < inl;) {
    uint32_t a = sex(data, dec, i++), b = sex(data, dec, i++), c = sex(data, dec, i++), d = sex(data, dec, i++);
    uint32_t tri = (a << 3 * 6) + (b << 2 * 6) + (c << 1 * 6) + (d << 0 * 6);
    if (j < ol) {
      for (int k = 2; k >= 0; k--) {
        dd[j++] = (tri >> k * 8) & 0xff;
      }
    }
  }
  return ol;
}

//
// urandom generate u64
u64 u64rnd(void) {
  u64 f7 = 0x7fffffffffffffff;
  int r[5], f = open("/dev/urandom", O_RDONLY);
  int rr = read(f, &r, sizeof r);
  close(f);
  if (rr < 0) return -1;
  return (r[0] & f7) << 48 ^ (r[1] & f7) << 35 ^ (r[2] & f7) << 22 ^ (r[3] & f7) << 9 ^ (r[4] & f7) >> 4;
}

void u64rnd_array(uint8_t h[], u64 k[], int len) {
  u64 f7 = 0x7fffffffffffffff;
  int r[2*len], f = open("/dev/urandom", O_RDONLY);
  int rr = read(f, &r, sizeof r);
  close(f);
  if (rr >= 0)
  for (int i = 0; i < len; ++i) {
    h[i] = (uint8_t)(r[i] & f7);
    k[i] = (u64)(r[i] & f7);
  }
}

// Auth: smurfd, 2023 More reading & Borrow/Stolen parts read at the bottom of the file; 2 spacs indent; 120 width    //
#ifndef CRPT_H
#define CRPT_H 1
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <netinet/in.h>
#include "bmec.h"

#define u64 unsigned long long int // because linux uint64_t is not same as on mac
#define A1INTEGER 0x02 // Header byte of the ASN.1 type INTEGER
#define A1BITSTRI 0x03 // Header byte of the ASN.1 type BIT STRING
#define A1OCTSTRI 0x04 // Header byte of the ASN.1 type OCTET STRING
#define A1NULL000 0x05 // Header byte of the ASN.1 type NULL
#define A1OBJIDEN 0x06 // Header byte of the ASN.1 type OBJECT IDENTIFIER
#define A1SEQUENC 0x30 // Header byte of the ASN.1 type SEQUENCE
#define A1SET0000 0x31 // Header byte of the ASN.1 type SET
#define A1UTF8STR 0x12 // Header byte of the ASN.1 type UTF8String
#define A1PRNTSTR 0x19 // Header byte of the ASN.1 type PrintableString
#define A1T61STRI 0x20 // Header byte of the ASN.1 type T61String
#define A1IA5STRI 0x22 // Header byte of the ASN.1 type IA5String
#define A1UTCTIME 0x23 // Header byte of the ASN.1 type UTCTime
#define A1GENTIME 0x24 // Header byte of the ASN.1 type GeneralizedTime
#define BLOCK 1024 // Imitate pythons %. -1 % 5 = 4, not -1
#define KLEN 4096
#define LEN 4096

typedef struct asn asn;
typedef struct keys key;
typedef struct sockaddr sock;
typedef struct sockaddr_in sock_in;

struct keys {u64 publ, priv, shar;};
struct asn {
  uint8_t type, pos;
  uint32_t len;
  const uint8_t *data;
};
typedef struct header {
  u64 len;                   // length
  u64 ver;                   // version
  u64 g;                     // global
  u64 p;                     // private
} head;

typedef struct cryptokeys {
  u64 publ;                  // public key
  u64 priv;                  // private key
  u64 shar;                  // shared key
} cryptokey;

typedef struct connection {
  int socket;                // socket used for connection
  int *clisocket;            // clientsocket
  uint8_t type;              // what type of client/server: 1 = keyvaluestore, 2 = tablesdb
  uint8_t err;               // error
} connection;

typedef struct sockets {
  int8_t descriptor;         // socket descriptor
  struct sockaddr_in addr;   // socket addr
} sockets;

typedef struct encd {
  uint32_t data[1024];
} encd;

connection connection_init(int8_t descriptor, int type);
sockets communication_init(const char *host, const char *port);

// Send/Receive
void send_publickey(connection c, bint *pubx, bint *puby);
void receive_publickey(connection c, bint *pubx, bint *puby);
int send_encrypteddata(connection c, encd *e);
int receive_encrypteddata(connection c, encd *e);

// Generators
void handler_cryptography(u64 data, cryptokey k, u64 *enc);

connection crpt_client_init(const char *host, const char *port);
void crpt_client_end(connection c);
int crpt_client_handle(connection conn);
connection crpt_server_init(const char *host, const char *port);
void crpt_server_end(connection c);
int crpt_server_handle(connection conn);

// Tooling
u64 crypto_handle_cert(char d[LEN], const char *cert);
u64 crypto_handle_asn(char c[LEN], const char *cert);
int base64enc(char ed[], const uint8_t *data, int inl);
int base64dec(uint8_t dd[], const char *data, int inl);
#endif
// Very simple handshake

//    |                                                     |                    .
// cli|                                                     |srv                 .
//    |                                                     |                    .
//                                                                               .
//     _____________ [1] TCP HANDSHAKE _____________________                     .
//                                                                               |
//     ----- >>> --- [1.1] syn ------------------- >   ----v                     |
//     v---- <   --- [1.2] syn ack --------------- <<< -----        handled by os|
//     ----- >>> --- [1.3] ack ------------------- >   -----                     |
//                              v                                                |
//                                                                               .
//     _____________ [2] TLS HANDSHAKE _____________________                     .
//                                                                               .
//     ----- >>> --- [2.1] client hi ------------- >   ----v                     .
//     ----- <   --- [2.1] server hi ------------- <<< -----                     .
//     v---- <   --- [2.2] verify server crt ----- <<< -----                     .
//     ----- >>> --- [2.3] client crt ------------ >   -----                     .
//     ----- >>> --- [2.4] key exchange ---------- >   -----                     .
//     ----- >>> --- [2.5] change cipher spec ---- >   -----                     .
//     ----- >>> --- [2.6] client finish --------- >   ----v                     .
//     ----- <   --- [2.7] change cipher spec ---- <<< -----                     .
//     v---- <   --- [2.8] server finished ------- <<< -----                     .
//     =-=-= >>> -=- [2.9] encrypted traffic -=-=- <<< -=-=-                     .
//                                                                               .
//     ...                                                                       .
//     ...                                                                       .
//     ...                                                                       .
//     ...                                                                       .


// asn1 - stolen / inspired from https://gitlab.com/mtausig/tiny-asn1

// [1] https://www.khanacademy.org/computing/computers-and-internet/xcae6f4a7ff015e7d:the-internet/xcae6f4a7ff015e7d:transporting-packets/a/transmission-control-protocol--tcp
// https://en.wikipedia.org/wiki/Handshaking#TCP_three-way_handshake
//
// [2] https://www.khanacademy.org/computing/computers-and-internet/xcae6f4a7ff015e7d:online-data-security/xcae6f4a7ff015e7d:secure-internet-protocols/a/transport-layer-security-protocol-tls
// https://en.wikipedia.org/wiki/Transport_Layer_Security#TLS_handshake
//
// [2.1]
//   prot ver : tls 1.3
//   cipher suite : TLS_RSA_256_SHA
// [2.2]
//   prot ver : tls 1.3
//   cipher suite : TLS_RSA_256_SHA
//   cert : pubkey : 0x123456789abcdef
// [2.3]
// [2.4]
//   cli send pre-master key,
//   encrypted with servers public key
//   cli calculate shared key from pre-master
//   store preshared key locally
// [2.5]
// [2.6]
//   send "finish" encrypted with calculated share key
// [2.7]
// [2.8]
//   server calculate shared key & try to decrypt clients "finish
//   if successful, send back "finish" encrypted
// [2.9]
//   cli send data using symmetric encryption and shared key

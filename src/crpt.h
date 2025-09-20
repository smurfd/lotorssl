// Auth: smurfd, 2023 More reading & Borrow/Stolen parts read at the bottom of the file; 2 spacs indent; 120 width    //
#ifndef CRYPTO_H
#define CRYPTO_H 1
#include <stdbool.h>
#include <stdint.h>
#include <netinet/in.h>
#include "bmec.h"

#define u64 unsigned long long int // because linux uint64_t is not same as on mac
#define BLOCK 1024

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
  bint data[1024];
} encd;

connection connection_init(int8_t descriptor, int type);
sockets communication_init(const char *host, const char *port);

// Send/Receive
//int send_cryptodata(connection c, void* data, head *h, u64 len);
//void send_cryptokey(connection c, head *h, cryptokey *k);
//int receive_cryptodata(connection c, void* data, head *h, u64 len);
//void receive_cryptokey(connection c, head *h, cryptokey *k);

void send_publickey(connection c, bint *pubx, bint *puby);
void receive_publickey(connection c, bint *pubx, bint *puby);
int send_encrypteddata(connection c, encd e);
int receive_encrypteddata(connection c, encd e);

// Generators
//cryptokey generate_cryptokeys(head *h);
void handler_cryptography(u64 data, cryptokey k, u64 *enc);

// Tooling
//int usage(char *arg, int count, char *clisrv);
#endif

// Very simple handshake
/*
```
    |                                                     |                    .
 cli|                                                     |srv                 .
    |                                                     |                    .
                                                                               .
     _____________ [1] TCP HANDSHAKE _____________________                     .
                                                                               |
     ----- >>> --- [1.1] syn ------------------- >   ----v                     |
     v---- <   --- [1.2] syn ack --------------- <<< -----        handled by os|
     ----- >>> --- [1.3] ack ------------------- >   -----                     |
                              v                                                |
                                                                               .
     _____________ [2] TLS HANDSHAKE _____________________                     .
                                                                               .
     ----- >>> --- [2.1] client hi ------------- >   ----v                     .
     ----- <   --- [2.1] server hi ------------- <<< -----                     .
     v---- <   --- [2.2] verify server crt ----- <<< -----                     .
     ----- >>> --- [2.3] client crt ------------ >   -----                     .
     ----- >>> --- [2.4] key exchange ---------- >   -----                     .
     ----- >>> --- [2.5] change cipher spec ---- >   -----                     .
     ----- >>> --- [2.6] client finish --------- >   ----v                     .
     ----- <   --- [2.7] change cipher spec ---- <<< -----                     .
     v---- <   --- [2.8] server finished ------- <<< -----                     .
     =-=-= >>> -=- [2.9] encrypted traffic -=-=- <<< -=-=-                     .
                                                                               .
     ...                                                                       .
     ...                                                                       .
     ...                                                                       .
     ...                                                                       .
```
*/

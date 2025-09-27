// Auth: smurfd, 2025 More reading & Borrow/Stolen parts read at the bottom of the file; 2 spacs indent; 150 width                                  //
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "ciph.h"
#include "bmec.h"
#include "crpt.h"

//
// Communication init
sockets communication_init(const char *host, const char *port) {
  sockets sock;
  sock.descriptor = socket(AF_INET , SOCK_STREAM , 0);
  if (sock.descriptor == -1) {
    perror("\"[o.o]\"  Could not create a socket\n");
    exit(0);
  }
  memset(&(sock.addr), '\0', sizeof(sock.addr));
  sock.addr.sin_family = AF_INET;
  sock.addr.sin_port = atoi(port);
  sock.addr.sin_addr.s_addr = inet_addr(host);
  return sock;
}

//
// Connection init
connection connection_init(int8_t descriptor, int type) {
  connection c;
  c.socket = descriptor;
  c.type = type;
  if (descriptor >= 0) c.err = 0;
  else c.err = -1;
  return c;
}

//
// Send public key
void send_publickey(connection c, bint *pubx, bint *puby) {
  int sock = *((connection*)&c)->clisocket;
  send(sock, pubx, sizeof(bint), 0);
  send(sock, puby, sizeof(bint), 0);
}

//
// Receive public key
void receive_publickey(connection c, bint *pubx, bint *puby) {
  int sock = *((connection*)&c)->clisocket;
  recv(sock, pubx, sizeof(bint), 0);
  recv(sock, puby, sizeof(bint), 0);
}

//
// Send encrypted data
int send_encrypteddata(connection c, encd *e) {
  int sock = *((connection*)&c)->clisocket;
  return send(sock, e, sizeof(encd), 0);
}

//
// Receive encrypted data
int receive_encrypteddata(connection c, encd *e) {
  int sock = *((connection*)&c)->clisocket;
  return recv(sock, e, sizeof(encd), 0);
}

static void *server_connection_handler_ssl(void *conn) { // TODO: use template from examples
  if (((connection*)conn)->socket == -1) return (void*) - 1;
  return 0;
}

static void *server_connection_handler(void *conn) {
  uint32_t iv[32] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff},
  plain[32] = {0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff, 0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff},
  cipher[32] = {0}, tag[32] = {0}, tag2[32] = {0}, aad[32] = {0}, plain2[32] = {0}, red[32] = {0};
  bint alsk = bcreate(), alpkx = bcreate(), alpky = bcreate(), bopkx = bcreate(), bopky = bcreate();
  bint boshrx = bcreate(), boshry = bcreate();
  encd re, se;
  if (((connection*)conn)->socket == -1) return (void*) - 1;                     // Handshake vvv
  genkeypair(&alpkx, &alpky, &alsk);                         // Generate server keypair         |
  send_publickey(*(connection*)conn, &alpkx, &alpky);        // Send public key to client       |
  receive_publickey(*(connection*)conn, &bopkx, &bopky);     // Receive public key from client  |
  gensharedsecret(&boshrx, &boshry, &alsk, &bopkx, &bopky);  // Generate Server shared secret   |
  gcm_ciphertag32bit(cipher, tag, boshrx.wrd, iv, plain, aad, 32); // Encrypt data              |
  memcpy(se.data, cipher, 32 * sizeof(uint32_t));            // Copy encrypted data             |
  send_encrypteddata(*(connection*)conn, &se);               // Send encrypted data to client   |
  if (receive_encrypteddata(*(connection*)conn, &re) > 0) {                      // Handshake ^^^
    memcpy(red, re.data, 32 * sizeof(uint32_t));
    gcm_inv_ciphertag32bit(plain2, tag2, boshrx.wrd, iv, red, aad, tag);
    assert(memcmp(plain, plain2, 32 * sizeof(uint32_t)) == 0);
    pthread_t ssl_thread;
    if (pthread_create(&ssl_thread, NULL, server_connection_handler_ssl, (void*)conn) < 0) {
      perror("\"[o.o]\"  Could not create thread");
    }
    pthread_join(ssl_thread, NULL);
    printf("\"[o.o]\"  Mmmm food\n");
  }
  return 0;
}

static int server_run(const char *host, const char *port) {
  sockets sock = communication_init(host, port);
  if (bind(sock.descriptor, (struct sockaddr*)&(sock.addr), sizeof(sock.addr)) < 0) {
    perror("\"[o.o]\"  Bind error");
    exit(0);
  }
  listen(sock.descriptor, 3);
  return sock.descriptor;
}

int crpt_server_handle(connection conn) {
  int client_sock, c = sizeof(struct sockaddr_in);
  struct sockaddr_in client;
  while ((client_sock = accept(conn.socket, (struct sockaddr*)&client, (socklen_t*)&c))) {
    pthread_t thread;
    conn.clisocket = &client_sock;
    if (pthread_create(&thread, NULL, server_connection_handler, (void*)&conn) < 0) {
      perror("\"[o.o]\"  Could not create thread");
      return 1;
    }
    pthread_join(thread, NULL);
  }
  if (client_sock < 0) {
    perror("\"[o.o]\"  No clients connected");
    return 1;
  }
  return client_sock;
}

//
// Initialize server
connection crpt_server_init(const char *host, const char *port) {
  int type = 1, socket_desc = server_run(host, port);
  printf("\"[o.o]\"  eating food...\n");
  return connection_init(socket_desc, type);
}

//
// End connection
void crpt_server_end(connection c) {
  close(c.socket);
}

static void *client_connection_handler_ssl(void *conn) { // TODO: use template from examples
  if (((connection*)conn)->socket == -1) return (void*) - 1;
  return 0;
}

static void *client_connection_handler(void *conn) {
  uint32_t iv[32] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff},
  plain[32] = {0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff, 0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff},
  cipher[32] = {0}, tag[32] = {0}, tag2[32] = {0}, aad[32] = {0}, plain2[32] = {0}, red[32] = {0};
  bint alsk = bcreate(), alpkx = bcreate(), alpky = bcreate(), bopkx = bcreate(), bopky = bcreate();
  bint boshrx = bcreate(), boshry = bcreate();
  encd re, se;
  if (((connection*)conn)->socket == -1) return (void*) - 1;                    // Handshake vvv
  genkeypair(&alpkx, &alpky, &alsk);                         // Generate client keypair        |
  receive_publickey(*(connection*)conn, &bopkx, &bopky);     // Receive server public key      |
  send_publickey(*(connection*)conn, &alpkx, &alpky);        // Send server public key         |
  gensharedsecret(&boshrx, &boshry, &alsk, &bopkx, &bopky);  // Generate client shared secret  |
  gcm_ciphertag32bit(cipher, tag, boshrx.wrd, iv, plain, aad, 32); // Encrypt data             |
  memcpy(se.data, cipher, 32 * sizeof(uint32_t));            // Copy encrypted data            |
  receive_encrypteddata(*(connection*)conn, &re);            // Receive encrypted data         |
  memcpy(red, re.data, 32 * sizeof(uint32_t));               // Copy received data             |
  gcm_inv_ciphertag32bit(plain2, tag2, boshrx.wrd, iv, red, aad, tag); // Decrypt data         |
  assert(memcmp(plain, plain2, 32 * sizeof(uint32_t)) == 0); // Assert encryption is ok        |
  if (send_encrypteddata(*(connection*)conn, &se) >= 0) {                       // Handshake ^^^
    pthread_t ssl_thread;
    if (pthread_create(&ssl_thread, NULL, client_connection_handler_ssl, (void*)conn) < 0) {
      perror("\"[o.o]\"  Could not create thread");
    }
    pthread_join(ssl_thread, NULL);
    printf("\"[o.o]\"  Found food *runs away*\n");
  }
  return 0;
}

static int client_run(const char *host, const char *port) {
  sockets sock = communication_init(host, port);
  if (connect(sock.descriptor , (struct sockaddr*)&(sock.addr) , sizeof(sock.addr)) < 0) {
    perror("\"[o.o]\"  Connection error");
    return 1;
  }
  return sock.descriptor;
}

int crpt_client_handle(connection conn) {
  pthread_t thread;
  conn.clisocket = &(conn.socket);
  if (pthread_create(&thread, NULL, client_connection_handler, (void*)&conn) < 0) {
    perror("\"[o.o]\"  Could not create a thread");
    return 1;
  }
  pthread_join(thread, NULL);
  return conn.socket;
}

//
// Initialize client
connection crpt_client_init(const char *host, const char *port) {
  int type = 1, socket_desc = client_run(host, port);
  printf("\"[o.o]\"  finding food...\n");
  return connection_init(socket_desc, type);
}

//
// End connection
void crpt_client_end(connection c) {
  close(c.socket);
}

// Very simple handshake

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
    perror("\"[o.o]\" \t Could not create a socket\n");
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
int send_encrypteddata(connection c, encd e) {
  int sock = *((connection*)&c)->clisocket;
  return send(sock, &e, sizeof(encd), 0);
}

//
// Receive encrypted data
int receive_encrypteddata(connection c, encd e) {
  int sock = *((connection*)&c)->clisocket;
  return recv(sock, &e, sizeof(encd), 0);
}

static void *server_connection_handler_ssl(void *conn) { // TODO: use template from examples
  if (((connection*)conn)->socket == -1) return (void*) - 1;
  return 0;
}

static void *server_connection_handler(void *conn) {
  bint alsk = bcreate(), alpkx = bcreate(), alpky = bcreate(), bopkx = bcreate(), bopky = bcreate();
  bint alshrx = bcreate(), alshry = bcreate(), boshrx = bcreate(), boshry = bcreate();
  if (((connection*)conn)->socket == -1) return (void*) - 1;                    // Handshake vvv
  // generate server keypair
  genkeypair(&alpkx, &alpky, &alsk); // Alice's keypair generated
  // send server public key to client
  send_publickey(*(connection*)conn, &alpkx, &alpky);
  // receive public key from client
  receive_publickey(*(connection*)conn, &bopkx, &bopky);
  // generate shared secret
  gensharedsecret(&alshrx, &alshry, &alsk, &alpkx, &alpky); // Alice's shared secret(but with alices private key)
  gensharedsecret(&boshrx, &boshry, &alsk, &bopkx, &bopky); // Bob's shared secret
  // verify that the shared secrets are the same
  verifysharedsecret(&alshrx, &alshry, &boshrx, &boshry, &alsk, &alsk); // Verify Alice's and Bob's shared secrets
  encd re, se;
  uint32_t iv[32] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff},
  plain[32] = {0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff, 0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff},
  cipher[32] = {0}, tag[32] = {0}, tag2[32] = {0}, aad[32] = {0}, plain2[32] = {0};
  gcm_ciphertag32bit(cipher, tag, alshrx.wrd, iv, plain, aad,  32);
  for (int i = 0; i < 1024; i++) {re.data[i] = bcreate(); se.data[i] = bcreate();}
  for (int i = 0; i < 32; i++) {wrd2bint(&se.data[i], cipher[i]);}
  send_encrypteddata(*(connection*)conn, se);
  if (receive_encrypteddata(*(connection*)conn, re) > 0) {                                           // Handshake ^^^
    uint32_t red[32] = {0};
    for (int i = 0; i < 32; i++) {red[i] = re.data[i].wrd[0];}
    gcm_inv_ciphertag32bit(plain2, tag2, alshrx.wrd, iv, red, aad, tag);
    // assert(memcmp(plain, plain2, 8 * sizeof(uint32_t)) == 0);
    pthread_t ssl_thread;
    if (pthread_create(&ssl_thread, NULL, server_connection_handler_ssl, (void*)conn) < 0) {
      perror("\"[o.o]\" \t Could not create thread");
    }
    pthread_join(ssl_thread, NULL);
  }
  return 0;
}

static int server_run(const char *host, const char *port) {
  sockets sock = communication_init(host, port);
  if (bind(sock.descriptor, (struct sockaddr*)&(sock.addr), sizeof(sock.addr)) < 0) {
    perror("\"[o.o]\" \t Bind error");
    exit(0);
  }
  listen(sock.descriptor, 3);
  return sock.descriptor;
}

int server_handle(connection conn) {
  int client_sock, c = sizeof(struct sockaddr_in);
  struct sockaddr_in client;
  while ((client_sock = accept(conn.socket, (struct sockaddr*)&client, (socklen_t*)&c))) {
    pthread_t thread;
    conn.clisocket = &client_sock;
    if (pthread_create(&thread, NULL, server_connection_handler, (void*)&conn) < 0) {
      perror("\"[o.o]\" \t Could not create thread");
      return 1;
    }
    pthread_join(thread, NULL);
  }
  if (client_sock < 0) {
    perror("\"[o.o]\" \t No clients connected");
    return 1;
  }
  return client_sock;
}

//
// Initialize server
connection server_init(const char *host, const char *port, int type) {
  int socket_desc = server_run(host, port);
  printf("\"[o.o]\" \t eating food...\n");
  return connection_init(socket_desc, type);
}

//
// End connection
void server_end(connection c) {
  close(c.socket);
}

static void *client_connection_handler_ssl(void *conn) { // TODO: use template from examples
  if (((connection*)conn)->socket == -1) return (void*) - 1;
  return 0;
}

static void *client_connection_handler(void *conn) {
  bint alsk = bcreate(), alpkx = bcreate(), alpky = bcreate(), bopkx = bcreate(), bopky = bcreate();
  bint alshrx = bcreate(), alshry = bcreate(), boshrx = bcreate(), boshry = bcreate();
  if (((connection*)conn)->socket == -1) return (void*) - 1;                    // Handshake vvv
  // generate client keypair
  genkeypair(&alpkx, &alpky, &alsk); // Alice's keypair generated
  // receive server public key from server
  receive_publickey(*(connection*)conn, &bopkx, &bopky);
  // send public key to server
  send_publickey(*(connection*)conn, &alpkx, &alpky);
  // generate shared secret
  gensharedsecret(&alshrx, &alshry, &alsk, &alpkx, &alpky); // Alice's shared secret(but with alices private key)
  gensharedsecret(&boshrx, &boshry, &alsk, &bopkx, &bopky); // Bob's shared secret
  // verify that the shared secrets are the same
  verifysharedsecret(&alshrx, &alshry, &boshrx, &boshry, &alsk, &alsk); // Verify Alice's and Bob's shared secrets
  encd re, se;
  uint32_t iv[32] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff},
  plain[32] = {0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff, 0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff},
  cipher[32] = {0}, tag[32] = {0}, tag2[32] = {0}, aad[32] = {0}, plain2[32] = {0};
  gcm_ciphertag32bit(cipher, tag, alshrx.wrd, iv, plain, aad,  32);
  for (int i = 0; i < 1024; i++) {re.data[i] = bcreate(); se.data[i] = bcreate();}
  for (int i = 0; i < 32; i++) {wrd2bint(&se.data[i], cipher[i]);}
  receive_encrypteddata(*(connection*)conn, re);
  uint32_t red[32] = {0};
  for (int i = 0; i < 32; i++) {red[i] = re.data[i].wrd[0];}
  gcm_inv_ciphertag32bit(plain2, tag2, alshrx.wrd, iv, red, aad, tag);
  // assert(memcmp(plain, plain2, 8 * sizeof(uint32_t)) == 0);
  if (send_encrypteddata(*(connection*)conn, se) >= 0) {                                        // Handshake ^^^
    pthread_t ssl_thread;
    if (pthread_create(&ssl_thread, NULL, client_connection_handler_ssl, (void*)conn) < 0) {
      perror("\"[o.o]\" \t Could not create thread");
    }
    pthread_join(ssl_thread, NULL);
  }
  return 0;
}

static int client_run(const char *host, const char *port) {
  sockets sock = communication_init(host, port);
  if (connect(sock.descriptor , (struct sockaddr*)&(sock.addr) , sizeof(sock.addr)) < 0) {
    perror("\"[o.o]\" \t Connection error");
    return 1;
  }
  return sock.descriptor;
}

int client_handle(connection conn) {
  pthread_t thread;
  conn.clisocket = &(conn.socket);
  if (pthread_create(&thread, NULL, client_connection_handler, (void*)&conn) < 0) {
    perror("\"[o.o]\" \t Could not create a thread");
    return 1;
  }
  pthread_join(thread, NULL);
  return conn.socket;
}

//
// Initialize client
connection client_init(const char *host, const char *port, int type) {
  printf("\"[o.o]\" \t finding food...\n");
  int socket_desc = client_run(host, port);
  return connection_init(socket_desc, type);
}

//
// End connection
void client_end(connection c) {
  close(c.socket);
}

// Very simple handshake

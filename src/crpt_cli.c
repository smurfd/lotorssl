#include <stdio.h>
#include "crpt.h"

int main(void) {
  connection c = crpt_client_init("127.0.0.1", "9998");
  if (crpt_client_handle(c) < 0) {
    printf("Cant connect to server\n");
    exit(0);
  }
  crpt_client_end(c);
}

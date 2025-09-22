#include <stdio.h>
#include "crpt.h"

int main(void) {
  printf("server\n");
  connection c = crpt_server_init("127.0.0.1", "9998");//, usage(argv[2], argc, "server"));
  crpt_server_handle(c);
  //crpt_server_end(c);
}

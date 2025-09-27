#include <stdio.h>
#include "crpt.h"

int main(void) {
  connection c = crpt_server_init("127.0.0.1", "9998");
  crpt_server_handle(c);
  crpt_server_end(c);
}

#include <stdio.h>

#include "socket.h"

#pragma comment(lib, "ws2_32.lib")

// Only ran when standalone server is hosted
int main(int argc, char **argv) {
  struct ZealSocket zeal_socket;
  socket_init(&zeal_socket);

  while (1) {
    char buf[BUFLEN];
    memset(buf, '0', BUFLEN);

    socket_recieve(&zeal_socket, buf);
    socket_send(&zeal_socket, buf);
  }

  socket_delete(&zeal_socket);

  return 0;
}
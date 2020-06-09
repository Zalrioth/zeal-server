#pragma once
#ifndef SOCKET_H
#define SOCKET_H

#include <sys/types.h>

#include "servercommon.h"

#define PORT 8888
#define BUFLEN 512

#ifdef PLATFORM_WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

struct ZealSocket {
  SOCKET udp_socket_h, tcp_socket_h;
  struct sockaddr_in serv_addr, client_addr;
  int s_len, recv_len;
  WSADATA wsa_data;
};
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

struct ZealSocket {
  int socket_h;
  struct sockaddr_in serv_addr, client_addr;
};
#endif

static inline int socket_init(struct ZealSocket *zeal_socket) {
#ifdef PLATFORM_WIN32
  zeal_socket->s_len = sizeof(zeal_socket->client_addr);

  // Init windows socket
  printf("\nInitialising Winsock...");
  if (WSAStartup(MAKEWORD(2, 2), &zeal_socket->wsa_data) != 0) {
    printf(" Failed.Error Code : %d", WSAGetLastError());
    return 1;
  }
  printf("Initialised.\n");

  // Create a socket
  if ((zeal_socket->udp_socket_h = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
    printf("Could not create socket : %d", WSAGetLastError());
    return 1;
  }
  printf("Socket created.\n");

  // Prepare the sockaddr_in structure
  zeal_socket->serv_addr.sin_family = AF_INET;
  zeal_socket->serv_addr.sin_addr.s_addr = INADDR_ANY;
  zeal_socket->serv_addr.sin_port = htons(PORT);

  // Bind
  if (bind(zeal_socket->udp_socket_h, (struct sockaddr *)&zeal_socket->serv_addr, sizeof(zeal_socket->serv_addr)) == SOCKET_ERROR) {
    printf("Bind failed with error code : %d", WSAGetLastError());
    return 1;
  }
  puts("Bind done ");

  return 0;
#else
#endif
}

static inline int socket_delete(struct ZealSocket *zeal_socket) {
  closesocket(zeal_socket->udp_socket_h);
  WSACleanup();

  return 0;
}

static inline int socket_recieve(struct ZealSocket *zeal_socket, char *buf) {
  printf("Waiting for data...");
  fflush(stdout);

  //try to receive some data, this is a blocking call
  if ((zeal_socket->recv_len = recvfrom(zeal_socket->udp_socket_h, buf, BUFLEN, 0, (struct sockaddr *)&zeal_socket->client_addr, &zeal_socket->s_len)) == SOCKET_ERROR) {
    printf("recvfrom() failed with error code : %d ", WSAGetLastError());
    return 1;
  }

  //print details of the client/peer and the data received
  char str1[BUFLEN];
  inet_ntop(AF_INET, &zeal_socket->client_addr.sin_addr, str1, BUFLEN);
  printf("Received packet from %s : %d\n", str1, ntohs(zeal_socket->client_addr.sin_port));
  printf("Data : %s\n ", buf);

  return 0;
}

static inline int socket_send(struct ZealSocket *zeal_socket, char *buf) {
  //now reply the client with nothing
  if (sendto(zeal_socket->udp_socket_h, buf, zeal_socket->recv_len, 0, (struct sockaddr *)&zeal_socket->client_addr, zeal_socket->s_len) == SOCKET_ERROR) {
    printf(" sendto() failed with error code : %d", WSAGetLastError());
    return 1;
  }

  return 0;
}

#endif  // SOCKET_H

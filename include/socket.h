#pragma once
#ifndef SOCKET_H
#define SOCKET_H

#include <sys/types.h>

#include "servercommon.h"

#define PORT 8888
#define BUFLEN 512

#ifdef PLATFORM_WIN32
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define sleep(a) Sleep((a))

struct ZealSocket {
  SOCKET udp_socket_h, tcp_socket_h;
  struct sockaddr_in serv_addr, client_addr;
  socklen_t s_len, recv_len;
  WSADATA wsa_data;
};
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#define sleep(a) usleep((a)*1000)

struct ZealSocket {
  int socket_h;
  struct sockaddr_in serv_addr, client_addr;
};
#endif

static inline int socket_init(struct ZealSocket *zeal_socket) {
#ifdef PLATFORM_WIN32
  zeal_socket->s_len = sizeof(zeal_socket->client_addr);
  zeal_socket->serv_addr.sin_family = AF_INET;
  zeal_socket->serv_addr.sin_addr.s_addr = INADDR_ANY;
  zeal_socket->serv_addr.sin_port = htons(PORT);
  u_long mode = 1;  // 1 to enable non-blocking socket

  // Init windows socket
  printf("Initialising Winsock...\n");
  if (WSAStartup(MAKEWORD(2, 2), &zeal_socket->wsa_data) != 0) {
    printf(" Failed.Error Code : %d", WSAGetLastError());
    return 1;
  }
  printf("Winsock OK!\n");

  // Create udp socket
  printf("Initialising UDP Socket...\n");
  if ((zeal_socket->udp_socket_h = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
    printf("Could not create udp socket : %d", WSAGetLastError());
    return 1;
  }
  ioctlsocket(zeal_socket->udp_socket_h, FIONBIO, &mode);
  printf("UDP socket created!\n");

  // Bind udp socket
  printf("Binding UDP Socket...\n");
  if (bind(zeal_socket->udp_socket_h, (struct sockaddr *)&zeal_socket->serv_addr, sizeof(zeal_socket->serv_addr)) == SOCKET_ERROR) {
    printf("Bind failed with error code : %d", WSAGetLastError());
    return 1;
  }
  printf("UDP socket binded!\n");

  // Create tcp socket
  printf("Initialising TCP Socket...\n");
  if ((zeal_socket->tcp_socket_h = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
    printf("Could not create tcp socket : %d", WSAGetLastError());
    return 1;
  }
  ioctlsocket(zeal_socket->tcp_socket_h, FIONBIO, &mode);
  printf("TCP socket created!\n");

  // Bind tcp socket
  printf("Binding TCP Socket...\n");
  if (bind(zeal_socket->tcp_socket_h, (struct sockaddr *)&zeal_socket->serv_addr, sizeof(zeal_socket->serv_addr)) == SOCKET_ERROR) {
    printf("Bind failed with error code : %d", WSAGetLastError());
    return 1;
  }
  // TODO: Allow this to be changed
  listen(zeal_socket->tcp_socket_h, USHRT_MAX);  // SOMAXCONN for absolute max
  printf("TCP socket binded!\n");

  return 0;
#else
#endif
}

static inline int socket_delete(struct ZealSocket *zeal_socket) {
#ifdef PLATFORM_WIN32
  closesocket(zeal_socket->udp_socket_h);
  WSACleanup();
#else
#endif
  return 0;
}

static inline int socket_recieve(struct ZealSocket *zeal_socket, char *buf) {
#ifdef PLATFORM_WIN32
  printf("Checking for udp data...\n");
  // Try to receive udp data
  if ((zeal_socket->recv_len = recvfrom(zeal_socket->udp_socket_h, buf, BUFLEN, 0, (struct sockaddr *)&zeal_socket->client_addr, &zeal_socket->s_len)) == SOCKET_ERROR)
    printf("recvfrom() failed with error code : %d\n", WSAGetLastError());
  else {
    // Print details of the client/peer and the data received
    char str1[BUFLEN];
    inet_ntop(AF_INET, &zeal_socket->client_addr.sin_addr, str1, BUFLEN);
    printf("Received packet from %s : %d\n", str1, ntohs(zeal_socket->client_addr.sin_port));
    printf("Data : %s\n ", buf);
  }

  printf("Checking for tcp data...\n");
  SOCKET client_con;
  if ((client_con = accept(zeal_socket->tcp_socket_h, (struct sockaddr *)&zeal_socket->client_addr, &zeal_socket->recv_len)) == INVALID_SOCKET)
    printf("accept() failed with error: %d\n", WSAGetLastError());
  else {
    int result = recv(zeal_socket->tcp_socket_h, buf, BUFLEN, 0);
  }

#else
#endif
  return 0;
}

static inline int socket_send(struct ZealSocket *zeal_socket, char *buf) {
#ifdef PLATFORM_WIN32
  //now reply the client with nothing
  if (sendto(zeal_socket->udp_socket_h, buf, zeal_socket->recv_len, 0, (struct sockaddr *)&zeal_socket->client_addr, zeal_socket->s_len) == SOCKET_ERROR) {
    printf("sendto() failed with error code : %d\n", WSAGetLastError());
    return 1;
  }
#else
#endif
  return 0;
}

#endif  // SOCKET_H

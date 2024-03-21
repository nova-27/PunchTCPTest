#include "stunutil.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const char *STUN_SERVER_ADDR = "stun.sipnet.net";
const char *STUN_SERVER_PORT = "3478";

u_int16_t getExternalPort(const sockaddr_in &localaddr) {
  addrinfo hints, *srv_addr;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  int err;
  if ((err = getaddrinfo(STUN_SERVER_ADDR, STUN_SERVER_PORT, &hints,
                         &srv_addr)) != 0) {
    fprintf(stdout, "getaddrinfo() failed: %s", gai_strerror(err));
    exit(EXIT_FAILURE);
  }

  int sock =
      socket(srv_addr->ai_family, srv_addr->ai_socktype, srv_addr->ai_protocol);
  if (bind(sock, (sockaddr *)&localaddr, sizeof(localaddr)) == -1) {
    perror("bind() failed");
    exit(EXIT_FAILURE);
  }
  if (connect(sock, srv_addr->ai_addr, srv_addr->ai_addrlen) == -1) {
    perror("connect() failed");
    exit(EXIT_FAILURE);
  }

  unsigned char binding_request[20];
  *(short *)(&binding_request[0]) =
      htons(0x0001); // Message Type (Binding Request this time)
  *(int *)(&binding_request[4]) =
      htonl(0x2112A442); // Magic Cookie (Fixed value to distinguish STUN
                         // traffic from other protocols)
  *(int *)(&binding_request[8]) =
      htonl(0x471B519F); // Transaction ID (Random value to pair up a request
                         // and corresponding response)
  if (send(sock, binding_request, sizeof(binding_request), 0) < 0) {
    perror("sendto() failed");
    exit(EXIT_FAILURE);
  }

  unsigned char buf[1020];
  if (recv(sock, buf, sizeof(buf), 0) < 0) {
    perror("recv() failed");
    exit(EXIT_FAILURE);
  }

  if (*(short *)(&buf[0]) != htons(0x0101)) {
    printf("invalid response.");
    exit(EXIT_FAILURE);
  }

  int i = 20;
  short attribute_type;
  short attribute_length;
  unsigned short port;
  // Continuously read attributes in the data section
  while (i < sizeof(buf)) {
    attribute_type = htons(*(short *)(&buf[i]));
    attribute_length = htons(*(short *)(&buf[i + 2]));
    // If the attribute is XOR_MAPPED_ADDRESS, parse it
    if (attribute_type == 0x0020) {
      port = ntohs(*(short *)(&buf[i + 6]));
      port ^= 0x2112;
      break;
    }
    i += 4 + attribute_length;
  }

  freeaddrinfo(srv_addr);
  close(sock);

  return port;
}
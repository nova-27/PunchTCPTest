#include "stunutil.h"
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/types.h>

using namespace std;

const uint16_t LISTEN_PORT = 25565;

int main() {
  sockaddr_in localaddr;
  memset(&localaddr, 0, sizeof(localaddr));
  localaddr.sin_family = AF_INET;
  localaddr.sin_port = LISTEN_PORT;

  const u_int16_t port = getExternalPort(localaddr);
  cout << port;

  return 0;
}

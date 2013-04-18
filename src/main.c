#include <stdio.h>

#include "server.h"
#include "client.h"

int main (int argc, char * argv[])
{
  server_t  server;
  robot_t   robot;

  switch (argc) {
    case 3: /* client */
      robot_client_run(&robot, argv[1], argv[2]);
      break;

    case 2: /* server */
      server_run(&server, argv[1]);
      break;

    default:
      fprintf(stderr, "usage: %s [hostname] port\n", argv[0]);
      return 1;
      break;
  }

  return 0;
}

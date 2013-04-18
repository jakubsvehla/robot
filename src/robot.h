#ifndef _ROBOT_
#define _ROBOT_

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <regex.h>

#include "helpers.h"

#define MAXDATASIZE             1024 // max number of bytes we can get at once

#define RESPONSE_GREETING       "210 My name is %s."
#define RESPONSE_OK             "240 OK (%d,%d)"
#define RESPONSE_SUCCESS        "260 SUCCESS %s"
#define RESPONSE_UNKNOWN        "500 UNKNOWN COMMAND"
#define RESPONSE_CRASH          "530 CRASH"
#define RESPONSE_CANNOT_LIFT    "550 CANNOT LIFT"
#define RESPONSE_PROC_OK        "571 PROCESSOR OK"
#define RESPONSE_BROKEN_UP      "572 ROBOT BROKE UP"
#define RESPONSE_PROC_FAILURE   "580 PROCESSOR %d NOT WORKING"

#define CMD_MOVE                "MOVE"
#define CMD_LEFT                "LEFT"
#define CMD_REPAIR              "REPAIR"
#define CMD_LIFT                "LIFT"

typedef enum direction_t
{
  UP,
  LEFT,
  DOWN,
  RIGHT
} direction_t;

typedef enum state_t
{
  PENDING,
  CMD_ALMOST_DONE,
  CMD_DONE
} state_t;

typedef struct robot_t
{
  int               fd;
  int               x;
  int               y;
  int               cnt;
  int               broken_proc;
  char            * cmd;
  int               cmd_len;
  char              buf [MAXDATASIZE * 2];
  int               buf_len;
  char              greeting [MAXDATASIZE];
  direction_t       dir;
  struct server_t * server;
  state_t           state;
} robot_t;

#endif /* _ROBOT_ */

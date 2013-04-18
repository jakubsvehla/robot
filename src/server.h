#ifndef _ROBOT_SERVER_
#define _ROBOT_SERVER_

#include "robot.h"

#define SECRET    "Hooray!"
#define GREETING  "Terminator"

typedef struct server_t
{
  char            * port;
  int               listener;
  int               fdmax;
  fd_set            master;
  fd_set            read_fds;
  struct robot_t  * robots[1024];
} server_t;

void robot_init                 (robot_t ** p, int fd);
void robot_destroy              (robot_t * r);
void robot_send                 (robot_t * r, const char * format, ...);
void robot_recv                 (robot_t * r);
void robot_greet                (robot_t * r);
void robot_unknown_cmd          (robot_t * r);
void robot_exec_cmd             (robot_t * r);
void robot_move                 (robot_t * r);
void robot_rotate               (robot_t * r);
void robot_repair               (robot_t * r, int proc);
void robot_lift                 (robot_t * r);
void robot_ok                   (robot_t * r);

void server_bind                (server_t * s, const char * port);
void server_listen              (server_t * s);
void server_accept              (server_t * s);
void server_run                 (server_t * s, const char * port);
void server_shutdown            (server_t * s);
void server_perror              (server_t * s);

#endif /* _ROBOT_SERVER_ */

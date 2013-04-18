#ifndef _ROBOT_CLIENT_
#define _ROBOT_CLIENT_

#include "robot.h"

#define MAXGROUPS               2
#define GREETING_REGEXP         "My name is ([^ ][^\\.]+)\\."

void robot_client_run           (robot_t * r, const char * hostname, const char * port);

/* Connects to the server */
void robot_client_connect       (robot_t * r, const char * hostname, const char * port);

/* Closes connection with the server */
void robot_client_close         (robot_t * r);

/* Receives data from the server */
void robot_client_recv          (robot_t * r);

/* Sends data to the server */
void robot_client_send          (robot_t * r, const char * str);

/* Gets name from the greeting */
void robot_client_get_greeting  (robot_t * r);

void robot_client_rotate        (robot_t * r);
void robot_client_move          (robot_t * r);

#endif /* _ROBOT_CLIENT_ */

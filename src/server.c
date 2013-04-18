#include "server.h"

void robot_init (robot_t ** p, int fd)
{
  robot_t * r     = (robot_t *) malloc(sizeof(robot_t));
  r->x            = (rand() % 34) - 17; /* <-17, 17> */
  r->y            = (rand() % 34) - 17; /* <-17, 17> */
  r->dir          = UP;
  r->fd           = fd;
  r->cnt          = 0;
  r->broken_proc  = 0;
  r->buf_len      = 0;
  r->cmd          = r->buf;
  r->cmd_len      = 0;
  r->state        = PENDING;

  strcpy(r->greeting, GREETING);
  memset(r->buf, 0, sizeof(r->buf));

  *p = r;
}

void robot_destroy (robot_t * r)
{
  r->server->robots[r->fd] = NULL;
  FD_CLR(r->fd, &(r->server->master));
  close(r->fd);
  free(r);
}

void robot_send (robot_t * r, const char * format, ...)
{
  char buf [256];
  va_list args;

  va_start(args, format);

  // memset(buf, 0, sizeof(buf));
  vsprintf(buf, format, args);
  printf("(%d) server: %s\n", r->fd, buf);

  strcat(buf, "\r\n");
  send(r->fd, buf, strlen(buf), 0);

  va_end(args);
}

void robot_recv (robot_t * r)
{
  int rt;
  char * buf;
  int i;

  buf = r->buf + r->buf_len;
  rt = recv(r->fd, buf, MAXDATASIZE, 0);

  /* Client closed connection */
  if (rt == 0) {
    printf("(%d) client: closed connection\n", r->fd);
    robot_destroy(r);
    return;
  }

  /* Connection error */
  if (rt < 0) {
    printf("(%d) client: connection error\n", r->fd);
    robot_destroy(r);
    return;
  }

  r->buf_len += rt;

  for (i = 0; i < rt; i ++) {

    switch (r->state) {
      case PENDING:
        if (*buf == '\r')
          r->state = CMD_ALMOST_DONE;
        break;

      case CMD_ALMOST_DONE:
        if (*buf == '\n')
          r->state = CMD_DONE;
        else
          r->state = PENDING;
        break;

      case CMD_DONE:
        break;
    }

    r->cmd_len ++;

    if (r->state == CMD_DONE) {
      if (r->cmd_len <= MAXDATASIZE) {
        *(buf - 1) = 0;
        robot_exec_cmd(r);
      }
      else
        robot_unknown_cmd(r);

      r->cmd_len = 0;
      r->cmd = buf + 1;
      r->state = PENDING;
    }

    buf++;
  }

  /* Shift the buffer */
  if (r->buf != r->cmd) {
    r->buf_len = r->cmd_len;
    memcpy(r->buf, r->cmd, r->buf_len);
    r->cmd = r->buf;
  }

  /* Empty the buffer if it's full */
  if (r->buf_len > MAXDATASIZE) {
    r->buf_len = 0;
  }
}

void robot_greet (robot_t * r)
{
  robot_send(r, RESPONSE_GREETING, r->greeting);
}

void robot_unknown_cmd (robot_t * r)
{
  robot_send(r, RESPONSE_UNKNOWN);
}

void robot_exec_cmd (robot_t * r)
{
  char * cmd, * arg, * greeting;
  int proc;

  /* Does the name match? */
  if (!(greeting = strtok(r->cmd, " ")) || strcmp(greeting, GREETING) != 0) {
    robot_unknown_cmd(r);
    return;
  }

  if (!(cmd = strtok(NULL, " "))) {
    robot_unknown_cmd(r);
    return;
  }

  /* MOVE */
  if (strcmp(cmd, CMD_MOVE) == 0) {
    robot_move(r);
  }
  /* LEFT */
  else if (strcmp(cmd, CMD_LEFT) == 0) {
    robot_rotate(r); /* TODO rename to robot_left */
  }
  /* REPAIR */
  else if (strcmp(cmd, CMD_REPAIR) == 0) {
    if (!(arg = strtok(NULL, " ")) || (sscanf(arg, "%d", &proc) != 1) || proc < 1 || proc > 9) {
      robot_unknown_cmd(r);
      return;
    }
    robot_repair(r, proc);
  }
  /* LIFT */
  else if (strcmp(cmd, CMD_LIFT) == 0) {
    robot_lift(r);
  }
  /* UNKNOWN */
  else {
    printf("Unknown command.\n");
    robot_unknown_cmd(r);
  }
}

void robot_move (robot_t * r)
{
  if (r->broken_proc > 0) {
    robot_send(r, RESPONSE_BROKEN_UP);
    robot_destroy(r);
    return;
  }

  r->cnt++;

  if (r->cnt > 6) {
    r->cnt = 0;
    r->broken_proc = (rand() % 9) + 1; // choose a random processor
    robot_send(r, RESPONSE_PROC_FAILURE, r->broken_proc);
    return;
  }

  switch (r->dir) {
    case UP:
      r->y++;
      break;
    case LEFT:
      r->x--;
      break;
    case DOWN:
      r->y--;
      break;
    case RIGHT:
      r->x++;
      break;
  }

  // Got out of the town
  if (r->x < -18 || r->x > 18 || r->y < -18 || r->y > 18) {
    robot_send(r, RESPONSE_CRASH);
    robot_destroy(r);
    return;
  }

  robot_ok(r);
}

void robot_rotate (robot_t * r)
{
  r->dir = (r->dir + 1) % 4;
  robot_ok(r);
}

void robot_repair (robot_t * r, int proc)
{
  r->cnt = 0;

  if (proc == r->broken_proc) {
    r->broken_proc = 0;
    robot_ok(r);
  } else {
    robot_send(r, RESPONSE_PROC_OK);
    robot_destroy(r);
  }
}

void robot_lift (robot_t * r)
{
  if (r->x == 0 && r->y == 0) {
    robot_send(r, RESPONSE_SUCCESS, SECRET);
  }
  else {
    robot_send(r, RESPONSE_CANNOT_LIFT);
  }

  robot_destroy(r);
}

void robot_ok (robot_t * r)
{
  robot_send(r, RESPONSE_OK, r->x, r->y);
}

void server_bind (server_t * s, const char * port)
{
  char              addr [INET6_ADDRSTRLEN];
  int               yes = 1; // for setsockopt() SO_REUSEADDR, below
  int               rv;
  struct addrinfo   hints, *ai, *p;

  // Get us a socket and bind it
  memset(&hints, 0, sizeof hints);
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_PASSIVE;

  if ((rv = getaddrinfo(NULL, port, &hints, &ai)) != 0) {
    exit(1);
  }

  for(p = ai; p != NULL; p = p->ai_next) {
    s->listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (s->listener < 0) {
      exit(1);
    }

    setsockopt(s->listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr), addr, sizeof(addr));
    printf("server: binding to %s\n", addr);

    if (bind(s->listener, p->ai_addr, p->ai_addrlen) < 0) {
      close(s->listener);
      exit(1);
    }

    break;
  }

  // If we got here, it means we didn't get bound
  if (p == NULL) {
    exit(1);
  }

  freeaddrinfo(ai); // all done with this
}

void server_listen (server_t * s)
{
  if (listen(s->listener, 16) < 0)
    exit(1);
}

void server_accept (server_t * s)
{
  struct sockaddr_storage   remoteaddr; // client address
  socklen_t                 addrlen;
  int                       newfd;
  char                      remote_ip[INET6_ADDRSTRLEN];

  addrlen = sizeof(remoteaddr);
  newfd = accept(s->listener, (struct sockaddr *) &remoteaddr, &addrlen);

  if (newfd == -1) {
    // server_perror("accept");
  } else {
    FD_SET(newfd, &(s->master)); // add to master set

    // Keep track of the max fd
    if (newfd > s->fdmax) {
      s->fdmax = newfd;
    }

    printf("(%d) client: new connection from %s\n",
      newfd, inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*) &remoteaddr), remote_ip, INET6_ADDRSTRLEN));

    robot_init(&(s->robots[newfd]), newfd);
    s->robots[newfd]->server = s;
    robot_greet(s->robots[newfd]);
  }
}

void server_run (server_t * s, const char * port)
{
  int i;

  server_bind(s, port);
  server_listen(s);

  // Clear the master and temp sets
  FD_ZERO(&(s->master));
  FD_ZERO(&(s->read_fds));

  // Add the listener to the master set
  FD_SET(s->listener, &(s->master));

  // Keep track of the biggest file descriptor
  s->fdmax = s->listener; // so far, it's this one

  while (1) {

    s->read_fds = s->master;
    if (select(s->fdmax + 1, &(s->read_fds), NULL, NULL, NULL) == -1) {
      exit(1);
    }

    for (i = 0; i <= s->fdmax; i ++) {
      if (FD_ISSET(i, &(s->read_fds))) {
        if (i == s->listener) {
          // Handle new connection
          server_accept(s);
        } else {
          // Handle data from a client
          robot_recv(s->robots[i]);
        }
      }
    }
  }
}

void server_shutdown (server_t * s)
{
  /* TODO */
}

void server_perror (server_t * s)
{
  /* TODO */
}

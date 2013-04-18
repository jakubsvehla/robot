#include "client.h"

void robot_client_run (robot_t * r, const char * hostname, const char * port)
{
  int tmp_x, tmp_y;

  robot_client_connect(r, hostname, port);

  robot_client_get_greeting(r);

  // Initial position
  robot_client_rotate(r);
  tmp_x = r->x;
  tmp_y = r->y;

  // Initial direction
  robot_client_move(r);

  if (tmp_x != r->x) {
    if (tmp_x > r->x) r->dir = LEFT;
    else r->dir = RIGHT;
  } else {
    if (tmp_y > r->y) r->dir = DOWN;
    else r->dir = UP;
  }

  // Move until x == 0
  if (r->x < 0) {
    // Move right
    while (r->dir != RIGHT) {
      robot_client_rotate(r);
    }

    while (r->x != 0) {
      robot_client_move(r);
    }

  }
  else if (r->x > 0) {
    // Move left
    while (r->dir != LEFT) {
      robot_client_rotate(r);
    }

    while (r->x != 0) {
      robot_client_move(r);
    }

  }

  // Move until y == 0
  if (r->y < 0) {
    // Move up
    while (r->dir != UP) {
      robot_client_rotate(r);
    }

    while (r->y != 0) {
      robot_client_move(r);
    }

  }
  else if (r->y > 0) {
    // Move down
    while (r->dir != DOWN) {
      robot_client_rotate(r);
    }

    while (r->y != 0) {
      robot_client_move(r);
    }
  }

  robot_client_send(r, "LIFT");
  robot_client_recv(r);
  printf("server: %s", r->buf);

  robot_client_close(r);
}

void robot_client_connect (robot_t * r, const char * hostname, const char * port)
{
  struct addrinfo   hints, *servinfo, *p;
  int               rv;
  char              s[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    exit(1);
  }

  // loop through all the results and connect to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((r->fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr), s, sizeof s);
    printf("client: connecting to %s\n", s);

    if (connect(r->fd, p->ai_addr, p->ai_addrlen) == -1) {
      close(r->fd);
      perror("client: connect");
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    exit(1);
  }

  freeaddrinfo(servinfo);
}

void robot_client_close (robot_t * r)
{
  close(r->fd);
}

void robot_client_recv (robot_t * r)
{
  int numbytes = 0, total = 0;

  memset(r->buf, 0, sizeof(r->buf));

  while (1) {

    if ((numbytes = recv(r->fd, &(r->buf[total]), MAXDATASIZE - 1, 0)) == -1) {
      perror("recv");
      exit(1);
    }

    if (strstr(r->buf, "\r\n") != NULL) {
      total += numbytes;
      break;
    }

    total += numbytes;
  }
}

void robot_client_send (robot_t * r, const char * str)
{
  char buf[MAXDATASIZE];

  memset(buf, 0, sizeof(buf));
  sprintf(buf, "%s %s\r\n", r->greeting, str);
  send(r->fd, buf, strlen(buf), 0);

  printf("client: %s", buf);
}

void robot_client_get_greeting (robot_t * r)
{
  char        regex[] = GREETING_REGEXP;
  regex_t     regex_compiled;
  regmatch_t  match[MAXGROUPS];

  memset(r->greeting, 0, sizeof(r->greeting));

  robot_client_recv(r);

  if (regcomp(&regex_compiled, regex, REG_EXTENDED)) {
    fprintf(stderr, "client: could not compile regular expression\n");
    robot_client_close(r);
    exit(1);
  }

  if (regexec(&regex_compiled, r->buf, MAXGROUPS, match, 0)) {
    fprintf(stderr, "client: greeting does not match regular expression\n");
    robot_client_close(r);
    exit(1);
  }

  strncpy(r->greeting, &(r->buf[match[1].rm_so]), match[1].rm_eo - match[1].rm_so);

  regfree(&regex_compiled);
}

void robot_client_rotate (robot_t * r)
{
  robot_client_send(r, "LEFT");
  robot_client_recv(r);

  sscanf(r->buf, RESPONSE_OK, &(r->x), &(r->y));

  r->dir = (r->dir + 1) % 4;

  printf("server: %s", r->buf);
}

void robot_client_move (robot_t * r)
{
  int broken_proc;
  char cmd [32];

  robot_client_send(r, "MOVE");
  robot_client_recv(r);
  printf("server: %s", r->buf);

  if (sscanf(r->buf, RESPONSE_PROC_FAILURE, &broken_proc) == 1) {
    sprintf(cmd, "REPAIR %d", broken_proc);
    robot_client_send(r, cmd);
    robot_client_recv(r);
    printf("server: %s", r->buf);

    robot_client_move(r);
  }
  else {
    sscanf(r->buf, RESPONSE_OK, &(r->x), &(r->y));
  }
}

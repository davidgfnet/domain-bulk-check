/* Copyright 1998 by the Massachusetts Institute of Technology.
 *
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 * M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <ares.h>

static void callback(void *arg, int status, int timeouts, struct hostent *host);

int main(int argc, char **argv) {
  ares_channel channel;
  int status, nfds, c, addr_family = AF_INET;
  fd_set read_fds, write_fds;
  struct timeval *tvp, tv;
  struct in_addr addr4;

  status = ares_library_init(ARES_LIB_INIT_ALL);
  if (status != ARES_SUCCESS)
    {
      fprintf(stderr, "ares_library_init: %s\n", ares_strerror(status));
      return 1;
    }

  addr_family = AF_INET;

  status = ares_init(&channel);
  if (status != ARES_SUCCESS)
    {
      fprintf(stderr, "ares_init: %s\n", ares_strerror(status));
      return 1;
    }

  std::string domain;
  while (std::cin >> domain) {
    /* Initiate the queries, one per command-line argument. */
    int n = 100;
    do
    {
        ares_gethostbyname(channel, domain.c_str(), addr_family, callback, (void*)domain.c_str());
    } while(n-- > 0 && std::cin >> domain);

    /* Wait for all queries to complete. */
    for (;;)
    {
      FD_ZERO(&read_fds);
      FD_ZERO(&write_fds);
      nfds = ares_fds(channel, &read_fds, &write_fds);
      if (nfds == 0)
        break;
      tvp = ares_timeout(channel, NULL, &tv);
      select(nfds, &read_fds, &write_fds, NULL, tvp);
      ares_process(channel, &read_fds, &write_fds);
    }
  }

  ares_destroy(channel);

  ares_library_cleanup();

  return 0;
}

static void callback(void *arg, int status, int timeouts, struct hostent *host)
{
  char **p;

  (void)timeouts;

  if (status != ARES_SUCCESS)
    return;

  //for (p = host->h_addr_list; *p; p++)
  //  {
	//unsigned char ip[4] = { *(*p+0), *(*p+1), *(*p+2), *(*p+3) };
   // }

  printf("%s\n", host->h_name);
}



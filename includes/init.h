# ifndef INIT_H
# define INIT_H

# include "args_parsing.h"
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>


void resolve_host(t_config *config);

# endif
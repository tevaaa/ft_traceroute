#ifndef CONFIG_H
# define CONFIG_H
# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <netinet/in.h>

typedef struct s_config {
    char            *target;           // hostname/IP
    struct sockaddr_in dest_addr;      // adresse destination
    char            *resolved_target_hostname;      // IP en string pour affichage
    
    int             max_hops;          // -m (défaut: 30)
    int             nqueries;          // -q (défaut: 3)
    int             port;              // -p (défaut: 33434)
    char            *source_addr;      // -s (optionnel)
    int             squeries;          // -N (défaut: 16)
    
    // Options système
    int             sockfd;
    int             timeout_ms;        // timeout en ms (défaut: 5000)
} t_config;

t_config   parse_arguments(int argc, char **argv);

#endif
#include "../includes/init.h"

void resolve_host(t_config *config) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    int ret = getaddrinfo(config->target, NULL, &hints, &res);
    if (ret != 0) {
        printf("./ft_traceroute: %s: unknown host\n", config->target);
        exit(EXIT_FAILURE);
    }
    config->dest_addr = *(struct sockaddr_in *)res->ai_addr;

    config->resolved_target_hostname = strdup(inet_ntoa(config->dest_addr.sin_addr));

    freeaddrinfo(res);
}
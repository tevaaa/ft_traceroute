#include "../includes/args_parsing.h"
#include "../includes/init.h"
#include "../includes/socket.h"

int main(int argc, char *argv[])
{
    t_config config;

    config = parse_arguments(argc, argv);

    resolve_host(&config);

    printf("traceroute to %s (%s), %d hops max\n", 
       config.target, 
       config.resolved_target_hostname, 
       config.max_hops);

    config.sockfd = create_raw_socket(config.source_addr);
    printf("Created Raw Socket: %d\n", config.sockfd);
    if (config.source_addr) {
            printf("✅ Bind sur adresse source: %s\n", config.source_addr);
    }

    close(config.sockfd);
    return 0;
}
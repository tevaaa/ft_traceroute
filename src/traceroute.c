#include "../includes/traceroute.h"
#include "../includes/args_parsing.h"

void traceroute_loop(t_config *config) {
    for (int ttl = 1; ttl <= config->max_hops; ttl++) {
        printf("%2d  ", ttl);
        
        if (setsockopt(config->sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
            printf("setsockopt error\n");
            continue;
        }
        
        bool destination_reached = false;
        struct sockaddr_in last_responder;
        memset(&last_responder, 0, sizeof(last_responder));

        // Send nqueries packets for this TTL (default: 3)
        for (int i = 0; i < config->nqueries; i++) {
            struct timeval start_time;
            gettimeofday(&start_time, NULL);
            
            // TODO: send_icmp_packet(config->sockfd, &config->dest_addr, i);
            // TODO: int response = receive_response(config->sockfd, &responder, 5000);
            // TODO: calculer RTT et afficher
            
            printf(" *");  // Placeholder pour l'instant
        }
        
        printf("\n");
        
        // TODO: vérifier si on a atteint la destination
        if (destination_reached) {
            break;
        }
    }
}
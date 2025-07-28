#include "../includes/traceroute.h"
#include "../includes/args_parsing.h"
#include "../includes/socket.h"

double calculate_rtt(struct timeval *start, struct timeval *end) {
    double start_ms = (start->tv_sec * 1000.0) + (start->tv_usec / 1000.0);
    double end_ms = (end->tv_sec * 1000.0) + (end->tv_usec / 1000.0);
    
    return end_ms - start_ms;
}

void traceroute_loop(t_config *config) {
    for (int ttl = 1; ttl <= config->max_hops; ttl++) {
        printf("%2d ", ttl);
        
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
            send_icmp_packet(config->sockfd, &config->dest_addr, i + 1);
            int received = receive_response(config->sockfd, &last_responder, config->timeout_ms, i + 1, &destination_reached);
            if (received > 0) {
                struct timeval end_time;
                gettimeofday(&end_time, NULL);
                double rtt = calculate_rtt(&start_time, &end_time);
                if (i == 0)
                    printf(" %s %.3f ms", inet_ntoa(last_responder.sin_addr), rtt);
                else
                    printf(" %.3f ms", rtt);
            }
            else {
                printf(" *");
            }
        }
        
        printf("\n");
        
        // TODO: vérifier si on a atteint la destination
        if (destination_reached) {
            break;
        }
    }
}
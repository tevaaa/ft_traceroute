#ifndef TRACEROUTE_H
# define TRACEROUTE_H

# include "args_parsing.h"
# include <sys/time.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>
# include <stdbool.h>
# include <stdio.h>
# include <sys/select.h>


// Main traceroute logic
void traceroute_loop(t_config *config);

// Packet processing
void process_icmp_response(char *packet, int packet_len, struct sockaddr_in *from, t_config *config);
bool is_destination_reached(struct sockaddr_in *from, struct sockaddr_in *dest);
bool is_time_exceeded(struct icmphdr *icmp);

// Time calculation
double calculate_rtt(struct timeval *start, struct timeval *end);
void get_current_time(struct timeval *tv);

// Display functions
void print_hop_header(int ttl);
void print_hop_response(struct sockaddr_in *from, double rtt_ms);
void print_timeout(void);
void print_final_stats(t_config *config);

// Response analysis
int get_icmp_type_from_packet(char *packet, int packet_len);
struct sockaddr_in extract_source_from_packet(char *packet, int packet_len);

#endif /* TRACEROUTE_H */
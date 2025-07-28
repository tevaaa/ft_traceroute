#ifndef SOCKET_H
# define SOCKET_H

# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>
# include <sys/time.h>
# include <unistd.h>
# include <errno.h>
# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>

// Forward declaration
typedef struct s_config t_config;

// Socket creation and configuration
int create_raw_socket(char *source_addr);
void set_socket_timeout(int sockfd, int timeout_ms);

// Packet sending
int send_icmp_packet(int sockfd, struct sockaddr_in *dest, int sequence);

// Packet receiving
int receive_response(int sockfd, struct sockaddr_in *from, int timeout_ms, int expected_sequence, bool *dest_reached);

// ICMP packet construction
void build_icmp_packet(struct icmphdr *icmp, int sequence);
uint16_t calculate_checksum(uint16_t *data, int len);

// Socket utilities
void close_socket(int sockfd);

#endif /* SOCKET_H */
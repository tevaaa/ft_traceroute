#include "../includes/socket.h"
#include "../includes/args_parsing.h"

int create_raw_socket(char *source_addr) {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    // Bonus -s: bind on source address
    if (source_addr) {
        struct sockaddr_in src_addr;
        memset(&src_addr, 0, sizeof(src_addr));
        src_addr.sin_family = AF_INET;
        
        if (inet_pton(AF_INET, source_addr, &src_addr.sin_addr) <= 0) {
            perror("inet_pton source address");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        
        if (bind(sockfd, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {
            perror("bind");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    }
    
    return sockfd;
}
#include "../includes/socket.h"
#include "../includes/args_parsing.h"

uint16_t checksum(void *data, int len) {
    uint32_t sum = 0;
    uint16_t *ptr = data;

    while (len > 1) {
        sum += *ptr++;
        len -= 2;
    }
    
    if (len == 1)
        sum += *(uint8_t *)ptr;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return ~sum;
}

void build_icmp_packet(struct icmphdr *icmp, int sequence) {
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = htons(getpid());
    icmp->un.echo.sequence = htons(sequence);
    icmp->checksum = 0;
    
    icmp->checksum = checksum((void *)icmp, sizeof(struct icmphdr));
}

int send_icmp_packet(int sockfd, struct sockaddr_in *dest, int sequence) {
    struct icmphdr icmp;
    build_icmp_packet(&icmp, sequence);
    
    int bytes_sent = sendto(sockfd, &icmp, sizeof(icmp), 0, 
                           (struct sockaddr *)dest, sizeof(*dest));
    
    if (bytes_sent < 0) {
        printf("sendto error: %s\n", strerror(errno));
        return -1;
    }
    
    return bytes_sent;
}

int create_raw_socket(char *source_addr) {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        printf("socket error: %s\n", strerror(errno));
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    // Bonus -s: bind on source address
    if (source_addr) {
        struct sockaddr_in src_addr;
        memset(&src_addr, 0, sizeof(src_addr));
        src_addr.sin_family = AF_INET;
        
        if (inet_pton(AF_INET, source_addr, &src_addr.sin_addr) <= 0) {
            printf("inet_pton error: %s\n", strerror(errno));
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        
        if (bind(sockfd, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {
            printf("bind error: %s\n", strerror(errno));
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    }
    
    return sockfd;
}


int is_our_packet(struct icmphdr *icmp, int expected_sequence) {
    uint16_t our_pid = getpid();
    if (ntohs(icmp->un.echo.id) == our_pid && 
            ntohs(icmp->un.echo.sequence) == expected_sequence) {
        return 1;
    }
    return 0;
}

typedef enum {
    PACKET_NOT_OURS = -1,        // not our packet
    PACKET_TIME_EXCEEDED = 11,   // Type 11 -  time exceeded (route intermediate)
    PACKET_DEST_REACHED = 0,    // Type 0 - destination reached
} packet_type_t;

packet_type_t validate_packet(char *buffer, int expected_sequence) {

    struct iphdr *ip_hdr = (struct iphdr *)buffer;
    struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + (ip_hdr->ihl * 4));
    
    if (icmp_hdr->type == ICMP_TIME_EXCEEDED) {
        // Le paquet original est encapsulé dans les données
        struct iphdr *orig_ip = (struct iphdr *)(icmp_hdr + 1);
        struct icmphdr *orig_icmp = (struct icmphdr *)((char *)orig_ip + (orig_ip->ihl * 4));
        
        // Vérifier que c'est NOTRE paquet original
        if (is_our_packet(orig_icmp, expected_sequence)) {
            return PACKET_TIME_EXCEEDED; // Routeur intermédiaire
        }
    }
    else if (icmp_hdr->type == ICMP_ECHOREPLY) {
        // Réponse directe de la destination
        if (is_our_packet(icmp_hdr, expected_sequence)) {
            return PACKET_DEST_REACHED; // Destination atteinte !
        }
    }
    return PACKET_NOT_OURS;
}


int receive_response(int sockfd, struct sockaddr_in *from, int timeout_ms, int expected_sequence, bool *dest_reached) {
    char buffer[1024];
    socklen_t fromlen = sizeof(*from);

    fd_set readfds;
    struct timeval timeout;

    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    int ready = select(sockfd + 1, &readfds, NULL, NULL, &timeout);

    if (ready < 0) {
        // Error in select
        printf("select error: %s\n", strerror(errno));
        return -1;
    }

    if (ready == 0) // Timeout reached no fd ready
        return 0;
    
    // 5. ready > 0: data available
    int bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                                 (struct sockaddr *)from, &fromlen);
    
    if (bytes_received < 0) {
        printf("recvfrom error: %s\n", strerror(errno));
        return -1;
    }

    if (bytes_received > 0) {
        packet_type_t packet_type = validate_packet(buffer, expected_sequence);
        
        if (packet_type == PACKET_NOT_OURS) {
            // not our packet, continue waiting for the right one
            return receive_response(sockfd, from, timeout_ms, expected_sequence, dest_reached);
        }
        
        if (packet_type == PACKET_DEST_REACHED) {
            *dest_reached = true; // Destination reached
        }
        
        return bytes_received; // valid packet received
    }
    
    return bytes_received;
}
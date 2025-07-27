#include "../includes/args_parsing.h"

void print_usage(char *program_name) {
    printf("Usage: %s [OPTION...] HOST\n", program_name);
    printf("Print the route packets take to network host.\n\n");
    printf("  -m NUMBER                  maximum number of hops (default: 30)\n");
    printf("  -q NUMBER                  number of queries per hop (default: 3)\n");
    printf("  -p NUMBER                  destination port (default: 33434)\n");
    printf("  -s ADDRESS                 source address to use\n");
    printf("  -N NUMBER                  number of simultaneous queries (default: 16)\n");
    printf("      --help                 display this help and exit\n");
    exit(0);
}

t_config init_config() {
    t_config config = {0};

    config.max_hops = 30;
    config.nqueries = 3;
    config.port = 33434;
    config.source_addr = NULL;
    config.squeries = 16;
    config.timeout_ms = 5000;
    config.sockfd = -1;

    return config;
}



int is_valid_number(char *str) {
    if (!str || *str == '\0') return 0;
    
    if (*str == '-') str++;
    
    while (*str) {
        if (*str < '0' || *str > '9')
            return 0;
        str++;
    }
    return 1;
}

t_config parse_arguments(int argc, char **argv) {
    t_config config = init_config();
    
    int i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
        }
        
        else if (strcmp(argv[i], "-m") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "%s: option requires an argument -- m\n", argv[0]);
                exit(2);
            }
            if (!is_valid_number(argv[i])) {
                fprintf(stderr, "%s: invalid max hops: '%s'\n", argv[0], argv[i]);
                exit(2);
            }
            config.max_hops = atoi(argv[i]);
            if (config.max_hops <= 0 || config.max_hops > 255) {
                fprintf(stderr, "%s: invalid max hops: %d\n", argv[0], config.max_hops);
                exit(2);
            }
        }
        
        else if (strcmp(argv[i], "-q") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "%s: option requires an argument -- q\n", argv[0]);
                exit(2);
            }
            if (!is_valid_number(argv[i])) {
                fprintf(stderr, "%s: invalid number of queries: '%s'\n", argv[0], argv[i]);
                exit(2);
            }
            config.nqueries = atoi(argv[i]);
            if (config.nqueries <= 0 || config.nqueries > 10) {
                fprintf(stderr, "%s: invalid number of queries: %d\n", argv[0], config.nqueries);
                exit(2);
            }
        }
        
        else if (strcmp(argv[i], "-p") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "%s: option requires an argument -- p\n", argv[0]);
                exit(2);
            }
            if (!is_valid_number(argv[i])) {
                fprintf(stderr, "%s: invalid port: '%s'\n", argv[0], argv[i]);
                exit(2);
            }
            config.port = atoi(argv[i]);
            if (config.port <= 0 || config.port > 65535) {
                fprintf(stderr, "%s: invalid port: %d\n", argv[0], config.port);
                exit(2);
            }
        }
        
        else if (strcmp(argv[i], "-s") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "%s: option requires an argument -- s\n", argv[0]);
                exit(2);
            }
            config.source_addr = argv[i];
        }
        
        else if (strcmp(argv[i], "-N") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "%s: option requires an argument -- N\n", argv[0]);
                exit(2);
            }
            if (!is_valid_number(argv[i])) {
                fprintf(stderr, "%s: invalid simultaneous queries: '%s'\n", argv[0], argv[i]);
                exit(2);
            }
            config.squeries = atoi(argv[i]);
            if (config.squeries <= 0 || config.squeries > 64) {
                fprintf(stderr, "%s: invalid simultaneous queries: %d\n", argv[0], config.squeries);
                exit(2);
            }
        }
        
        else if (argv[i][0] != '-') {
            config.target = argv[i];
            i++;
            break;
        }
        
        else {
            fprintf(stderr, "%s: invalid option -- '%s'\n", argv[0], argv[i] + 1);
            fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
            exit(2);
        }
        i++;
    }
    
    if (!config.target) {
        fprintf(stderr, "%s: missing host operand\n", argv[0]);
        fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
        exit(2);
    }
    
    // Vérifier qu'il n'y a pas d'arguments supplémentaires
    if (i < argc) {
        fprintf(stderr, "%s: extra operand '%s'\n", argv[0], argv[i]);
        fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
        exit(2);
    }
    
    return config;
}
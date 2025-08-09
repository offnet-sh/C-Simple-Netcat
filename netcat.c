#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

int sockfd;

void *recv_handler(void *arg) {
    char buffer[BUFFER_SIZE];
    int n;
    while ((n = read(sockfd, buffer, sizeof(buffer)-1)) > 0) {
        buffer[n] = '\0';
        printf("%s", buffer);
        fflush(stdout);
    }
    return NULL;
}

void run_client(const char *ip, int port) {
    struct sockaddr_in serv_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to %s:%d\n", ip, port);

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, recv_handler, NULL);

    char input[BUFFER_SIZE];
    while (fgets(input, sizeof(input), stdin) != NULL) {
        write(sockfd, input, strlen(input));
    }

    close(sockfd);
}

void run_server(int port) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d...\n", port);

    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    sockfd = new_socket;

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, recv_handler, NULL);

    char input[BUFFER_SIZE];
    while (fgets(input, sizeof(input), stdin) != NULL) {
        write(sockfd, input, strlen(input));
    }

    close(new_socket);
    close(server_fd);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage:\n");
        printf("  %s -l <port>      Run as server/listener\n", argv[0]);
        printf("  %s <ip> <port>    Run as client\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-l") == 0) {
        int port = atoi(argv[2]);
        run_server(port);
    } else {
        char *ip = argv[1];
        int port = atoi(argv[2]);
        run_client(ip, port);
    }

    return 0;
}

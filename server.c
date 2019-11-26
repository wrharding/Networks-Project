// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>
#define PORT 8080 

typedef struct{
    int sock;
}connection_t;

void *SockServer(void *arguments){
    connection_t * conn;
    int valread, menu_opt, new_socket;
    char buffer[100000] = {0}; 
    FILE *fp;
    long lSize;

    conn = (connection_t *)arguments;
    while(1){
        valread = recv(conn->sock, &menu_opt, sizeof(menu_opt),0); 
        
        if (valread == 0){
            printf("Client Terminated Connection!\n");
            break;
        }

        menu_opt = ntohl(menu_opt);
        printf("Received Menu Option: %d\n", menu_opt);

        if(menu_opt == 1){
            fp = popen("date", "r");
            printf("Sending client Date & Time...\n"); 
        }
        else if(menu_opt == 2){
            fp = popen("uptime", "r");
            printf("Sending client Server Uptime...\n"); 
        }
        else if(menu_opt == 3){
            fp = popen("free -m", "r");
            printf("Sending client Mem Usage...\n"); 
        }
        else if(menu_opt == 4){
            fp = popen("netstat -p", "r");
            printf("Sending Netstat...\n"); 
        }   
        else if(menu_opt == 5){
            fp = popen("users", "r");
            printf("Sending client Current Users...\n"); 
        }
        else if(menu_opt == 6){
            fp = popen("ps -e", "r");
            printf("Sending client Running Processes...\n"); 
        }
        else if(menu_opt == 7){
            printf("Client Terminated Connection!\n");
            break;
        }

        fseek(fp, 0, SEEK_END);
        lSize = ftell(fp);
        rewind(fp);
        bzero(buffer, 100000);
        fread(buffer,1,lSize,fp);
        printf("%s", buffer); 

        valread = htonl(strlen(buffer));
        send(conn->sock, &valread, sizeof(valread), 0);
        valread = send(conn->sock, buffer, strlen(buffer), 0);
    }
    close(conn->sock);
    free(conn);
    pthread_exit(0);
}

int main(int argc, char const *argv[]){ 
    int opt = 1, server_fd, new_socket, buff_size; 
    struct sockaddr_in address; 
    connection_t * connection;
    pthread_t thread;
    int addrlen = sizeof(address); 
    printf("Starting up server...\n");   
    

    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){ 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){ 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
    
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){ 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 5) < 0){ 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    printf("Socket created...\n");

    while(1){
        printf("Waiting for connection...\n");
        connection = (connection_t *)malloc(sizeof(connection_t));
        connection->sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

    	if (connection->sock <= 0)
			free(connection);
		else{
            printf("Accepted New Connection!\n");
            //printf(" Socket %d\n", new_socket);
            pthread_create(&thread, NULL, SockServer, (void *)connection);
            pthread_detach(thread);
        }
    }

    return 0; 
} 

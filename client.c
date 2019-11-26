// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <ctype.h>
#include <string.h> 
#define PORT 8080 
   
int main(int argc, char const *argv[]) { 
    if(argc == 1){
        printf("Invalid host name! Please enter hostname as an argument.\n");
        return 0;
    }
    else if(argc >= 2){
        printf("Connecting to %s...\n", argv[1]);
    }

    int sock = 0, valread, buff_size, buff_counter; 
    struct sockaddr_in serv_addr; 
    char buffer[100000] = {0}; 
    char *menu = "\n-------------------------------------------\nChoose a command:\n\t1 - Current Date and Time\n\t2 - Current Uptime\n\t3 - Memory Use\n\t4 - Netstat\n\t5 - Current Users\n\t6 - Running Processes\n\t7 - Quit\n-------------------------------------------\nEnter your option: "; 
    char *menu_err = "Incorrect option, please re-enter a valid option!";
    char *exit_msg = "Goodbye!";
    int menu_opt, converted_menu, c = 0;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported\n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 

    while(1){
        printf("%s", menu); 
        if(scanf("%d", &menu_opt) == 0){
            do { c = getchar(); } while ( c != '\n' && c != EOF);
            menu_opt = -1;
        }

        if(menu_opt > 7 || menu_opt < 1)
            printf("%s\n", menu_err);
        else{
            converted_menu = htonl(menu_opt);
            send(sock, &converted_menu, sizeof(converted_menu), 0); 

            if(menu_opt == 7){
                printf("%s\n", exit_msg);
                break;
            }

            bzero(buffer, 100000);
            recv(sock, &buff_size, sizeof(buff_size), 0);
            buff_size = ntohl(buff_size);

            buff_counter = 0;
            while(buff_counter < buff_size){
                valread = recv(sock, buffer, 100000,0);
                printf("%s", buffer); 
                buff_counter += valread;
            }

        }
    }
    return 0; 
} 
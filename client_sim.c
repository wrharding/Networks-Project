#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <time.h>
#include <pthread.h>
#define PORT 8080 
//#define NO_OF_CLIENTS 100

struct arg_struct{
    char const* arg1;
    int arg2;
    float time;
};

void SockClient(void *arguments){
    clock_t clock1;
    struct arg_struct *args = arguments;
    int sock = 0, valread, buff_size, buff_counter; 
    struct sockaddr_in serv_addr; 
    char buffer[100000] = {0}; 
    char *menu = "\n-------------------------------------------\n"
    "Choose a command:\n"
    "\t1 - Current Date and Time\n"
    "\t2 - Current Uptime\n"
    "\t3 - Memory Use\n"
    "\t4 - Netstat\n"
    "\t5 - Current Users\n"
    "\t6 - Running Processes\n"
    "\t7 - Quit\n-------------------------------------------\n"
    "Enter your option: "; 
    char *menu_err = "Incorrect option, please re-enter a valid option!";
    char *exit_msg = "Goodbye!";
    int menu_opt, converted_menu;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, args -> arg1, &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported\n"); 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
    } 

    //test shit DONT FORGET TO REMOVE
    //converted_menu = htonl((int)args -> arg2);
    //send(sock, &converted_menu, sizeof(converted_menu), 0); 
    //test shit done

    converted_menu = htonl(4);
    clock1 = clock();
    send(sock, &converted_menu, sizeof(converted_menu), 0); 

    bzero(buffer, 100000);
    recv(sock, &buff_size, sizeof(buff_size), 0);
    buff_size = ntohl(buff_size);


    buff_counter = 0;
    while(buff_counter < buff_size){
        valread = recv(sock, buffer, 100000,0);
        //printf("%s", buffer); 
        buff_counter += valread;
    }
    
    clock1 = clock() - clock1;
    float executiontime = ((float)clock1/CLOCKS_PER_SEC);
    args->time = executiontime;

    printf("\nThread #%d\t-\t%f\n", args -> arg2, executiontime);
    shutdown(sock, 2);
}

float averagetime(struct arg_struct *argsarray, int size){
    float average = 0;

    printf("averagetime() = ");

    for(int i = 0; i < size; ++i){
        average += argsarray[i].time;
    }

    average = average / (float)size;

    return average;
}
   
int main(int argc, char const *argv[]) {
    int numclients = atoi(argv[2]);
    
    if(argc == 1){
        printf("Invalid host name! Please enter hostname as an argument.\n");
        return 0;
    }
    else if(argc >= 2){
        printf("Connecting to %s...\n", argv[1]);
    }

    struct arg_struct args[numclients];
    pthread_t thread[numclients];

    for(int i = 0; i < numclients; i++){
        args[i].arg1 = argv[1];
        args[i].arg2 = i;
        pthread_create(&thread[i], NULL, SockClient, (void *)&args[i]);
    }


    for(int i = 0; i < numclients; i++){
        pthread_join(thread[i], NULL);
    }

    printf("%f\n", averagetime(args, numclients));

    return 0; 
}

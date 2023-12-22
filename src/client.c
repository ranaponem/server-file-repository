/*The purpose of the client is to either send a file to the server
or request a file from the server if available. It may also request
to see which files are available on the server*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "macros.h"
#include "debug.h"
#include "client_opt.h"
#include "communication.h"

uint8_t clientOption(struct gengetopt_args_info args);
struct sockaddr_in getServerAddr(char* serverIp);

int main(int argc, char *argv[]) {
    //Vars
    struct gengetopt_args_info args;
    struct sockaddr_in serverAddr;
    uint8_t option;
    int clientSocket;

    // INIT
    if (cmdline_parser(argc, argv, &args) != 0)
        ERROR(ERR_ARGS, "Parsing of command line arguments.\n");

    option = clientOption(args);

    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        ERROR(ERR_SOCK, "Creation of socket.\n");

    serverAddr = getServerAddr(args.ip_arg);

    if (connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
        ERROR(1, "connect");
    }
    // END INIT

    //START COMM
    sendOption(clientSocket, option);

    switch (option){
        case 0:
            sendFile(clientSocket, args.file_arg);
            break;
        case 1:
            receiveFile(clientSocket, args.file_arg);
            break;
        default:
            seeServerFiles(clientSocket);
    }
    //END COMM

    // FREE
    close(clientSocket);
    cmdline_parser_free(&args);

    return 0;
}

uint8_t clientOption(struct gengetopt_args_info args){
    uint8_t option;
    if(strcmp(args.option_arg, "send") == 0 && args.file_given)
        option = 0;
    
    else if(strcmp(args.option_arg, "get") == 0 && args.file_given)
        option = 1;

    else if(strcmp(args.option_arg, "see") == 0)
        option = 2;

    else
        ERROR(ERR_ARGS, "Invalid option '%s' (or lack of file given) - must be:\n\tsend,\n\tget,\n\tsee;\n", args.option_arg);

    return option;
}

struct sockaddr_in getServerAddr(char* serverIp){
    struct sockaddr_in serverAddr;

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    switch (inet_pton(AF_INET, serverIp, &serverAddr.sin_addr)) {
        case 0:
            cmdline_parser_print_help();
            ERROR(ERR_ARGS, "Invalid IP address '%s'\n", serverIp);
            break;  //Shuts warnings down, thats it
        case -1:
            cmdline_parser_print_help();
            ERROR(ERR_ARGS, "Unknown or unreachable IP '%s'\n", serverIp);
    }

    return serverAddr;
}
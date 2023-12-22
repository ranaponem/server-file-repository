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
#include "communication.h"

void sendOption(int sockfd, uint8_t option){
    printf("[CLIENT] Sent option %d.\n", option);
    if(send(sockfd, &option, sizeof(uint8_t),0) == -1)
        ERROR(ERR_SEND, "Sending option to server.\n");
}

void sendFile(int sockfd, const char* filepath){
    int readBytes;
    uint8_t buffer[CHUNK_SIZE], signal = 1;
    char* newfilepath = "";
    int file;

    if((file = openFile(&newfilepath, filepath, O_RDONLY)) == -1){
        if(send(sockfd, &signal, 1, 0) == -1)
            ERROR(ERR_SEND, "Sending signal to server.\n");
        ERROR(ERR_OPEN, "Opening file to send.\n");
    }

    printf("[CLIENT] Sending file '%s' to server.\n", newfilepath);

    if(send(sockfd, newfilepath, strlen(newfilepath), 0) == -1)
        ERROR(ERR_SEND, "Sending file name to server.\n");

    if(recv(sockfd, &signal, 1, 0) == -1)
        ERROR(ERR_RECEIVE, "Receiving signal from server.\n");

    while(1){
        readBytes = read(file, buffer, CHUNK_SIZE);
        
        if(send(sockfd, &readBytes, sizeof(int), 0) == -1)
            ERROR(ERR_SEND, "Sending signal to server.\n");
        if(recv(sockfd, &signal, 1, 0) == -1)
            ERROR(ERR_RECEIVE, "Receiving signal from server.\n");
        
        if(readBytes <= 0)
            break;

        if((readBytes = send(sockfd, buffer, readBytes ,0)) == -1)
            ERROR(ERR_SEND, "Sending buffer to server.\n");

        printf("[CLIENT] Sending %d bytes to server.\n", readBytes);
    }

    printf("[CLIENT] Finished sending file.\n");

    free(newfilepath);
    close(file);
}

void receiveFile(int sockfd, const char* filepath){
    int recvBytes;
    uint8_t buffer[CHUNK_SIZE] = {0}, signal = 1;
    char* newfilepath = "";
    int file = openFile(&newfilepath, filepath, O_WRONLY | O_TRUNC | O_CREAT);

    printf("[CLIENT] Requesting file '%s' from server.\n", newfilepath);

    if(send(sockfd, newfilepath, strlen(newfilepath), 0) == -1)
        ERROR(ERR_SEND, "Sending file name to server.\n");

    if(recv(sockfd, &signal, 1, 0) == -1)
        ERROR(ERR_SEND, "Receiving signal from server.\n");

    while(1){
        if(recv(sockfd, &recvBytes, sizeof(int), 0) == -1)
            ERROR(ERR_RECEIVE, "Receiving signal from server.\n");
        if(send(sockfd, &signal, 1, 0) == -1)
            ERROR(ERR_SEND, "Sending signal to server.\n");

        if(recvBytes == 0)
            break;

        if(recvBytes == -1){
            fprintf(stderr, "[CLIENT] Error reading file in server. Deleting file...\n");
            remove(newfilepath);
            break;
        }

        if((recvBytes = recv(sockfd, buffer, CHUNK_SIZE ,0)) == -1)
            ERROR(ERR_RECEIVE, "Receiving buffer from server.\n");

        printf("[CLIENT] Received %d bytes from server.\n", recvBytes);

        if((recvBytes = write(file, buffer, recvBytes)) == -1)
            ERROR(ERR_WRITE, "Writing file from server.\n");

        printf("[CLIENT] Wrote %d bytes.\n", recvBytes);
    }

    printf("[CLIENT] Finished recieving file.\n");

    free(newfilepath);
    close(file);
}

void seeServerFiles(int sockfd){
    int recvBytes;
    char file[FILE_NAME_SIZE];
    int numFiles = 0;
    uint8_t signal = 1;

    printf("Files in server:\n");

    while(1){
        if((recvBytes = recv(sockfd, file, FILE_NAME_SIZE - 1 ,0)) == -1)
            ERROR(ERR_RECEIVE, "Receiving file name from server.\n");

        if(send(sockfd, &signal, 1, 0) == -1)
            ERROR(ERR_SEND, "Sending signal to server.\n");

        if(recvBytes == 0)
            break;

        numFiles++;
        file[recvBytes] = '\0';
        printf("\t- %s ;\n", file);
    }

    if(numFiles == 0)
        printf("No files in server...\n");
    
    else
        printf("%d files found\n", numFiles);

    printf("[CLIENT] Finished receiving file names.\n");
}

int openFile(char** newfilepath, const char* oldfilepath, int flags){
    *newfilepath = malloc(strlen(oldfilepath) + strlen("./files/") + 1);
    strcpy(*newfilepath, "./files/");
    strcat(*newfilepath, oldfilepath);
    (*newfilepath)[strlen(oldfilepath) + strlen("./files/")] = '\0';

    return open(*newfilepath, flags, 0777);
}
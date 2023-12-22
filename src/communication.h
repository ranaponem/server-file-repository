#ifndef COMMUNICATION_H
#define COMMUNICATION_H

void sendFile(int sockfd, const char* filepath);
void receiveFile(int sockfd, const char* filepath);
void seeServerFiles(int sockfd);
void sendOption(int sockfd, uint8_t option);
int openFile(char** newfilepath, const char* oldfilepath, int flags);

#endif
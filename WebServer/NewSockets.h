#ifndef TCP_H
#define TCP_H

int newTCPServerSocket4(const u_short port, const int queue_size);
void closeTCPSocket(const int socketTCPDescriptor);
int waitConnection4(int socket);
int newTCPClientSocket4(const char *ip, const u_short port);
int readTCPLine4(const int socket, char *buffer, const unsigned int maxRead );

#endif
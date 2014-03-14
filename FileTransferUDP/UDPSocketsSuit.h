#ifndef UDPSocketsSuit_H
#define UDPSocketsSuit_H

int newUDPServerSocket4(const u_short port);
void closeUDPSocket4(const int socketUDPDescriptor);
int newUDPClientSocket4();

#endif
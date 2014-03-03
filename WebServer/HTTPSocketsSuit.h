#ifndef HTTPSocketsSuit_H
#define HTTPSocketsSuit_H

char* getReading( const int clientSocket );
char* getProcessing( char* firstLine );
void sendHeader (char* header, const int clientSocket);
void sendErrorHeader(const int clientSocket);

#endif
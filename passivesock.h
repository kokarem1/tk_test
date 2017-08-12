#ifndef PASSIVESOCK_H
#define PASSIVESOCK_H

int passivesock(const char* service, const char* transport, int qlen);

int passiveUDP(const char* service);

int passiveTCP(const char* service, int qlen);

#endif // PASSIVESOCK_H

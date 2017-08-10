#include "passivesock.h"

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "errexit.h"

unsigned short portbase = 0; /* точка отсчёта номера порта */

int passivesock(const char* service, const char* transport, int qlen)
/*
 *  Параметры:
 *      service     - служба, связанная с портом
 *      transport   - имя протокола ("udp"/"tcp")
 *      qlen        - максимальная длина очереди на подключение
 *                    к серверу (только для tcp)
 */
{
    struct servent *pse;        // указатель на запись с инфо о службе
    struct protoent *ppe;       // указатель на запись с инфо о протоколе
    struct sockaddr_in sin;     // IP-адрес
    int s, type;                // дескриптор сокета и тип сокета

    memset(&sin, 0, sizeof(sin));

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    if ( pse = getservbyname(service, transport) )

        sin.sin_port = htons( ntohs( (unsigned short) pse->s_port ) + portbase );

    else if ( ( sin.sin_port = htons( (unsigned short) atoi(service) ) ) == 0 )

        errexit("can't get \"%s\" service entry\n", service);

    if ( ( ppe = getprotobyname(transport) ) == 0 )

        errexit("can't get \"%s\" protocol entry\n", transport);

    if ( strcmp(transport, "udp") == 0 )

        type = SOCK_DGRAM;

    else

        type = SOCK_STREAM;

    //  Сокет
    s = socket(PF_INET, type, ppe->p_proto);

    if (s < 0)

        errexit("can't create socket: %s\n", strerror(errno));

    //  Привязываем сокет
    if ( bind( s, (struct sock_addr *) &sin, sizeof(sin) ) < 0 )

        errexit("can't bind to %s port: %s\n", service, strerror(errno));

    if ( type == SOCK_STREAM && listen(s, qlen) )

        errexit("can't listen to %s port: %s\n", service, strerror(errno));

    return s;
}

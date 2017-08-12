#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>

#include "errexit.h"
#include "passivesock.h"


#define QLEN        32      //  Длина очереди запросов на подключение
#define BUFSIZE     4096    //  Размер буфера
#define SBUFSIZE    12      //  Размера буфера отправки


void reaper(int);
int TCPsubseqcnt(int fd, char b_symb, char e_symb);

int main(int argc, char *argv[])
{
    char *service = "2241";     //  Имя службы или номер порта
    char bsymb = 'S';           //  Стартовый и конечный символы определяющие
    char esymb = 'E';           //  подпоследовательность
    struct sockaddr_in fsin;    //  Адрес клиента
    unsigned int alen;          //  Длина адреса клиента
    int msock;                  //  Главный сокет сервера
    int ssock;                  //  Рабочий сокет сервера

    //  Обрабатываем аргументы командной строки, если они есть
    switch (argc) {

    case 1:
        break;
    case 2:
        service = argv[1];
        break;
    case 4:
        service = argv[1];
        bsymb = *argv[2];
        esymb = *argv[3];
        break;
    default:
        errexit("usage: testTK [port] [begin_symbol end_symbol]\n");

    }

    msock = passiveTCP(service, QLEN);

    printf("service:       %s\nbegin symbol:  '%c'\nend symbol:    '%c'\n",
           service, bsymb, esymb);

    signal(SIGCHLD, reaper);

    while (1) {

        alen = sizeof(fsin);

        ssock = accept(msock, (struct sockaddr *) &fsin, &alen);

        if (ssock < 0) {

            if (errno == EINTR)

                continue;

            errexit( "accept: %s\n", strerror(errno) );

        }

        switch (fork()) {

        case 0:                     //  Дочерний процесс

            close(msock);           //  Закрываем главный сокет в дочернем процессе
            exit(TCPsubseqcnt(ssock, bsymb, esymb));

        default:                    //  Родительский процесс

            close(ssock);           //  Закрываем рабочий сокет в родительском процессе
            break;

        case -1:

            errexit("fork: %s\n", strerror(errno));

        }
    }

    return 0;
}

/*
 * Убирает отработавший процесс из системных таблиц
 */
void reaper(int sig)
{
    while (waitpid(-1, 0, WNOHANG) >= 0);
}


int TCPsubseqcnt(int fd, char b_symb, char e_symb)
/*
 *  Параметры:
 *      fd          - дескриптор
 *      b_symb      - символ, с которого начинается подпоследовательность
 *      e_symb      - символ, которым кончается подпоследовательность
 */
{
    char buf[BUFSIZE];  //  Буфер приёма
    char sbuf[SBUFSIZE]; //  Буфер отправки
    int rc, sc;         //  Количество считанных и записываемых байт
    int symbc;      //  Количество символов в подпоследовательности
    int wphase = 0;     //  Фаза работы обрабатывающего процесса

    while ( rc = read(fd, buf, sizeof buf) ) {

        if (rc < 0)

            errexit("subseq read: %s\n", strerror(errno));

        for (int i = 0; i < rc; ++i)
        {
            switch (wphase) {

            case 0:                           //    Ищем стартовый символ

                if (buf[i] == b_symb) {

                    symbc = 0;
                    wphase = 1;

                }

                break;

            case 1:                           //    Ищем конечный символ. Считаем

                if (buf[i] == e_symb) {

                    sc = sprintf(sbuf, "%d\r\n", symbc);

                    if (write(fd, sbuf, sc) < 0)

                        errexit("subseq write: %s\n", strerror(errno));

                    wphase = 0;

                }

                ++symbc;

                break;

            default:

                errexit("invalid work phase: %s\n", wphase);

            }

        }

    }

    return 0;
}

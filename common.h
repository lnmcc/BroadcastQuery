#ifndef __COMMON_H
#define __COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>

/*客户端查询消息*/
#define QUERY_SERV "QUERY_SERV"
/*服务器应答消息*/
#define IAMSERV "IAMSERV"
#define PORT 9999

#define ERR 1
#define OK 0

#endif

#include "common.h"

int main(int argc, char *argv[]) {
    int ret = -1;
    struct sockaddr_in serv_addr;
    socklen_t serv_len = sizeof(struct sockaddr_in);
    fd_set readfd;
    char buffer[1024] = {0};
	char *netDev = NULL;

    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

	/*设置客户端的网口*/
	if(argc == 2) {
		netDev = argv[1];	
	} else {
		/*默认使用eth0*/
		netDev = "eth0";	
	}

    int sock = -1;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock ==  -1) {
		perror("socket error");
        return ERR; 
    }
    
    struct ifreq ifr;
	memset(&ifr, 0x0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, netDev, strlen(netDev));
	printf("use device: %s\n", ifr.ifr_name);
	/*查询网口的广播地址*/
    if(ioctl(sock, SIOCGIFBRDADDR, &ifr) == -1) {
        perror("ioctl error");
        return ERR;
    }

    int so_broadcast = 1;
    struct sockaddr_in broadcast_addr; 
    memcpy(&broadcast_addr, &ifr.ifr_broadaddr, sizeof(struct sockaddr_in));
    printf("broadcast IP is: %s\n", inet_ntoa(broadcast_addr.sin_addr));
    
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(PORT);
    ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));
	if(ret == -1) {
		perror("setsockopt error");
		return ERR;
	}

	/*广播查询服务器信息，如果没有回应，则重新发送，共重复10次*/
    int times = 10;
	int trys = 0;

    for(trys = 0; trys < times; trys++) {
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        ret = sendto(sock, QUERY_SERV, strlen(QUERY_SERV), 0, (struct sockaddr*)&broadcast_addr, sizeof(struct sockaddr));
        if(ret == -1) {
            continue;
        }
        
        FD_ZERO(&readfd);
        FD_SET(sock, &readfd);

        ret = select(sock + 1, &readfd, NULL, NULL, &timeout);
        switch(ret) {
        case -1:
			perror("select error");
            break;
        case 0:
            fprintf(stderr, "select timeout\n");
            break;
        default:
            if(FD_ISSET(sock, &readfd)) {
                recvfrom(sock, buffer, 1024, 0, (struct sockaddr*)&serv_addr, &serv_len);
                printf("recvmsg is: %s\n", buffer);

                if(strstr(buffer, IAMSERV)) {
                    printf("Server found, IP: %s PORT: %d \n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
                }
                return OK;
            }
            break;
        }
    }

	if(trys == 10) {
		printf("Sorry, cannot find the server\n");
	}
	
    return OK;
}

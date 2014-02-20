#include "common.h"

int main(int argc, char *argv[]) {
	int ret = -1;
	int sock = -1;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	socklen_t cli_len = sizeof(struct sockaddr_in);
	fd_set readfd;
	char buffer[1024] = {0};
	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == -1) {
		perror("sock error");
		return ERR;
	}

	memset(&serv_addr, 0x0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	ret = bind(sock, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr));
	if(ret == -1) {
		perror("bind error");
		return ERR;
	}

	while(1) {
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;

		FD_ZERO(&readfd);
		FD_SET(sock, &readfd);

		ret = select(sock + 1, &readfd, NULL, NULL, /*&timeout*/NULL);
		switch(ret) {
		case -1:
			perror("select error");
			break;
		case 0:
			fprintf(stderr, "select timeout\n");
			break;
		default:
			if(FD_ISSET(sock, &readfd)) {
				recvfrom(sock, buffer, 1024, 0, (struct sockaddr*)&cli_addr, &cli_len);
				if(strstr(buffer, QUERY_SERV)) {
					printf("Client IP: %s  PORT: %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
					sendto(sock, IAMSERV, strlen(IAMSERV), 0, (struct sockaddr*)&cli_addr, cli_len);
				}
			}
			break;
		}
	}
	return OK;
}

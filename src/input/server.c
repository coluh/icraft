#include "server.h"
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <math.h>
#include "../util/log.h"

#define PORT 9496
#define BUFFER_SIZE 1024

static int sockfd;
static char buffer[BUFFER_SIZE];

void server_init() {
	struct sockaddr_in server_addr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	Assert(sockfd >= 0, "fail to create socket");
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		close(sockfd);
		loge("fail to bind socket");
	}

	// set non-block
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

// return wsadjkhl value, all in [0, 1]
const float *server_receive() {
	static float wsadjkhl[8];
	memset(wsadjkhl, 0, sizeof(wsadjkhl));

	int bytes_read = recv(sockfd, buffer, BUFFER_SIZE, 0);
	if (bytes_read > 0) {
		buffer[bytes_read] = '\0';

		float x1, y1, x2, y2;
		int ret = sscanf(buffer, "x1:%f, y1:%f, x2:%f, y2:%f", &x1, &y1, &x2, &y2);
		if (ret == 4) {
			// logd("x1: %6.2f\ty1: %6.2f\tx2: %6.2f\ty2: %6.2f", x1, y1, x2, y2);
			wsadjkhl[0] = y1 > 0 ? fabsf(y1) : 0;
			wsadjkhl[1] = y1 < 0 ? fabsf(y1) : 0;
			wsadjkhl[2] = x1 < 0 ? fabsf(x1) : 0;
			wsadjkhl[3] = x1 > 0 ? fabsf(x1) : 0;
			wsadjkhl[4] = y2 < 0 ? fabsf(y2) : 0;
			wsadjkhl[5] = y2 > 0 ? fabsf(y2) : 0;
			wsadjkhl[6] = x2 < 0 ? fabsf(x2) : 0;
			wsadjkhl[7] = x2 > 0 ? fabsf(x2) : 0;
		}
		// logd("Receive: %d \"%s\"", ret, buffer);
	}

	return wsadjkhl;
}


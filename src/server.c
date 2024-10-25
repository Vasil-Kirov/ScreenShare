#include "server.h"
#include "log.h"

Server create_server()
{
	Server res = {};
	int fn_result = 0;

#if _WIN32
	WSADATA wsa;
	fn_result = WSAStartup(MAKEWORD(2, 2), &wsa);
	if(fn_result != 0)
	{
		lfatal("WSAStartup failed: %d", fn_result);
	}
#endif

#if 0
	struct addrinfo hints = {};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *info = NULL;
	fn_result = getaddrinfo(MULTIGROUP_ADDRESS, "27723", &hints, &info);
	if(fn_result != 0)
	{
		lfatal("getaddrinfo failed: %d", fn_result);
	}
	freeaddrinfo(info);
#endif



	res.s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(res.s == INVALID_SOCKET)
	{
		lfatal("Failed to create socket: %d", WSAGetLastError());
	}
	else
	{
		linfo("Created server socket");
	}
	

	struct sockaddr_in localif = {};
	localif.sin_family = AF_INET;
	localif.sin_port = htons(27723);
	localif.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	fn_result = bind(res.s, (struct sockaddr *)&localif, sizeof(localif));
	if(fn_result == SOCKET_ERROR)
	{
		lfatal("Failed to bind server socket: %d", WSAGetLastError());
	}

	return res;
}

void server_run_update(Server s)
{
	char buf[4096] = {};
	while(recv(s.s, buf, 4096, 0) != SOCKET_ERROR)
	{
		memset(buf, 0, 4096);
	}
}


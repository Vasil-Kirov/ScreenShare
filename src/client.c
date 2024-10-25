#include "client.h"
#include "log.h"
#include <vbytestream.h>

void send_connect(Client c)
{
	char buf[1024] = {};
	int at = 0;
	buf[at++] = Package_Connect;

	sendto(c.s, buf, at, 0, (struct sockaddr *)&c.server_addr, sizeof(c.server_addr));
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
Client create_client()
{
	Client res = {};

	int fn_result = 0;
#if _WIN32
	WSADATA wsa;
	fn_result = WSAStartup(MAKEWORD(2, 2), &wsa);
	if(fn_result != 0)
	{
		lfatal("WSAStartup failed: %d", fn_result);
	}
#endif


	res.s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	res.server_addr.sin_family = AF_INET;
	res.server_addr.sin_port = htons(27723);
	res.server_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	send_connect(res);

	return res;
}
#pragma clang diagnostic pop


void client_run_update(Client c, char *buf)
{
	int from_len = sizeof(c.server_addr);
	while(!c.is_connected)
	{
		while(recvfrom(c.s, buf, MAX_BUF_SIZE, 0, (struct sockaddr *)&c.server_addr, &from_len) == SOCKET_ERROR)
		{
			ldebug("Failed recvfrom: %d", WSAGetLastError());
			send_connect(c);
		}
		if(buf[0] == Package_Connect)
		{
			c.is_connected = buf[1];
		}
		else
		{
			lerror("Missed connect package");
		}
	}

	while(true)
	{
		int bytes = recvfrom(c.s, buf, MAX_BUF_SIZE, 0, (struct sockaddr *)&c.server_addr, &from_len);
		if(bytes == SOCKET_ERROR)
			break;
		
		BinaryReader reader = make_reader((u8 *)buf, bytes);
		BinaryReader *r = &reader;
		switch(read_uint8(r))
		{
			case Package_Frame:
			{
				i32 width  = read_int32(r);
				i32 height = read_int32(r);
				lerror("Got frame [%d x %d]", width, height);
			} break;
			default:
			{
				lerror("Unknown packet header: %d", buf[0]);
			} break;
		}
	}
}



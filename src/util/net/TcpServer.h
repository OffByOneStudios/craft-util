#pragma once
#include "util/common.h"

#ifdef win_x64_vc140
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#endif


namespace craft {

	class TcpServer
	{
		// Platform Specific
#ifdef win_x64_vc140
		std::once_flag wsapi;
#endif
		std::thread _worker;
		std::string _node;
		int _service;
		int _maxcon;
		//addrinfo _info;
		std::function<void(int descriptor)> _handler;
	public:
		std::atomic<bool> running;


		CRAFT_UTIL_EXPORTED TcpServer(std::string node, int service, int _maxcon, std::function<void(int descriptor)> handler);
		CRAFT_UTIL_EXPORTED ~TcpServer();

	public:
		CRAFT_UTIL_EXPORTED void start();
		CRAFT_UTIL_EXPORTED void stop();
		CRAFT_UTIL_EXPORTED void serve_forever();
	};
}

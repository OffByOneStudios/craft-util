#pragma once

#include "util/common.h"

#include "http.h"
#include "TcpServer.h"

namespace craft {
namespace net
{
	class HttpServer
	{
		TcpServer* _serve;
		int _port;
		std::shared_ptr<spdlog::logger> _logger;
	public:
		CRAFT_UTIL_EXPORTED HttpServer(std::shared_ptr<spdlog::logger> logger, int port);

		CRAFT_UTIL_EXPORTED void init();
		CRAFT_UTIL_EXPORTED void serve_forever();
	};
}}
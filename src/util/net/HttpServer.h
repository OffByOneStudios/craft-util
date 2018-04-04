#pragma once

#include "util/common.h"

#include "http.h"
#include "TcpServer.h"

namespace craft {
namespace net
{
	/*
	*/
	class HTTPRequestHandler
	{
		/*
			Return True to stop propagation
		*/
	public:
		virtual bool handle(HTTPRequest& req, HttpResponse& rep) = 0;
	};

	class HttpServer
	{
		TcpServer* _serve;
		int _port;
		std::shared_ptr<spdlog::logger> _logger;

		
	public:
		inline virtual ~HttpServer()
		{
			for (HTTPRequestHandler* h : handlers)
			{
				delete h;
			}
		}

		std::vector<HTTPRequestHandler*> handlers;

		CRAFT_UTIL_EXPORTED HttpServer(std::shared_ptr<spdlog::logger> logger, int port);

		CRAFT_UTIL_EXPORTED void init();
		CRAFT_UTIL_EXPORTED void serve_forever();
	};
}}
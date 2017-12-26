#include "util/common.h"

#include "HttpServer.h"

#include "util/net/TcpServer.h"

using namespace craft;
using namespace craft::net;

#ifdef win_x64_vc140
#define STAHP_CLIENT SD_BOTH
#define SOCKET_FAIL INVALID_SOCKET
#define ACCEPT_FAIL INVALID_SOCKET
#define LISTEN_ERROR SOCKET_ERROR
#define BIND_ERROR SOCKET_ERROR
#define SEND_ERROR SOCKET_ERROR
#define CLOSE_FUNCTION(s) ((closesocket(s)))
#else
#define STAHP_CLIENT SHUT_RDWR
#define SOCKET_FAIL -1
#define ACCEPT_FAIL -1
#define BIND_ERROR -1
#define LISTEN_ERROR -1
#define SEND_ERROR -1
#define CLOSE_FUNCTION(s) ((close(s)))
#endif

HttpServer::HttpServer(std::shared_ptr<spdlog::logger> logger, int port)
	: _port(port)
	, _logger(logger)
{

}

void HttpServer::init()
{
	_serve = new craft::net::TcpServer("", _port, 100, [this](int socket) {
		std::string buf(8192, '\0');
		int _read = recv(socket, (char*)buf.data(), 8192, 0);

		if (_read < 0)
		{
			_logger->error("Error reading from Socket.");
			return;
		}
		else if (_read == 0)
		{
			_logger->error("Unexpected Client Disconnect");
			return;
		}
		craft::net::HttpResponse resp;
		craft::net::HTTPRequest req;
		try
		{
			req = craft::net::parse_request(buf.data(), _read);
			resp.code = 404;
			resp.content_type = "text/plain";
			for(auto hand : handlers)
			{
				if(hand->handle(req, resp)) break;
			}
		}
		catch (stdext::exception e)
		{
			resp.code = 404;
			resp.content_type = "text/plain";
			resp.data << ":(";
		}

		std::string res = resp.format();
		auto sres = send(socket, res.data(), res.size(), 0);
		if (sres == SEND_ERROR)
		{
			//auto err = WSAGetLastError();
			//auto s = GetLastErrorStdStr(WSAGetLastError());
			_logger->error("Unable to Send:");
		}

	});
	_serve->start();
}

void HttpServer::serve_forever()
{
	_serve->serve_forever();
}

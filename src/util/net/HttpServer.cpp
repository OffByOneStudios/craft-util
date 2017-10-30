#include "util/common.h"

#include "HttpServer.h"

#include "util/net/TcpServer.h"

using namespace craft;
using namespace craft::net;


HttpServer::HttpServer(std::shared_ptr<spdlog::logger> logger, int port)
	: _logger(logger)
	, _port(port)
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
		std::string resp;
		try
		{
			craft::net::HTTPRequest req = craft::net::parse_request(buf.data(), _read);
			resp = fmt::format("{0}\r\n{1}\r\n\r\n{2}",
				"HTTP / 1.0 200 OK",
				"Content-Type: text/plain",
				"OK!");
		}
		catch (stdext::exception e)
		{
			resp = fmt::format("{0}\r\n{1}\r\n\r\n{2}",
				"HTTP / 1.0 400 Bad Request",
				"Content-Type: text/plain",
				"Ya Goofed!");
		}

		auto sres = send(socket, resp.data(), resp.size(), 0);
		if (sres == SOCKET_ERROR)
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
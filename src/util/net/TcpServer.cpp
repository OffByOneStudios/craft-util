#include "util/common.h"

#include "TcpServer.h"
#include "util/exception.h"

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


using namespace craft;


#ifdef win_x64_vc140
std::string GetLastErrorStdStr(DWORD error)
{
	if (error)
	{
		LPVOID lpMsgBuf;
		DWORD bufLen = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);
		if (bufLen)
		{
			LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
			std::string result(lpMsgStr, lpMsgStr + bufLen);

			LocalFree(lpMsgBuf);

			return result;
		}
	}
	return std::string();
}
#endif // win_x64_vc140

TcpServer::~TcpServer()
{
	stop();
}

TcpServer::TcpServer(std::string node, int service, int maxconn, std::function<void(int descriptor)> handler)
	: _node(node)
	, _service(service)
	, _maxcon(maxconn)
	, _handler(handler)
{
}

void TcpServer::start()
{
	std::call_once(wsapi, []() {
		WSADATA wsaData;
		auto iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult)
		{
			throw stdext::exception("Fatal: Failed to Initialize Winsock");
		}
	});
	addrinfo *result, *ptr, hints; result = nullptr; ptr = nullptr;
	int listenfd = 0;
	hints = {};

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	char port[5];
	itoa(_service, port, 10);
	if (getaddrinfo((_node.size()) ? _node.c_str() : nullptr, port, &hints, &result) != 0)
	{
		throw stdext::exception("Could not resolve host");
	}

	// socket and bind
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		listenfd = socket(ptr->ai_family, ptr->ai_socktype, 0);
		if (listenfd == ACCEPT_FAIL) continue;
		if (bind(listenfd, ptr->ai_addr, ptr->ai_addrlen) == BIND_ERROR)
		{
			throw stdext::exception("Unable to bind to {0}:{1}", _node, _service);
		}
		else
		{
			break;
		}
	}
	if (ptr == nullptr)
	{
		throw stdext::exception("Unable to bind to {0}:{1}", _node, _service);
	}

	freeaddrinfo(result);

	if (listen(listenfd, _maxcon) == LISTEN_ERROR)
	{
		throw stdext::exception("Failed To Listen to That socket yo");
	}

	_worker = std::thread([this, listenfd]() {
		std::vector<std::future<void>> queue;

		while (this->running) {
			sockaddr clientaddr = {};
			socklen_t addrlen = 0;

			auto client = accept(listenfd, &clientaddr, nullptr);
			if (client == ACCEPT_FAIL)
			{
				auto s = GetLastErrorStdStr(WSAGetLastError());
				throw stdext::exception("This Socket is Borked Yo! I'm Closing it, {0}", s);
			}
			queue.push_back(std::async([=]() {
				_handler(client);
				shutdown(client, STAHP_CLIENT);
				CLOSE_FUNCTION(client);
			}));

			// Service outstanding finished connections
			std::chrono::milliseconds span(1);
			std::vector<size_t> to_remove;
			for (auto it = queue.begin(); it != queue.end(); ++it)
			{
				if (it->wait_for(span) != std::future_status::timeout)
				{
					try
					{
						to_remove.push_back(it - queue.begin());
					}
					catch (std::exception const& ex)
					{
						throw stdext::exception(ex, "Unable to Service Request...");
					}
					catch (...)
					{
						throw stdext::exception("Unable to Service Request...");
					}
				}
			}
			for (auto it = to_remove.rbegin(); it != to_remove.rend(); ++it)
			{
				queue.erase(queue.begin() + *it);
			}
		}
		close(listenfd);
	});
	
	running = true;
}

void TcpServer::stop()
{
	running = false;
	_worker.join();
}

void TcpServer::serve_forever()
{
	_worker.join();
}

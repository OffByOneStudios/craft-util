#include "util/common.h"

#include "TcpServer.h"
#include "util/exception.h"

using namespace craft;

TcpServer::~TcpServer()
{
  stop();
}

TcpServer::TcpServer(std::string node, int service, int maxconn, std::function<void(int descriptor)> handler)
  :_node(node)
  , _service(service)
  , _maxcon(maxconn)
  , _handler(handler)
{
}

void TcpServer::start()
{
  addrinfo *result, *ptr, hints; result = nullptr; ptr = nullptr;
  int listenfd = 0;
  hints = {};
  
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;
  
  // socket and bind
  for (ptr = result; ptr !=NULL; ptr = ptr->ai_next)
  {
    listenfd = socket (ptr->ai_family, ptr->ai_socktype, 0);
    if (listenfd == -1) continue;
    if (bind(listenfd, ptr->ai_addr, ptr->ai_addrlen) == 0) break;
  }
  if (ptr == nullptr)
  {
    throw stdext::exception("Unable to bind to {0}:{1}", _node, _service);
  }

  freeaddrinfo(result);
  
  _worker = std::thread([this, listenfd](){
    std::vector<std::future<void>> queue;
    
    while (this->running) {
      sockaddr_in clientaddr;
      socklen_t addrlen;
      // this needs to be non blocking
      auto client = accept(listenfd, (struct sockaddr *) &clientaddr, &addrlen);
      queue.push_back(std::async([=](){
        _handler(client);
        shutdown (client, SHUT_RDWR);
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
    
  });
  listen(listenfd, _maxcon);
  running = true;
}

void TcpServer::stop()
{
  running = false;
  _worker.join();
}

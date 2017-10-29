#pragma once
#include "util/common.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

namespace craft {

  class TcpServer
  {
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
  };
}

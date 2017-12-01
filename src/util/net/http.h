#pragma once

#include "util/common.h"

#include "util/exception.h"

namespace craft {
namespace net {
  enum class HTTPMethod 
  {
    GET,
    POST,
    PUT,
    DELET
  };

  struct HTTPRequest 
  {
    std::map<std::string, std::string> headers;
    std::string user_agent;
	std::string path;
	HTTPMethod type;
    void* body;
    size_t body_size;
	bool follow_redirects;
  };
  

  CRAFT_UTIL_EXPORTED HTTPRequest parse_request(const char* data, size_t length);

}}

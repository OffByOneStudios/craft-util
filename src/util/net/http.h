#pragma once

#include "util/common.h"

namespace craft {
namespace net {
  enum class HTTPType {
    GET,
    POST,
    PUT,
    DELET
  };

  struct HTTPRequest {

    std::map<std::string, std::string> headers;
    std::string user_agent;
    HTTPType type;
    void* body;
    size_t body_size;
  };
  
  struct SerializedHttpRequest {
    void* data;
    size_t length;
  }
    
    inline SerializedHttpRequest(void* d, size_t l)
    {
      data = d;
      length = l;
    }
    
    inline SerializedHttpRequest(const& SerializedHttpRequest)
    {
      
    }
    
    inline ~SerializedHttpRequest()
    {
      delete[] data;
    }
  }
  
  HTTPRequest parse_request(char* data, size_t length);
  SerializedHttpRequest serialize_request()
}}

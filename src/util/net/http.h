#pragma once

#include "util/common.h"

#include "util/exception.h"

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
	std::string path;
    HTTPType type;
    void* body;
    size_t body_size;
  };
  
  struct SerializedHttpRequest {
    uint8_t* data;
    size_t length;

	CRAFT_UTIL_EXPORTED SerializedHttpRequest()
		: data(nullptr)
		, length(0)
	{
		
	}

	CRAFT_UTIL_EXPORTED inline SerializedHttpRequest(uint8_t d[], size_t l)
	{
		data = d;
		length = l;
	}

	CRAFT_UTIL_EXPORTED inline SerializedHttpRequest(SerializedHttpRequest const& other)
		: SerializedHttpRequest()
	{
		length = other.length;
		data = new uint8_t[length];
		std::copy(other.data, other.data + other.length, data);
	}

	CRAFT_UTIL_EXPORTED inline SerializedHttpRequest(SerializedHttpRequest&& other)
		: SerializedHttpRequest()
	{
		std::swap(data, other.data);
		std::swap(length, other.length);
	}

	CRAFT_UTIL_EXPORTED inline ~SerializedHttpRequest()
	{
		if(data != nullptr) delete[] data;
	}
  };
  
  CRAFT_UTIL_EXPORTED HTTPRequest parse_request(const char* data, size_t length);
  CRAFT_UTIL_EXPORTED SerializedHttpRequest serialize_request(HTTPRequest& req);
}}

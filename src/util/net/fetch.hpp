#pragma once
#include "util/common.h"

namespace craft {
namespace net {
	typedef size_t(*CURL_WRITEFUNCTION_PTR)(void*, size_t, size_t, void*);
	namespace _impl
	{
		CRAFT_UTIL_EXPORTED bool curl_init;
		struct CurlBuffer
		{
			char *memory;
			size_t size;
		};

		size_t curl_write(void *contents, size_t size, size_t nmemb, void *userp)
		{
			size_t realsize = size * nmemb;
			_impl::CurlBuffer *mem = (_impl::CurlBuffer *)userp;

			mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
			if (mem->memory == NULL) {
				/* out of memory! */
				return size_t(0);
			}

			memcpy(&(mem->memory[mem->size]), contents, realsize);
			mem->size += realsize;
			mem->memory[mem->size] = 0;

			return realsize;
		}
	}

	enum class HTTPType {
		GET,
		POST,
		PUT,
		DELET
	};

	struct FetchOptions {
		
		std::map<std::string, std::string> headers;
		std::string user_agent;
		HTTPType type;
		
	};

	template<typename T>
	inline std::future<T> craft::net::fetch(std::string url, FetchOptions opts, std::function<T(void*, size_t)> serialize)
	{
		if (!_impl::curl_init)
		{
			curl_global_init(CURL_GLOBAL_DEFAULT);
			_impl::curl_init = true;
		}
		return std::async(std::launch::async, [=]() 
		{
			std::string errstr(CURL_ERROR_SIZE, 0);

			CURL *curl = curl_easy_init();
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

			_impl::CurlBuffer chunk = {};

			curl_easy_setopt(curl, CURLOPT_USERAGENT, ((opts.user_agent.size()) ? opts.user_agent.c_str()) : "libcurl-agent/1.0"));
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errstr.data());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &_impl::curl_write);

			auto err = curl_easy_perform(curl);
			if (err)
			{
				throw stdext::exception("Exception in Curl: {0}", errstr);
			}
			T res = serialize(chunk.memory, chunk.size);
			free(chunk.memory);

			return res;
		});

	}
}}
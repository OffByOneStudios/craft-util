#pragma once
#include "util/common.h"

#include "util/threading/threading.h"
#include "util/exception.h"
#include "http.h"

namespace craft {
namespace net {
	typedef size_t(*CURL_WRITEFUNCTION_PTR)(void*, size_t, size_t, void*);

	namespace _impl
	{
		CRAFT_UTIL_EXPORTED extern bool curl_init;
		struct CurlBuffer
		{
			char *memory;
			size_t size;
			size_t offset;
		};

		inline size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
		{
			CurlBuffer* buf = (CurlBuffer*)userp;

			/* in real-world cases, this would probably get this data differently
			as this fread() stuff is exactly what the library already would do
			by default internally */
			size_t remaining = buf->size - buf->offset;
			size_t write = (size * nmemb > remaining) ? remaining : size * nmemb;
			memcpy(ptr, buf->memory + buf->offset, write);
			buf->offset += write;

			return write;
		}

		inline size_t curl_write(void *contents, size_t size, size_t nmemb, void *userp)
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

		inline std::string http_type(HTTPMethod fs)
		{
			switch (fs)
			{
			case HTTPMethod::GET:
				return "GET";
			case HTTPMethod::POST:
				return "POST";
			case HTTPMethod::PUT:
				return "PUT";
			case HTTPMethod::DELET:
				return "DELETE";
			default:
				return "GET";
			}
		}
	}



	template<typename ResultType>
	inline stdext::future<ResultType> fetch(std::string url, HTTPRequest opts, std::function<ResultType(void*, size_t)> deserialize)
	{
		if (!_impl::curl_init)
		{
			curl_global_init(CURL_GLOBAL_DEFAULT);
			_impl::curl_init = true;
		}
		return std::async(std::launch::async, [=]()
		{
			std::string errstr(CURL_ERROR_SIZE, 0);
			curl_slist *list = nullptr;
			CURL *curl = curl_easy_init();
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

			_impl::CurlBuffer chunk = {};

			if (opts.headers.size())
			{
				for (auto it : opts.headers)
				{
					list = curl_slist_append((list) ? list : nullptr, fmt::format("{0}: {1}", it.first, it.second).c_str());
				}
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

			}

			if (opts.body_size)
			{
				_impl::CurlBuffer readbuf = {};
				readbuf.memory = (char*)opts.body;
				readbuf.size = opts.body_size;
				curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(curl, CURLOPT_READFUNCTION, &_impl::read_callback);
				curl_easy_setopt(curl, CURLOPT_READDATA, &readbuf);
				curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, readbuf.size);
				curl_easy_setopt(curl, CURLOPT_POST, 1L);
			}
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, _impl::http_type(opts.type).c_str());
			curl_easy_setopt(curl, CURLOPT_USERAGENT, ((opts.user_agent.size()) ? opts.user_agent.c_str() : "libcurl-agent/1.0"));
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errstr.data());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &_impl::curl_write);

			auto err = curl_easy_perform(curl);
			if (err)
			{
				throw stdext::exception("Exception in Curl: {0}", errstr);
			}
			ResultType res = deserialize(chunk.memory, chunk.size);
			free(chunk.memory);
			if (list)
			{
				curl_slist_free_all(list);
			}
			return res;
		});

	}
}}

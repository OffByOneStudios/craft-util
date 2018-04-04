#include "util/common.h"
#include "http.h"

using namespace craft;
using namespace craft::net;

//
// Request
//

HTTPMethod _method_for(const char* str, size_t length)
{
	if (strncmp("GET", str, length) == 0)
	{
		return HTTPMethod::GET;
	}
	else if (strncmp("POST", str, length) == 0)
	{
		return HTTPMethod::POST;
	}
	else if (strncmp("PUT", str, length) == 0)
	{
		return HTTPMethod::PUT;
	}
	else if (strncmp("DELETE", str, length) == 0)
	{
		return HTTPMethod::DELET;
	}
	else
	{
		return HTTPMethod::GET;
	}
}

HTTPRequest craft::net::parse_request(const char* data, size_t length)
{
	HTTPRequest res;
	int body_pos;
	const char *method, *path;
	size_t prevbuflen = 0, method_len, path_len, num_headers;
	int minor_version;
	phr_header* headers_buffer = new phr_header[128];
	num_headers = 128;

	body_pos = phr_parse_request(data, length,
		&method, &method_len, &path, &path_len, &minor_version, (phr_header*)headers_buffer,
		&num_headers, prevbuflen);

	if (body_pos == -1)
	{
		delete[] headers_buffer;
		throw stdext::exception("Invalid HTTP Request.");
	}

	res.type = _method_for(method, method_len);
	res.path = std::string(path, path_len);
	for (auto i = 0; i < num_headers; i++)
	{
		phr_header p = headers_buffer[i];
		res.headers[std::string(p.name, p.name_len)] = std::string(p.value, p.value_len);
	}

	auto l = res.headers.find("Content-Length");
	if (l != res.headers.end())
	{
		res.body_size = atoi(l->second.c_str());
	}
	res.body = (void*)(data + body_pos);

	delete[] headers_buffer;

	return res;
}


namespace _impl 
{
	std::string http_code(uint16_t code)
	{
		switch(code)
		{
			// Informational responses
			case 100: return "Continue";
			case 101: return "Switching Protocols";
			case 102: return "Processing";
			case 103: return "Early Hints";

			// Success
			case 200: return "OK";
			case 201: return "Created";
			case 202: return "Accepted";
			case 203: return "Non-Authoritative Information";
			case 204: return "No Content";
			case 205: return "Reset Content";
			case 206: return "Partial Content";
			case 207: return "Multi-Status";
			case 208: return "Already Reported";
			case 226: return "IM Used";

			//Redirection
			case 300: return "Multiple Choices";
			case 301: return "Moved Permanently";
			case 302: return "Found";
			case 303: return "See Other";
			case 304: return "Not Modified";
			case 305: return "Use Proxy";
			case 306: return "Switch Proxy";
			case 307: return "Temporary Redirect";
			case 308: return "Permanent Redirect";

			//Client errors
			case 400: return "Bad Request";
			case 401: return "Unauthorized";
			case 402: return "Payment Required";
			case 403: return "Forbidden";
			case 404: return "Not Found";
			case 405: return "Method Not Allowed";
			case 406: return "Not Acceptable";
			case 407: return "Proxy Authentication Required";
			case 408: return "Request Timeout";
			case 409: return "Conflict";
			case 410: return "Gone";
			case 411: return "Length Required";
			case 412: return "Precondition Failed";
			case 413: return "Payload Too Large";
			case 414: return "URI Too Long";
			case 415: return "Unsupported Media Type";
			case 416: return "Range Not Satisfiable";
			case 417: return "Expectation Failed";
			case 418: return "I'm a teapot";
			case 421: return "Misdirected Request";
			case 422: return "Unprocessable Entity";
			case 423: return "Locked";
			case 424: return "Failed Dependency";
			case 426: return "Upgrade Required";
			case 428: return "Precondition Required";
			case 429: return "Too Many Requests";
			case 431: return "Request Header Fields Too Large";
			case 451: return "Unavailable For Legal Reasons";

			// Server errors
			case 500: return "Internal Server Error";
			case 501: return "Not Implemented";
			case 502: return "Bad Gateway";
			case 503: return "Service Unavailable";
			case 504: return "Gateway Timeout";
			case 505: return "HTTP Version Not Supported";
			case 506: return "Variant Also Negotiates";
			case 507: return "Insufficient Storage";
			case 508: return "Loop Detected";
			case 510: return "Not Extended";
			case 511: return "Network Authentication Required";

		}
		return "Unknown Status Code";
	}
}

std::string HttpResponse::format()
{
	return fmt::format("{0}\r\n{1}\r\n\r\n{2}",
				fmt::format("HTTP / 1.1 {0} {1}", code, _impl::http_code(code)),
				fmt::format("Content-Type: {0}", content_type),
				data.str());
}

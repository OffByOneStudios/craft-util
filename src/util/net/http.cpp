#include "util/common.h"
#include "http.h"

using namespace craft;
using namespace craft::net;


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

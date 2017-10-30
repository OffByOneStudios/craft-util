//#include "common.h"
//#undef CRAFT_UTIL_DLL
#include "util/common.h"
#include "util/all.h"


#include <spdlog/spdlog.h>
#include <bandit/bandit.h>

#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif


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


using namespace bandit;

std::shared_ptr<spdlog::logger> console;


std::string project_root()
{
	std::string here = path::absolute("./");
	while (!path::exists(path::join(here, "CraftUtil.lock")))
	{
		here = path::normalize(path::join(here, ".."));
		if (here == "/")
		{
			throw stdext::exception("Running Tests outside of the repo director structure");
		}
	}
	return here;
}

 //Tell bandit there are tests here.
go_bandit([](){
  describe("util_dll", [](){
      describe("algorithms_hpp", [](){
          it("topological_sort", [](){
              throw std::exception();
          });
          it("trim", [](){
              return stdext::trim(" FF ") == "FF";
          });
          it("ltrim", [](){
              return stdext::trim(" FF") == "FF";
          });
          it("rtrim", [](){
              return stdext::trim("FF ") == "FF";
          });
          it("split", [](){
//              auto res = stdext::split("foo.bar.baz", ".");
//              for(auto i: res)
//              {
//
//              }

              throw std::exception();
          });
          it("join", [](){
              throw std::exception();
          });
          it("longest_common_prefix", [](){
              throw std::exception();
          });
      });
	  describe("fetch_hpp", []() {
		  it("fetch", []() {
			  std::function<std::string(void*, size_t)> f = [](void* d, size_t s) {
				  return std::string((char*)d, s);
			  };

			  std::string data = "{\n  \"foo\": \"bar\"\n}";
			  craft::net::HTTPRequest opts;
			  opts.type = craft::net::HTTPType::PUT;
			  opts.user_agent = "craftengine/test";
			  opts.headers =
			  {
				  { "foo", "Bar Baz" },
				  { "Content-Type", "application/json"}
			  };
			  opts.body = (void*)data.data();
			  opts.body_size = data.size();

			
			  stdext::future<std::string> fut = craft::net::fetch(
				  std::string("http://localhost:8080/jokes/random"),
				  opts,
				  f
			  );

			  std::string s = fut.get();

 			  if (s.size())
			  {
				  printf("%s\n", s.c_str());
			  }
		  });
	  });
	  describe("fs_hpp", []() {
		  it("read", []() {
			  std::function<std::string(uint8_t[], size_t)> f = [](uint8_t d[], size_t s) {
				  auto res = std::string((char*)d, s);
				  delete[] d;

				  return res;
			  };

			  std::string here = project_root();

			  stdext::future<std::string> fut = craft::fs::read(path::join(here, "test/main.cpp"), f);

			  std::string s = fut.get();

			  if (s.size())
			  {
				  printf("%s\n", s.c_str());
			  }
		  });
	  });
  });
});

int main(int argc, char const *argv[]) {
	console = spdlog::stdout_color_mt("console");

  auto serve = new craft::net::TcpServer("", 6112, 100, [](int socket) {
		std::string buf(2048, '\0');
		int _read = recv(socket, (char*)buf.data(), 2048, 0);

		if (_read < 0)
		{
			console->error("Error reading from Socket.");
			return;
		}
		else if (_read == 0)
		{
			console->error("Unexpected Client Disconnect");
			return;
		}
		console->info("\n{0}\n", buf);
		std::string resp = fmt::format("{0}\r\n{1}\r\n\r\n{2}",
			"HTTP / 1.0 200 OK",
			"Content-Type: text/plain",
			"Ya Goofed!");

		auto sres = send(socket, resp.data(), resp.size(), 0);
		if (sres == SOCKET_ERROR)
		{
			//auto err = WSAGetLastError();
			//auto s = GetLastErrorStdStr(WSAGetLastError());
			console->error("Unable to Send:");
		}

	});
	serve->start();

	serve->serve_forever();
  //return bandit::run(argc, (char**)argv);
}

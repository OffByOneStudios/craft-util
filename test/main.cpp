//#include "common.h"
//#undef CRAFT_UTIL_DLL
#include "util/common.h"
#include "util/all.h"


#include <spdlog/spdlog.h>
#include <bandit/bandit.h>
using namespace bandit;


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
			  craft::net::FetchOptions opts;
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
				  printf(s.c_str());
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
				  printf(s.c_str());
			  }
		  });
	  });
  });
});

int main(int argc, char const *argv[]) {
  return bandit::run(argc, (char**)argv);
}

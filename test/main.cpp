//#include "common.h"
#undef CRAFT_UTIL_DLL
#include "util/common.h"
#include "util/all.h"


#include <spdlog/spdlog.h>
#include <bandit/bandit.h>
using namespace bandit;

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
  });
});

int main(int argc, char const *argv[]) {
  return bandit::run(argc, (char**)argv);
}

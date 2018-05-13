//#include "common.h"
//#undef CRAFT_UTIL_DLL
#include "util/common.h"
#include "util/all.h"


int main(int argc, char const *argv[]) {
	auto console = spdlog::stdout_color_mt("console");

	/*craft::net::HttpServer server(console, 6112);
	server.init();
	server.serve_forever();*/
	//return bandit::run(argc, (char**)argv);
}

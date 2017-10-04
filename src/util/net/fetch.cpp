#include "util/common.h"
#include "fetch.hpp"

using namespace craft;
using namespace craft::net;

namespace craft {
namespace _impl {
	bool curl_init = false;
}}

void craft::net::test()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
}


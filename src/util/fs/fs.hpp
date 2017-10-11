#pragma once
#include "util/common.h"

#include "util/exception.h
#include "util/threading/threading.h"

namespace craft {
namespace fs {

	template<typename ResultType>
	inline stdext::future<ResultType> read(std::string path, std::function<ResultType(uint8_t*, size_t)> deserialize)
	{
		return std::async(std::launch::async, [=]() 
		{
			if (!path::exists(path))
			{
				throw stdext::exception("No Such File Exists: {0}", path);
			}

			std::ifstream file(path, std::ifstream::binary);

			std::streampos fsize = file.tellg();
			file.seekg(0, file.end);
			fsize = (file.tellg() - fsize);
			file.seekg(0, file.beg);

			uint8_t* data_raw = new uint8_t[fsize];
			file.read((char*)data_raw, fsize);

			if (!file)
			{
				delete[] data_raw;
				throw stdext::exception(fmt::format("error: only {0} could be read", file.gcount()));
			}

			ResultType res = deserialize(data_raw, fsize);
			return res;
		});

	}
}}

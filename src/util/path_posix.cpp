#include "common.h"
#if !defined(_WIN32)
#include "path.h"

#include "algorithms.hpp"
#include "exception.h"

using namespace path;

std::string path::join(std::string const& first, std::string const& second)
{
    std::string nfirst = first;
    if(nfirst != ".")
    {
        nfirst = path::normalize(first);
    }
    return fmt::format("{0}/{1}", nfirst, path::normalize(second));
}

std::string path::normalize(std::string const& path)
{
    // https://hg.python.org/cpython/file/v2.7.3/Lib/posixpath.py#l312
    if (path == "")
    {
        return ".";
    }
    int initial_slashes = path[0] == '/';

    if (initial_slashes && (path[1] == '/') && !(path[2] == '/'))
    {
        initial_slashes = 2;
    }

    std::vector<std::string> comps;
  	auto e = path.end();
  	auto i = path.begin();
  	while (i != e) {
    		i = std::find_if_not(i, e, [](char c) {return c == '/'; });
    		if (i == e) break;
    		auto j = std::find_if(i, e, [](char c) {return c == '/'; });
    	  comps.push_back(std::string(i, j));
    		i = j;
  	}
    std::vector<std::string> new_comps;
    for(auto comp : comps)
    {
        if (comp == "" || comp == ".")
        {
          continue;
        }
        if (comp != ".." || (!initial_slashes && !new_comps.size()) ||
            (new_comps.size() && new_comps.back() == ".."))
        {
            new_comps.push_back(comp);
        }
        else if (new_comps.size())
        {
          new_comps.pop_back();
        }
    }

    std::string res;
//    if (initial_slashes == 0)
//    {
//        res = "./";
//    }
    if (initial_slashes == 1)
    {
      res = "/";
    }
    else if (initial_slashes == 2)
    {
      res = "//";
    }

    for (auto comp: new_comps)
    {
      res += comp + "/";
    }
    res.pop_back();

    return res;
}
bool path::is_absolute(std::string const& path)
{
    return path::normalize(path)[0] == '/';
}
bool path::is_relative(std::string const& path)
{
    return path::normalize(path)[0] != '/';
}

bool path::is_root(const std::string &path)
{
  return path == "/";
}

std::string path::absolute(std::string const& path)
{
    char* f = getwd(NULL);
    std::string cwd(f);
    free(f);

    return path::join(cwd, path);
}
std::string path::relative(std::string const& path_with_relative, std::string const& base_path)
{
    std::string i_left = path::normalize(path_with_relative);
    std::string i_right = path::normalize(base_path);
    if (i_left.length() < i_right.length())
        return "";

    size_t counter = 0;
    while (counter < i_right.length() && i_left[counter] == i_right[counter])
    {
        counter++;
    }
    if (counter == 0)
    {
        return std::string("");
    }
    else
    {
      if (i_left[counter] == '/')
          counter++;
      return i_left.substr(counter);
    }
}

std::string path::common(std::string const& first, std::string const& second)
{
  std::string i_first = path::normalize(first);
  std::string i_second = path::normalize(second);

  int c = 0, last_slash = 0;
  while (i_first[c] == i_second[c])
  {
    if (i_first[c] == '/' || i_first[c] == '\\')
    {
      last_slash = c;
    }
    c++;

  }
  if (c == 0)
  {
    return std::string("");
  }
  auto res =i_first.substr(0, last_slash + 1);
  return res;
}

bool path::is_dir(std::string const& path)
{
    return path::filename(path) == std::string("");
}

bool path::is_file(std::string const& path)
{
    return !is_dir(path);
}

std::string path::filename(std::string const& path)
{
  std::string i_path = path::normalize(path);
  size_t sindex = i_path.find_last_of("/");
  size_t dindex = i_path.find_last_of(".");
  if (dindex == std::string::npos)
  {
    return std::string("");
  }
  else if(sindex == std::string::npos)
  {
    return i_path;
  }
  else
  {
    return i_path.substr(sindex + 1);
  }
}

void path::make_directory(std::string const&path)
{
	if (path::exists(path))
	{
		throw stdext::exception("Path Exists");
	}

	auto res = mkdir(path.c_str(), 0664);
	if (!res)
	{
		if (ENOENT == res)
		{
			throw stdext::exception("One or more intermediate directories do not exist. call path::ensure_directory instead");
		}
    else
    {
      throw stdext::exception("Unimplemented mkdir Error");
    }

	}
}

void path::ensure_directory(std::string const& path)
{
	auto d = path::dir(path::normalize(path));
	if (path::exists(d)) return;

	std::vector<std::string> parts;
	stdext::split(d, "/", std::back_inserter(parts));
	std::string p = path::absolute("./");
	for (auto part : parts)
	{
		p = path::join(p, part);
		if (!path::exists(p))
		{
			path::make_directory(p);
		}
	}
}

//TODO(clark) Moves these away
#include <stdio.h>
#include <ftw.h>
#include <unistd.h>
void path::remove_directory(std::string const& path)
{
	//https://stackoverflow.com/a/5467788
	auto d = path::dir(path::normalize(path));
	if (!path::exists(d)) return;
	
	nftw(d.c_str(), [](const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
	{
		int rv = remove(fpath);

		if (rv)
			perror(fpath);

		return rv;
	}, 64, FTW_DEPTH | FTW_PHYS);
}


std::string path::filebase(std::string const& path)
{
	auto fname = path::filename(path);
	auto ext = path::extname(path);

	auto index = fname.find(ext);
	if (index == fname.npos)
	{
		return fname;
	}
	else
	{
		return fname.substr(0, index);
	}
}

std::string path::extname(std::string const& path)
{
    std::string i_path = path::normalize(path);
    size_t dindex = i_path.find_last_of(".");
    if (dindex == std::string::npos)
    {
        return std::string("");
    }
    else
    {
        return i_path.substr(dindex + 1);
    }
}

std::string path::dirname(std::string const& path)
{
  std::string i_path = path::normalize(path);
	size_t sindex = i_path.find_last_of("/");
	size_t dindex = i_path.find_first_of(".");
	// No Slashes
	if (sindex == std::string::npos)
	{
		// Identity, Hidden directory like .git
		if (dindex == std::string::npos || dindex == 0)
		{
			return i_path;
		}
		// Is just a file
		else
		{
			return std::string("");
		}
	}
	else
	{
		i_path = i_path.substr(0, sindex);
		sindex = i_path.find_last_of("/");
		if (sindex == std::string::npos)
		{
		    return i_path + "/";
		}
		else
		{
		    return i_path.substr(sindex + 1) + "/";
		}
	}
}

std::string path::file(std::string const& path, std::string const& ext)
{
    std::string i_path = path::normalize(path);
    size_t sindex = i_path.find_last_of("/");
    size_t dindex = i_path.find_last_of(".");
    if (dindex == std::string::npos)
    {
    	 return std::string("");
    }
    else if(sindex == std::string::npos)
    {
    	 return i_path;
    }
    else
    {
    	 return i_path.substr(sindex + 1);
    }
}

std::string path::dir(std::string const& path)
{
    std::string i_path = path::normalize(path);
    size_t sindex = i_path.find_last_of("/");
    if (sindex == std::string::npos)
    {
        return "./";
    }
    return i_path.substr(0, sindex);
}

bool path::exists(std::string const& path)
{
    std::string npath = path::normalize(path);
    struct stat buf;
    int res = stat(npath.c_str(), &buf);
    if(res == -1)
    {
      return false;
    }
    return true;
}

std::vector<std::string> path::list_dirs(std::string const& path)
{
    std::string npath = path::normalize(path);
    std::vector<std::string> res;
    struct dirent *dp;
  	DIR *dfd = opendir(npath.c_str());
  	if(dfd != NULL) {
        while((dp = readdir(dfd)) != NULL)
        {
            std::string name(dp->d_name);
            if(name == "." || name == "..")
            {
                continue;
            }
            if(dp->d_type == DT_DIR)
            {
                res.push_back(name);
            }
        }
        closedir(dfd);
  	}

    return res;
}

std::vector<std::string> path::list_files(std::string const& path)
{
    std::string npath = path::normalize(path);
    std::vector<std::string> res;
    struct dirent *dp;
    DIR *dfd = opendir(npath.c_str());
    if(dfd != NULL) {
        while((dp = readdir(dfd)) != NULL)
        {
            std::string name(dp->d_name);
            if(name == "." || name == "..")
            {
                continue;
            }
            if(dp->d_type == DT_REG)
            {
                res.push_back(name);
            }
        }

        closedir(dfd);
    }

    return res;
}


std::vector<std::string> path::list_extensions(std::string const& path)
{
    std::vector<std::string> ret;

  	// TODO call file?
  	std::vector<std::string> res = path::list_files(path);

  	for (auto r : res)
  	{
  		size_t rm = r.find_first_of('.');
  		ret.push_back(std::string(r, rm + 1, std::string::npos));
  	}

  	return ret;
}

#include <sys/stat.h>
std::string path::executable_path()
{
	//https://stackoverflow.com/a/4025415
	char path[PATH_MAX];
	char dest[PATH_MAX];
	memset(dest, 0, sizeof(dest)); // readlink does not null terminate!
	struct stat info;
	pid_t pid = getpid();
	sprintf(path, "/proc/%d/exe", pid);
	if (readlink(path, dest, PATH_MAX) == -1)
		throw stdext::exception("Error Derefing symlink");
	else {
		return std::string(dest);
	}
}


path::watch::watch(std::string const& path)
{
}

path::watch::~watch()
{
}

void path::watch::check()
{
}

#endif

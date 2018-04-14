#include "util/common.h"
#if defined(_WIN32)
#include "path.h"

#include "algorithms.hpp"
#include "exception.h"
/* T:
This implementation uses wide (`*W` rather than `*A`) functions for all features. The engine's
strings are expect to be in UTF8 unicode at all times, we define conversion helpers for this
purpose using the std::wstring
*/

// See:
// * http://stackoverflow.com/questions/18580945/need-clarification-for-converting-paths-into-long-unicode-paths-or-the-ones-star
// * http://utf8everywhere.org/#windows

#include <windows.h>
#include <Shlwapi.h> // does not syupport long paths, for PathIsRelative and relative paths ONLY
#include "Shlobj.h"
#include <Pathcch.h>

#include <atomic>
#include <iterator>

#include "templates.h"
#include "concurrency/SafeQueue.hpp"

namespace impl
{
	typedef std::wstring string;

	const std::string longPathSpec = "\\\\?\\";

	bool isLongPath(std::string path)
	{
		return path.substr(0, 4) == longPathSpec;
	}

	string to(std::string in)
	{
		int len = lstrlenA(in.c_str());
		size_t size = (size_t)MultiByteToWideChar(CP_UTF8, 0,
			in.c_str(), len,
			nullptr, 0);

		string ret;
		ret.resize(size);

		/*size_t wrote = (size_t)*/MultiByteToWideChar(CP_UTF8, 0,
			in.c_str(), len,
			&ret[0], (int)ret.size());
		return ret;
	}

	std::string from(string in)
	{
		int len = lstrlenW(in.c_str());
		size_t size = (size_t)WideCharToMultiByte(CP_UTF8, 0,
			in.c_str(), len,
			nullptr, 0,
			nullptr, nullptr);

		std::string ret;
		ret.resize(size);

		/*size_t wrote = (size_t)*/WideCharToMultiByte(CP_UTF8, 0,
			in.c_str(), len,
			&ret[0], (int)ret.size(),
			nullptr, nullptr);

		if (isLongPath(ret))
			ret = ret.substr(longPathSpec.size());

		return ret;
	}

	bool find_files_filter_file(WIN32_FIND_DATAW* data)
	{
		// TODO Filter wierd ones
		return (data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
	}

	bool find_files_filter_dir(WIN32_FIND_DATAW* data)
	{
		// TODO Filter wierd ones
		if (data->cFileName[0] == '.'
			&& (data->cFileName[1] == '\0'
				|| (data->cFileName[1] == '.' && data->cFileName[2] == '\0')))
			return false;
		return (data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
	}

	std::vector<std::string> find_files(std::string search_path, std::function<bool(WIN32_FIND_DATAW*)> test = nullptr)
	{
		std::vector<std::string> ret;
		WIN32_FIND_DATAW find_data;

		bool is_already_search = search_path.back() == '*';
		bool is_dir_path = search_path.back() == '\\';

		string i_search_path = impl::to(search_path);

		if (!is_already_search)
		{
			if (!is_dir_path)
				i_search_path.push_back('\\');
			i_search_path.push_back('*');
		}

		/* T:
		This method appears to be limited to 260 char filenames, but does work with long path names
		as filenames are limited to 256 chars anyway, and does, in the unicode version, take the
		\\?\ style paths.
		*/

		HANDLE h = FindFirstFileW(i_search_path.c_str(), &find_data);
		if (h == INVALID_HANDLE_VALUE)
		{
			// No need for FindClose
			// TODO throw exception
			return ret;
		}

		do
		{
			if (test == nullptr || test(&find_data))
			{
				ret.push_back(impl::from(find_data.cFileName));
			}

		} while (FindNextFileW(h, &find_data) != 0);

		if (GetLastError() != ERROR_NO_MORE_FILES)
		{
			// TODO throw exception and then also call:
			// FindClose(h);
		}

		FindClose(h);
		return ret;
	}

#undef ABSOLUTE // Who thought that was a good idea in a header?
	enum PATHPREFIX
	{
		UNKNOWN,            // Probably relative
		ABSOLUTE,           // Normal absolute path that is none of the other types
		UNC,                // Found \\server\share\ prefix
		LONG_UNICODE,       // Found \\?\ prefix
		LONG_UNICODE_UNC,   // Found \\?\UNC\ prefix
	};

	PATHPREFIX path_type(string path, size_t* index = 0)
	{
		//Checks if 'path' begins with the drive, share, prefix, etc
		//EXAMPLES:
		//   Path                          Return:   Points at:                 PrefixType:
		//  Relative\Folder\File.txt        0         Relative\Folder\File.txt   PPT_UNKNOWN
		//  \RelativeToRoot\Folder          1         RelativeToRoot\Folder      PPT_ABSOLUTE
		//  C:\Windows\Folder               3         Windows\Folder             PPT_ABSOLUTE
		//  \\server\share\Desktop          15        Desktop                    PPT_UNC
		//  \\?\C:\Windows\Folder           7         Windows\Folder             PPT_LONG_UNICODE
		//  \\?\UNC\server\share\Desktop    21        Desktop                    PPT_LONG_UNICODE_UNC
		//RETURN:
		//      = Index in 'pPath' after the root, or
		//      = 0 if no root was found
		size_t ind = 0;
		PATHPREFIX ret_p = UNKNOWN;

		if (path.substr(1, 2) == string(L":\\"))
		{
			ind = 3;
			ret_p = ABSOLUTE;
		}
		else if (path[0] == '\\' && path[1] != '\\')
		{
			ind = 2;
			ret_p = ABSOLUTE;
		}
		else if (path.substr(0, 2) == string(L"\\\\"))
		{
			if (path[2] != '?')
			{
				ind = 2;
				ret_p = UNC;
			}
			else if (path.substr(3, 5) == string(L"\\UNC\\"))
			{
				ind = 8;
				ret_p = LONG_UNICODE_UNC;
			}
			else
			{
				ind = 8;
				ret_p = LONG_UNICODE_UNC;
			}
		}

		if (index != nullptr)
			*index = ind;
		return ret_p;
	}

	bool is_relative(string const& path)
	{
		if (path.length() > MAX_PATH)
		{
			assert(path.substr(0, 4) == string(L"\\?\\\\"));
			return false;
		}
		return PathIsRelativeW(path.c_str()) == TRUE;
	}
}

/******************************************************************************
** Function Implementations
******************************************************************************/

std::string path::join(std::string const& first, std::string const& second)
{
  if(second == "") return first;
  if(first == "") return second;
  
	impl::string i_first = impl::to(first);
	impl::string i_second = impl::to(second);

	if (impl::is_relative(i_first))
	{
		return impl::from(i_first + L"\\" + i_second);
	}

	// 4 for `\\?`; 1 for intermeidate `\`; 1 for trailing `\`; 1 for NULL
	size_t buf_size = i_first.size() + i_second.size() + 4 + 1 + 1;
	WCHAR* buf = new WCHAR[buf_size];
	HRESULT res = PathCchCombineEx(buf, buf_size, i_first.c_str(), i_second.c_str(), PATHCCH_ALLOW_LONG_PATHS);

	if (res != S_OK)
	{
		delete[] buf;
		assert(false && "PathCchCombineEx failed"); // TODO: Exception?
	}

	std::string ret = impl::from(impl::string(buf));
	delete[] buf;
	return ret;
}

std::string path::normalize(std::string const& path)
{
	impl::string i_path = impl::to(path);

	for (size_t i = 0; i < i_path.length(); i++)
	{
		if (i_path[i] == '/')
		{
			i_path[i] = '\\';
		}
	}
	return impl::from(i_path);
}

bool path::is_absolute(std::string const& path)
{
	impl::string i_path = impl::to(path);
	if (impl::is_relative(i_path))
		return false;

	impl::PATHPREFIX kind = impl::path_type(i_path, nullptr);
	return kind != impl::UNKNOWN;
}

bool path::is_relative(std::string const& path)
{
	return impl::is_relative(impl::to(path));
}

bool path::is_root(std::string const& path)
{
  std::regex e (R"re([A-Za-z]:\\)re");
  return std::regex_match(path, e);
}

std::string path::absolute(std::string const& path)
{
	if (path == "")
	{
		DWORD buf_size = GetCurrentDirectoryW(0, nullptr);
		if (buf_size == 0)
		{
			assert(false && "GetCurrentDirectoryW failed."); // TODO exceptions
		}

		WCHAR* buf = new WCHAR[buf_size];
		buf_size = GetCurrentDirectoryW(buf_size, buf);
		if (buf_size == 0)
		{
			delete[] buf;
			assert(false && "GetCurrentDirectoryW failed."); // TODO exceptions
		}

		std::string ret = impl::from(buf);
		delete[] buf;
		return ret;
	}
	else
	{
		impl::string i_path = impl::to(path);

		if (!impl::is_relative(i_path))
			return path;

		DWORD buf_size = GetFullPathNameW(i_path.c_str(), 0, nullptr, nullptr);
		if (buf_size == 0)
		{
			assert(false && "GetFullPathNameW failed."); // TODO exceptions
		}

		WCHAR* buf = new WCHAR[buf_size];
		buf_size = GetFullPathNameW(i_path.c_str(), buf_size, buf, nullptr);
		if (buf_size == 0)
		{
			delete[] buf;
			assert(false && "GetFullPathNameW failed."); // TODO exceptions
		}

		std::string ret = impl::from(buf);
		delete[] buf;
		return ret;
	}
}

std::string path::file(std::string const& path, std::string const& ext)
{
	// TODO actually do correct windows file logic
	if (ext == "")
		return path;
	return path + "." + ext;
}



std::string path::dir(std::string const& path)
{
	impl::string i_path = impl::to(path);

	HRESULT res = PathCchRemoveFileSpec(&i_path[0], i_path.size());

	if (res != S_OK)
	{
		assert(false && "PathCchRemoveFileSpec failed."); // TODO exceptions
	}

	auto ret = impl::from(i_path);

	if (ret.empty())
		return ".";

	return ret + "\\";
}

bool path::exists(std::string const& path)
{
	DWORD attrs = GetFileAttributesW(impl::to(path).c_str());

	if (attrs == INVALID_FILE_ATTRIBUTES)
	{
		// TODO Exception based off of getlasterror?
		return false;
	}

	return true;
}

std::vector<std::string> path::list_dirs(std::string const& path)
{
	std::vector<std::string> ret = impl::find_files(absolute(path), impl::find_files_filter_dir);

	return ret;
}

std::vector<std::string> path::list_files(std::string const& path)
{
	std::vector<std::string> ret = impl::find_files(absolute(path), impl::find_files_filter_file);

	return ret;
}

std::vector<std::string> path::list_extensions(std::string const& path)
{
	std::vector<std::string> ret;

	// TODO call file?
	std::vector<std::string> res = impl::find_files(absolute(path) + ".*", impl::find_files_filter_file);

	for (auto r : res)
	{
		size_t rm = r.find_first_of('.');
		ret.push_back(std::string(r, rm + 1, std::string::npos));
	}

	return ret;
}

std::string path::common(std::string const& first, std::string const& second)
{
	impl::string i_first = impl::to(first);
	impl::string i_second = impl::to(second);

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
	auto res = impl::from(i_first.substr(0, last_slash + 1));
	return res;
}

std::string path::dirname(std::string const& path)
{
	impl::string i_path = impl::to(path);
	size_t sindex = i_path.find_last_of(impl::to("\\"));
	size_t dindex = i_path.find_first_of(impl::to("."));
	// No Slashes
	if (sindex == impl::string::npos)
	{
		// Identity, Hidden directory like .git
		if (dindex == impl::string::npos || dindex == 0)
		{
			return impl::from(i_path);
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
		sindex = i_path.find_last_of(impl::to("\\"));
		if (sindex == impl::string::npos)
		{
			return impl::from(i_path + impl::to("\\"));
		}
		else
		{
			return impl::from(i_path.substr(sindex + 1) + impl::to("\\"));
		}
	}
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
		return fname.substr(0, index - 1);
	}
}

std::string path::filename(std::string const& path)
{
	size_t sindex = path.find_last_of("\\");
	
	if (sindex == impl::string::npos)
	{
		return path;
	}
	else
	{
		return path.substr(sindex + 1);
	}
}

std::string path::extname(std::string const & path)
{
	size_t sindex = path.find_last_of("\\");
	size_t dindex = path.find_last_of(".");

	if (dindex == impl::string::npos || dindex < sindex)
	{
		return std::string("");
	}
	else
	{
		return path.substr(dindex + 1);
	}
}


bool path::is_dir(std::string const& path)
{
	return path::filename(path) == std::string("");
}

bool path::is_file(std::string const& path)
{
	return !is_dir(path);
}

std::string path::relative(std::string const& path_with_relative, std::string const& base_path)
{
	impl::string i_left = impl::to(path::absolute(path_with_relative));
	impl::string i_right = impl::to(path::absolute(base_path));

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
		if (i_left[counter] == '\\')
			counter++;
		return impl::from(i_left.substr(counter));
	}
}

void path::make_directory(std::string const&path)
{
	if (path::exists(path))
	{
		throw stdext::exception("Path Exists");
	}
	impl::string i_path = impl::to(path);
	auto res = CreateDirectoryW(i_path.c_str(), NULL);
	if (!res)
	{
		auto err = GetLastError();
		if (ERROR_PATH_NOT_FOUND == err)
		{
			throw stdext::exception("One or more intermediate directories do not exist. call path::ensure_directory instead");
		}

	}
}

void path::ensure_directory(std::string const& path)
{
	auto d = (is_file(path)) ? path::dir(path::normalize(path)): path;
	if (path::exists(d)) return;

	std::vector<std::string> parts;
	stdext::split(d, "\\", std::back_inserter(parts));
	std::string p = path::absolute("");
	for (auto part : parts)
	{
		p = path::join(p, part);
		if (!path::exists(p))
		{
			path::make_directory(p);
		}
	}
}

void path::remove_directory(std::string const& path)
{
	auto d = path::normalize(path);
	if (!path::exists(d)) return;
	for (auto f : path::list_files(d))
	{
		impl::string i_f = impl::to(path::join(d,f));
		DeleteFileW(i_f.c_str());
	}
	for (auto dr : path::list_dirs(d))
	{
		remove_directory(path::join(d, dr));
		impl::string i_d = impl::to(path::join(d,dr));
	}

	impl::string i_d = impl::to(d);
	RemoveDirectoryW(i_d.c_str());
}


std::string path::executable_path()
{
	//https://stackoverflow.com/a/2647446
	HMODULE hModule = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH];
	GetModuleFileNameW(hModule, path, MAX_PATH);
	impl::string i_res(path);
	return impl::from(i_res);
}

void path::set_cwd(std::string const& path)
{
	auto abs_path = path::absolute(path);

	impl::string i_path;
	i_path = impl::to(abs_path);
	auto res = SetCurrentDirectoryW(i_path.c_str());
	if (!res)
	{
		throw stdext::exception("Folder is not a directory");
	}
}


std::string path::system_temp_path()
{
	wchar_t temppath[_MAX_PATH];      // absolute path of temporary .bat file
	GetTempPathW(_MAX_PATH, temppath);

	impl::string i_res(temppath);
	return impl::from(i_res);
}

// Get the path to the user temporary directory 
std::string path::user_temp_path()
{
	wchar_t temppath[_MAX_PATH];
	SHGetFolderPathW(NULL,
		CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,
		NULL,
		0,
		temppath);

	impl::string i_res(temppath);
	return path::join(impl::from(i_res), "Temp");
}

// Get the path to system data directory
std::string path::system_data_path()
{
	//TODO Find the CISDL code for this
	return "C:\\ProgramData";
}

// Get the path to the user temporary directory 
std::string path::user_data_path()
{
	wchar_t temppath[_MAX_PATH];
	SHGetFolderPathW(NULL,
		CSIDL_APPDATA | CSIDL_FLAG_CREATE,
		NULL,
		0,
		temppath);

	impl::string i_res(temppath);
	return impl::from(i_res);
}

/******************************************************************************
** Watch Implementation
******************************************************************************/

// See:
// * http://qualapps.blogspot.com/2010/05/understanding-readdirectorychangesw.html
// * https://social.msdn.microsoft.com/Forums/vstudio/en-US/4465cafb-f4ed-434f-89d8-c85ced6ffaa8/filesystemwatcher-reliability?forum=netfxbcl

// We implement this with ReadDirectoryChangesW

struct _path_watch_implementation
{
	const static int c_delay = 250;

	path::watch* _this;
	_path_watch_implementation(path::watch* _w);
	~_path_watch_implementation();

	std::atomic<bool> _running;
	HANDLE _threadHandle;
	std::thread* _thread;
	void _main(void);

	struct t_event
	{
		std::string file;
		DWORD action;
	};
	craft::concurrency::SafeQueue<t_event> _queue;

	struct t_state
	{
		bool originally_existed;
		bool was_created;
		bool was_modified;
		bool was_destroyed;
		bool was_renamed;
		std::string renamed_from;
		std::chrono::milliseconds decay;

		t_state()
			: originally_existed(false)
			, was_created(false), was_modified(false), was_destroyed(false), was_renamed(false)
		{
		}
	};
	std::map<std::string, t_state> _states;

	HANDLE dir;
	FILE_NOTIFY_INFORMATION buffer[1024];
};

path::watch::watch(std::string const& path)
	: path(path)
{
	// TODO throw exception for bad path
	// Should be a directory

	auto _this = new _path_watch_implementation(this);
	_implementation = _this;
}

path::watch::~watch()
{
	delete (_path_watch_implementation*)_implementation;
}

void path::watch::check()
{
	_path_watch_implementation* that = (_path_watch_implementation*)this->_implementation;

	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

	// We are assuming a rename always follows the pattern: [..., OLD_NAME, NEW_NAME, ...]
	std::string last_name;
	bool last_was_rename_old(false);

	_path_watch_implementation::t_event entry;
	_path_watch_implementation::t_state* state;
	while (that->_queue.dequeueAntsy(entry))
	{
		bool just_created = false;
		auto it = that->_states.find(entry.file);
		if (it == that->_states.end())
		{
			state = &that->_states[entry.file];
			just_created = true;
		}
		else
		{
			state = &it->second;
		}

		// TODO state machine?
		switch (entry.action)
		{
		case FILE_ACTION_MODIFIED:
			if (just_created)
				state->originally_existed = true;
			state->was_modified = true;
			break;
		case FILE_ACTION_RENAMED_OLD_NAME:
		case FILE_ACTION_REMOVED:
			if (just_created)
				state->originally_existed = true;
			if (state->was_created && !state->originally_existed) // Decay to nothing
			{
				state->was_destroyed = false;
				state->was_modified = false;
				state->was_created = false;
			}
			else
			{
				state->was_destroyed = true;
			}
			break;
		case FILE_ACTION_RENAMED_NEW_NAME:
			if (just_created)
				state->originally_existed = false;
			if (last_was_rename_old)
			{ // Microsoft is the greater evil
				state->renamed_from = last_name; // Do we need a stack...?
			}
			state->was_renamed = true;

			// I'm worried about these state:
			if (state->originally_existed && !state->was_destroyed)
			{
				state->was_modified = true; // I guess?
				break;
			}
			if (!state->originally_existed && state->was_created)
			{
				state->was_modified = true; // I guess?
				break;
			}
		case FILE_ACTION_ADDED:
			if (just_created)
				state->originally_existed = false;

			if (state->was_destroyed && state->originally_existed) // Decay to modified
			{
				state->was_created = false;
				state->was_destroyed = false;
				state->was_modified = true;
			}
			else
			{
				state->was_created = true;
			}
			break;
		}

		last_was_rename_old = FILE_ACTION_RENAMED_OLD_NAME == entry.action;
		last_name = entry.file;

		state->decay = ms + std::chrono::milliseconds(_path_watch_implementation::c_delay);
	}

	// Check existing states:
	for (auto it = that->_states.cbegin(); it != that->_states.cend(); /* erase */ )
	{
		if (it->second.decay < ms)
		{
			std::string path = it->first;
			_path_watch_implementation::t_state state = it->second;
			that->_states.erase(it++); // ++   Correct use of post-increment

			if (state.originally_existed  && state.was_modified
				&& !state.was_created && !state.was_destroyed)
			{
				onMutFile.emit(path);
				continue;
			}

			if (!state.originally_existed && state.was_created)
			{
				onAddFile.emit(path);
				if (state.was_modified)
					onMutFile.emit(path);
				continue;
			}

			if (state.originally_existed && state.was_destroyed)
			{
				onRemFile.emit(path);
				continue;
			}
		}
		else
		{
			++it;
		}
	}
}

_path_watch_implementation::_path_watch_implementation(path::watch* _w)
	: _this(_w)
	, _running(false)
{
	_thread = new std::thread(std::bind(&_path_watch_implementation::_main, this));

	/* T:
	We aren't fully constructed untill the thread has set _running;
	*/
	while(!_running) { }
}
_path_watch_implementation::~_path_watch_implementation()
{
	_running = false;
	CancelSynchronousIo(_threadHandle);
	_thread->join();
}
void _path_watch_implementation::_main(void)
{
	_threadHandle = GetCurrentThread();
	_running = true;

	this->dir = CreateFileA(
		_this->path.c_str(),
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,
		NULL
	);

	DWORD bytes_written;
	while (_running)
	{
		const size_t buf_size = sizeof(this->buffer);

		BOOL status = ReadDirectoryChangesW(
			this->dir,
			this->buffer,
			buf_size,
			TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME
				| FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE
				| FILE_NOTIFY_CHANGE_LAST_WRITE /*| FILE_NOTIFY_CHANGE_LAST_ACCESS*/ | FILE_NOTIFY_CHANGE_CREATION,
			&bytes_written,
			NULL,
			NULL
		);

		if (!status)
			continue;

		//craft::engine()->log()->trace("[watch] got {0} actions", bytes_written / sizeof(FILE_NOTIFY_INFORMATION));

		FILE_NOTIFY_INFORMATION* ptr = this->buffer;

		while (ptr != nullptr && ptr < this->buffer + bytes_written)
		{
			if (ptr->FileNameLength < USHRT_MAX)
			{
				size_t len = ptr->FileNameLength / sizeof(impl::string::value_type);
				impl::string i_file;

				i_file.resize(len);
				std::copy(ptr->FileName, ptr->FileName + len,
					stdext::checked_array_iterator<WCHAR*>(&i_file[0], i_file.size()));

				std::string file = impl::from(i_file);

				t_event entry;
				entry.file = file;
				entry.action = ptr->Action;
				_queue.enqueue(entry);

				//craft::engine()->log()->trace("[watch] action {0} on file {1}", ptr->Action, file);
			}

			if (ptr->NextEntryOffset == 0)
				break;
			ptr = (FILE_NOTIFY_INFORMATION*)(((uint8_t*)ptr) + ptr->NextEntryOffset);
		}
	}
}
#endif

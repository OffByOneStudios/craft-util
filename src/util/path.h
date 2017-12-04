#pragma once
#include "common.h"

namespace path
{

/******************************************************************************
** Platform Path Implementations
******************************************************************************/

	// Joins two path strings togeather, embeds relative imports into the string.
	//
	// Pass the string ".." to go up a directory.
	CRAFT_UTIL_EXPORTED std::string join(std::string const& first, std::string const& second);

	// Normalizes a path (removes . and .., other characters) to an implementation specific valid
	// path.
	//
	// This is called by all methods below that use the file system, prior to making their calls.
	CRAFT_UTIL_EXPORTED std::string normalize(std::string const& path);

	// Checks to see if the path describes an absolute location (rather than absolute)
	CRAFT_UTIL_EXPORTED bool is_absolute(std::string const& path);

  // Check to see if path is equal to the filesystem root
  CRAFT_UTIL_EXPORTED bool is_root(std::string const& path);
	// Checks to see if the path describes a relative location (rather than relative)
	CRAFT_UTIL_EXPORTED bool is_relative(std::string const& path);

	// Ensure an absolute path, uses current working directory if a path isn't absolute.
	//
	// Calling this with an empty string will simply return the current working directory.
	//
	// This is called by all methods below that use the file system, prior to making their calls.
	CRAFT_UTIL_EXPORTED std::string absolute(std::string const& path = "");

	// Extracts the relative portion of two paths (difference), empty string if niether
	CRAFT_UTIL_EXPORTED std::string relative(std::string const& path_with_relative, std::string const& base_path);

	// Extracts the common portion of two paths (intersection).
	CRAFT_UTIL_EXPORTED std::string common(std::string const& first, std::string const& second);

	// Checks to see if the path describes a directory (rather than a file)
	CRAFT_UTIL_EXPORTED bool is_dir(std::string const& path);

	// Checks to see if the path describes a file (rather than a directory)
	CRAFT_UTIL_EXPORTED bool is_file(std::string const& path);

	// Extracts the filename, including the extenion, of the path, if the path is not a file then
	// returns "".
	CRAFT_UTIL_EXPORTED std::string filename(std::string const& path);

	// Extracts the filename extension of the path, if the path does not
	// have an extension then returns "".
	CRAFT_UTIL_EXPORTED std::string extname(std::string const& path);

	// Extracts the dirname of the path, if the path is a file it removes the filename section of
	// the path and then returns the dirname.
	CRAFT_UTIL_EXPORTED std::string dirname(std::string const& path);

	// Takes a path and constructs a file path out of it.
	//
	// If the path is a directory it will attempt to build a filename out of it and append the
	// extension like normal. The extension can contain dot seperators, only the first one counts.
	// If there is an existing file extension in the path it will be removed.
	//
	// By default simply removes the directory mark and strips file extension.
	CRAFT_UTIL_EXPORTED std::string file(std::string const& path, std::string const& ext = "");

	// Takes a path and turns it into a directory. Truncates trailing file information.
	CRAFT_UTIL_EXPORTED std::string dir(std::string const& path);

	//
	// Ensure a directory exists
	//
	CRAFT_UTIL_EXPORTED void ensure_directory(std::string const& path);
	CRAFT_UTIL_EXPORTED void make_directory(std::string const& path);
	CRAFT_UTIL_EXPORTED void remove_directory(std::string const& path);
	// Checks if a path exists (uses FS).
	//
	// Only checks for *exactly* the path requested, e.g. checks for either a dir or a file.
	CRAFT_UTIL_EXPORTED bool exists(std::string const& path);

	// Given a directory (calls dir) return a list of dirs (relative to path) existing in that dir
	// (uses FS).
	CRAFT_UTIL_EXPORTED std::vector<std::string> list_dirs(std::string const& path);

	// Given a directory (calls dir) return a list of files (relative to path) that existing in
	// that dir (uses FS).
	CRAFT_UTIL_EXPORTED std::vector<std::string> list_files(std::string const& path);

	// Given a file (calls file) list all of the extensions that exist for that file (uses FS).
	//
	// Lists addition extensions to the given path. Call file first.
	CRAFT_UTIL_EXPORTED std::vector<std::string> list_extensions(std::string const& path);


	// Get the fully qualified path to the current executable
	CRAFT_UTIL_EXPORTED std::string executable_path();
	
	// Get the path to the user's homefolder (%HOMEPATH% windows, `~` POSIX)
	CRAFT_UTIL_EXPORTED std::string home_path();

	// Get the path to system temporary directory
	CRAFT_UTIL_EXPORTED std::string system_temp_path();
	// Get the path to the user temporary directory 
	CRAFT_UTIL_EXPORTED std::string user_temp_path();

	// Get the path to system data directory
	CRAFT_UTIL_EXPORTED std::string system_data_path();
	// Get the path to the user temporary directory 
	CRAFT_UTIL_EXPORTED std::string user_data_path();


	// Watches a directory and it's subdirectories for changes.
	// Calls check to signal the signals.
	// Filters unused file information.
	class watch final
	{
	public:
		void* _implementation;
		std::string path;
	public:
		CRAFT_UTIL_EXPORTED watch(std::string const& path);
		CRAFT_UTIL_EXPORTED ~watch();

		CRAFT_UTIL_EXPORTED void check();

		Signal< void(std::string const& filename) > onMutFile;
		Signal< void(std::string const& newFilename) > onAddFile;
		Signal< void(std::string const& delFilename) > onRemFile;
	};

/******************************************************************************
** Shared Path Utilities
******************************************************************************/

	// Walks a tree of files from the given root directory (uses FS).
	class walk
	{
		std::string _rootPath;
	public:
		struct data
		{
			std::string path;
			std::vector<std::string> files;
			std::vector<std::string> dirs;
		};

		class const_iterator
			: public std::iterator<std::forward_iterator_tag, const data>
		{
			struct _data : public data
			{
				size_t pos;
			};

			std::stack<_data> _dataStack;

			const_iterator(std::string const& path);

			friend walk;

			void _pushPath(std::string const& path);
		public:
			CRAFT_UTIL_EXPORTED const_iterator();
			CRAFT_UTIL_EXPORTED const_iterator(const const_iterator&);
			CRAFT_UTIL_EXPORTED ~const_iterator();

			CRAFT_UTIL_EXPORTED const_iterator& operator=(const const_iterator&);

			CRAFT_UTIL_EXPORTED bool operator==(const const_iterator&) const;
			CRAFT_UTIL_EXPORTED bool operator!=(const const_iterator&) const;

			CRAFT_UTIL_EXPORTED const_iterator& operator++();

			CRAFT_UTIL_EXPORTED reference operator*() const;
			CRAFT_UTIL_EXPORTED pointer operator->() const;
		};

		CRAFT_UTIL_EXPORTED walk(std::string const& path);

		CRAFT_UTIL_EXPORTED const_iterator cbegin() const;
		inline const_iterator begin() const { return cbegin(); }
		CRAFT_UTIL_EXPORTED const_iterator cend() const;
		inline const_iterator end() const { return cend(); }
	};

/******************************************************************************
** Inline Helpers
******************************************************************************/

	inline std::string join(std::string a0, std::string a1, std::string last)
	{
		return join(join(a0, a1), last);
	}
	inline std::string join(std::string a0, std::string a1, std::string a2, std::string last)
	{
		return join(join(a0, a1, a2), last);
	}
	inline std::string join(std::string a0, std::string a1, std::string a2, std::string a3, std::string last)
	{
		return join(join(a0, a1, a2, a3), last);
	}

}

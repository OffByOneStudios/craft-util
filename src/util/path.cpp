#include "common.h"

#include "path.h"

using namespace path;

/******************************************************************************
** `walk` Implementation
******************************************************************************/

//
// Interesting implementation section
//

walk::walk(std::string const& path)
	: _rootPath(path::normalize(path))
{
}

void walk::const_iterator::_pushPath(std::string const& path)
{
	_data d;
	d.path = path;

	d.dirs = path::list_dirs(path);
	d.files = path::list_files(path);
	d.pos = 0;

	_dataStack.push(d);
}

walk::const_iterator::const_iterator(std::string const& path)
{
	_pushPath(path);
}

walk::const_iterator& walk::const_iterator::operator++()
{
	// Continue from where we were
	_data* dataTop = &_dataStack.top();

	// Skip directories that we have finished iterating over
	while (dataTop->dirs.size() == dataTop->pos)
	{
		_dataStack.pop();
		if (_dataStack.empty())
			return *this;
		dataTop = &_dataStack.top();
	}

	// Go to the next directory
	auto newPath = path::join(dataTop->path, dataTop->dirs[dataTop->pos++]);
	_pushPath(newPath);

	return *this;
}

//
// Iterator equality
//

bool walk::const_iterator::operator==(const walk::const_iterator& that) const
{
	return _dataStack.size() == that._dataStack.size()
		&& (_dataStack.size() == 0 || _dataStack.top().path == that._dataStack.top().path);
}
bool walk::const_iterator::operator!=(const walk::const_iterator& that) const
{
	return !(*this == that);
}

//
// Iterator constructors / Assignment
//

walk::const_iterator::const_iterator()
	: _dataStack()
{
}
walk::const_iterator::const_iterator(const walk::const_iterator& copy)
{
	_dataStack = copy._dataStack;
}
walk::const_iterator::~const_iterator()
{
}

walk::const_iterator& walk::const_iterator::operator=(const walk::const_iterator& copy)
{
	_dataStack = copy._dataStack;

	return *this;
}

//
// Iterator deref
//

walk::const_iterator::reference walk::const_iterator::operator*() const
{
	return _dataStack.top();
}
walk::const_iterator::pointer walk::const_iterator::operator->() const
{
	return &_dataStack.top();
}

//
// `walk` class
//

walk::const_iterator walk::cbegin() const
{
	return const_iterator(_rootPath);
}
walk::const_iterator walk::cend() const
{
	return const_iterator();
}

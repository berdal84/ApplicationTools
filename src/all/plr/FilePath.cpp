////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/FilePath.h>

#ifdef PLR_COMPILER_MSVC
	// most of cstring generates warnings; disable them!
	#pragma warning(disable: 4996)
#endif
#include <cstring>

using namespace plr;

/// \todo make these platform dependent
static const char* kDirSeparator = "\\/";
static const char* kExtSeparator = ".";

FilePath::FilePath(const char* _path)
	: m_len(0)
	, m_dirsCount(0)
	, m_fname(0)
	, m_ext(0)
{
	if (_path) {
		setPath(_path);
	} else {
		m_buf[0] = m_pth[0] = 0;
	}
}

void FilePath::setPath(const char* _path)
{
	m_len = (int)strlen(_path); // doesn't count null terminator
	PLR_ASSERT(m_len < PLR_MAX_PATH_LENGTH);
	strncpy(m_pth, _path, PLR_MAX_PATH_LENGTH); // pads with zeroes
	parse();
}

void FilePath::setDirectory(int i, const char* _dir)
{
  // remove leading separators
	while (strchr(kDirSeparator, (int)(*_dir)) != 0) {
		++_dir;
	}
	replace(getDirectory(i), _dir);
}

void FilePath::setExtension(const char* _ext)
{
  // remove leading separators
	while (strchr(kExtSeparator, (int)(*_ext)) != 0) {
		++_ext;
	}
	replace(getExtension(), _ext);
}


// PRIVATE

void FilePath::parse()
{
	strncpy(m_buf, m_pth, PLR_MAX_PATH_LENGTH); // pads with zeroes

  // tokenize directories into list
	while (m_dirs[m_dirsCount] = strtok(m_dirsCount ? 0 : m_buf, kDirSeparator))
		++m_dirsCount;

  // fname is tokenized as a directory, extract/decrement dir count
	m_fname = m_dirs[--m_dirsCount];

  // ext is the characters after the last ext sep, can't use strtok as we need a reverse parse
	for (m_ext = &m_buf[m_len - 1]; m_ext != m_buf; --m_ext) {
		if (*m_ext != 0 && strchr(kExtSeparator, (int)(*m_ext)) != 0) { // test for 0 otherwise strchr will match it
		// ext sep was found, insert null and move m_ext forward
			*m_ext = 0;
			++m_ext;
			break;
		}
	}
	if (m_ext == m_buf) {
		m_ext = 0;
	}

}

void FilePath::replace(const char* _beg, const char* _in)
{
	int lenIn  = (int)strlen(_in);
	int lenOut = (int)strlen(_beg); // length of region to be replaced

	int delta = lenIn - lenOut;
	if (delta != 0) {
	 // move characters up/down to fit new region
		if (delta > 0) {
			PLR_ASSERT(m_len + delta < PLR_MAX_PATH_LENGTH); // overflow
		// delta positive, start at the end and work backwards
			int lim = (int)(_beg - m_buf) + lenOut;
			int i = m_len + delta - 1;
			for (; i != lim; --i) {
				m_buf[i] = m_buf[i - delta];
				m_pth[i] = m_pth[i - delta];
			}
		} else {
	  	 // delta negative, start at the beginning and work forwards
			int lim = m_len + delta + 1;
			int i = (int)(_beg - m_buf) + lenIn;
			for (; i != lim; ++i) {
				m_buf[i] = m_buf[i - delta];
				m_pth[i] = m_pth[i - delta];
			}
		}
	}

  // replace region
	int i = (int)(_beg - m_buf);
	for (; *_in != 0; ++i, ++_in) {
		m_buf[i] = *_in;
		m_pth[i] = *_in;
	}

  // shift metadata after the replace region by -delta
	m_len += delta;
	for (int i = 0; i < m_dirsCount; ++i) {
		if (m_dirs[i] > _beg) {
			m_dirs[i] += delta;
		}
	}
	if (m_fname > _beg) {
		m_fname += delta;
	}
	if (m_ext > _beg) {
		m_ext += delta;
	}
}

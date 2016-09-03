////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_TextParser_h
#define plr_TextParser_h

#include <plr/def.h>

namespace plr {

////////////////////////////////////////////////////////////////////////////////
/// \class TextParser
/// Common text parsing operations; advance a string ptr with character
/// classification, check if a region of the string contains characters, keep
/// a count of line endings, etc. Basically wraps cctype and cstring.
/// \note Only line feed '\n' are counted as line endings; carriage return '\r'
///   are treated as whitespace only.
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
class TextParser
{
public:
	TextParser(const char* _str)
		: m_start(_str)
		, m_pos(_str)
	{
	}

	/// Classification functions test the char at the current position.
	bool isNull() const { return *m_pos == 0; }
	bool isWhitespace() const;
	bool isAlpha() const;
	bool isNum() const;
	bool isAlphaNum() const;
	bool isLineEnd() const;

	/// advance*() and skip*() functions return the character they stop on, which
	/// is 0 if the function hit the end of the string.
	char advance(sint _n = 1) { m_pos += _n; return *m_pos;  }
	char advanceToNext(char _c); // advance to next occurence of _c
	char advanceToNext(const char* _list); // advance to next occurence of any char in _list
	char advanceToNextWhitespace();
	char advanceToNextWhitespaceOr(char _c);
	char advanceToNextWhitespaceOr(const char* _list);
	char advanceToNextAlpha();
	char advanceToNextNum();
	char advanceToNextAlphaNum();
	char advanceToNextNonAlphaNum();
	char skipLine();
	char skipWhitespace();

	void reset() { m_pos = m_start; }
	
	/// \return First occurence of any characters from _list in the region
	///   between _beg and the current position, or 0 if none.
	char containsAny(const char* _beg, const char* _list);

	/// \return True if the region between _beg and the current position
	///  exactly matches _str.
	bool matches(const char *_beg, const char* _str);

	/// \return Occurences of '\n' up to and including _pos (or the current
	///   position if _pos is 0).
	uint getLineCount(const char* _pos = 0) const;

	/// \return Number of chars between the start of the string and the current
	///   position.
	uint getCharCount() const { return m_pos - m_start; }

	operator const char*()  { return m_pos; }

private:
	const char* m_start;
	const char* m_pos;

}; // class TextParser

} // namespace plr

#endif // plr_TextParser_h

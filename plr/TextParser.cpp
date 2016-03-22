////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/TextParser.h>

#include <cctype>

using namespace plr;

inline bool TextParser::isWhitespace() const
{
	return isspace(*m_pos) != 0;
}
inline bool TextParser::isAlpha() const
{
	return isalpha(*m_pos) != 0;
}
inline bool TextParser::isNum() const
{
	return isdigit(*m_pos) != 0;
}
inline bool TextParser::isAlphaNum() const
{
	return isalnum(*m_pos) != 0;
}

char TextParser::advanceToNext(char _c)
{
	while (!isNull() && *m_pos != _c) {
		++m_pos;
	}
	return *m_pos;
}

char TextParser::advanceToNext(const char* _list)
{
	while (!isNull()) {
		const char* c = _list;
		while (*c != 0) {
			if (*m_pos == *c) {
				return *c;
			}
			++c;
		}
		++m_pos;
	}
	return *m_pos;
}

char TextParser::advanceToNextWhitespace()
{
	while (!isNull() && !isWhitespace()) {
		++m_pos;
	}
	return *m_pos;
}

char TextParser::advanceToNextWhitespaceOr(char _c)
{
	while (!isNull() && !isWhitespace() && *m_pos != _c) {
		++m_pos;
	}
	return *m_pos;
}

char TextParser::advanceToNextWhitespaceOr(const char* _list)
{	
	while (!isNull() && !isWhitespace()) {
		const char* c = _list;
		while (*c != 0) {
			if (*m_pos == *c) {
				return *c;
			}
			++c;
		}
		++m_pos;
	}
	return *m_pos;
}

char TextParser::advanceToNextAlpha()
{
	while (!isNull() && !isAlpha()) {
		++m_pos;
	}
	return *m_pos;
}

char TextParser::advanceToNextNum()
{
	while (!isNull() && !isNum()) {
		++m_pos;
	}
	return *m_pos;
}

char TextParser::advanceToNextAlphaNum()
{
	while (!isNull() && !isAlphaNum()) {
		++m_pos;
	}
	return *m_pos;
}

char TextParser::advanceToNextNonAlphaNum()
{
	while (!isNull() && isAlphaNum()) {
		++m_pos;
	}
	return *m_pos;
}

char TextParser::skipLine() 
{
	advanceToNext('\n');
	return advance();
}

char TextParser::skipWhitespace()
{
	while (!isNull() && isWhitespace()) {
		++m_pos;
	}
	return *m_pos;
}

char TextParser::containsAny(const char* _beg, const char* _list)
{
	while (*_beg != 0 && _beg != m_pos) {
		const char* c = _list;
		while (*c != 0) {
			if (*_beg == *c) {
				return *c;
			}
			++c;
		}
		++_beg;
	}
	return (char)0;
}

uint TextParser::getLineCount(const char* _pos) const
{
	const char* c = _pos ? _pos : m_pos;
	uint ret = 0u;
	do {
		ret += *c == '\n' ? 1u : 0u;
		--c;
	} while (c != m_start);
	return ret;
}

#ifndef UTILITY_TOKEN_H
#define UTILITY_TOKEN_H

#include <string>
#include <cstdlib>

#include "utility/Typedefs.h"

#define ENABLE_LARGE_TOKENS 1

#if ENABLE_LARGE_TOKENS

//##############################################################################
static unsigned const TokenCharactersPerWord = 12;
static unsigned const TokenWordsPerToken     = 2;
static unsigned const TokenCharacters        =
  TokenCharactersPerWord * TokenWordsPerToken;

//##############################################################################
class Token
{
public:
  Token(void);
  Token(char const * str);
  Token(std::string const & str);

  bool operator ==(Token const & token) const;
  bool operator !=(Token const & token) const;
  bool operator <(Token const & token) const;
  bool operator >(Token const & token) const;
  bool operator <=(Token const & token) const;
  bool operator >=(Token const & token) const;

  std::string Str(void) const;

  Token & operator = (Token const & token) = default;

private:
  u64 data_[TokenWordsPerToken];
};

#else

//##############################################################################
typedef std::u64 Token;

//##############################################################################
static unsigned const TokenCharacters = 12;

//##############################################################################
std::string TokenToString(Token token);
Token StringToToken(std::string const & str);

#endif

#endif

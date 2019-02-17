#include "engine/utility/Debug.h"
#include "engine/utility/Token.h"

namespace
{
  enum TokenValue
  {
    TV_Space,
    TV_A,
    TV_B,
    TV_C,
    TV_D,
    TV_E,
    TV_F,
    TV_G,
    TV_H,
    TV_I,
    TV_J,
    TV_K,
    TV_L,
    TV_M,
    TV_N,
    TV_O,
    TV_P,
    TV_Q,
    TV_R,
    TV_S,
    TV_T,
    TV_U,
    TV_V,
    TV_W,
    TV_X,
    TV_Y,
    TV_Z,
    TV_0,
    TV_1,
    TV_2,
    TV_3,
    TV_4,
    TV_5,
    TV_6,
    TV_7,
    TV_8,
    TV_9,
    TV_Unknown,
    TV_Count
  };

  //############################################################################
  TokenValue TokenValueFromCharacter(char value)
  {
    if (value >= 'a' && value <= 'z')
      return TokenValue(TV_A + value - 'a');
    else if (value >= 'A' && value <= 'Z')
      return TokenValue(TV_A + value - 'A');
    else if (value >= '0' && value <= '9')
      return TokenValue(TV_0 + value - '0');
    else if (value == ' ' || value == '_' || value == '\0')
      return TV_Space;
    else
      return TV_Unknown;
  }

  //############################################################################
  char CharacterFromTokenValue(TokenValue value)
  {
    if (value >= TV_A && value <= TV_Z)
      return char('a' + value - TV_A);
    else if (value >= TV_0 && value <= TV_9)
      return char('0' + value - TV_0);
    else if (value == TV_Space)
      return '_';
    else
      return '.';
  }

  //############################################################################
  TokenValue ObtainNextTokenPart(u64 & data)
  {
    u64 value = data % TV_Count;
    data = data / TV_Count;

    return TokenValue(value);
  }
}

#if ENABLE_LARGE_TOKENS

//##############################################################################
Token::Token(void)
{
  for (unsigned i = 0; i < TokenWordsPerToken; ++i)
    data_[i] = 0;
}

//##############################################################################
Token::Token(char const * str) :
  Token(std::string(str))
{}

//##############################################################################
Token::Token(std::string const & str)
{
  ASSERT(str.size() <= TokenCharacters);

  for (unsigned i = 0; i < TokenWordsPerToken; ++i)
  {
    u64 word = 0;

    for (unsigned j = 0; j < TokenCharactersPerWord; ++j)
    {
      unsigned const index = i * TokenCharactersPerWord + j;

      word *= TV_Count;

      TokenValue val =
        index < str.size() ? TokenValueFromCharacter(str[index]) : TV_Space;

      word += val;
    }

    data_[i] = word;
  }
}

//##############################################################################
bool Token::operator ==(Token const & token) const
{
  for (unsigned i = 0; i < TokenWordsPerToken; ++i)
  {
    if (data_[i] != token.data_[i])
      return false;
  }

  return true;
}

//##############################################################################
bool Token::operator !=(Token const & token) const
{
  return !(operator ==(token));
}

//##############################################################################
bool Token::operator <(Token const & token) const
{
  for (unsigned i = 0; i < TokenWordsPerToken; ++i)
  {
    if (data_[i] < token.data_[i])
      return true;
    else if (data_[i] > token.data_[i])
      return false;
  }

  return false;
}

//##############################################################################
bool Token::operator >(Token const & token) const
{
  return token.operator <(*this);
}

//##############################################################################
bool Token::operator <=(Token const & token) const
{
  return !token.operator <(*this);
}

//##############################################################################
bool Token::operator >=(Token const & token) const
{
  return !operator <(token);
}

//##############################################################################
std::string Token::Str(void) const
{
  char characters[TokenCharacters + 1] = { 0 };

  for (unsigned i = TokenWordsPerToken - 1; i != unsigned(-1); --i)
  {
    u64 data = data_[i];

    for (unsigned j = TokenCharactersPerWord - 1; j != unsigned(-1); --j)
    {
      unsigned const index = i * TokenCharactersPerWord + j;

      TokenValue tokenPart = ObtainNextTokenPart(data);
      char nextVal = CharacterFromTokenValue(tokenPart);

      characters[index] = nextVal;
    }
  }

  unsigned lastCharacterIndex = TokenCharacters - 1;

  while (lastCharacterIndex != unsigned(-1))
  {
    char val = characters[lastCharacterIndex];

    if (val != '_')
      break;

    --lastCharacterIndex;
  }

  return std::string(characters, lastCharacterIndex + 1);
}

#else

//##############################################################################
std::string TokenToString(Token token)
{
  char characters[TokenCharacters + 1] = {0};

  std::u64 data = token;

  for (unsigned i = TokenCharacters - 1; i != unsigned(-1); --i)
  {
    TokenValue tokenPart = ObtainNextTokenPart(data);

    char nextVal = CharacterFromTokenValue(tokenPart);

    characters[i] = nextVal;
  }

  unsigned lastCharacterIndex = TokenCharacters - 1;

  while (lastCharacterIndex != unsigned(-1))
  {
    char val = characters[lastCharacterIndex];

    if (val != '_')
      break;

    --lastCharacterIndex;
  }

  return std::string(characters, lastCharacterIndex + 1);
}

//##############################################################################
Token StringToToken(std::string const & str)
{
  ASSERT(str.size() <= 12);

  Token token = 0;
  for (unsigned i = 0; i < TokenCharacters; ++i)
  {
    token *= TV_Count;

    TokenValue val = i < str.size() ? TokenValueFromCharacter(str[i]) : TV_Space;

    token += val;
  }

  return token;
}

#endif

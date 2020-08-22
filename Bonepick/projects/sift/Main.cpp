#include <memory>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_set>
#include <variant>

namespace {
  struct Token;
  struct TokenDefinition;
  struct TokenDefHash;

  enum class MatchStringType {
    Literal,
    Regex
  };

  struct TokenDefinition {
    TokenDefinition(void) = default;
    TokenDefinition(std::string const & definition, MatchStringType type) :
      definition(definition),
      type(type)
    {}

    bool operator ==(TokenDefinition const & tokenDef) const {
      return definition == tokenDef.definition && type == tokenDef.type;
    }

    std::string     definition;
    MatchStringType type       = MatchStringType::Literal;
  };

  struct TokenDefHash {
    size_t operator()(TokenDefinition const & tokenDef) const {
      return std::hash<std::string>()(tokenDef.definition) ^ std::hash<MatchStringType>()(tokenDef.type);
    }
  };

  using TokenSet = std::unordered_set<TokenDefinition, TokenDefHash>;

  struct Token {
    Token(void) = default;
    Token(std::string_view const & reference, TokenSet matchDefinitions) :
      reference(reference),
      matchDefinitions(matchDefinitions)
    {}

    std::string_view reference;
    TokenSet         matchDefinitions;
  };

  struct SyntaxDefinition;
  struct SyntaxNode;

  struct SyntaxRuleIdentifier;
  struct SyntaxRuleToken;
  struct SyntaxRuleOptional;
  struct SyntaxRuleRepetition;
  struct SyntaxRuleOr;
  struct SyntaxRuleSequence;

  using SyntaxRule = std::variant<
    SyntaxRuleIdentifier,
    SyntaxRuleToken,
    SyntaxRuleOptional,
    SyntaxRuleRepetition,
    SyntaxRuleOr,
    SyntaxRuleSequence
  >;

  using Syntax = std::vector<SyntaxDefinition>;

  struct SyntaxDefinition {
    SyntaxDefinition(void) = default;
    SyntaxDefinition(
      std::string const &            type,
      std::shared_ptr<SyntaxRule> && rule,
      unsigned                       minChildren,
      bool                           isOptional,
      bool                           storesValues
    ) :
      type(type),
      rule(std::move(rule)),
      minChildren(minChildren),
      isOptional(isOptional),
      storesValues(storesValues)
    {}

    std::string                 type;
    std::shared_ptr<SyntaxRule> rule;
    unsigned                    minChildren;
    bool                        isOptional;
    bool                        storesValues;
  };

  struct SyntaxRuleIdentifier {
    std::string type;
  };

  struct SyntaxRuleToken {
    TokenDefinition tokenDef;
  };

  struct SyntaxRuleOptional {
    std::shared_ptr<SyntaxRule> rule;
  };

  struct SyntaxRuleRepetition {
    std::shared_ptr<SyntaxRule> rule;
  };

  struct SyntaxRuleOr {
    std::vector<SyntaxRule> rules;
  };

  struct SyntaxRuleSequence {
    std::vector<SyntaxRule> rules;
  };

  struct SyntaxNode {
    SyntaxNode(void) = default;
    SyntaxNode(
      std::string             const & type,
      std::string_view        const & location,
      std::vector<SyntaxNode> const & children
    ) :
      type(type),
      location(location),
      children(children)
    {}

    std::string             type;
    std::string_view        location;
    std::vector<SyntaxNode> children;
  };

  std::vector<Token> GetTokensFromString(std::string_view const & input, TokenSet const & allTokens);
  Token GetNextToken(std::string_view & io_str, TokenSet const & allTokens);
  bool DoesTokenMatch(std::string_view const & str, TokenDefinition const & tokenDef, std::string_view & o_matchStr);
  bool IsSyntaxRuleOptional(SyntaxRule const & rule, Syntax const & syntax);
  bool DoesSyntaxRuleStoreValues(SyntaxRule const & rule, Syntax const & syntax);

  std::vector<Token> GetTokensFromString(std::string_view const & input, TokenSet const & allTokens) {
    std::vector<Token> result;
    std::string_view current = input;

    while (!current.empty()) {
      result.emplace_back(GetNextToken(current, allTokens));
    }

    return result;
  }

  Token GetNextToken(std::string_view & io_str, TokenSet const & allTokens) {
    Token result;

    size_t consumedChars = 0;
    for (TokenDefinition const & tokenDef : allTokens) {
      std::string_view matchStr;
      if (DoesTokenMatch(io_str, tokenDef, matchStr)) {
        if (matchStr.length() > consumedChars) {
          consumedChars = matchStr.length();

          result.matchDefinitions.clear();
          result.matchDefinitions.emplace(tokenDef);
        }
        else if (matchStr.length() == consumedChars) {
          result.matchDefinitions.emplace(tokenDef);
        }
      }
    }

    result.reference = io_str.substr(0, consumedChars);
    io_str.remove_prefix(consumedChars);

    if (consumedChars == 0) {
      io_str.remove_prefix(io_str.length());
    }

    return result;
  }

  bool DoesTokenMatch(std::string_view const & str, TokenDefinition const & tokenDef, std::string_view & o_matchStr) {
    if (tokenDef.type == MatchStringType::Literal) {
      std::string_view testStr = str.substr(0, tokenDef.definition.length());

      if (testStr == tokenDef.definition) {
        o_matchStr = testStr;
        return true;
      }
    }
    else if (tokenDef.type == MatchStringType::Regex) {
      std::regex expression(tokenDef.definition);

      std::cmatch match;
      if (std::regex_search(str.data(), match, expression, std::regex_constants::match_continuous)) {
        o_matchStr = str.substr(0, match.length(0));
        return true;
      }
    }
    return false;
  }

  bool IsSyntaxRuleOptional(SyntaxRule const & rule, Syntax const & syntax) {
    if (std::holds_alternative<SyntaxRuleIdentifier>(rule)) {
      auto const & identifierRule = std::get<SyntaxRuleIdentifier>(rule);
      for (SyntaxDefinition const & definition : syntax) {
        if (definition.type == identifierRule.type) {
          return definition.isOptional;
        }
      }
    }
    else if (std::holds_alternative<SyntaxRuleToken>(rule)) {
      auto const & tokenRule = std::get<SyntaxRuleToken>(rule);
      std::string_view matchStr;
      if (DoesTokenMatch("", tokenRule.tokenDef, matchStr)) {
        return true;
      }
    }
    else if (std::holds_alternative<SyntaxRuleOptional>(rule)) {
      return true;
    }
    else if (std::holds_alternative<SyntaxRuleRepetition>(rule)) {
      auto const & repetitionRule = std::get<SyntaxRuleRepetition>(rule);
      return IsSyntaxRuleOptional(*repetitionRule.rule, syntax);
    }
    else if (std::holds_alternative<SyntaxRuleOr>(rule)) {
      auto const & orRule = std::get<SyntaxRuleOr>(rule);
      for (SyntaxRule const & rule : orRule.rules) {
        if (IsSyntaxRuleOptional(rule, syntax)) {
          return true;
        }
      }
    }
    else if (std::holds_alternative<SyntaxRuleSequence>(rule)) {
      auto const & sequenceRule = std::get<SyntaxRuleSequence>(rule);
      for (SyntaxRule const & rule : sequenceRule.rules) {
        if (!IsSyntaxRuleOptional(rule, syntax)) {
          return false;
        }
      }
      return true;
    }

    return false;
  }

  bool DoesSyntaxRuleStoreValues(SyntaxRule const & rule, Syntax const & syntax) {
    if (std::holds_alternative<SyntaxRuleIdentifier>(rule)) {
      auto const & identifierRule = std::get<SyntaxRuleIdentifier>(rule);
      for (SyntaxDefinition const & definition : syntax) {
        if (definition.type == identifierRule.type) {
          return definition.storesValues;
        }
      }
    }
    else if (std::holds_alternative<SyntaxRuleToken>(rule)) {
      auto const & tokenRule = std::get<SyntaxRuleToken>(rule);
      return tokenRule.tokenDef.type == MatchStringType::Regex;
    }
    else if (std::holds_alternative<SyntaxRuleOptional>(rule)) {
      auto const & optionalRule = std::get<SyntaxRuleOptional>(rule);
      return DoesSyntaxRuleStoreValues(*optionalRule.rule, syntax);
    }
    else if (std::holds_alternative<SyntaxRuleRepetition>(rule)) {
      auto const & repetitionRule = std::get<SyntaxRuleRepetition>(rule);
      return DoesSyntaxRuleStoreValues(*repetitionRule.rule, syntax);
    }
    else if (std::holds_alternative<SyntaxRuleOr>(rule)) {
      auto const & orRule = std::get<SyntaxRuleOr>(rule);
      for (SyntaxRule const & rule : orRule.rules) {
        if (DoesSyntaxRuleStoreValues(rule, syntax)) {
          return true;
        }
      }
    }
    else if (std::holds_alternative<SyntaxRuleSequence>(rule)) {
      auto const & sequenceRule = std::get<SyntaxRuleSequence>(rule);
      for (SyntaxRule const & rule : sequenceRule.rules) {
        if (DoesSyntaxRuleStoreValues(rule, syntax)) {
          return true;
        }
      }
    }

    return false;
  }

  SyntaxNode GetSyntaxNodeFromToken(Token const & token) {
    SyntaxNode result;
    result.location = token.reference;
    return result;
  }

  std::vector<SyntaxNode> BuildInitialNodesFromTokens(std::vector<Token> const & tokens) {
    std::vector<SyntaxNode> result;
    result.reserve(tokens.size());

    for (Token const & token : tokens) {
      result.emplace_back(GetSyntaxNodeFromToken(token));
    }

    return result;
  }

  std::string_view GetSyntaxNodeLocationFromChildren(std::vector<SyntaxNode> const & nodes, int beginIndex, int childrenCount) {
    if (nodes.size() == 0) {
      return std::string_view();
    }
    else if (childrenCount + beginIndex == nodes.size()) {
      return std::string_view(nodes[beginIndex].location.data());
    }
    else {
      char const * locationBegin = nodes[beginIndex].location.data();
      char const * locationEnd   = nodes[beginIndex + childrenCount].location.data();
      return std::string_view(locationBegin, locationEnd - locationBegin);
    }
  }

  SyntaxDefinition const * GetSyntaxDef(
    Syntax const &      syntax,
    std::string const & type
  ) {
    SyntaxDefinition * result = nullptr;

    for (SyntaxDefinition const & definition : syntax) {
      if (definition.type == type) {
        return &definition;
      }
    }

    return nullptr;
  }

  bool TryReduceSyntaxTreeUsingRuleAtIndex(
    std::vector<SyntaxNode> const & nodes,
    SyntaxRule const &              rule,
    Syntax const &                  syntax,
    int                             nodeIndex,
    std::vector<SyntaxNode> &       o_children,
    int &                           o_consumedTokens
  ) {
    o_consumedTokens = 0;

    if (nodeIndex >= nodes.size()) {
      return false;
    }

    if (std::holds_alternative<SyntaxRuleIdentifier>(rule)) {
      auto const & identifierRule = std::get<SyntaxRuleIdentifier>(rule);

      SyntaxDefinition const * definition = GetSyntaxDef(syntax, identifierRule.type);
      if (definition == nullptr) {
        return false;
      }

      if (nodes[nodeIndex].type == definition->type) {
        if (definition->storesValues) {
          o_children.emplace_back(nodes[nodeIndex]);
        }

        o_consumedTokens = 1;
        return true;
      }

      return definition->isOptional;
    }
    else if (std::holds_alternative<SyntaxRuleToken>(rule)) {
      auto const & tokenRule = std::get<SyntaxRuleToken>(rule);

      std::string_view matchStr;
      if (nodes[nodeIndex].type.empty() && DoesTokenMatch(nodes[nodeIndex].location, tokenRule.tokenDef, matchStr)) {
        if (matchStr.length() == nodes[nodeIndex].location.length()) {
          if (tokenRule.tokenDef.type == MatchStringType::Regex) {
            o_children.emplace_back(nodes[nodeIndex]);
          }

          o_consumedTokens = 1;
          return true;
        }
      }

      return false;
    }
    else if (std::holds_alternative<SyntaxRuleOptional>(rule)) {
      auto const & optionalRule = std::get<SyntaxRuleOptional>(rule);

      TryReduceSyntaxTreeUsingRuleAtIndex(nodes, *optionalRule.rule, syntax, nodeIndex, o_children, o_consumedTokens);
      return true;
    }
    else if (std::holds_alternative<SyntaxRuleRepetition>(rule)) {
      auto const & repetitionRule = std::get<SyntaxRuleRepetition>(rule);

      int                     currentConsumedTokens;
      std::vector<SyntaxNode> currentChildren;

      if (TryReduceSyntaxTreeUsingRuleAtIndex(nodes, *repetitionRule.rule, syntax, nodeIndex, currentChildren, currentConsumedTokens)) {
        while (true) {
          o_consumedTokens += currentConsumedTokens;
          o_children.insert(o_children.end(), currentChildren.begin(), currentChildren.end());

          currentConsumedTokens = 0;
          currentChildren.clear();

          if (!TryReduceSyntaxTreeUsingRuleAtIndex(nodes, *repetitionRule.rule, syntax, nodeIndex + o_consumedTokens, currentChildren, currentConsumedTokens) || currentConsumedTokens == 0) {
            return true;
          }
        }
      }
      else {
        return false;
      }
    }
    else if (std::holds_alternative<SyntaxRuleOr>(rule)) {
      auto const & orRule = std::get<SyntaxRuleOr>(rule);

      for (SyntaxRule const & rule : orRule.rules) {
        int                     currentConsumedTokens;
        std::vector<SyntaxNode> currentChildren;
        if (TryReduceSyntaxTreeUsingRuleAtIndex(nodes, rule, syntax, nodeIndex, currentChildren, currentConsumedTokens)) {
          o_consumedTokens = currentConsumedTokens;
          o_children       = currentChildren;
          return true;
        }
      }

      return false;
    }
    else if (std::holds_alternative<SyntaxRuleSequence>(rule)) {
      auto const & sequenceRule = std::get<SyntaxRuleSequence>(rule);

      int                     storedConsumedTokens = 0;
      std::vector<SyntaxNode> storedChildren;

      for (SyntaxRule const & rule : sequenceRule.rules) {
        int                     currentConsumedTokens;
        std::vector<SyntaxNode> currentChildren;
        if (!TryReduceSyntaxTreeUsingRuleAtIndex(nodes, rule, syntax, nodeIndex + storedConsumedTokens, currentChildren, currentConsumedTokens)) {
          return false;
        }

        storedConsumedTokens += currentConsumedTokens;
        storedChildren.insert(storedChildren.end(), currentChildren.begin(), currentChildren.end());
      }

      o_consumedTokens = storedConsumedTokens;
      o_children       = storedChildren;
      return true;
    }

    return false;
  }

  bool TryReduceSyntaxTreeUsingDefAtIndex(
    std::vector<SyntaxNode> const & nodes,
    SyntaxDefinition const &        syntaxDef,
    Syntax const &                  syntax,
    int                             nodeIndex,
    std::vector<SyntaxNode> &       io_children,
    int &                           consumedTokens
  ) {
    if (nodeIndex < 0 || nodeIndex + syntaxDef.minChildren > nodes.size()) {
      return false;
    }

    return TryReduceSyntaxTreeUsingRuleAtIndex(nodes, *syntaxDef.rule, syntax, nodeIndex, io_children, consumedTokens) && consumedTokens != 0;
  }

  bool ReduceSyntaxTreeUsingDef(std::vector<SyntaxNode> & io_result, SyntaxDefinition const & syntaxDef, Syntax const & syntax) {
    for (int i = 0; i < std::min(io_result.size(), io_result.size() - syntaxDef.minChildren + 1); ++i) {
      std::vector<SyntaxNode> children;
      int                     consumedTokens;
      if (TryReduceSyntaxTreeUsingDefAtIndex(io_result, syntaxDef, syntax, i, children, consumedTokens)) {
        std::string_view location = GetSyntaxNodeLocationFromChildren(io_result, i, consumedTokens);
        SyntaxNode       newNode  = { syntaxDef.type, location, children };

        io_result.erase(io_result.begin() + i, io_result.begin() + i + consumedTokens);
        io_result.insert(io_result.begin() + i, newNode);

        return true;
      }
    }

    return false;
  }

  bool ReduceSyntaxTree(std::vector<SyntaxNode> & io_result, Syntax const & syntax) {
    for (SyntaxDefinition const & syntaxDef : syntax) {
      if (ReduceSyntaxTreeUsingDef(io_result, syntaxDef, syntax)) {
        return true;
      }
    }

    return false;
  }

  std::vector<SyntaxNode> BuildSyntaxTreeFromTokens(std::vector<Token> const & tokens, Syntax const & syntax) {
    std::vector<SyntaxNode> result = BuildInitialNodesFromTokens(tokens);
    while (ReduceSyntaxTree(result, syntax));

    return result;
  }
}

/*
  identifier        := whitespace "[a-zA-Z_][a-zA-Z_0-9]*"
  regex_pattern     := whitespace "\"([^\\]|(\\.))*\""
  string_literal    := whitespace "'([^\\]|(\\.))*'"
  whitespace        := [('\r' | '\n' | '\t' | ' ')...]
  definition_header := identifier whitespace ':='
  definition        := definition_header value
  optional          := whitespace '[' value whitespace ']'
  repetition        := value whitespace '...'
  group             := whitespace '(' value whitespace ')'
  or                := value (whitespace '|' value)...
  sequence          := value value...
  value             := identifier | regex_pattern | string_literal | optional | repetition | group | or | sequence
  language          := [definition...] whitespace
*/

int main(void) {
  TokenSet tokens = {
    TokenDefinition("[a-zA-Z]+", MatchStringType::Regex),
    TokenDefinition(" ", MatchStringType::Literal),
    TokenDefinition(".", MatchStringType::Literal),
    TokenDefinition("!", MatchStringType::Literal),
    TokenDefinition("?", MatchStringType::Literal),
  };

  /*
    word        := whitespace "[a-zA-Z]+"
    sentence    := word ... punctuation 
    punctuation := whitespace (('.' ...) | (('!' | '?') ...))
    whitespace  := [('\r' | '\n' | '\t' | ' ')...]
    statement   := sentence ...
  */
  
  auto sentenceTokens = GetTokensFromString("Let the seas part and the fish die. I do not want your failures!!!", tokens);

  Syntax syntax = {
    SyntaxDefinition("whitespace",
      std::make_shared<SyntaxRule>(
        SyntaxRuleOptional { std::make_shared<SyntaxRule>(
          SyntaxRuleRepetition { std::make_shared<SyntaxRule>(
            SyntaxRuleOr {{
              SyntaxRuleToken { TokenDefinition("\\r", MatchStringType::Literal) },
              SyntaxRuleToken { TokenDefinition("\\n", MatchStringType::Literal) },
              SyntaxRuleToken { TokenDefinition("\\t", MatchStringType::Literal) },
              SyntaxRuleToken { TokenDefinition(" ",   MatchStringType::Literal) },
            }}
          )}
        )}
      ),
      0, true, false
    ),
    SyntaxDefinition("word",
      std::make_shared<SyntaxRule>(
        SyntaxRuleSequence {{
          SyntaxRuleIdentifier { "whitespace" },
          SyntaxRuleToken { TokenDefinition("[a-zA-Z]+", MatchStringType::Regex) },
        }}
      ),
      1, false, true
    ),
    SyntaxDefinition("sentence",
      std::make_shared<SyntaxRule>(
        SyntaxRuleSequence {{
          SyntaxRuleRepetition { std::make_shared<SyntaxRule>(
            SyntaxRuleIdentifier { "word" }
          )},
          SyntaxRuleIdentifier { "punctuation" },
        }}
      ),
      2, false, true
    ),
    SyntaxDefinition("punctuation",
      std::make_shared<SyntaxRule>(
        SyntaxRuleSequence {{
          SyntaxRuleIdentifier { "whitespace" },
          SyntaxRuleOr {{
            SyntaxRuleRepetition { std::make_shared<SyntaxRule>(
              SyntaxRuleToken { TokenDefinition(".", MatchStringType::Literal) }
            )},
            SyntaxRuleRepetition { std::make_shared<SyntaxRule>(
              SyntaxRuleOr {{
                SyntaxRuleToken { TokenDefinition("!", MatchStringType::Literal) },
                SyntaxRuleToken { TokenDefinition("?", MatchStringType::Literal) }
              }}
            )},
          }}
        }}
      ),
      1, false, false
    ),
    SyntaxDefinition("statement",
      std::make_shared<SyntaxRule>(
        SyntaxRuleRepetition { std::make_shared<SyntaxRule>(
          SyntaxRuleIdentifier { "sentence" }
        )}
      ),
      1, false, true
    ),
  };

  std::vector<SyntaxNode> nodes = BuildSyntaxTreeFromTokens(sentenceTokens, syntax);

  return 0;
}

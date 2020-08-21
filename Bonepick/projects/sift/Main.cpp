#include <memory>
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

  struct SyntaxRule;
  struct SyntaxRuleIdentifier;
  struct SyntaxRuleToken;
  struct SyntaxRuleOptional;
  struct SyntaxRuleRepetition;
  struct SyntaxRuleOr;
  struct SyntaxRuleSequence;

  using Syntax = std::vector<SyntaxDefinition>;

  struct SyntaxDefinition {
    SyntaxDefinition(void) = default;
    SyntaxDefinition(
      std::string const &            type,
      std::unique_ptr<SyntaxRule> && rule,
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
    std::unique_ptr<SyntaxRule> rule;
    unsigned                    minChildren;
    bool                        isOptional;
    bool                        storesValues;
  };

  using SyntaxRule = std::variant<
    struct SyntaxRuleIdentifier,
    struct SyntaxRuleToken,
    struct SyntaxRuleOptional,
    struct SyntaxRuleRepetition,
    struct SyntaxRuleOr,
    struct SyntaxRuleSequence
  >;

  struct SyntaxRuleIdentifier {
    SyntaxRuleIdentifier(void) = default;
    SyntaxRuleIdentifier(std::string const & type) :
      type(type)
    {}

    std::string type;
  };

  struct SyntaxRuleToken {
    SyntaxRuleToken(void) = default;
    SyntaxRuleToken(TokenDefinition const & tokenDef) :
      tokenDef(tokenDef)
    {}

    TokenDefinition tokenDef;
  };

  struct SyntaxRuleOptional {
    SyntaxRuleOptional(void) = default;
    SyntaxRuleOptional(std::unique_ptr<SyntaxRule> && rule) :
      rule(std::move(rule))
    {}

    std::unique_ptr<SyntaxRule> rule;
  };

  struct SyntaxRuleRepetition {
    SyntaxRuleRepetition(void) = default;
    SyntaxRuleRepetition(std::unique_ptr<SyntaxRule> && rule) :
      rule(std::move(rule))
    {}

    std::unique_ptr<SyntaxRule> rule;
  };

  struct SyntaxRuleOr {
    SyntaxRuleOr(void) = default;
    SyntaxRuleOr(std::vector<SyntaxRule> const & rules) :
      rules(rules)
    {}

    std::vector<SyntaxRule> rules;
  };

  struct SyntaxRuleSequence {
    SyntaxRuleSequence(void) = default;
    SyntaxRuleSequence(std::vector<SyntaxRule> const & rules) :
      rules(rules)
    {}

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

  std::vector<SyntaxNode> BuildSyntaxTreeFromTokens(std::vector<Token> const & tokens, Syntax const & syntax) {
    std::vector<SyntaxNode> result = BuildInitialNodesFromTokens(tokens);
    while (ReduceSyntaxTree(result, syntax));

    return result;
  }

  std::vector<SyntaxNode> BuildInitialNodesFromTokens(std::vector<Token> const & tokens) {
    std::vector<SyntaxNode> result;
    result.reserve(tokens.size());

    for (Token const & token : tokens) {
      result.emplace_back(GetSyntaxNodeFromToken(token));
    }
  }

  SyntaxNode GetSyntaxNodeFromToken(Token const & token) {
    SyntaxNode result;
    result.location = token.reference;
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

  bool TryMatchSyntaxDef(
    std::vector<SyntaxNode> const & nodes,
    SyntaxDefinition const &        syntaxDef,
    Syntax const &                  syntax,
    int &                           io_currentIndex
  ) {
    // HERE
  }

  bool DoesSectionMatchSyntaxDef(
    std::vector<SyntaxNode> const & nodes,
    int                             beginIndex,
    SyntaxDefinition const &        syntaxDef,
    Syntax const &                  syntax,
    int &                           o_childrenCount
  ) {
    int currentIndex = beginIndex;
    if (TryMatchSyntaxDef(nodes, syntaxDef, syntax, currentIndex)) {
      o_childrenCount = currentIndex - beginIndex;
      return true;
    }

    return false;
  }

  bool ReduceSyntaxTreeUsingDef(std::vector<SyntaxNode> & io_result, SyntaxDefinition const & syntaxDef, Syntax const & syntax) {
    for (int i = 0; i < io_result.size() - syntaxDef.minChildren + 1; ++i) {
      int consumedTokens = 0;
      if (DoesSectionMatchSyntaxDef(io_result, i, syntaxDef, syntax, consumedTokens)) {
        std::string_view location = GetSyntaxNodeLocationFromChildren(io_result, i, consumedTokens);

        SyntaxNode newNode(syntaxDef.type, location, std::vector<SyntaxNode>(io_result.begin() + i, io_result.begin() + i + consumedTokens));

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
}

int main(void) {
}

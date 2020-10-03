#include <functional>
#include <memory>
#include <optional>
#include <regex>
#include <set>
#include <string>
#include <string_view>
#include <unordered_set>
#include <unordered_map>
#include <variant>
#include <iostream>

#include "utility/Debug.h"

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
  struct SyntaxRuleNot;
  struct SyntaxRuleAny;
  struct SyntaxRuleContext;

  using SyntaxRule = std::variant<
    SyntaxRuleIdentifier,
    SyntaxRuleToken,
    SyntaxRuleOptional,
    SyntaxRuleRepetition,
    SyntaxRuleOr,
    SyntaxRuleSequence,
    SyntaxRuleNot,
    SyntaxRuleAny,
    SyntaxRuleContext
  >;

  using Syntax = std::vector<SyntaxDefinition>;

  enum class StorageType {
    Branch,
    Absent,
    Leaf,
    PassThrough,
    Splitting,
    Context,
    Removed
  };

  struct SyntaxDefinition {
    SyntaxDefinition(void) = default;
    SyntaxDefinition(
      std::string const &                 type,
      std::shared_ptr<SyntaxRule> const & rule,
      unsigned                            minChildren,
      StorageType                         storage,
      std::string const &                 context
    ) :
      type(type),
      rule(rule),
      minChildren(minChildren),
      storage(storage),
      context(context)
    {}

    std::string                 type;
    std::shared_ptr<SyntaxRule> rule;
    unsigned                    minChildren;
    StorageType                 storage;
    std::string                 context;
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

  struct SyntaxRuleNot {
    std::shared_ptr<SyntaxRule> rule;
  };

  struct SyntaxRuleAny {
  };

  struct SyntaxRuleContext {
    std::string                 context;
    std::shared_ptr<SyntaxRule> rule;
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
  bool ReduceSyntaxTree(std::vector<SyntaxNode> & io_result, Syntax const & syntax, std::string const & currentContext);

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

      std::match_results<std::string_view::const_iterator> match;
      if (std::regex_search(str.begin(), str.end(), match, expression, std::regex_constants::match_continuous)) {
        o_matchStr = str.substr(0, match.length(0));
        return true;
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
      char const * locationEnd   = nodes[beginIndex + childrenCount - 1].location.data() + nodes[beginIndex + childrenCount - 1].location.length();
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
    int &                           o_consumedNodes
  ) {
    o_consumedNodes = 0;

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
        switch (definition->storage) {
          case StorageType::Branch:
          case StorageType::Leaf: {
            o_children.emplace_back(nodes[nodeIndex]);
            break;
          }
          case StorageType::Absent: {
            break;
          }
          case StorageType::Splitting: {
            if (nodes[nodeIndex].children.size() == 1) {
              o_children.insert(o_children.end(), nodes[nodeIndex].children.front());
            }
            else {
              o_children.emplace_back(nodes[nodeIndex]);
            }
            break;
          }
          case StorageType::PassThrough: {
            o_children.insert(o_children.end(), nodes[nodeIndex].children.begin(), nodes[nodeIndex].children.end());
            break;
          }
          case StorageType::Removed: {
            // error on this
            return false;
          }
        }

        o_consumedNodes = 1;
        return true;
      }

      return definition->minChildren == 0;
    }
    else if (std::holds_alternative<SyntaxRuleToken>(rule)) {
      auto const & tokenRule = std::get<SyntaxRuleToken>(rule);

      std::string_view matchStr;
      if (nodes[nodeIndex].type.empty() && DoesTokenMatch(nodes[nodeIndex].location, tokenRule.tokenDef, matchStr)) {
        if (matchStr.length() == nodes[nodeIndex].location.length()) {
          o_consumedNodes = 1;
          return true;
        }
      }

      return false;
    }
    else if (std::holds_alternative<SyntaxRuleOptional>(rule)) {
      auto const & optionalRule = std::get<SyntaxRuleOptional>(rule);

      TryReduceSyntaxTreeUsingRuleAtIndex(nodes, *optionalRule.rule, syntax, nodeIndex, o_children, o_consumedNodes);
      return true;
    }
    else if (std::holds_alternative<SyntaxRuleRepetition>(rule)) {
      auto const & repetitionRule = std::get<SyntaxRuleRepetition>(rule);

      int                     currentConsumedNodes;
      std::vector<SyntaxNode> currentChildren;

      if (TryReduceSyntaxTreeUsingRuleAtIndex(nodes, *repetitionRule.rule, syntax, nodeIndex, currentChildren, currentConsumedNodes)) {
        while (true) {
          o_consumedNodes += currentConsumedNodes;
          o_children.insert(o_children.end(), currentChildren.begin(), currentChildren.end());

          currentConsumedNodes = 0;
          currentChildren.clear();

          if (!TryReduceSyntaxTreeUsingRuleAtIndex(nodes, *repetitionRule.rule, syntax, nodeIndex + o_consumedNodes, currentChildren, currentConsumedNodes) || currentConsumedNodes == 0) {
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
        int                     currentConsumedNodes;
        std::vector<SyntaxNode> currentChildren;
        if (TryReduceSyntaxTreeUsingRuleAtIndex(nodes, rule, syntax, nodeIndex, currentChildren, currentConsumedNodes)) {
          o_consumedNodes = currentConsumedNodes;
          o_children      = currentChildren;
          return true;
        }
      }

      return false;
    }
    else if (std::holds_alternative<SyntaxRuleSequence>(rule)) {
      auto const & sequenceRule = std::get<SyntaxRuleSequence>(rule);

      int                     storedConsumedNodes = 0;
      std::vector<SyntaxNode> storedChildren;

      for (SyntaxRule const & rule : sequenceRule.rules) {
        int                     currentConsumedNodes;
        std::vector<SyntaxNode> currentChildren;
        if (!TryReduceSyntaxTreeUsingRuleAtIndex(nodes, rule, syntax, nodeIndex + storedConsumedNodes, currentChildren, currentConsumedNodes)) {
          return false;
        }

        storedConsumedNodes += currentConsumedNodes;
        storedChildren.insert(storedChildren.end(), currentChildren.begin(), currentChildren.end());
      }

      o_consumedNodes = storedConsumedNodes;
      o_children      = storedChildren;
      return true;
    }
    else if (std::holds_alternative<SyntaxRuleNot>(rule)) {
      auto const & notRule = std::get<SyntaxRuleNot>(rule);

      int                     currentConsumedNodes;
      std::vector<SyntaxNode> currentChildren;

      if (TryReduceSyntaxTreeUsingRuleAtIndex(nodes, *notRule.rule, syntax, nodeIndex, currentChildren, currentConsumedNodes) && currentConsumedNodes != 0) {
        return false;
      }
      else {
        o_children.emplace_back(nodes[nodeIndex]);
        o_consumedNodes = 1;
        return true;
      }
    }
    else if (std::holds_alternative<SyntaxRuleAny>(rule)) {
      o_children.emplace_back(nodes[nodeIndex]);
      o_consumedNodes = 1;
      return true;
    }
    else if (std::holds_alternative<SyntaxRuleContext>(rule)) {
      auto const & contextRule = std::get<SyntaxRuleContext>(rule);

      if (TryReduceSyntaxTreeUsingRuleAtIndex(nodes, *contextRule.rule, syntax, nodeIndex, o_children, o_consumedNodes)) {
        while (ReduceSyntaxTree(o_children, syntax, contextRule.context));

        return true;
      }
      return false;
    }

    return false;
  }

  bool TryReduceSyntaxTreeUsingDefAtIndex(
    std::vector<SyntaxNode> const & nodes,
    SyntaxDefinition const &        syntaxDef,
    Syntax const &                  syntax,
    int                             nodeIndex,
    std::vector<SyntaxNode> &       io_children,
    int &                           consumedNodes
  ) {
    if (nodeIndex < 0 || nodeIndex + syntaxDef.minChildren > nodes.size()) {
      return false;
    }

    return TryReduceSyntaxTreeUsingRuleAtIndex(nodes, *syntaxDef.rule, syntax, nodeIndex, io_children, consumedNodes) && consumedNodes != 0;
  }

  bool ReduceSyntaxTreeUsingDef(std::vector<SyntaxNode> & io_result, SyntaxDefinition const & syntaxDef, Syntax const & syntax) {
    for (int i = 0; i < std::min(io_result.size(), io_result.size() - syntaxDef.minChildren + 1); ++i) {
      std::vector<SyntaxNode> children;
      int                     consumedNodes;
      if (TryReduceSyntaxTreeUsingDefAtIndex(io_result, syntaxDef, syntax, i, children, consumedNodes)) {
        std::string_view location = GetSyntaxNodeLocationFromChildren(io_result, i, consumedNodes);

        io_result.erase(io_result.begin() + i, io_result.begin() + i + consumedNodes);

        if (syntaxDef.storage == StorageType::Leaf) {
          children.clear();
        }

        SyntaxNode newNode  = { syntaxDef.type, location, children };

        if (syntaxDef.storage != StorageType::Removed) {
          io_result.insert(io_result.begin() + i, newNode);
        }

        return true;
      }
    }

    return false;
  }

  bool ReduceSyntaxTree(std::vector<SyntaxNode> & io_result, Syntax const & syntax, std::string const & currentContext) {
    for (SyntaxDefinition const & syntaxDef : syntax) {
      if (syntaxDef.context != currentContext) {
        continue;
      }

      if (ReduceSyntaxTreeUsingDef(io_result, syntaxDef, syntax)) {
        return true;
      }
    }

    return false;
  }

  std::vector<SyntaxNode> BuildSyntaxTreeFromTokens(std::vector<Token> const & tokens, Syntax const & syntax) {
    std::vector<SyntaxNode> result = BuildInitialNodesFromTokens(tokens);
    while (ReduceSyntaxTree(result, syntax, ""));

    return result;
  }
}

//"[removed]     whitespace      := \"[\\r\\n\\t ]+\"; "
//"              definition      := ['[' <definition_type: .> ']'] <definition_name: .> ':=' <definition_value: !';'...> ';'; "
//"              context_header  := identifier ':'; "
//"              language        := (definition | context_header)...; "
//"[leaf]        identifier      := \"[a-zA-Z_][a-zA-Z_0-9]*\"; "
//
//"definition_value: "
//"[leaf]        identifier      := \"[a-zA-Z_][a-zA-Z_0-9]*\"; "
//"[leaf]        any             := '.'; "
//"[leaf]        regex_pattern   := \"\\\"([^\\\"\\\\]|(\\\\.))*\\\"\"; "
//"[leaf]        string_literal  := \"'([^'\\\\]|(\\\\.))*'\"; "
//"[passthrough] value           := identifier | any | regex_pattern | string_literal | group | optional | context | not | repetition | or | sequence; "
//"[passthrough] group           := '(' value ')'; "
//"              optional        := '[' value ']'; "
//"              context         := '<' <context_access: !('<' | '>')...> '>'; "
//"              not             := '!' value; "
//"              repetition      := value '...'; "
//"              or              := value ('|' value)...; "
//"              sequence        := value value...; "
//
//"definition_type: "
//"[leaf]        definition_type := \"removed|leaf|absent|passthrough|splitting|branch\"; "
//
//"definition_name: "
//"[leaf]        identifier      := \"[a-zA-Z_][a-zA-Z_0-9]*\"; "
//
//"context_access: "
//"              context         := identifier ':' <definition_value: . ...>; "
//"[leaf]        identifier      := \"[a-zA-Z_][a-zA-Z_0-9]*\"; "

auto identifierRule = std::make_shared<SyntaxRule>(
  SyntaxRuleToken { TokenDefinition("[a-zA-Z_][a-zA-Z_0-9]*", MatchStringType::Regex) }
);

Syntax languageSyntax = {
  SyntaxDefinition("whitespace",
    std::make_shared<SyntaxRule>(
      SyntaxRuleToken { TokenDefinition("[\\r\\n\\t ]+", MatchStringType::Regex) }
    ),
    1, StorageType::Removed, ""
  ),
  SyntaxDefinition("definition",
    std::make_shared<SyntaxRule>(
      SyntaxRuleSequence {{
        SyntaxRuleOptional { std::make_shared<SyntaxRule>(
          SyntaxRuleSequence {{
            SyntaxRuleToken   { TokenDefinition("[", MatchStringType::Literal) },
            SyntaxRuleContext { "definition_type", std::make_shared<SyntaxRule>(
              SyntaxRuleAny()
            )},
            SyntaxRuleToken   { TokenDefinition("]", MatchStringType::Literal) },
          }}
        )},
        SyntaxRuleContext { "definition_name", std::make_shared<SyntaxRule>(
          SyntaxRuleAny()
        )},
        SyntaxRuleToken { TokenDefinition(":=", MatchStringType::Literal) },
        SyntaxRuleContext { "definition_value", std::make_shared<SyntaxRule>(
          SyntaxRuleRepetition { std::make_shared<SyntaxRule>(
            SyntaxRuleNot { std::make_shared<SyntaxRule>(
              SyntaxRuleToken { TokenDefinition(";", MatchStringType::Literal) }
            )}
          )}
        )},
        SyntaxRuleToken { TokenDefinition(";", MatchStringType::Literal) },
      }}
    ),
    4, StorageType::Branch, ""
  ),
  SyntaxDefinition("context_header",
    std::make_shared<SyntaxRule>(
      SyntaxRuleSequence {{
        SyntaxRuleIdentifier { "identifier" },
        SyntaxRuleToken      { TokenDefinition(":", MatchStringType::Literal) },
      }}
    ),
    2, StorageType::Branch, ""
  ),
  SyntaxDefinition("identifier",
    identifierRule,
    1, StorageType::Leaf, ""
  ),
  SyntaxDefinition("language",
    std::make_shared<SyntaxRule>(
      SyntaxRuleRepetition { std::make_shared<SyntaxRule>(
        SyntaxRuleOr {{
          SyntaxRuleIdentifier { "definition" },
          SyntaxRuleIdentifier { "context_header" },
        }}
      )}
    ),
    1, StorageType::Branch, ""
  ),

  SyntaxDefinition("identifier",
    identifierRule,
    1, StorageType::Leaf, "definition_value"
  ),
  SyntaxDefinition("any",
    std::make_shared<SyntaxRule>(
      SyntaxRuleToken { TokenDefinition(".", MatchStringType::Literal) }
    ),
    1, StorageType::Leaf, "definition_value"
  ),
  SyntaxDefinition("regex_pattern",
    std::make_shared<SyntaxRule>(
      SyntaxRuleToken { TokenDefinition("\"([^\"\\\\]|(\\\\.))*\"", MatchStringType::Regex) }
    ),
    1, StorageType::Leaf, "definition_value"
  ),
  SyntaxDefinition("string_literal",
    std::make_shared<SyntaxRule>(
      SyntaxRuleToken { TokenDefinition("'([^'\\\\]|(\\\\.))*'", MatchStringType::Regex) }
    ),
    1, StorageType::Leaf, "definition_value"
  ),
  SyntaxDefinition("value",
    std::make_shared<SyntaxRule>(
      SyntaxRuleOr {{
        SyntaxRuleIdentifier { "identifier" },
        SyntaxRuleIdentifier { "any" },
        SyntaxRuleIdentifier { "regex_pattern" },
        SyntaxRuleIdentifier { "string_literal" },
        SyntaxRuleIdentifier { "group" },
        SyntaxRuleIdentifier { "optional" },
        SyntaxRuleIdentifier { "context" },
        SyntaxRuleIdentifier { "not" },
        SyntaxRuleIdentifier { "repetition" },
        SyntaxRuleIdentifier { "or" },
        SyntaxRuleIdentifier { "sequence" },
      }}
    ),
    1, StorageType::PassThrough, "definition_value"
  ),
  SyntaxDefinition("group",
    std::make_shared<SyntaxRule>(
      SyntaxRuleSequence {{
        SyntaxRuleToken      { TokenDefinition("(", MatchStringType::Literal) },
        SyntaxRuleIdentifier { "value" },
        SyntaxRuleToken      { TokenDefinition(")", MatchStringType::Literal) },
      }}
    ),
    3, StorageType::PassThrough, "definition_value"
  ),
  SyntaxDefinition("optional",
    std::make_shared<SyntaxRule>(
      SyntaxRuleSequence {{
        SyntaxRuleToken      { TokenDefinition("[", MatchStringType::Literal) },
        SyntaxRuleIdentifier { "value" },
        SyntaxRuleToken      { TokenDefinition("]", MatchStringType::Literal) },
      }}
    ),
    3, StorageType::Branch, "definition_value"
  ),
  SyntaxDefinition("context",
    std::make_shared<SyntaxRule>(
      SyntaxRuleSequence {{
        SyntaxRuleToken   { TokenDefinition("<", MatchStringType::Literal) },
        SyntaxRuleContext { "context_access", std::make_shared<SyntaxRule>(
          SyntaxRuleRepetition { std::make_shared<SyntaxRule>(
            SyntaxRuleNot { std::make_shared<SyntaxRule>(
              SyntaxRuleOr {{
                SyntaxRuleToken { TokenDefinition("<", MatchStringType::Literal) },
                SyntaxRuleToken { TokenDefinition(">", MatchStringType::Literal) },
              }}
            )}
          )}
        )},
        SyntaxRuleToken   { TokenDefinition(">", MatchStringType::Literal) },
      }}
    ),
    3, StorageType::Branch, "definition_value"
  ),
  SyntaxDefinition("not",
    std::make_shared<SyntaxRule>(
      SyntaxRuleSequence {{
        SyntaxRuleToken      { TokenDefinition("!", MatchStringType::Literal) },
        SyntaxRuleIdentifier { "value" },
      }}
    ),
    2, StorageType::Branch, "definition_value"
  ),
  SyntaxDefinition("repetition",
    std::make_shared<SyntaxRule>(
      SyntaxRuleSequence {{
        SyntaxRuleIdentifier { "value" },
        SyntaxRuleToken      { TokenDefinition("...", MatchStringType::Literal) },
      }}
    ),
    2, StorageType::Branch, "definition_value"
  ),
  SyntaxDefinition("or",
    std::make_shared<SyntaxRule>(
      SyntaxRuleSequence {{
        SyntaxRuleIdentifier { "value" },
        SyntaxRuleRepetition { std::make_shared<SyntaxRule>(
          SyntaxRuleSequence {{
            SyntaxRuleToken      { TokenDefinition("|", MatchStringType::Literal) },
            SyntaxRuleIdentifier { "value" },
          }}
        )}
      }}
    ),
    3, StorageType::Branch, "definition_value"
  ),
  SyntaxDefinition("sequence",
    std::make_shared<SyntaxRule>(
      SyntaxRuleSequence {{
        SyntaxRuleIdentifier { "value" },
        SyntaxRuleRepetition { std::make_shared<SyntaxRule>(
          SyntaxRuleIdentifier { "value" }
        )}
      }}
    ),
    2, StorageType::Branch, "definition_value"
  ),

  SyntaxDefinition("definition_type",
    std::make_shared<SyntaxRule>(
      SyntaxRuleOptional { std::make_shared<SyntaxRule>(
        SyntaxRuleToken { TokenDefinition("removed|leaf|absent|passthrough|splitting|branch|context", MatchStringType::Regex) }
      )}
    ),
    1, StorageType::Leaf, "definition_type"
  ),

  SyntaxDefinition("identifier",
    identifierRule,
    1, StorageType::Leaf, "definition_name"
  ),
      
  SyntaxDefinition("context",
    std::make_shared<SyntaxRule>(
      SyntaxRuleSequence {{
        SyntaxRuleIdentifier { "identifier" },
        SyntaxRuleToken      { TokenDefinition(":", MatchStringType::Literal) },
        SyntaxRuleContext    { "definition_value", std::make_shared<SyntaxRule>(
          SyntaxRuleRepetition { std::make_shared<SyntaxRule>(
            SyntaxRuleAny()
          )}
        )},
      }}
    ),
    3, StorageType::Leaf, "context_access"
  ),
  SyntaxDefinition("identifier",
    identifierRule,
    1, StorageType::Leaf, "context_access"
  ),
};

StorageType GetStorageTypeFromString(std::string_view const & str) {
  if (str == "removed") {
    return StorageType::Removed;
  }
  else if (str == "leaf") {
    return StorageType::Leaf;
  }
  else if (str == "absent") {
    return StorageType::Absent;
  }
  else if (str == "passthrough") {
    return StorageType::PassThrough;
  }
  else if (str == "splitting") {
    return StorageType::Splitting;
  }
  else if (str == "branch") {
    return StorageType::Branch;
  }

  ERROR(str + " is not a storage type.");

  return StorageType::Branch;
}

SyntaxRule BuildSyntaxRuleFromSyntaxNode(SyntaxNode const & syntaxNode) {
  SyntaxRule result;

  if (syntaxNode.type == "regex_pattern") {
    std::string_view regexPattern = syntaxNode.location.substr(1, syntaxNode.location.length() - 2);
    result.emplace<SyntaxRuleToken>(SyntaxRuleToken { TokenDefinition(std::string(regexPattern), MatchStringType::Regex) });
  }
  else if (syntaxNode.type == "string_literal") {
    std::string_view stringLiteral = syntaxNode.location.substr(1, syntaxNode.location.length() - 2);
    result.emplace<SyntaxRuleToken>(SyntaxRuleToken { TokenDefinition(std::string(stringLiteral), MatchStringType::Literal) });
  }
  else if (syntaxNode.type == "identifier") {
    result.emplace<SyntaxRuleIdentifier>(SyntaxRuleIdentifier { std::string(syntaxNode.location) });
  }
  else if (syntaxNode.type == "optional") {
    ASSERT(syntaxNode.children.size() == 1);
    result.emplace<SyntaxRuleOptional>(SyntaxRuleOptional { std::make_shared<SyntaxRule>(BuildSyntaxRuleFromSyntaxNode(syntaxNode.children[0])) });
  }
  else if (syntaxNode.type == "repetition") {
    ASSERT(syntaxNode.children.size() == 1);
    result.emplace<SyntaxRuleRepetition>(SyntaxRuleRepetition { std::make_shared<SyntaxRule>(BuildSyntaxRuleFromSyntaxNode(syntaxNode.children[0])) });
  }
  else if (syntaxNode.type == "or") {
    ASSERT(!syntaxNode.children.empty());
    SyntaxRuleOr orNode;

    for (auto & rule : syntaxNode.children) {
      orNode.rules.emplace_back(BuildSyntaxRuleFromSyntaxNode(rule));
    }
    result.emplace<SyntaxRuleOr>(orNode);
  }
  else if (syntaxNode.type == "sequence") {
    ASSERT(!syntaxNode.children.empty());
    SyntaxRuleSequence sequenceNode;

    for (auto & rule : syntaxNode.children) {
      sequenceNode.rules.emplace_back(BuildSyntaxRuleFromSyntaxNode(rule));
    }
    result.emplace<SyntaxRuleSequence>(sequenceNode);
  }
  else if (syntaxNode.type == "not") {
    ASSERT(!syntaxNode.children.empty());
    result.emplace<SyntaxRuleNot>(SyntaxRuleNot { std::make_shared<SyntaxRule>(BuildSyntaxRuleFromSyntaxNode(syntaxNode.children[0])) });
  }
  else if (syntaxNode.type == "any") {
    ASSERT(syntaxNode.children.empty());
    result.emplace<SyntaxRuleAny>();
  }
  else if (syntaxNode.type == "context") {
    ASSERT(syntaxNode.children.size() == 2);
    ASSERT(syntaxNode.children[0].type == "identifier");
    result.emplace<SyntaxRuleContext>(SyntaxRuleContext { std::string(syntaxNode.children[0].location), std::make_shared<SyntaxRule>(BuildSyntaxRuleFromSyntaxNode(syntaxNode.children[1])) });
  }
  else {
    ERROR(std::string("Unexpected type '") + syntaxNode.type + "'");
  }

  return result;
}

SyntaxDefinition BuildSyntaxDefinitonFromDefinitionNode(SyntaxNode const & definition, std::string const & context) {
  SyntaxDefinition result;
  result.context = context;

  int currentChild = 0;

  if (definition.children[currentChild].type == "definition_type") {
    SyntaxNode const & definitionType = definition.children[currentChild];

    result.storage = GetStorageTypeFromString(definitionType.location);
    ++currentChild;
  }
  else {
    result.storage = StorageType::Branch;
  }

  ASSERT(currentChild < definition.children.size());
  ASSERT(definition.children[currentChild].type == "identifier");
  result.type = definition.children[currentChild].location;
  ++currentChild;

  ASSERT(currentChild < definition.children.size());
  result.rule = std::make_shared<SyntaxRule>(BuildSyntaxRuleFromSyntaxNode(definition.children[currentChild]));

  return result;
}

struct DefinitionDependencyInfo {
  SyntaxDefinition *              definition;
  std::unordered_set<std::string> dependants;
  int                             cycles;
};

std::unordered_set<std::string> GetDependenciesFromRule(SyntaxRule const & rule) {
  if (std::holds_alternative<SyntaxRuleIdentifier>(rule)) {
    auto const & identifierRule = std::get<SyntaxRuleIdentifier>(rule);
    return { identifierRule.type };
  }
  else if (std::holds_alternative<SyntaxRuleRepetition>(rule)) {
    auto const & repetitionRule = std::get<SyntaxRuleRepetition>(rule);
    return GetDependenciesFromRule(*repetitionRule.rule);
  }
  else if (std::holds_alternative<SyntaxRuleOr>(rule)) {
    auto const & orRule = std::get<SyntaxRuleOr>(rule);

    std::unordered_set<std::string> result;
    for (SyntaxRule const & rule : orRule.rules) {
      std::unordered_set<std::string> currentDependencies = GetDependenciesFromRule(rule);
      result.insert(currentDependencies.begin(), currentDependencies.end());
    }

    return result;
  }
  else if (std::holds_alternative<SyntaxRuleSequence>(rule)) {
    auto const & sequenceRule = std::get<SyntaxRuleSequence>(rule);

    std::unordered_set<std::string> result;
    for (SyntaxRule const & rule : sequenceRule.rules) {
      std::unordered_set<std::string> currentDependencies = GetDependenciesFromRule(rule);
      result.insert(currentDependencies.begin(), currentDependencies.end());
    }

    return result;
  }
  else if (std::holds_alternative<SyntaxRuleNot>(rule)) {
    auto const & notRule = std::get<SyntaxRuleNot>(rule);
    return GetDependenciesFromRule(*notRule.rule);
  }

  return {};
}

int EvaluateMinChildrenForRule(SyntaxRule const & rule, Syntax const & syntax) {
  if (std::holds_alternative<SyntaxRuleIdentifier>(rule)) {
    auto const & identifierRule = std::get<SyntaxRuleIdentifier>(rule);
    for (SyntaxDefinition const & definition : syntax) {
      if (definition.type == identifierRule.type) {
        return definition.minChildren == 0 ? 0 : 1;
      }
    }

    // error here
    return 0;
  }
  else if (std::holds_alternative<SyntaxRuleToken>(rule)) {
    return 1;
  }
  else if (std::holds_alternative<SyntaxRuleOptional>(rule)) {
    return 0;
  }
  else if (std::holds_alternative<SyntaxRuleRepetition>(rule)) {
    auto const & repetitionRule = std::get<SyntaxRuleRepetition>(rule);
    return EvaluateMinChildrenForRule(*repetitionRule.rule, syntax);
  }
  else if (std::holds_alternative<SyntaxRuleOr>(rule)) {
    auto const & orRule = std::get<SyntaxRuleOr>(rule);

    int minVal = std::numeric_limits<int>::max();
    for (SyntaxRule const & rule : orRule.rules) {
      int currentVal = EvaluateMinChildrenForRule(rule, syntax);
      if (currentVal < minVal) {
        minVal = currentVal;
        if (minVal == 0) {
          break;
        }
      }
    }
    return minVal;
  }
  else if (std::holds_alternative<SyntaxRuleSequence>(rule)) {
    auto const & sequenceRule = std::get<SyntaxRuleSequence>(rule);

    int total = 0;
    for (SyntaxRule const & rule : sequenceRule.rules) {
      total += EvaluateMinChildrenForRule(rule, syntax);
    }
    return total;
  }
  else if (std::holds_alternative<SyntaxRuleNot>(rule)) {
    return 1;
  }
  else if (std::holds_alternative<SyntaxRuleAny>(rule)) {
    return 1;
  }
  else if (std::holds_alternative<SyntaxRuleContext>(rule)) {
    auto const & contextRule = std::get<SyntaxRuleContext>(rule);
    return EvaluateMinChildrenForRule(*contextRule.rule, syntax);
  }

  return 0;
}

void EvaluateMinChildren(Syntax & io_syntax) {
  for (auto & definition : io_syntax) {
    definition.minChildren = 1;
  }

  std::vector<DefinitionDependencyInfo> dependancyInfo(io_syntax.size());
  std::unordered_map<std::string, int>  definitionIndices;

  for (int i = 0; i < io_syntax.size(); ++i) {
    definitionIndices[io_syntax[i].type] = i;
    dependancyInfo[i].cycles             = 0;
    dependancyInfo[i].definition         = &io_syntax[i];
  }

  for (auto & info : dependancyInfo) {
    std::unordered_set<std::string> dependencies = GetDependenciesFromRule(*info.definition->rule);

    for (auto dependency : dependencies) {
      dependancyInfo[definitionIndices[dependency]].dependants.emplace(info.definition->type);
    }
  }

  std::set<int, std::function<bool(int, int)>> evaluationQueue(
    [&](int lhs, int rhs) -> bool {
      DefinitionDependencyInfo const & lInfo = dependancyInfo[lhs];
      DefinitionDependencyInfo const & rInfo = dependancyInfo[rhs];

      if (lInfo.cycles == rInfo.cycles) {
        return lhs < rhs;
      }
      else {
        return lInfo.cycles < rInfo.cycles;
      }
    }
  );

  for (int i = 0; i < io_syntax.size(); ++i) {
    evaluationQueue.emplace(i);
  }

  while (!evaluationQueue.empty()) {
    int currentIndex = *evaluationQueue.begin();
    evaluationQueue.erase(evaluationQueue.begin());

    DefinitionDependencyInfo & info = dependancyInfo[currentIndex];

    ++info.cycles;

    // select arbitrary max depth
    if (info.cycles > 100) {
      //error message here
    }

    int prevMinChildren = info.definition->minChildren;
    info.definition->minChildren = EvaluateMinChildrenForRule(*info.definition->rule, io_syntax);

    if (prevMinChildren == info.definition->minChildren) {
      continue;
    }

    for (std::string const & dependant : info.dependants) {
      int nextIndex = definitionIndices[dependant];
      evaluationQueue.emplace(nextIndex);
    }
  }
}

Syntax BuildSyntaxFromSyntaxTree(std::vector<SyntaxNode> const & languageNodes) {
  Syntax result;

  if (languageNodes.size() != 1) {
    //todo: error message
    return result;
  }

  SyntaxNode const & language = languageNodes[0];

  if (language.type != "language") {
    // todo: error messaging
    return result;
  }

  std::string currentContext = "";

  for (SyntaxNode const & node : language.children) {
    if (node.type == "context") {
      ASSERT(node.children.size() == 1);
      ASSERT(node.children[0].type == "identifier");

      currentContext = node.children[0].location;
    }
    else if (node.type == "definition") {
      result.emplace_back(BuildSyntaxDefinitonFromDefinitionNode(node, currentContext));
    }
    else {
      // todo: error messaging
      continue;
    }
  }

  EvaluateMinChildren(result);

  return result;
}

void AddTokensFromSyntaxNode(SyntaxNode const & node, TokenSet & io_tokens) {
  if (node.type == "string_literal") {
    io_tokens.emplace(std::string(node.location.substr(1, node.location.size() - 2)), MatchStringType::Literal);
  }
  else if (node.type == "regex_pattern") {
    io_tokens.emplace(std::string(node.location.substr(1, node.location.size() - 2)), MatchStringType::Regex);
  }
  else {
    for (SyntaxNode const & childNode : node.children) {
      AddTokensFromSyntaxNode(childNode, io_tokens);
    }
  }
}

TokenSet BuildTokenSetFromSyntaxTree(std::vector<SyntaxNode> const & languageNodes) {
  TokenSet result;

  if (languageNodes.size() != 1) {
    //todo: error message
    return result;
  }

  SyntaxNode const & language = languageNodes[0];

  if (language.type != "language") {
    // todo: error messaging
    return result;
  }

  for (SyntaxNode const & definition : language.children) {
    if (definition.type != "definition") {
      // todo: error messaging
      continue;
    }

    AddTokensFromSyntaxNode(definition.children.back(), result);
  }

  return result;
}

int main(void) {
  TokenSet tokens = {
    TokenDefinition("[\\r\\n\\t ]+", MatchStringType::Regex),
    TokenDefinition("[a-zA-Z_][a-zA-Z_0-9]*", MatchStringType::Regex),
    TokenDefinition("\"([^\"\\\\]|(\\\\.))*\"", MatchStringType::Regex),
    TokenDefinition("'([^'\\\\]|(\\\\.))*'", MatchStringType::Regex),
    TokenDefinition("removed|leaf|absent|passthrough|splitting|branch", MatchStringType::Regex),
    TokenDefinition("...", MatchStringType::Literal),
    TokenDefinition("(", MatchStringType::Literal),
    TokenDefinition(")", MatchStringType::Literal),
    TokenDefinition("[", MatchStringType::Literal),
    TokenDefinition("]", MatchStringType::Literal),
    TokenDefinition("<", MatchStringType::Literal),
    TokenDefinition(">", MatchStringType::Literal),
    TokenDefinition(":=", MatchStringType::Literal),
    TokenDefinition(".", MatchStringType::Literal),
    TokenDefinition("|", MatchStringType::Literal),
    TokenDefinition(";", MatchStringType::Literal),
    TokenDefinition(":", MatchStringType::Literal),
    TokenDefinition("!", MatchStringType::Literal),
  };

  std::string languageDef =
    "[removed]     whitespace      := \"[\\r\\n\\t ]+\"; "
    "              definition      := ['[' <definition_type: .> ']'] <definition_name: .> ':=' <definition_value: !';'...> ';'; "
    "              context_header  := identifier ':'; "
    "[leaf]        identifier      := \"[a-zA-Z_][a-zA-Z_0-9]*\"; "
    "              language        := (definition | context_header)...; "
    
    "definition_value: "
    "[leaf]        identifier      := \"[a-zA-Z_][a-zA-Z_0-9]*\"; "
    "[leaf]        any             := '.'; "
    "[leaf]        regex_pattern   := \"\\\"([^\\\"\\\\]|(\\\\.))*\\\"\"; "
    "[leaf]        string_literal  := \"'([^'\\\\]|(\\\\.))*'\"; "
    "[passthrough] value           := identifier | any | regex_pattern | string_literal | group | optional | context | not | repetition | or | sequence; "
    "[passthrough] group           := '(' value ')'; "
    "              optional        := '[' value ']'; "
    "              context         := '<' <context_access: !('<' | '>')...> '>'; "
    "              not             := '!' value; "
    "              repetition      := value '...'; "
    "              or              := value ('|' value)...; "
    "              sequence        := value value...; "
    
    "definition_type: "
    "[leaf]        definition_type := \"removed|leaf|absent|passthrough|splitting|branch\"; "
    
    "definition_name: "
    "[leaf]        identifier      := \"[a-zA-Z_][a-zA-Z_0-9]*\"; "
    
    "context_access: "
    "              context         := identifier ':' <definition_value: . ...>; "
    "[leaf]        identifier      := \"[a-zA-Z_][a-zA-Z_0-9]*\"; "
  ;

  
  std::string siftLanguage =
    "[removed] whitespace                 := \"[\\r\\n\\t ]+\"; "
    "[removed] block_comment              := \"/\\*(.|[\\r\\n])\\**/\"; "
    "[removed] line_comment               := \"//.*\"; "
    "[leaf]    integer                    := \"[0-9]+\"; "
    "[leaf]    float                      := \"([.][0-9]+)|([0-9]+[.][0-9]*)\"; "
    "[leaf]    string                     := \"\\\"([^\\\"\\\\]|(\\\\.))*\\\"\"; "
    "[leaf]    module                     := \"`([^'\\\\]|(\\\\.))*`\"; "
    "[leaf]    collection                 := '~~'; "
    "[leaf]    identifier                 := \"[a-zA-Z_][a-zA-Z_0-9]*\"; "
    "          struct_content             := [(member_definition | inheritance_tag | collection_push)...]; "
    "          struct                     := '{' struct_content '}'; "
    "          function                   := '(' parameter_list ')' '->' value; "
    "          array                      := '[' [value...] ']'; "
    "          inheritance_tag            := '[' value ']'; "
    "          language                   := struct_content; "
    "          function_call              := value '(' [value [(',' value)...]] [','] ')'; "
    "          member_access              := value '.' identifier; "
    "          member_definition_header   := [member_definition_modifier] identifier '='; "
    "          member_definition          := member_definition_header value ';'; "
    "          parameter_list             := '(' [parameter [(',' parameter)...]] [','] ')'; "
    "          parameter                  := [value...] identifier; "
    "          member_definition_modifier := '#' | '!'; "
  ;

  /*

    A[apple]
    B[apple]

  */


  auto syntax       = languageSyntax;
  auto syntaxTokens = tokens;

  std::vector<SyntaxNode> nodes;
  for (int i = 0; i < 30; ++i) {
    auto inputTokens = GetTokensFromString(languageDef, syntaxTokens);

    nodes = BuildSyntaxTreeFromTokens(inputTokens, syntax);
    syntax = BuildSyntaxFromSyntaxTree(nodes);
    syntaxTokens = BuildTokenSetFromSyntaxTree(nodes);
  }

  return 0;
}

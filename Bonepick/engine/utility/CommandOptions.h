#ifndef ENGINE_UTILITY_COMMANDOPTIONS_H
#define ENGINE_UTILITY_COMMANDOPTIONS_H

#include <string>
#include <variant>
#include <vector>

#include "engine/utility/Typedefs.h"

//##############################################################################
enum CommandOptionType;
struct CommandOptionInfo;
struct CommandOptionOutput;

//##############################################################################
enum CommandOptionType
{
  COT_Flag,
  COT_String,
  COT_Int,
  COT_Float,
  COT_Count
};

//##############################################################################
struct CommandOptionInfo
{
  unsigned option;
  CommandOptionType type;
  char shortName;
  std::string longName;
  std::string description;
};

//##############################################################################
struct CommandOptionOutput
{
  unsigned option;
  std::variant<std::string, int, float> value;
};

//##############################################################################
std::vector<CommandOptionOutput> GetCommandOptions(
  std::vector<CommandOptionInfo> const & options, int argc, char const ** args,
  int * currentArg);

//##############################################################################
void PrintCommandOptions(std::vector<CommandOptionInfo> const & options);

#endif

#include "engine/Utility/CommandOptions.h"

#include <iostream>

namespace
{
  //############################################################################
  CommandOptionOutput GetCommandOptionOutput(CommandOptionInfo const & option,
    char const * param, bool * advanceArg)
  {
    CommandOptionOutput optionResult;
    optionResult.option = option.option;
    *advanceArg = false;

    switch (option.type)
    {
    case COT_Flag:
      break;
    case COT_String:
      optionResult.value = std::string(param);
      *advanceArg = true;
      break;
    case COT_Int:
      optionResult.value = std::atoi(param);
      *advanceArg = true;
      break;
    case COT_Float:
      optionResult.value = float(std::atof(param));
      *advanceArg = true;
      break;
    default:
      break;
    }

    return optionResult;
  }
}

//##############################################################################
std::vector<CommandOptionOutput> GetCommandOptions(
  std::vector<CommandOptionInfo> const & options, int argc, char const ** args,
  int * currentArg)
{
  std::vector<CommandOptionOutput> result;

  while (*currentArg < argc)
  {
    if (args[*currentArg][0] == '-')
    {
      bool usesLongName = (args[*currentArg][1] == '-');

      for (unsigned i = 0; i < options.size(); ++i)
      {
        if (usesLongName ?
          std::strcmp(args[*currentArg] + 2,
            options[i].longName.c_str()) == 0 :
          args[*currentArg][1] == options[i].shortName)
        {
          bool advanceArg;

          result.emplace_back(GetCommandOptionOutput(options[i],
            args[*currentArg + 1], &advanceArg));

          if (advanceArg)
            ++*currentArg;
        }
      }

      ++*currentArg;
    }
    else
      break;
  }

  return result;
}

//##############################################################################
void PrintCommandOptions(std::vector<CommandOptionInfo> const & options)
{
  unsigned longNameLength = 0;

  for (CommandOptionInfo const & option : options)
  {
    if (option.longName.length() > longNameLength)
      longNameLength = unsigned(option.longName.length());
  }

  for (CommandOptionInfo const & option : options)
  {
    std::cout << "  -" << option.shortName << ", --" <<
      option.longName << " ";

    for (unsigned i = unsigned(option.longName.length());
      i < longNameLength; ++i)
    {
      std::cout << " ";
    }

    std::cout << option.description << std::endl;
  }
}

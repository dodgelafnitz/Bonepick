#include <iostream>
#include <vector>

namespace Hubal {
  enum class Resource {
    Red,
    Green,
    Blue,
    Count,
  };

  namespace Init {
    Resource GetNextRestartVal(std::vector<Resource> const & previousCard, int currentIndex) {
      if (currentIndex == 0)
        return Resource::Count;

      Resource const nextExpectedRestartValue = Resource(int(previousCard[currentIndex - 1]) + 1);

      if (nextExpectedRestartValue == Resource::Count)
        return GetNextRestartVal(previousCard, currentIndex - 1);

      return nextExpectedRestartValue;
    }

    void ProcessNextResourceCardOption(std::vector<Resource> & cardOptionInOut) {
      for (int i = cardOptionInOut.size() - 1; i >= 0; --i) {
        Resource const nextRestartVal = GetNextRestartVal(cardOptionInOut, i);
        Resource nextVal = Resource(int(cardOptionInOut[i]) + 1);

        bool shouldEnd = true;
        if (nextVal == Resource::Count) {
          nextVal = nextRestartVal;
          shouldEnd = false;
        }

        cardOptionInOut[i] = nextVal;

        if (shouldEnd)
          return;
      }
    }

    std::vector<std::vector<Resource>> GetResourceOptionsForResourceCount(int resourceCount) {
      std::vector<std::vector<Resource>> result;

      std::vector<Resource> currentResourceCard(resourceCount, Resource(0));

      while (currentResourceCard[0] != Resource::Count) {
        result.emplace_back(currentResourceCard);
        ProcessNextResourceCardOption(currentResourceCard);
      }

      return result;
    }

    std::vector<std::vector<std::vector<Resource>>> GetResourceOptions(int maxResourcesInACard) {
      std::vector<std::vector<std::vector<Resource>>> result;
      result.reserve(maxResourcesInACard);

      for (int i = 0; i < maxResourcesInACard; ++i) {
        result.emplace_back(GetResourceOptionsForResourceCount(i + 1));
      }

      return result;
    }
  }

  const std::vector<std::vector<std::vector<Resource>>> resourceOptions = Init::GetResourceOptions(3);

  char const * GetResourceName(Resource resource) {
    switch(resource) {
    case Resource::Red:
      return "Red";
    case Resource::Green:
      return "Green";
    case Resource::Blue:
      return "Blue";
    case Resource::Count:
      return "Count";
    default:
      return "Invalid";
    }
  }
}

int main(void) {

  for (int i = 0; i < Hubal::resourceOptions.size(); ++i) {
    auto && cardList = Hubal::resourceOptions[i];

    std::cout << cardList.size() << " of size " << (i + 1) << std::endl;

    for (auto && card : cardList) {
      std::cout << "{ ";

      for (auto && resource : card) {
        std::cout << Hubal::GetResourceName(resource) << " ";
      }

      std::cout << "}" << std::endl;
    }

    std::cout << std::endl;
  }

  std::cin.get();
  return 0;
}

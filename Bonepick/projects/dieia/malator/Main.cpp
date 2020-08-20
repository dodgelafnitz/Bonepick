#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace Malator {
  enum RollType {
    Invalid,

    High4Next21,
    High4Next31,

    High5Next21,

    High5Next31,
    High5Next32,

    High5Next41,
    High5Next42,

    High6Next21,

    High6Next31,
    High6Next32,

    High6Next41,
    High6Next42,
    High6Next43,

    High6Next51,
    High6Next52,
    High6Next53,

    Pair1Next2,
    Pair1Next3,
    Pair1Next4,
    Pair1Next5,
    Pair1Next6,

    Pair2Next1,

    Pair2Next3,
    Pair2Next4,
    Pair2Next5,
    Pair2Next6,

    Pair3Next1,
    Pair3Next2,

    Pair3Next4,
    Pair3Next5,
    Pair3Next6,

    Pair4Next1,
    Pair4Next2,
    Pair4Next3,

    Pair4Next5,
    Pair4Next6,

    Pair5Next1,
    Pair5Next2,
    Pair5Next3,
    Pair5Next4,

    Pair5Next6,

    Pair6Next1,
    Pair6Next2,
    Pair6Next3,
    Pair6Next4,
    Pair6Next5,

    Run3,
    Run4,
    Run5,
    Run6,

    Triple1,
    Triple2,
    Triple3,
    Triple4,
    Triple5,
    Triple6,

    Count
  };

  bool IsANothing(RollType result) {
    return result >= High4Next21 && result < Pair1Next2;
  }

  bool IsAPair(RollType result) {
    return result >= Pair1Next2 && result < Run3;
  }

  bool IsARun(RollType result) {
    return result >= Run3 && result < Triple1;
  }

  bool IsATriple(RollType result) {
    return result >= Triple1 && result < Count;
  }

  char const * s_rollTypeNames[Count] = {
    "Invalid",
    "High4Next21",
    "High4Next31",
    "High5Next21",
    "High5Next31",
    "High5Next32",
    "High5Next41",
    "High5Next42",
    "High6Next21",
    "High6Next31",
    "High6Next32",
    "High6Next41",
    "High6Next42",
    "High6Next43",
    "High6Next51",
    "High6Next52",
    "High6Next53",
    "Pair1Next2",
    "Pair1Next3",
    "Pair1Next4",
    "Pair1Next5",
    "Pair1Next6",
    "Pair2Next1",
    "Pair2Next3",
    "Pair2Next4",
    "Pair2Next5",
    "Pair2Next6",
    "Pair3Next1",
    "Pair3Next2",
    "Pair3Next4",
    "Pair3Next5",
    "Pair3Next6",
    "Pair4Next1",
    "Pair4Next2",
    "Pair4Next3",
    "Pair4Next5",
    "Pair4Next6",
    "Pair5Next1",
    "Pair5Next2",
    "Pair5Next3",
    "Pair5Next4",
    "Pair5Next6",
    "Pair6Next1",
    "Pair6Next2",
    "Pair6Next3",
    "Pair6Next4",
    "Pair6Next5",
    "Run3",
    "Run4",
    "Run5",
    "Run6",
    "Triple1",
    "Triple2",
    "Triple3",
    "Triple4",
    "Triple5",
    "Triple6",
  };

  class Roll {
  public:
    Roll(void) = default;
    Roll(unsigned char die0, unsigned char die1, unsigned char die2) {
      m_dice[0] = die0;
      m_dice[1] = die1;
      m_dice[2] = die2;

      std::sort(m_dice, m_dice + 3, std::greater<unsigned char>());

      m_result = CalculateResult();
    }

    unsigned char const & operator [](int index) const {
      return m_dice[index % 3];
    }

    RollType GetRollType(void) const {
      return m_result;
    }

  private:
    RollType CalculateResult(void) {
      if (m_dice[0] == m_dice[1] && m_dice[0] == m_dice[2]) {
        return static_cast<RollType>(Triple1 + m_dice[0] - 1);
      }
      else if (m_dice[0] == m_dice[1] + 1 && m_dice[1] == m_dice[2] + 1) {
        return static_cast<RollType>(Run3 + m_dice[0] - 3);
      }
      else if (m_dice[0] != m_dice[1] && m_dice[1] != m_dice[2]) {
        switch (m_dice[0]) {
        case 4: {
            return static_cast<RollType>(High4Next21 + m_dice[1] - 2);
          }
        case 5: {
            switch (m_dice[1]) {
            case 2: return High5Next21;
            case 3: return static_cast<RollType>(High5Next31 + m_dice[2] - 1);
            case 4: return static_cast<RollType>(High5Next41 + m_dice[2] - 1);
            }
            return Invalid;
          }
        case 6: {
            switch (m_dice[1]) {
            case 2: return High6Next21;
            case 3: return static_cast<RollType>(High6Next31 + m_dice[2] - 1);
            case 4: return static_cast<RollType>(High6Next41 + m_dice[2] - 1);
            case 5: return static_cast<RollType>(High6Next51 + m_dice[2] - 1);
            }
            return Invalid;
          }
        }
      }
      else if (m_dice[0] == m_dice[1]) {
        switch (m_dice[0]) {
        case 2: return Pair2Next1;
        case 3: return static_cast<RollType>(Pair3Next1 + m_dice[2] - 1);
        case 4: return static_cast<RollType>(Pair4Next1 + m_dice[2] - 1);
        case 5: return static_cast<RollType>(Pair5Next1 + m_dice[2] - 1);
        case 6: return static_cast<RollType>(Pair6Next1 + m_dice[2] - 1);
        }
      }
      else if (m_dice[1] == m_dice[2]) {
        switch (m_dice[1]) {
        case 1: return static_cast<RollType>(Pair1Next2 + m_dice[0] - 2);
        case 2: return static_cast<RollType>(Pair2Next3 + m_dice[0] - 3);
        case 3: return static_cast<RollType>(Pair3Next4 + m_dice[0] - 4);
        case 4: return static_cast<RollType>(Pair4Next5 + m_dice[0] - 5);
        case 5: return Pair5Next6;
        }
      }
      return Invalid;
    }

    unsigned char m_dice[3];
    RollType m_result;
  };

  int const PossibleRolls = 6 * 6 * 6;

  Roll s_Rolls[PossibleRolls];
  void GenerateResults(void) {
    int currentResult = 0;

    for (int i = 1; i <= 6; ++i) {
      for (int j = 1; j <= 6; ++j) {
        for (int k = 1; k <= 6; ++k) {
          s_Rolls[currentResult] = Roll(i, j, k);

          ++currentResult;
        }
      }
    }
  }

  using DieValueModifier = std::function<unsigned char(unsigned char)>;

  struct DieSelection {
    bool          selectOther;
    unsigned char value;

    void SetValue(Roll & io_roll, Roll & io_other, DieValueModifier const & func) const {
      Roll & selectedRoll = selectOther ? io_other : io_roll;
      int index = -1;

      for (int i = 0; i < 3; ++i) {
        if (selectedRoll[i] == value) {
          index = i;
          break;
        }
      }

      if (index == -1) {
        return;
      }

      unsigned char newValue = func(value);

      selectedRoll = Roll(
        index == 0 ? newValue : selectedRoll[0],
        index == 1 ? newValue : selectedRoll[1],
        index == 2 ? newValue : selectedRoll[2]
      );
    }
  };

  using RollCondition   = std::function<bool(Roll const &, Roll const &)>;
  using DieFilter       = std::function<bool(Roll const &, Roll const &, DieSelection const &)>;
  using SelectionFilter = std::function<bool(Roll const &, Roll const &, std::vector<DieSelection> const &)>;
  using DieModifier     = std::function<void(Roll &, Roll &, std::vector<DieSelection> const &)>;

  struct DieEffect {
    std::vector<DieFilter> dieFilters;
    SelectionFilter        selectionFilter;
    DieModifier            mod;
  };

  struct Modifier {
    std::string            name;
    RollCondition          condition;
    std::vector<DieEffect> effects;
  };

  bool CanApplyMod(
    Roll const &                                   roll,
    Roll const &                                   other,
    Modifier const &                               mod,
    std::vector<std::vector<DieSelection>> const & selections
  ) {
    if (!mod.condition(roll, other)) {
      return false;
    }

    if (selections.size() != mod.effects.size()) {
      return false;
    }

    for (int i = 0; i < selections.size(); ++i) {
      std::vector<DieSelection> const & selectionGroup = selections[i];
      DieEffect const &                 effect = mod.effects[i];

      if (selectionGroup.size() != effect.dieFilters.size()) {
        return false;
      }

      for (int j = 0; j < selectionGroup.size(); ++j) {
        DieSelection const & selection = selectionGroup[j];
        DieFilter const &    filter = effect.dieFilters[j];

        if (!filter(roll, other, selection)) {
          return false;
        }
      }

      if (!effect.selectionFilter(roll, other, selectionGroup)) {
        return false;
      }
    }

    return true;
  }

  struct ModificationResult {
    Roll roll;
    Roll other;
    bool modified;
  };

  ModificationResult TryApplyMod(
    Roll const &                                   roll,
    Roll const &                                   other,
    Modifier const &                               mod,
    std::vector<std::vector<DieSelection>> const & selections
  ) {
    ModificationResult result = { roll, other, false };

    if (!CanApplyMod(roll, other, mod, selections)) {
      return result;
    }

    result.modified = true;

    for (int i = 0; i < mod.effects.size(); ++i) {
      std::vector<DieSelection> const & selectionGroup = selections[i];
      DieEffect const &                 effect         = mod.effects[i];

      effect.mod(result.roll, result.other, selectionGroup);
    }

    return result;
  }

  struct GetBestModChoiceRecursiveInput {
    Roll const &                             roll;
    Roll const &                             other;
    Modifier const &                         mod;
    std::vector<std::vector<DieSelection>> & i_currentSelection;
    ModificationResult &                     io_result;
    int &                                    io_bestDiff;
  };

  bool GetBestModChoiceRecursiveGetNextSelectionIsSelectionGroupFull(
    Modifier const &                               mod,
    std::vector<std::vector<DieSelection>> const & io_currentSelection,
    int                                            selectionGroup
  ) {
    if (selectionGroup < 0) {
      return true;
    }


    //todo: handle being out of slots too...
    //todo: asserts for this
    return io_currentSelection[selectionGroup].size() == mod.effects[selectionGroup].dieFilters.size();
  }

  void GetBestModChoiceRecursiveGetNextSelection(
    Modifier const &                         mod,
    std::vector<std::vector<DieSelection>> & io_currentSelection,
    int &                                    selectionGroup,
    int &                                    selectionIndex,
    DieSelection *&                          nextSelection
  ) {
    while (GetBestModChoiceRecursiveGetNextSelectionIsSelectionGroupFull(mod, io_currentSelection, io_currentSelection.size() - 1)) {
      if (io_currentSelection.size() == mod.effects.size()) {
        selectionGroup = -1;
        selectionIndex = -1;
        nextSelection  = nullptr;

        return;
      }
      io_currentSelection.emplace_back();
    }

    io_currentSelection.back().emplace_back();

    selectionGroup = io_currentSelection.size() - 1;
    selectionIndex = io_currentSelection.back().size() - 1;
    nextSelection = &io_currentSelection.back().back();
  }

  void GetBestModChoiceRecursiveRemoveSelection(
    std::vector<std::vector<DieSelection>> & io_currentSelection,
    int                                      selectionGroup,
    int                                      selectionIndex
  ) {
    // todo: assert here: io_currentSelection.size() == selectionGroup + 1
    // todo: assert here: io_currentSelection[selectionGroup].size() == selectionIndex + 1

    if (selectionIndex == 0) {
      if (selectionGroup == 0) {
        io_currentSelection.clear();
      }
      else {
        io_currentSelection.pop_back();
      }
    }
    else {
      io_currentSelection[selectionGroup].pop_back();
    }
  }

  void GetBestModChoiceRecursiveBuildNext(
    GetBestModChoiceRecursiveInput & io_workingData,
    int                              selectionGroup,
    int                              selectionIndex,
    DieSelection *                   nextSelection
  ) {
    // todo: get some asserts for this

    DieFilter const & dieFilter = io_workingData.mod.effects[selectionGroup].dieFilters[selectionIndex];

    for (bool selectOther : { false, true }) {
      nextSelection->selectOther = selectOther;

      for (int i = 0; i < 3; ++i) {
        nextSelection->value = (selectOther ? io_workingData.other : io_workingData.roll)[i];

        if (dieFilter(io_workingData.roll, io_workingData.other, *nextSelection)) {
          GetBestModChoiceRecursive(io_workingData);
        }
      }
    }
  }

  void GetBestModChoiceRecursiveEvaluateSelection(GetBestModChoiceRecursiveInput & io_workingData) {
    ModificationResult result = TryApplyMod(io_workingData.roll, io_workingData.other, io_workingData.mod, io_workingData.i_currentSelection);

    //todo: assert on this
    if (!result.modified) {
      return;
    }

    int diff = result.roll.GetRollType() - result.other.GetRollType();

    if (diff > io_workingData.io_bestDiff) {
      io_workingData.io_bestDiff = diff;
      io_workingData.io_result   = result;
    }
  }

  void GetBestModChoiceRecursive(GetBestModChoiceRecursiveInput & io_workingData) {
    int            selectionGroup;
    int            selectionIndex;
    DieSelection * nextSelection;
    GetBestModChoiceRecursiveGetNextSelection(
      io_workingData.mod,
      io_workingData.i_currentSelection,
      selectionGroup,
      selectionIndex,
      nextSelection
    );

    if (nextSelection) {
      GetBestModChoiceRecursiveBuildNext(
        io_workingData,
        selectionGroup,
        selectionIndex,
        nextSelection
      );

      GetBestModChoiceRecursiveRemoveSelection(
        io_workingData.i_currentSelection,
        selectionGroup,
        selectionIndex
      );
    }
    else {
      GetBestModChoiceRecursiveEvaluateSelection(io_workingData);
    }
  }

  ModificationResult GetBestMod(Roll const & roll, Roll const & other, Modifier const & mod) {
    ModificationResult result = { roll, other, false };
    int                bestDiff = roll.GetRollType() - other.GetRollType();

    if (!mod.condition(roll, other)) {
      return result;
    }

    GetBestModChoiceRecursiveInput workingData = { roll, other, mod, {}, result, bestDiff };
    GetBestModChoiceRecursive(workingData);

    return result;
  }

  enum RollDescriptors {
    None        = 0x0,

    Own1        = 0x1 << 0,
    Own2        = 0x1 << 1,
    Own3        = 0x1 << 2,
    Own4        = 0x1 << 3,
    Own5        = 0x1 << 4,
    Own6        = 0x1 << 5,
    
    Other1      = 0x1 << 6,
    Other2      = 0x1 << 7,
    Other3      = 0x1 << 8,
    Other4      = 0x1 << 9,
    Other5      = 0x1 << 10,
    Other6      = 0x1 << 11,

    OwnUnder2   = Own1,
    OwnUnder3   = OwnUnder2 | Own2,
    OwnUnder4   = OwnUnder3 | Own3,
    OwnUnder5   = OwnUnder4 | Own4,
    OwnUnder6   = OwnUnder5 | Own5,

    OwnOver5    = Own6,
    OwnOver4    = OwnOver5 | Own5,
    OwnOver3    = OwnOver4 | Own4,
    OwnOver2    = OwnOver3 | Own3,
    OwnOver1    = OwnOver2 | Own2,

    OwnNot1     = OwnOver1,
    OwnNot2     = OwnUnder2 | OwnOver2,
    OwnNot3     = OwnUnder3 | OwnOver3,
    OwnNot4     = OwnUnder3 | OwnOver4,
    OwnNot5     = OwnUnder5 | OwnOver5,
    OwnNot6     = OwnUnder6,

    OtherUnder2 = Other1,
    OtherUnder3 = OtherUnder2 | Other2,
    OtherUnder4 = OtherUnder3 | Other3,
    OtherUnder5 = OtherUnder4 | Other4,
    OtherUnder6 = OtherUnder5 | Other5,

    OtherOver5  = Other6,
    OtherOver4  = OtherOver5 | Other5,
    OtherOver3  = OtherOver4 | Other4,
    OtherOver2  = OtherOver3 | Other3,
    OtherOver1  = OtherOver2 | Other2,

    OtherNot1   = OtherOver1,
    OtherNot2   = OtherUnder2 | OtherOver2,
    OtherNot3   = OtherUnder3 | OtherOver3,
    OtherNot4   = OtherUnder3 | OtherOver4,
    OtherNot5   = OtherUnder5 | OtherOver5,
    OtherNot6   = OtherUnder6,

    Under2  = OwnUnder2 | OtherUnder2,
    Under3  = OwnUnder3 | OtherUnder3,
    Under4  = OwnUnder4 | OtherUnder4,
    Under5  = OwnUnder5 | OtherUnder5,
    Under6  = OwnUnder6 | OtherUnder6,

    Over5  = OwnOver5 | OtherOver5,
    Over4  = OwnOver4 | OtherOver4,
    Over3  = OwnOver3 | OtherOver3,
    Over2  = OwnOver2 | OtherOver2,
    Over1  = OwnOver1 | OtherOver1,

    Not1  = OwnNot1 | OtherNot1,
    Not2  = OwnNot2 | OtherNot2,
    Not3  = OwnNot3 | OtherNot3,
    Not4  = OwnNot4 | OtherNot4,
    Not5  = OwnNot5 | OtherNot5,
    Not6  = OwnNot6 | OtherNot6,
    
    OwnOdd      = Own1 | Own3 | Own5,
    OwnEven     = Own2 | Own4 | Own6,
    Own         = OwnOdd | OwnEven,
    
    OtherOdd    = Other1 | Other3 | Other5,
    OtherEven   = Other2 | Other4 | Other6,
    Other       = OtherOdd | OtherEven,

    All         = Own | Other,
  };

  struct NamedRollCondition {
    std::string   descriptionPart;
    RollCondition func;
    int           possibleRolls;
    int           requiredRolls;
  };

  struct NamedDieFilter {
    std::string descriptionPart;
    DieFilter   func;
    int         possibleRolls;
    int         requiredRolls;
    bool        generalOption;
  };

  struct NamedSelectionFilter {
    std::string     descriptionPart;
    SelectionFilter func;
    int             possibleRolls;
    int             requiredRolls;
  };

  struct NamedDieModifier {
    std::string descriptionPrefix;
    std::string descriptionPostfix;
    DieModifier func;
    int         possibleRolls;
    bool        requiresMultiplePossibleTargets;
  };

  bool AnyDieIs(Roll const & roll, int value) {
    return roll[0] == value || roll[1] == value || roll[2] == value;
  }

  bool AnyDieSatisfies(Roll const & roll, std::function<bool(int)> const & func) {
    for (int i = 0; i < 3; ++i) {
      if (func(roll[i])) {
        return true;
      }
    }
    return false;
  }

  bool AllDiceSatisfy(Roll const & roll, std::function<bool(int)> const & func) {
    for (int i = 0; i < 3; ++i) {
      if (!func(roll[i])) {
        return false;
      }
    }
    return true;
  }

  bool IsEven(unsigned char value) {
    return value % 2 == 0;
  }

  bool IsOdd(unsigned char value) {
    return value % 2 == 1;
  }

  NamedRollCondition s_namedRollConditions[] = {
    { "", [](Roll const &, Roll const &){ return true; }, All, None },


    //NamedRollCondition("If You Have a Nothing, ",         [](Roll const & roll) -> bool { return IsANothing(roll.GetRollType()); },  All    | Relative,   None | None),
    //NamedRollCondition("If You Have a Pair, ",            [](Roll const & roll) -> bool { return IsAPair(roll.GetRollType()); },     All    | Low | High, None | None),
    //NamedRollCondition("If You Have a Run, ",             [](Roll const & roll) -> bool { return IsARun(roll.GetRollType()); },      All    | Relative,   None | None),
    //NamedRollCondition("If You Have a Triple, ",          [](Roll const & roll) -> bool { return IsATriple(roll.GetRollType()); },   All    | Low | High, None | None),
    //NamedRollCondition("If You Don't Have a Nothing, ",   [](Roll const & roll) -> bool { return !IsANothing(roll.GetRollType()); }, All    | Relative,   None | None),
    //NamedRollCondition("If You Don't Have a Pair, ",      [](Roll const & roll) -> bool { return !IsAPair(roll.GetRollType()); },    All    | Relative,   None | None),
    //NamedRollCondition("If You Don't Have a Run, ",       [](Roll const & roll) -> bool { return !IsARun(roll.GetRollType()); },     All    | Relative,   None | None),
    //NamedRollCondition("If You Don't Have a Triple, ",    [](Roll const & roll) -> bool { return !IsATriple(roll.GetRollType()); },  All    | Relative,   None | None),
    //NamedRollCondition("If You Have a High Value of 2, ", [](Roll const & roll) -> bool { return roll[0] == 2; },                    Under3 | Low | High, _2   | High),
    //NamedRollCondition("If You Have a High Value of 3, ", [](Roll const & roll) -> bool { return roll[0] == 3; },                    Under4 | Relative,   _3   | High),
    //NamedRollCondition("If You Have a High Value of 4, ", [](Roll const & roll) -> bool { return roll[0] == 4; },                    Under5 | Relative,   _4   | High),
    //NamedRollCondition("If You Have a High Value of 5, ", [](Roll const & roll) -> bool { return roll[0] == 5; },                    Under6 | Relative,   _5   | High),
    //NamedRollCondition("If You Have a Low Value of 2, ",  [](Roll const & roll) -> bool { return roll[2] == 2; },                    Over1  | Relative,   _2   | Low),
    //NamedRollCondition("If You Have a Low Value of 3, ",  [](Roll const & roll) -> bool { return roll[2] == 3; },                    Over2  | Relative,   _3   | Low),
    //NamedRollCondition("If You Have a Low Value of 4, ",  [](Roll const & roll) -> bool { return roll[2] == 4; },                    Over3  | Relative,   _4   | Low),
    //NamedRollCondition("If You Have a Low Value of 5, ",  [](Roll const & roll) -> bool { return roll[2] == 5; },                    Over4  | Low | High, _5   | Low),
    //NamedRollCondition("If You Have a 1, ",               [](Roll const & roll) -> bool { return AnyDieIs(roll, 1); },               All    | Relative,   _1   | Low),
    //NamedRollCondition("If You Have a 2, ",               [](Roll const & roll) -> bool { return AnyDieIs(roll, 2); },               All    | Relative,   _2   | None),
    //NamedRollCondition("If You Have a 3, ",               [](Roll const & roll) -> bool { return AnyDieIs(roll, 3); },               All    | Relative,   _3   | None),
    //NamedRollCondition("If You Have a 4, ",               [](Roll const & roll) -> bool { return AnyDieIs(roll, 4); },               All    | Relative,   _4   | None),
    //NamedRollCondition("If You Have a 5, ",               [](Roll const & roll) -> bool { return AnyDieIs(roll, 5); },               All    | Relative,   _5   | None),
    //NamedRollCondition("If You Have a 6, ",               [](Roll const & roll) -> bool { return AnyDieIs(roll, 6); },               All    | Relative,   _6   | High),
    //NamedRollCondition("If You Don't Have a 1, ",         [](Roll const & roll) -> bool { return !AnyDieIs(roll, 1); },              Not1   | Relative,   None | None),
    //NamedRollCondition("If You Don't Have a 2, ",         [](Roll const & roll) -> bool { return !AnyDieIs(roll, 2); },              Not2   | Relative,   None | None),
    //NamedRollCondition("If You Don't Have a 3, ",         [](Roll const & roll) -> bool { return !AnyDieIs(roll, 3); },              Not3   | Relative,   None | None),
    //NamedRollCondition("If You Don't Have a 4, ",         [](Roll const & roll) -> bool { return !AnyDieIs(roll, 4); },              Not4   | Relative,   None | None),
    //NamedRollCondition("If You Don't Have a 5, ",         [](Roll const & roll) -> bool { return !AnyDieIs(roll, 5); },              Not5   | Relative,   None | None),
    //NamedRollCondition("If You Don't Have a 6, ",         [](Roll const & roll) -> bool { return !AnyDieIs(roll, 6); },              Not6   | Relative,   None | None),
    //NamedRollCondition("If All Values are Under 3, ",     [](Roll const & roll) -> bool { return roll[0] < 3; },                     Under3 | Low | High, None | None),
    //NamedRollCondition("If All Values are Under 4, ",     [](Roll const & roll) -> bool { return roll[0] < 4; },                     Under4 | Relative,   None | None),
    //NamedRollCondition("If All Values are Under 5, ",     [](Roll const & roll) -> bool { return roll[0] < 5; },                     Under5 | Relative,   None | None),
    //NamedRollCondition("If All Values are Under 6, ",     [](Roll const & roll) -> bool { return roll[0] < 6; },                     Under6 | Relative,   None | None),
    //NamedRollCondition("If All Values are Over 2, ",      [](Roll const & roll) -> bool { return roll[2] > 2; },                     Over2  | Relative,   None | None),
    //NamedRollCondition("If All Values are Over 3, ",      [](Roll const & roll) -> bool { return roll[2] > 3; },                     Over3  | Relative,   None | None),
    //NamedRollCondition("If All Values are Over 4, ",      [](Roll const & roll) -> bool { return roll[2] > 4; },                     Over4  | Relative,   None | None),
    //NamedRollCondition("If All Values are Over 5, ",      [](Roll const & roll) -> bool { return roll[2] > 5; },                     Over5  | Low | High, None | None),
    //NamedRollCondition("If All Values are Even, ",        [](Roll const & roll) -> bool { return AllDiceSatisfy(roll, IsEven); },    Even   | Relative,   None | None),
    //NamedRollCondition("If All Values are Odd, ",         [](Roll const & roll) -> bool { return AllDiceSatisfy(roll, IsOdd); },     Odd    | Relative,   None | None),
    //NamedRollCondition("If Any Value Is Even, ",          [](Roll const & roll) -> bool { return AnyDieSatisfies(roll, IsEven); },   All    | Relative,   None | None),
    //NamedRollCondition("If Any Value Is Odd, ",           [](Roll const & roll) -> bool { return AnyDieSatisfies(roll, IsOdd); },    All    | Relative,   None | None),
  };

  NamedDieFilter s_namedDieFilters[] = {
    { "Any Die", [](Roll const &, Roll const &, DieSelection const &) { return true; }, All, None, true },
    //NamedDieCondition("Any 1",                   [](Roll const &,      unsigned char dieVal) -> bool { return dieVal == 1; },               _1             | Relative,   _1   | None),
    //NamedDieCondition("Any 2",                   [](Roll const &,      unsigned char dieVal) -> bool { return dieVal == 2; },               _2             | Relative,   _2   | None),
    //NamedDieCondition("Any 3",                   [](Roll const &,      unsigned char dieVal) -> bool { return dieVal == 3; },               _3             | Relative,   _3   | None),
    //NamedDieCondition("Any 4",                   [](Roll const &,      unsigned char dieVal) -> bool { return dieVal == 4; },               _4             | Relative,   _4   | None),
    //NamedDieCondition("Any 5",                   [](Roll const &,      unsigned char dieVal) -> bool { return dieVal == 5; },               _5             | Relative,   _5   | None),
    //NamedDieCondition("Any 6",                   [](Roll const &,      unsigned char dieVal) -> bool { return dieVal == 6; },               _6             | Relative,   _6   | None),
    //NamedDieCondition("Any Odd Die",             [](Roll const &,      unsigned char dieVal) -> bool { return dieVal % 2 == 1; },           Odd            | Relative,   None | None),
    //NamedDieCondition("Any Even Die",            [](Roll const &,      unsigned char dieVal) -> bool { return dieVal % 2 == 0; },           Even           | Relative,   None | None),
    //NamedDieCondition("Any Die 2 or Under",      [](Roll const &,      unsigned char dieVal) -> bool { return dieVal <= 2; },               Under3         | Relative,   None | None),
    //NamedDieCondition("Any Die 3 or Under",      [](Roll const &,      unsigned char dieVal) -> bool { return dieVal <= 3; },               Under4         | Relative,   None | None),
    //NamedDieCondition("Any Die 4 or Under",      [](Roll const &,      unsigned char dieVal) -> bool { return dieVal <= 4; },               Under5         | Relative,   None | None),
    //NamedDieCondition("Any Die 5 or Under",      [](Roll const &,      unsigned char dieVal) -> bool { return dieVal <= 5; },               Under6         | Relative,   None | None),
    //NamedDieCondition("Any Die 2 or Over",       [](Roll const &,      unsigned char dieVal) -> bool { return dieVal >= 2; },               Over1          | Relative,   None | None),
    //NamedDieCondition("Any Die 3 or Over",       [](Roll const &,      unsigned char dieVal) -> bool { return dieVal >= 3; },               Over2          | Relative,   None | None),
    //NamedDieCondition("Any Die 4 or Over",       [](Roll const &,      unsigned char dieVal) -> bool { return dieVal >= 4; },               Over3          | Relative,   None | None),
    //NamedDieCondition("Any Die 5 or Over",       [](Roll const &,      unsigned char dieVal) -> bool { return dieVal >= 5; },               Over4          | Relative,   None | None),
    //NamedDieCondition("Any Non-2",               [](Roll const &,      unsigned char dieVal) -> bool { return dieVal != 2; },               Not2           | Relative,   None | None),
    //NamedDieCondition("Any Non-3",               [](Roll const &,      unsigned char dieVal) -> bool { return dieVal != 3; },               Not3           | Relative,   None | None),
    //NamedDieCondition("Any Non-4",               [](Roll const &,      unsigned char dieVal) -> bool { return dieVal != 4; },               Not4           | Relative,   None | None),
    //NamedDieCondition("Any Non-5",               [](Roll const &,      unsigned char dieVal) -> bool { return dieVal != 5; },               Not5           | Relative,   None | None),
    //NamedDieCondition("Any Die Between 2 and 3", [](Roll const &,      unsigned char dieVal) -> bool { return dieVal >= 2 && dieVal < 4; }, Over1 & Under4 | Relative,   None | None),
    //NamedDieCondition("Any Die Between 2 and 4", [](Roll const &,      unsigned char dieVal) -> bool { return dieVal >= 2 && dieVal < 5; }, Over1 & Under5 | Relative,   None | None),
    //NamedDieCondition("Any Die Between 2 and 5", [](Roll const &,      unsigned char dieVal) -> bool { return dieVal >= 2 && dieVal < 6; }, Over1 & Under6 | Relative,   None | None),
    //NamedDieCondition("Any Die Between 3 and 4", [](Roll const &,      unsigned char dieVal) -> bool { return dieVal >= 3 && dieVal < 5; }, Over2 & Under5 | Relative,   None | None),
    //NamedDieCondition("Any Die Between 3 and 5", [](Roll const &,      unsigned char dieVal) -> bool { return dieVal >= 3 && dieVal < 6; }, Over2 & Under6 | Relative,   None | None),
    //NamedDieCondition("Any Die Between 4 and 5", [](Roll const &,      unsigned char dieVal) -> bool { return dieVal >= 4 && dieVal < 6; }, Over3 & Under6 | Relative,   None | None),
    //NamedDieCondition("Your Highest Die",        [](Roll const & roll, unsigned char dieVal) -> bool { return roll[0] == dieVal; },         All            | Low | High, None | High),
    //NamedDieCondition("Your Lowest Die",         [](Roll const & roll, unsigned char dieVal) -> bool { return roll[2] == dieVal; },         All            | Low | High, None | Low),
  };

  NamedSelectionFilter s_namedSelectionFilters[] = {
    { "", [](Roll const &, Roll const &, std::vector<DieSelection> const &) { return true; }, All, None },
  };

  NamedDieModifier s_namedDieModifiers[] = {
    { "Add 1 to ", "", [](Roll & roll, Roll & other, std::vector<DieSelection> const & selections) { selections[0].SetValue(roll, other, [](unsigned char dieVal) { return dieVal + 1; }); }, Under6, true },

    //NamedDieModifier("Add 1 to ",      "",                           [](Roll const &,      unsigned char dieVal) -> unsigned char { return dieVal + 1; }, Under6,  true),
    //NamedDieModifier("Add 2 to ",      "",                           [](Roll const &,      unsigned char dieVal) -> unsigned char { return dieVal + 2; }, Under5,  true),
    //NamedDieModifier("Add 3 to ",      "",                           [](Roll const &,      unsigned char dieVal) -> unsigned char { return dieVal + 3; }, Under4,  true),
    //NamedDieModifier("Add 4 to ",      "",                           [](Roll const &,      unsigned char dieVal) -> unsigned char { return dieVal + 4; }, Under3,  true),
    //NamedDieModifier("Remove 1 from ", "",                           [](Roll const &,      unsigned char dieVal) -> unsigned char { return dieVal - 1; }, Over1,   true),
    //NamedDieModifier("Remove 2 from ", "",                           [](Roll const &,      unsigned char dieVal) -> unsigned char { return dieVal - 2; }, Over2,   true),
    //NamedDieModifier("Remove 3 from ", "",                           [](Roll const &,      unsigned char dieVal) -> unsigned char { return dieVal - 3; }, Over3,   true),
    //NamedDieModifier("Remove 4 from ", "",                           [](Roll const &,      unsigned char dieVal) -> unsigned char { return dieVal - 4; }, Over4,   true),
    //NamedDieModifier("Change ",        " to a 1",                    [](Roll const &,      unsigned char)        -> unsigned char { return 1; },          Not1,    false),
    //NamedDieModifier("Change ",        " to a 2",                    [](Roll const &,      unsigned char)        -> unsigned char { return 2; },          Not2,    false),
    //NamedDieModifier("Change ",        " to a 3",                    [](Roll const &,      unsigned char)        -> unsigned char { return 3; },          Not3,    false),
    //NamedDieModifier("Change ",        " to a 4",                    [](Roll const &,      unsigned char)        -> unsigned char { return 4; },          Not4,    false),
    //NamedDieModifier("Change ",        " to a 5",                    [](Roll const &,      unsigned char)        -> unsigned char { return 5; },          Not5,    false),
    //NamedDieModifier("Change ",        " to a 6",                    [](Roll const &,      unsigned char)        -> unsigned char { return 6; },          Not6,    false),
    //NamedDieModifier("Invert ",        "",                           [](Roll const &,      unsigned char dieVal) -> unsigned char { return 7 - dieVal; }, All,     true),
    //NamedDieModifier("Change ",        " to Match Your Highest Die", [](Roll const & roll, unsigned char dieVal) -> unsigned char { return roll[0]; },    OwnNot6, false),
    //NamedDieModifier("Change ",        " to Match Your Lowest Die",  [](Roll const & roll, unsigned char dieVal) -> unsigned char { return roll[2]; },    OwnNot1, false),
    //NamedDieModifier("Multiply ",      " by 2",                      [](Roll const &,      unsigned char dieVal) -> unsigned char { return dieVal * 2; }, Under4,  true),
    //NamedDieModifier("Multiply ",      " by 3",                      [](Roll const &,      unsigned char dieVal) -> unsigned char { return dieVal * 3; }, Under3,  true),
  };


  bool AreMultipleBitsOn(int value) {
    while (value) {
      int maskedVal = value & ~0x1;
      if (maskedVal) {
        if (maskedVal != value) {
          return true;
        }
      }
      else {
        return false;
      }

      value >>= 1;
    }

    return false;
  }

  bool CanCreateModifierFromNamedParts(
    NamedRollCondition const & rollCond,
    NamedDieFilter const &     dieCond,
    NamedDieModifier const &   dieMod
  ) {
    int const  requiredRollValues                  = rollCond.requiredRolls & All;
    int const  possibleRollValues                  = rollCond.possibleRolls & All;
    int const  possibleDieValues                   = dieCond.possibleRolls  & All;
    int const  possibleModValues                   = dieMod.possibleRolls   & All;

    bool const diceValuesInRoll                    = ((possibleDieValues    & ~possibleRollValues) == 0x0);
    bool const diceValuesInMod                     = ((possibleDieValues    & ~possibleModValues)  == 0x0);
    bool const diceValuesAndRollValuesAreIdentical = (possibleDieValues == possibleRollValues);
    bool const diceValuesAndModValuesAreIdentical  = (possibleDieValues == possibleModValues);
    bool const diceValuesAreGeneral                = dieCond.generalOption;
    bool const rollValuesInMod                     = ((possibleRollValues   & ~possibleModValues) == 0x0);

    if (!diceValuesInRoll && !diceValuesAreGeneral) {
      return false;
    }

    if (diceValuesAndRollValuesAreIdentical) {
      return false;
    }

    if (!diceValuesInMod && !diceValuesAreGeneral) {
      return false;
    }

    if (diceValuesAreGeneral && !rollValuesInMod) {
      return false;
    }

    if (dieMod.requiresMultiplePossibleTargets && !AreMultipleBitsOn(possibleDieValues)) {
      return false;
    }

    return true;
  }

  Modifier CreateModifierFromNamedParts(
    NamedRollCondition const & rollCond,
    NamedDieFilter const &     dieCond,
    NamedDieModifier const &   dieMod
  ) {
    std::string name = rollCond.descriptionPart + dieMod.descriptionPrefix + dieCond.descriptionPart + dieMod.descriptionPostfix;
    return {
      name,
      rollCond.func,
      { { {dieCond.func}, nullptr, dieMod.func } }
    };
  }

  std::vector<Modifier> s_modifiers(
    1,
    {
      "Do Nothing",
      [](Roll const &, Roll const &) { return false; },
      {}
    }
  );

  void GenerateModifiers(void) {
    for (auto & rollCond : s_namedRollConditions) {
      for (auto & dieCond : s_namedDieFilters) {
        for (auto & dieMod : s_namedDieModifiers) {
          if (CanCreateModifierFromNamedParts(rollCond, dieCond, dieMod)) {
            s_modifiers.emplace_back(CreateModifierFromNamedParts(rollCond, dieCond, dieMod));
          }
        }
      }
    }
  }
}

struct ModifierScore {
  ModifierScore(void) = default;
  ModifierScore(Malator::Modifier const & mod, float score) :
    mod(mod),
    score(score)
  {}

  Malator::Modifier mod;
  float             score = 0.0f;
};

struct Rarity {
  Rarity(void) = default;
  Rarity(
    std::string const & name,
    float               highestPossibleBenefit,
    float               range,
    unsigned            count
  ) :
    name(name),
    highestPossibleBenefit(highestPossibleBenefit),
    range(range),
    count(count)
  {}

  std::string name;
  float       highestPossibleBenefit;
  float       range;
  unsigned    count;
};

template <typename T>
void ShuffleVector(std::vector<T> & vec) {
  for (int i = 0; i < vec.size(); ++i) {
    int const target = i + std::rand() % (vec.size() - i);
    std::swap(vec[i], vec[target]);
  }
}

template <typename T>
std::vector<T> GetRandomUniqueValues(std::vector<T> const & input, int count) {
  std::vector<T> result;
  result.reserve(count);

  std::vector<int> indices;
  indices.reserve(input.size());
  for (int i = 0; i < indices.size(); ++i) {
    indices.emplace_back(i);
  }

  for (int i = 0; i < count && !indices.empty(); ++i) {
    int const indexIndex = std::rand() % indices.size();
    int const index      = indices[indexIndex];

    std::swap(indices[indexIndex], indices.back());
    indices.pop_back();

    result.emplace_back(input[index]);
  }

  return result;
}

template <typename T>
std::vector<T> GetRandomValues(std::vector<T> const & input, int count) {
  std::vector<T> result;
  result.reserve(count);

  for (int i = 0; i < count; ++i) {
    result.emplace_back(input[std::rand() % input.size()]);
  }

  return result;
}

std::vector<std::vector<Malator::Modifier>> GenerateCards(std::vector<Rarity> const & rarities) {
  std::vector<std::vector<Malator::Modifier>> results;
  results.resize(rarities.size());

  std::vector<ModifierScore> modifiers;
  for (auto mod : Malator::s_modifiers) {
    float totalWinChance = 0.0f;

    for (int i = 0; i < Malator::PossibleRolls; ++i) {
      Malator::Roll roll = Malator::s_Rolls[i];

      float wins = 0.0;
      for (int j = 0; j < Malator::PossibleRolls; ++j) {
        Malator::Roll other = Malator::s_Rolls[j];

        Malator::ModificationResult result = Malator::GetBestMod(roll, other, mod);

        if (result.roll.GetRollType() > result.other.GetRollType()) {
          wins += 1.0f;
        }
        else if (result.roll.GetRollType() == result.other.GetRollType()) {
          wins += 0.5f;
        }
      }

      float winChance = wins / static_cast<float>(Malator::PossibleRolls);
      totalWinChance += winChance;
    }

    totalWinChance /= Malator::PossibleRolls;
    float benefit = totalWinChance - 0.5f;

    modifiers.emplace_back(mod, benefit);
  }

  std::sort(
    modifiers.begin(),
    modifiers.end(),
    [](ModifierScore const & lhs, ModifierScore const & rhs) -> bool {
      return lhs.score > rhs.score;
    }
  );

  int currentMod = 0;
  for (int i = 0; i < rarities.size(); ++i) {
    Rarity const &                   rarity      = rarities[i];
    std::vector<Malator::Modifier> & rarityCards = results[i];

    if (rarity.count == 0) {
      continue;
    }

    while (currentMod < static_cast<int>(modifiers.size()) - static_cast<int>(rarity.count)) {
      if (
        (modifiers[currentMod].score <= rarity.highestPossibleBenefit) && (
          (currentMod + rarity.count > modifiers.size()) ||
          (modifiers[currentMod].score - modifiers[currentMod + rarity.count - 1].score <= rarity.range)
        )
      ) {
        rarityCards.reserve(rarity.count);

        for (int j = 0; j < std::min(rarity.count, modifiers.size() - currentMod); ++j) {
          rarityCards.emplace_back(modifiers[currentMod + j].mod);
          rarityCards.back().name = rarity.name + ": " + rarityCards.back().name;
        }

        currentMod += rarity.count;
        break;
      }
      else {
        ++currentMod;
      }
    }

    ShuffleVector(rarityCards);
  }

  return results;
}

std::vector<Malator::Modifier> GetRandomCards(
  std::vector<std::vector<Malator::Modifier>> const & cards,
  std::vector<int> const &                            cardsInHand
) {
  std::vector<Malator::Modifier> result;

  for (int i = 0; i < cards.size() && i < cardsInHand.size(); ++i) {
    auto newCards = GetRandomValues(cards[i], cardsInHand[i]);
    result.insert(result.end(), newCards.begin(), newCards.end());
  }

  return result;
}

void DisplayScore(
  std::string const & player0Name,
  int                 player0Score,
  std::string const & player1Name,
  int                 player1Score,
  int                 scoreToWin
) {
  if (player0Score > player1Score) {
    std::cout <<
      player0Name << " is winning with " <<
      player0Score << " points of " <<
      scoreToWin << ". " <<
      player1Name << " has " <<
      player1Score << "." <<
      std::endl;
  }
  else if (player1Score > player0Score) {
    std::cout <<
      player1Name << " is winning with " <<
      player1Score << " points of " <<
      scoreToWin << ". " <<
      player0Name << " has " <<
      player0Score << "." <<
      std::endl;
  }
  else {
    std::cout <<
      player0Name << " and " <<
      player1Name << " are tied with " <<
      player0Score << " points of " <<
      scoreToWin << "." <<
      std::endl;
  }
}

void DisplayRolls(std::string const & name, int namePadding, Malator::Roll const & roll) {
  std::cout <<
    name << ":" <<
    std::setw(namePadding + 1) << " " << "{" <<
    static_cast<int>(roll[0]) << ", " <<
    static_cast<int>(roll[1]) << ", " <<
    static_cast<int>(roll[2]) << "}" <<
    std::endl;
}

bool RunAiTurn(std::string const & playerName, std::vector<Malator::Modifier> & io_hand, Malator::Roll & io_roll, Malator::Roll & io_other) {
  if (io_hand.empty()) {
    std::cout << playerName << " has no cards left and passes the turn." << std::endl;
    return true;
  }

  std::string cardName;
  bool passTurn = true;
  for (int i = io_hand.size() - 1; i >= 0; --i) {
    Malator::ModificationResult result = Malator::GetBestMod(io_roll, io_other, io_hand[i]);

    if (result.modified && result.roll.GetRollType() > result.other.GetRollType()) {
      cardName = io_hand[i].name;
      io_roll  = result.roll;
      io_other = result.other;
      io_hand.erase(io_hand.begin() + i);
      passTurn = false;
      break;
    }
  }

  if (passTurn) {
    std::cout << playerName << " has no good moves to make and passes the turn." << std::endl;
  }
  else {
    std::cout <<
      playerName << " plays \"" <<
      cardName << "\" and now has {" <<
      static_cast<int>(io_roll[0]) << ", " <<
      static_cast<int>(io_roll[1]) << ", " <<
      static_cast<int>(io_roll[2]) << "}." <<
      std::endl;
  }

  return passTurn;
}

// HERE TODO

bool CanUseCard(Malator::Modifier const & card, Malator::Roll const & roll, Malator::Roll const & other) {
  return Malator::CanApplyMod(roll, other, card, GetPossibleSelection(roll, other, card));
}

Malator::Roll UseCardOnValue(Malator::Modifier const & card, Malator::Roll const & roll, unsigned char value) {
  for (int i = 0; i < 3; ++i) {
    if (roll[i] == value) {
      unsigned char newDieVal = card.dieMod(roll, roll[i]);

      return Malator::Roll(
        i == 0 ? newDieVal : roll[0],
        i == 1 ? newDieVal : roll[1],
        i == 2 ? newDieVal : roll[2]
      );
    }
  }

  return roll;
}

bool RunPlayerTurn(std::string const & playerName, std::vector<Malator::Modifier> & io_hand, Malator::Roll & io_roll) {
  if (io_hand.empty()) {
    std::cout << playerName << " has no cards left and passes the turn." << std::endl;
    return true;
  }

  std::cout << playerName << " has the following cards:" << std::endl;

  std::vector<int> playableCards;
  playableCards.reserve(io_hand.size());

  for (int i = 0; i < io_hand.size(); ++i) {
    auto const & card       = io_hand[i];
    bool const   canUseCard = CanUseCard(card, io_roll);

    if (canUseCard) {
      playableCards.emplace_back(i);
    }

    std::cout <<
      "  " <<
      (canUseCard ? std::to_string(playableCards.size()) : std::string("~")) <<
      ": " << card.name <<
      (canUseCard ? "" : " [Cannot be Played]") <<
      std::endl;
  }

  std::cout << std::endl;

  int cardToPlay = -1;

  if (playableCards.empty()) {
    std::cout << playerName << " has no good moves to make and passes the turn." << std::endl;
    return true;
  }
  else if (playableCards.size() == 1) {

    char input = '\0';
    do {
      std::cout << "Would " << playerName << " like to play \"" << io_hand[playableCards[0]].name << "? (y or n): ";
      std::cin >> input;
    } while (input != 'y' && input != 'n');

    if (input == 'y') {
      cardToPlay = playableCards[0];
    }
  }
  else {
    int input = -1;
    do {
      std::cout << "Which card would " << playerName << " like to play? (1 - " << playableCards.size() << ", 0 to pass): ";
      std::cin >> input;
    } while (input < 0 || input > playableCards.size());

    cardToPlay = (input == 0) ? -1 : playableCards[input - 1];
  }

  if (cardToPlay == -1) {
    std::cout << playerName << " decides to pass the turn." << std::endl;
    return true;
  }
  else {
    auto const & card = io_hand[cardToPlay];

    bool canUseCardOn2 = card.dieCond(io_roll, io_roll[2]);
    bool canUseCardOn1 = (io_roll[1] != io_roll[2]) && card.dieCond(io_roll, io_roll[1]);
    bool canUseCardOn0 = (io_roll[0] != io_roll[1]) && card.dieCond(io_roll, io_roll[0]);

    int input = 0;

    if (static_cast<int>(canUseCardOn0) + static_cast<int>(canUseCardOn1) + static_cast<int>(canUseCardOn2) == 1) {
      if (canUseCardOn0) {
        input = io_roll[0];
      }
      else if (canUseCardOn1) {
        input = io_roll[1];
      }
      else if (canUseCardOn2) {
        input = io_roll[2];
      }
    }
    else {
      do {
        std::cout << "Which die would " << playerName << " like to use \"" << card.name << "\" on? (" <<
          (canUseCardOn2 ? (std::to_string(io_roll[2]) + ", ") : std::string()) <<
          (canUseCardOn1 ? (std::to_string(io_roll[1]) + ", ") : std::string()) <<
          (canUseCardOn0 ? (std::to_string(io_roll[0]) + ", ") : std::string()) <<
          "0 to pass): ";
        std::cin >> input;
      } while (!(
        (canUseCardOn0 && (input == io_roll[0])) ||
        (canUseCardOn1 && (input == io_roll[1])) ||
        (canUseCardOn2 && (input == io_roll[2])) ||
        (input == 0)
      ));

      if (input == 0) {
        std::cout << playerName << " decides to pass the turn." << std::endl;
        return true;
      }
    }

    io_roll = UseCardOnValue(card, io_roll, static_cast<unsigned char>(input));

    std::cout <<
      playerName << " plays \"" <<
      card.name << "\" and now has {" <<
      static_cast<int>(io_roll[0]) << ", " <<
      static_cast<int>(io_roll[1]) << ", " << 
      static_cast<int>(io_roll[2]) << "}." <<
      std::endl;

    io_hand.erase(io_hand.begin() + cardToPlay);
  }

  return false;
}

void DisplayRoundWin(std::string const & winner, std::string const &) {
  std::cout << winner << " won the round!" << std::endl;
}

void DisplayRoundTie(std::string const & player0, std::string const & player1) {
  std::cout << player0 << " and " << player1 << " tied this round!" << std::endl;
}

void DisplayGameWin(std::string const & winner, std::string const &) {
  std::cout << winner << " won the game!" << std::endl;
}

void PlayGame(
  std::vector<std::vector<Malator::Modifier>> const & cards,
  std::vector<int> const &                            cardsInHand,
  int                                                 scoreToWin,
  bool                                                player0IsAi,
  bool                                                player1IsAi,
  std::string const &                                 player0Name,
  std::string const &                                 player1Name
) {
  int player0Score = 0;
  int player1Score = 0;

  std::vector<Malator::Modifier> player0cards = GetRandomCards(cards, cardsInHand);
  std::vector<Malator::Modifier> player1cards = GetRandomCards(cards, cardsInHand);

  int const player0Padding = player0Name.length() < player1Name.length() ? player1Name.length() - player0Name.length() : 0;
  int const player1Padding = player1Name.length() < player0Name.length() ? player0Name.length() - player1Name.length() : 0;

  while (player0Score < scoreToWin && player1Score < scoreToWin) {
    Malator::Roll player0Roll = Malator::Roll(std::rand() % 6 + 1, std::rand() % 6 + 1, std::rand() % 6 + 1);
    Malator::Roll player1Roll = Malator::Roll(std::rand() % 6 + 1, std::rand() % 6 + 1, std::rand() % 6 + 1);

    DisplayScore(player0Name, player0Score, player1Name, player1Score, scoreToWin);

    bool passTurn = false;
    do {
      std::cout << std::endl;

      DisplayRolls(player0Name, player0Padding, player0Roll);
      DisplayRolls(player1Name, player1Padding, player1Roll);

      std::cout << std::endl;

      if (player0Roll.GetRollType() < player1Roll.GetRollType()) {
        if (player0IsAi) {
          passTurn = RunAiTurn(player0Name, player0cards, player0Roll, player1Roll.GetRollType());
        }
        else {
          passTurn = RunPlayerTurn(player0Name, player0cards, player0Roll);
        }
      }
      else if (player0Roll.GetRollType() > player1Roll.GetRollType()) {
        if (player1IsAi) {
          passTurn = RunAiTurn(player1Name, player1cards, player1Roll, player0Roll.GetRollType());
        }
        else {
          passTurn = RunPlayerTurn(player1Name, player1cards, player1Roll);
        }
      }
      else {
        DisplayRoundTie(player0Name, player1Name);
        break;
      }
    } while (!passTurn);

    std::cout << std::endl;

    if (player0Roll.GetRollType() > player1Roll.GetRollType()) {
      DisplayRoundWin(player0Name, player1Name);
      ++player0Score;
    }
    else if (player0Roll.GetRollType() < player1Roll.GetRollType()) {
      DisplayRoundWin(player1Name, player0Name);
      ++player1Score;
    }

    std::cout << std::endl;
    std::system("pause");
  }

  if (player0Score > player1Score) {
    DisplayGameWin(player0Name, player1Name);
  }
  else {
    DisplayGameWin(player1Name, player0Name);
  }

}

int main(void) {
  std::srand(std::time(nullptr));

  Malator::GenerateResults();
  Malator::GenerateModifiers();

  int const cardCount = Malator::s_modifiers.size();

  std::vector<Rarity> rarities = {
    //Rarity("Platinum", 1.0f, 1.0f, cardCount / 9),
    //Rarity("Gold",     1.0f, 1.0f, cardCount / 9),
    //Rarity("Silver",   1.0f, 1.0f, cardCount / 9),
    //Rarity("Bronze",   1.0f, 1.0f, cardCount / 9),
    //Rarity("Iron",     1.0f, 1.0f, cardCount / 9),
    //Rarity("Lead",     1.0f, 1.0f, cardCount / 9),
    //Rarity("Copper",   1.0f, 1.0f, cardCount / 9),
    //Rarity("Rust",     1.0f, 1.0f, cardCount / 9),
    //Rarity("Trash",    1.0f, 1.0f, cardCount / 9 + 9),

    Rarity("Gold",   0.30f, 0.0100f, 10),
    Rarity("Silver", 0.24f, 0.0200f, 20),
    Rarity("Bronze", 0.18f, 0.0600f, 40),
  };

  std::vector<std::vector<Malator::Modifier>> cards = GenerateCards(rarities);

  const bool play = true;

  if (play) {
    char playAgain = '\0';
    do {
      PlayGame(cards, { 1, 2, 3 }, 3, false, true, "Player", "Computer");

      do {
        std::cout << "Would you like to play again? (y or n): ";
        std::cin >> playAgain;
      } while (playAgain != 'y' && playAgain != 'n');

      std::cout << std::endl;
    } while (playAgain == 'y');
  }
  else {
    int const rarityCount = std::min(rarities.size(), cards.size());
    for (int i = 0; i < rarityCount; ++i) {
      std::cout << rarities[i].name << ":" << std::endl;

      for (auto & card : cards[i]) {
        std::cout << "  " << card.name << std::endl;
      }

      std::cout << std::endl;
    }
    for (int i = 0; i < rarityCount; ++i) {
      std::cout << rarities[i].name << ": " << cards[i].size() << " cards." << std::endl;
    }
  }

  return 0;
}

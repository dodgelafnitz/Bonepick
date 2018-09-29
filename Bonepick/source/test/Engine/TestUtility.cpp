#include "test/Engine/TestUtility.h"

#include <cstring>
#include <type_traits>

#include "engine/Utility/Bitset.h"
#include "engine/Utility/Blob.h"
#include "engine/Utility/Containers/DumbVariant.h"
#include "engine/Utility/Containers/External.h"
#include "engine/Utility/Containers/Optional.h"
#include "engine/Utility/Containers/SortedArray.h"
#include "engine/Utility/Containers/TypeSet.h"
#include "engine/Utility/Containers/Variant.h"
#include "engine/Utility/DataLayout.h"
#include "engine/Utility/Debug.h"
#include "engine/Utility/Math/Vector.h"
#include "engine/Utility/String.h"
#include "engine/Utility/TemplateTools.h"
#include "engine/Utility/Token.h"

namespace
{
  //############################################################################
  class NontrivialNonleaking
  {
  public:
    NontrivialNonleaking(int * count) :
      count_(count)
    {
      ++(*count_);
    }

    NontrivialNonleaking(NontrivialNonleaking const & other) :
      count_(other.count_)
    {
      ++(*count_);
    }

    ~NontrivialNonleaking(void)
    {
      --(*count_);
    }

    int const & GetCount(void) const
    {
      return *count_;
    }

  private:
    int * count_;
  };

  //############################################################################
  struct ComplexConstructed
  {
    ComplexConstructed(int a, float b) :
      a(a), b(b)
    {}

    int a;
    float b;
  };

  //############################################################################
  struct MoveTester
  {
    enum MoveState
    {
      MoveStateDefaultConstructed,
      MoveStateCopyConstructed,
      MoveStateMoveConstructed,
      MoveStateCopied,
      MoveStateMoved,
      MoveStatePilfered,
      MoveStates
    };

    MoveTester(void) = default;

    MoveTester(MoveTester const &)
      : state_(MoveStateCopyConstructed)
    {}

    MoveTester(MoveTester && moveTester)
      : state_(MoveStateMoveConstructed)
    {
      moveTester.state_ = MoveStatePilfered;
    }

    MoveState GetState(void) const
    {
      return state_;
    }

    MoveTester & operator =(MoveTester const &)
    {
      state_ = MoveStateCopied;
      return *this;
    }

    MoveTester & operator =(MoveTester && moveTester)
    {
      state_ = MoveStateMoved;
      moveTester.state_ = MoveStatePilfered;
      return *this;
    }

  private:

    MoveState state_ = MoveStateDefaultConstructed;
  };

  //############################################################################
  void TestTokenCorrectness(void)
  {
    Token apple("apple");

    ASSERT(std::strcmp(apple.Str().c_str(), "apple") == 0);
  }

  //############################################################################
  void TestTokenComparisons(void)
  {
    Token apple("apple");
    Token app_le("app le");
    Token _apple(" apple");
    Token banana("banana");
    Token ban("ban");

    ASSERT(_apple < app_le);
    ASSERT(app_le < apple);
    ASSERT(apple < ban);
    ASSERT(ban < banana);
  }

  //############################################################################
  void TestTokenNoLongNames(void)
  {
    EXPECT_ERROR(Token("name longer than 24 chars"););
  }

  //############################################################################
  void TestTokenAssignment(void)
  {
    Token app0("apple");
    Token app1;

    app1 = app0;

    ASSERT(app1 == "apple");
  }

  //############################################################################
  void TestSortedArraySingleValue(void)
  {
    SortedArray<int> arr;

    ASSERT(arr.Size() == 0);

    arr.Emplace(5);

    ASSERT(arr.Size() == 1);
    ASSERT(arr[0] == 5);
    EXPECT_ERROR(arr[1];);
    ASSERT(*std::begin(arr) == 5);

    ASSERT(std::begin(arr) == arr.Begin());
    ASSERT(arr.Begin() + 1 == arr.End());
    ASSERT(std::end(arr) == arr.End());

    arr.Clear();

    ASSERT(arr.Size() == 0);
  }

  //############################################################################
  void TestSortedArrayValuesAddedInOrder(void)
  {
    SortedArray<int> arr;

    arr.Emplace(2);
    arr.Emplace(4);
    arr.Emplace(8);
    arr.Emplace(15);
    arr.Emplace(21);
    arr.Emplace(22);

    ASSERT(arr.Size() == 6);

    ASSERT(arr[0] == 2);
    ASSERT(arr[1] == 4);
    ASSERT(arr[2] == 8);
    ASSERT(arr[3] == 15);
    ASSERT(arr[4] == 21);
    ASSERT(arr[5] == 22);

    ASSERT(*std::begin(arr) == 2);

    ASSERT(std::begin(arr) == arr.Begin());
    ASSERT(arr.Begin() + 6 == arr.End());
    ASSERT(std::end(arr) == arr.End());

    arr.Clear();

    ASSERT(arr.Size() == 0);
  }

  //############################################################################
  void TestSortedArrayValuesAddedOutOfOrder(void)
  {
    SortedArray<int> arr;

    arr.Emplace(8);
    arr.Emplace(2);
    arr.Emplace(22);
    arr.Emplace(4);
    arr.Emplace(21);
    arr.Emplace(15);

    ASSERT(arr.Size() == 6);

    ASSERT(arr[0] == 2);
    ASSERT(arr[1] == 4);
    ASSERT(arr[2] == 8);
    ASSERT(arr[3] == 15);
    ASSERT(arr[4] == 21);
    ASSERT(arr[5] == 22);
  }

  //############################################################################
  void TestSortedArrayNoDuplicateValues(void)
  {
    SortedArray<int> arr;

    arr.Emplace(5);
    EXPECT_ERROR(arr.Emplace(5););
  }

  //############################################################################
  void TestSortedArrayFind(void)
  {
    SortedArray<int> arr;

    arr.Emplace(2);
    arr.Emplace(22);
    arr.Emplace(4);
    arr.Emplace(21);

    ASSERT(arr.Find(4) != nullptr);
    ASSERT(arr.Find(7) == nullptr);
  }

  //############################################################################
  void TestSortedArrayHas(void)
  {
    SortedArray<int> arr;

    arr.Emplace(2);
    arr.Emplace(22);
    arr.Emplace(4);
    arr.Emplace(21);

    ASSERT(arr.Has(4));
    ASSERT(!arr.Has(7));
  }

  //############################################################################
  void TestSortedArrayConstAccess(void)
  {
    SortedArray<int> arr = { 2, 5, 7, 3, 6, 9 };
    SortedArray<int> & ref = arr;

    ASSERT(arr.Begin() == ref.Begin());
    ASSERT(std::begin(arr) == std::begin(ref));
    ASSERT(arr.End() == ref.End());
    ASSERT(std::end(arr) == std::end(ref));

    for (unsigned i = 0; i < arr.Size(); ++i)
      ASSERT(&arr[i] == &ref[i]);

    ASSERT(arr.Find(5) == ref.Find(5));
    ASSERT(arr.Find(1) == ref.Find(1));
  }

  //############################################################################
  void TestSortedArrayAssignment(void)
  {
    SortedArray<int> arr0 = { 2, 5, 7, 3, 6, 9 };
    SortedArray<int> arr1;

    arr1 = arr0;

    ASSERT(arr0[0] == 2);
    ASSERT(arr0[1] == 3);
    ASSERT(arr0[2] == 5);
    ASSERT(arr0[3] == 6);
    ASSERT(arr0[4] == 7);
    ASSERT(arr0[5] == 9);

    ASSERT(arr1[0] == 2);
    ASSERT(arr1[1] == 3);
    ASSERT(arr1[2] == 5);
    ASSERT(arr1[3] == 6);
    ASSERT(arr1[4] == 7);
    ASSERT(arr1[5] == 9);

    arr1.Clear();
    ASSERT(arr1.Size() == 0);

    arr1 = std::move(arr0);
    ASSERT(arr1.Size() == 6);
    ASSERT(arr1[0] == 2);
    ASSERT(arr1[1] == 3);
    ASSERT(arr1[2] == 5);
    ASSERT(arr1[3] == 6);
    ASSERT(arr1[4] == 7);
    ASSERT(arr1[5] == 9);

    ASSERT(arr0.Size() == 0);
  }

  //############################################################################
  void TestArrayMapSingleValue(void)
  {
    ArrayMap<int, std::string> map;

    ASSERT(map.Size() == 0);

    map.Emplace(5, "hello");

    ASSERT(map.Size() == 1);
    ASSERT(map[0].key == 5);
    ASSERT(map[0].value == "hello");
    EXPECT_ERROR(map[1];);
    ASSERT(std::begin(map) == &map[0]);

    ASSERT(std::begin(map) == map.Begin());
    ASSERT(map.Begin() + 1 == map.End());
    ASSERT(std::end(map) == map.End());

    map.Clear();

    ASSERT(map.Size() == 0);
  }

  //############################################################################
  void TestArrayMapValuesAddedInOrder(void)
  {
    ArrayMap<int, std::string> map;

    ASSERT(map.Size() == 6);

    ASSERT(map[0].key == 2);
    ASSERT(map[1].key == 4);
    ASSERT(map[2].key == 8);
    ASSERT(map[3].key == 15);
    ASSERT(map[4].key == 21);
    ASSERT(map[5].key == 22);

    ASSERT(map[0].value == "For ");
    ASSERT(map[1].value == "sale: ");
    ASSERT(map[2].value == "baby ");
    ASSERT(map[3].value == "shoes, ");
    ASSERT(map[4].value == "never ");
    ASSERT(map[5].value == "worn");

    ASSERT(std::begin(map) == &map[0]);

    ASSERT(std::begin(map) == map.Begin());
    ASSERT(map.Begin() + 6 == map.End());
    ASSERT(std::end(map) == map.End());

    map.Clear();

    ASSERT(map.Size() == 0);
  }

  //############################################################################
  void TestArrayMapValuesAddedOutOfOrder(void)
  {
    ArrayMap<int, std::string> map;

    map.Emplace(8, "baby ");
    map.Emplace(2, "For ");
    map.Emplace(22, "worn");
    map.Emplace(4, "sale: ");
    map.Emplace(21, "never ");
    map.Emplace(15, "shoes, ");

    ASSERT(map.Size() == 6);

    ASSERT(map[0].key == 2);
    ASSERT(map[1].key == 4);
    ASSERT(map[2].key == 8);
    ASSERT(map[3].key == 15);
    ASSERT(map[4].key == 21);
    ASSERT(map[5].key == 22);

    ASSERT(map[0].value == "For ");
    ASSERT(map[1].value == "sale: ");
    ASSERT(map[2].value == "baby ");
    ASSERT(map[3].value == "shoes, ");
    ASSERT(map[4].value == "never ");
    ASSERT(map[5].value == "worn");
  }

  //############################################################################
  void TestArrayMapNoDuplicateValues(void)
  {
    ArrayMap<int, std::string> map;

    map.Emplace(5, "hi");
    EXPECT_ERROR(map.Emplace(5, "bye"););
  }

  //############################################################################
  void TestArrayMapFind(void)
  {
    ArrayMap<int, std::string> map;

    map.Emplace(2, "apple");
    map.Emplace(22, "banana");
    map.Emplace(4, "cheese");
    map.Emplace(21, "dip");

    ASSERT(map.Find(4) != nullptr);
    ASSERT(map.Find(7) == nullptr);
  }

  //############################################################################
  void TestArrayMapHas(void)
  {
    ArrayMap<int, std::string> map;

    map.Emplace(2, "apple");
    map.Emplace(22, "banana");
    map.Emplace(4, "cheese");
    map.Emplace(21, "dip");

    ASSERT(map.Has(4));
    ASSERT(!map.Has(7));
  }

  //############################################################################
  void TestArrayMapConstAccess(void)
  {
    ArrayMap<int, std::string> map =
    {
      { 8, "baby " },
      { 2, "For " },
      { 22, "worn" },
      { 4, "sale: " },
      { 21, "never " },
      { 15, "shoes, " }
    };
    ArrayMap<int, std::string> & ref = map;

    ASSERT(map.Begin() == ref.Begin());
    ASSERT(std::begin(map) == std::begin(ref));
    ASSERT(map.End() == ref.End());
    ASSERT(std::end(map) == std::end(ref));

    for (unsigned i = 0; i < map.Size(); ++i)
      ASSERT(&map[i] == &ref[i]);

    ASSERT(map.Find(4) == ref.Find(4));
    ASSERT(map.Find(1) == ref.Find(1));
  }

  //############################################################################
  void TestDataLayoutSingleEntry(void)
  {
    Token const pos("pos");
    std::type_index const typeIndex = GetTypeIndex<Vec3>();

    DataLayout layout;
    layout.Add(pos, typeIndex, 0);

    ASSERT(layout.Size() == 1);
    ASSERT(layout.Has(pos));
    ASSERT(layout.GetType(pos) == typeIndex);
    ASSERT(layout.GetOffset(pos) == 0);
  }

  //############################################################################
  void TestDataLayoutNoAddDuplicateEntry(void)
  {
    Token const pos("pos");
    Token const posX("pos.x");
    Token const posY("pos.y");
    Token const posZ("pos.z");
    std::type_index const vecType = GetTypeIndex<Vec3>();
    std::type_index const floatType = GetTypeIndex<float>();

    DataLayout layout;
    layout.Add(pos, vecType, 0);
    layout.Add(posX, floatType, sizeof(float) * 0);
    layout.Add(posY, floatType, sizeof(float) * 1);
    layout.Add(posZ, floatType, sizeof(float) * 2);

    ASSERT(layout.Size() == 4);
    ASSERT(layout.Has(pos));
    ASSERT(layout.Has(posX));
    ASSERT(layout.Has(posY));
    ASSERT(layout.Has(posZ));
    ASSERT(layout.GetType(pos) == vecType);
    ASSERT(layout.GetType(posX) == floatType);
    ASSERT(layout.GetType(posY) == floatType);
    ASSERT(layout.GetType(posZ) == floatType);
    ASSERT(layout.GetOffset(pos) == 0);
    ASSERT(layout.GetOffset(posX) == sizeof(float) * 0);
    ASSERT(layout.GetOffset(posY) == sizeof(float) * 1);
    ASSERT(layout.GetOffset(posZ) == sizeof(float) * 2);
  }

  //############################################################################
  void TestDataLayoutMultipleEntries(void)
  {
    Token const pos("pos");
    std::type_index const vecType = GetTypeIndex<Vec3>();
    std::type_index const floatType = GetTypeIndex<float>();

    DataLayout layout;
    layout.Add(pos, vecType, 0);
    EXPECT_ERROR(layout.Add(pos, floatType, 0););
  }

  //############################################################################
  void TestDataLayoutOrdering(void)
  {
    Token const pos("pos");
    Token const posX("pos.x");
    Token const posY("pos.y");
    Token const posZ("pos.z");
    std::type_index const vecType = GetTypeIndex<Vec3>();
    std::type_index const floatType = GetTypeIndex<float>();

    DataLayout layout;
    layout.Add(pos, vecType, 0);
    layout.Add(posX, floatType, sizeof(float) * 0);
    layout.Add(posY, floatType, sizeof(float) * 1);
    layout.Add(posZ, floatType, sizeof(float) * 2);

    Token previousToken;

    for (auto const & data : layout)
    {
      ASSERT(data.key > previousToken);
      previousToken = data.key;
    }
  }

  //############################################################################
  void TestDataLayoutAssignment(void)
  {
    Token const pos("pos");
    Token const posX("pos.x");
    Token const posY("pos.y");
    Token const posZ("pos.z");
    std::type_index const vecType = GetTypeIndex<Vec3>();
    std::type_index const floatType = GetTypeIndex<float>();

    DataLayout layout0;
    layout0.Add(pos, vecType, 0);
    layout0.Add(posX, floatType, sizeof(float) * 0);
    layout0.Add(posY, floatType, sizeof(float) * 1);
    layout0.Add(posZ, floatType, sizeof(float) * 2);

    DataLayout layout1;

    layout1 = layout0;

    ASSERT(layout0.Size() == 4);
    ASSERT(layout0.Has(pos));
    ASSERT(layout0.Has(posX));
    ASSERT(layout0.Has(posY));
    ASSERT(layout0.Has(posZ));
    ASSERT(layout0.GetType(pos) == vecType);
    ASSERT(layout0.GetType(posX) == floatType);
    ASSERT(layout0.GetType(posY) == floatType);
    ASSERT(layout0.GetType(posZ) == floatType);
    ASSERT(layout0.GetOffset(pos) == 0);
    ASSERT(layout0.GetOffset(posX) == sizeof(float) * 0);
    ASSERT(layout0.GetOffset(posY) == sizeof(float) * 1);
    ASSERT(layout0.GetOffset(posZ) == sizeof(float) * 2);

    ASSERT(layout1.Size() == 4);
    ASSERT(layout1.Has(pos));
    ASSERT(layout1.Has(posX));
    ASSERT(layout1.Has(posY));
    ASSERT(layout1.Has(posZ));
    ASSERT(layout1.GetType(pos) == vecType);
    ASSERT(layout1.GetType(posX) == floatType);
    ASSERT(layout1.GetType(posY) == floatType);
    ASSERT(layout1.GetType(posZ) == floatType);
    ASSERT(layout1.GetOffset(pos) == 0);
    ASSERT(layout1.GetOffset(posX) == sizeof(float) * 0);
    ASSERT(layout1.GetOffset(posY) == sizeof(float) * 1);
    ASSERT(layout1.GetOffset(posZ) == sizeof(float) * 2);

    layout1.Clear();
    ASSERT(layout1.Size() == 0);

    layout1 = std::move(layout0);
    ASSERT(layout0.Size() == 0);

    ASSERT(layout1.Size() == 4);
    ASSERT(layout1.Has(pos));
    ASSERT(layout1.Has(posX));
    ASSERT(layout1.Has(posY));
    ASSERT(layout1.Has(posZ));
    ASSERT(layout1.GetType(pos) == vecType);
    ASSERT(layout1.GetType(posX) == floatType);
    ASSERT(layout1.GetType(posY) == floatType);
    ASSERT(layout1.GetType(posZ) == floatType);
    ASSERT(layout1.GetOffset(pos) == 0);
    ASSERT(layout1.GetOffset(posX) == sizeof(float) * 0);
    ASSERT(layout1.GetOffset(posY) == sizeof(float) * 1);
    ASSERT(layout1.GetOffset(posZ) == sizeof(float) * 2);
  }

  //############################################################################
  void TestStringInfo(void)
  {
    char const * str = "string";
    String string0(str);
    String string1(str, 3);

    ASSERT(string0.Size() == 6);
    ASSERT(!string0.Empty());
    ASSERT(string0.IsNullTerminated());

    ASSERT(string1.Size() == 3);
    ASSERT(!string1.Empty());
    ASSERT(!string1.IsNullTerminated());

    string1.Clear();

    ASSERT(string1.Size() == 0);
    ASSERT(string1.Empty());
    ASSERT(string1.IsNullTerminated());
  }

  //############################################################################
  void TestStringMakeOwner(void)
  {
    char const * str = "string";
    String string(str);

    ASSERT(string.Ptr() == str);

    string.MakeOwner();

    ASSERT(string.Ptr() != str);
  }

  //############################################################################
  void TestStringAccess(void)
  {
    char const * str = "string";
    String string(str);

    ASSERT(string.Size() == int(std::strlen(str)));

    for (int i = 0; i < string.Size(); ++i)
      ASSERT(string[i] == str[i]);

    string.MakeOwner();

    for (int i = 0; i < string.Size(); ++i)
      ASSERT(string[i] == str[i]);
  }

  //############################################################################
  void TestStringAppend(void)
  {
    char const * hello = "hello";
    char const * world = "world";
    char const * helloWorld = "hello world";

    String string0;
    ASSERT(string0.Size() == 0);

    string0 += hello;

    ASSERT(string0 == hello);

    string0 += ' ';

    ASSERT(string0.Size() == 6);

    String string1 = string0 + world;

    ASSERT(string1 == helloWorld);

    String string2 = string1 + '!';

    ASSERT(string2.Size() == 12);
  }

  //############################################################################
  void TestStringCopy(void)
  {
    char const * str = "string";
    String string0(str);
    string0.MakeOwner();

    String string1 = string0;

    ASSERT(string0 == string1);
    string1.MakeOwner();
    ASSERT(string0 == string1);

    String string2;

    ASSERT(string0 != string2);
    string2 = string1;

    ASSERT(string0 == string2);
  }

  //############################################################################
  void TestStringMove(void)
  {
    char const * str = "string";
    String string0(str);
    string0.MakeOwner();

    String string1 = std::move(string0);
    String string2 = str;

    ASSERT(string0.Size() == 0);
    string1.MakeOwner();
    ASSERT(string1 == string2);

    string0 = std::move(string1);

    ASSERT(string1.Size() == 0);
    ASSERT(string0 == string2);
  }

  //############################################################################
  void TestStringSubstring(void)
  {
    char const * hello = "hello";
    char const * world = "world";
    char const * helloWorld = "hello world";

    String string0 = helloWorld;
    String string1 = hello;
    String string2 = world;

    ASSERT(string0.Substring(0, 5) == string1);
    ASSERT(string0.Substring(6, 5) == string2);
  }

  //############################################################################
  void TestDumbVariantEmplace(void)
  {
    DumbVariant<sizeof(NontrivialNonleaking)> var;

    int count = 0;

    var.Emplace<NontrivialNonleaking>(&count);
    ASSERT(count == 1);
  }

  //############################################################################
  void TestDumbVariantClear(void)
  {
    DumbVariant<sizeof(NontrivialNonleaking)> var;

    int count = 0;

    var.Emplace<NontrivialNonleaking>(&count);
    var.Clear<NontrivialNonleaking>();
    ASSERT(count == 0);
  }

  //############################################################################
  void TestDumbVariantGet(void)
  {
    DumbVariant<sizeof(NontrivialNonleaking)> var;

    int count = 0;

    var.Emplace<NontrivialNonleaking>(&count);
    ASSERT(&var.Get<NontrivialNonleaking>().GetCount() == &count);

    DumbVariant<sizeof(NontrivialNonleaking)> const & varRef = var;

    static_assert(
      !std::is_const<std::remove_reference<decltype(
      var.Get<NontrivialNonleaking>())>::type>::value);
    static_assert(
      std::is_const<std::remove_reference<decltype(
      varRef.Get<NontrivialNonleaking>())>::type>::value);

    ASSERT(
      &varRef.Get<NontrivialNonleaking>() == &var.Get<NontrivialNonleaking>());
  }

  //############################################################################
  void TestBitsetAccess(void)
  {
    Bitset bitset = { 2, 6, 9 };

    ASSERT(!bitset.Get(0));
    ASSERT(!bitset.Get(1));
    ASSERT(bitset.Get(2));
    ASSERT(!bitset.Get(3));
    ASSERT(!bitset.Get(4));
    ASSERT(!bitset.Get(5));
    ASSERT(bitset.Get(6));
    ASSERT(!bitset.Get(7));
    ASSERT(!bitset.Get(8));
    ASSERT(bitset.Get(9));
    ASSERT(!bitset.Get(10));

    ASSERT(!bitset[0]);
    ASSERT(!bitset[1]);
    ASSERT(bitset[2]);
    ASSERT(!bitset[3]);
    ASSERT(!bitset[4]);
    ASSERT(!bitset[5]);
    ASSERT(bitset[6]);
    ASSERT(!bitset[7]);
    ASSERT(!bitset[8]);
    ASSERT(bitset[9]);
    ASSERT(!bitset[10]);

    ASSERT(!bitset.Get(10000));
    EXPECT_ERROR(bitset.Get(-10););

    bitset.Set(4);
    bitset.Unset(6);
    bitset.Unset(7);
    bitset.Set(9);
    bitset.Set(10);

    ASSERT(!bitset.Get(0));
    ASSERT(!bitset.Get(1));
    ASSERT(bitset.Get(2));
    ASSERT(!bitset.Get(3));
    ASSERT(bitset.Get(4));
    ASSERT(!bitset.Get(5));
    ASSERT(!bitset.Get(6));
    ASSERT(!bitset.Get(7));
    ASSERT(!bitset.Get(8));
    ASSERT(bitset.Get(9));
    ASSERT(bitset.Get(10));
    ASSERT(!bitset.Get(11));
  }

  //############################################################################
  void TestBitsetCopy(void)
  {
    Bitset bitset0;

    bitset0.Set(3);
    bitset0.Set(6);

    Bitset bitset1 = bitset0;

    ASSERT(bitset1 == bitset0);

    ASSERT(!bitset0.Get(0));
    ASSERT(!bitset0.Get(1));
    ASSERT(!bitset0.Get(2));
    ASSERT(bitset0.Get(3));
    ASSERT(!bitset0.Get(4));
    ASSERT(!bitset0.Get(5));
    ASSERT(bitset0.Get(6));

    ASSERT(!bitset1.Get(0));
    ASSERT(!bitset1.Get(1));
    ASSERT(!bitset1.Get(2));
    ASSERT(bitset1.Get(3));
    ASSERT(!bitset1.Get(4));
    ASSERT(!bitset1.Get(5));
    ASSERT(bitset1.Get(6));

    Bitset bitset2 = std::move(bitset1);

    ASSERT(bitset1 != bitset2);

    ASSERT(!bitset1.Get(0));
    ASSERT(!bitset1.Get(1));
    ASSERT(!bitset1.Get(2));
    ASSERT(!bitset1.Get(3));
    ASSERT(!bitset1.Get(4));
    ASSERT(!bitset1.Get(5));
    ASSERT(!bitset1.Get(6));

    ASSERT(!bitset2.Get(0));
    ASSERT(!bitset2.Get(1));
    ASSERT(!bitset2.Get(2));
    ASSERT(bitset2.Get(3));
    ASSERT(!bitset2.Get(4));
    ASSERT(!bitset2.Get(5));
    ASSERT(bitset2.Get(6));

    bitset1 = std::move(bitset2);

    ASSERT(!bitset2.Get(0));
    ASSERT(!bitset2.Get(1));
    ASSERT(!bitset2.Get(2));
    ASSERT(!bitset2.Get(3));
    ASSERT(!bitset2.Get(4));
    ASSERT(!bitset2.Get(5));
    ASSERT(!bitset2.Get(6));

    ASSERT(!bitset1.Get(0));
    ASSERT(!bitset1.Get(1));
    ASSERT(!bitset1.Get(2));
    ASSERT(bitset1.Get(3));
    ASSERT(!bitset1.Get(4));
    ASSERT(!bitset1.Get(5));
    ASSERT(bitset1.Get(6));
  }

  //############################################################################
  void TestBitsetClear(void)
  {
    Bitset bitset;

    bitset.Set(4);
    bitset.Set(5);

    bitset.Clear();

    ASSERT(!bitset.Get(4));
    ASSERT(!bitset.Get(5));
  }

  //############################################################################
  void TestVariantAccess(void)
  {
    typedef Variant<int, float> VarType;

    VarType var;
    ASSERT(!var.IsType<int>());
    ASSERT(!var.IsType<float>());
    EXPECT_ERROR(var.Get<int>(););
    EXPECT_ERROR(var.Get<float>(););
    ASSERT(var.TryGet<int>() == nullptr);
    ASSERT(var.TryGet<float>() == nullptr);

    var.Emplace<int>(5);
    ASSERT(var.IsType<int>());
    ASSERT(!var.IsType<float>());
    ASSERT(var.Get<int>() == 5);
    EXPECT_ERROR(var.Get<float>(););
    ASSERT(var.TryGet<int>());
    ASSERT(var.TryGet<float>() == nullptr);

    var.Emplace<float>(3.0f);
    ASSERT(!var.IsType<int>());
    ASSERT(var.IsType<float>());
    EXPECT_ERROR(var.Get<int>(););
    ASSERT(var.Get<float>() == int(3.0f));
    ASSERT(var.TryGet<int>() == nullptr);
    ASSERT(var.TryGet<float>());

    var.Clear();
    ASSERT(!var.IsType<int>());
    ASSERT(!var.IsType<float>());
    EXPECT_ERROR(var.Get<int>(););
    EXPECT_ERROR(var.Get<float>(););
    ASSERT(var.TryGet<int>() == nullptr);
    ASSERT(var.TryGet<float>() == nullptr);
  }

  //############################################################################
  void TestVariantComplexEmplace(void)
  {
    typedef Variant<int, float, ComplexConstructed> VarType;
    VarType var;

    var.Emplace<ComplexConstructed>(5, 4.0f);
    ASSERT(var.IsType<ComplexConstructed>());
    ASSERT(var.Get<ComplexConstructed>().a == 5);
    ASSERT(var.Get<ComplexConstructed>().b == 4.0f);
  }

  //############################################################################
  void TestVariantInitDeinit(void)
  {
    typedef Variant<int, NontrivialNonleaking> VarType;

    VarType var0 = int(5);
    ASSERT(var0.IsType<int>());
    ASSERT(!var0.IsType<NontrivialNonleaking>());
    ASSERT(var0.Get<int>() == 5);
    EXPECT_ERROR(var0.Get<NontrivialNonleaking>(););
    ASSERT(var0.TryGet<int>());
    ASSERT(var0.TryGet<NontrivialNonleaking>() == nullptr);

    var0.Get<int>() = 0;
    {
      VarType var1 = NontrivialNonleaking(&var0.Get<int>());

      ASSERT(var0.Get<int>() == var1.Get<NontrivialNonleaking>().GetCount());
      ASSERT(var1.Get<NontrivialNonleaking>().GetCount() == 1);
    }

    ASSERT(var0.Get<int>() == 0);
  }

  //############################################################################
  void TestVariantCopy(void)
  {
    typedef Variant<int, float> VarType;

    VarType var0 = int(5);
    VarType var1 = var0;
    ASSERT(var1.IsType<int>());
    ASSERT(var1.Get<int>() == 5);
    ASSERT(var0.IsType<int>());
    ASSERT(var0.Get<int>() == 5);

    VarType var2;
    var2 = var1;
    ASSERT(var2.IsType<int>());
    ASSERT(var2.Get<int>() == 5);
    ASSERT(var1.IsType<int>());
    ASSERT(var1.Get<int>() == 5);
  }

  //############################################################################
  void TestVariantMove(void)
  {
    typedef Variant<int, float, MoveTester> VarType;

    VarType var0;
    var0.Emplace<MoveTester>();
    ASSERT(var0.Get<MoveTester>().GetState() ==
      MoveTester::MoveStateDefaultConstructed);

    VarType var1 = std::move(var0);
    ASSERT(var0.Get<MoveTester>().GetState() ==
      MoveTester::MoveStatePilfered);
    ASSERT(var1.Get<MoveTester>().GetState() ==
      MoveTester::MoveStateMoveConstructed);

    var0 = std::move(var1);
    ASSERT(var0.Get<MoveTester>().GetState() ==
      MoveTester::MoveStateMoved);
    ASSERT(var1.Get<MoveTester>().GetState() ==
      MoveTester::MoveStatePilfered);
  }

  //############################################################################
  void TestExternalAccess(void)
  {
    External<int> ext0;

    EXPECT_ERROR(*ext0;);
    ASSERT(ext0.Ptr() == nullptr);

    ext0.Emplace(5);
    ASSERT(*ext0 == 5);
    ASSERT(ext0.Ptr());

    ext0.Clear();
    EXPECT_ERROR(*ext0;);
    ASSERT(ext0.Ptr() == nullptr);

    External<ComplexConstructed> ext1(5, 3.0f);
    ASSERT(ext1->a == 5);
    ASSERT(ext1->b == 3.0f);
    ASSERT(ext1.Ptr());
  }

  //############################################################################
  void TestExternalConstAccess(void)
  {
    External<int> const ext0(5);

    ASSERT(*ext0 == 5);
    ASSERT(ext0.Ptr());

    External<ComplexConstructed> const ext1(5, 3.0f);
    ASSERT(ext1->a == 5);
    ASSERT(ext1->b == 3.0f);
    ASSERT(ext1.Ptr());
  }

  //############################################################################
  void TestExternalInitDeinit(void)
  {
    int count = 0;

    {
      External<NontrivialNonleaking> ext0;
      ASSERT(count == 0);
      {
        External<NontrivialNonleaking> ext1;
        ext1.Emplace(&count);

        ASSERT(count == 1);

        ext0 = ext1;
        ASSERT(count == 1);
      }

      ASSERT(count == 1);
    }

    ASSERT(count == 0);
  }

  //############################################################################
  void TestExternalCopy(void)
  {
    External<int> ext0;
    ASSERT(ext0.Ptr() == nullptr);
    ext0.Emplace(5);

    External<int> ext1 = ext0;
    ASSERT(ext1.Ptr());
    ASSERT(*ext1 == 5);

    ext1.Emplace(3);
    ext0 = ext1;
    ASSERT(ext0.Ptr() == ext1.Ptr());
    ASSERT(*ext0 == 3);
  }

  //############################################################################
  void TestExternalMove(void)
  {
    External<int> ext0;
    ext0.Emplace(5);

    External<int> ext1 = std::move(ext0);
    ASSERT(ext1.Ptr());
    ASSERT(ext0.Ptr() == nullptr);
    ASSERT(*ext1 == 5);

    ext0 = std::move(ext1);
    ASSERT(ext0.Ptr());
    ASSERT(ext1.Ptr() == nullptr);
    ASSERT(*ext0 == 5);
  }

  //############################################################################
  void TestWeakExternalAccess(void)
  {
    WeakExternal<Bitset> ext0;
    ASSERT(ext0.Ptr() == nullptr);

    {
      External<Bitset> ext1;
      ext1->Set(2);

      ext0 = ext1;
      ASSERT(ext0.Ptr());
      ASSERT((*ext0)[2]);
      ASSERT(ext0->Get(2));

      *ext0 = 4;
      ASSERT(*ext1 == 4);
    }

    ASSERT(ext0.Ptr() == nullptr);
  }

  //############################################################################
  void TestWeakExternalConstAccess(void)
  {
    External<int> ext0 = 4;

    WeakExternal<int> const ext1 = ext0;
    ASSERT(ext1.Ptr());
    ASSERT(ext1.Ptr() == ext0.Ptr());

    ext0.Clear();

    ASSERT(!ext1.Ptr());
  }

  //############################################################################
  void TestWeakExternalCopy(void)
  {
    External<int> ext0 = 3;

    WeakExternal<int> ext1 = ext0;
    WeakExternal<int> ext2 = ext1;

    ASSERT(ext1 == ext2);

    ext1.Clear();
    ASSERT(ext2.Ptr() == ext0.Ptr());

    ext1 = ext2;
    ASSERT(ext1 == ext2);

  }

  //############################################################################
  void TestOptionalAccess(void)
  {
    Optional<int> opt0;

    EXPECT_ERROR(*opt0;);
    ASSERT(opt0.Ptr() == nullptr);

    opt0.Emplace(5);
    ASSERT(*opt0 == 5);
    ASSERT(opt0.Ptr());

    opt0.Clear();
    EXPECT_ERROR(*opt0;);
    ASSERT(opt0.Ptr() == nullptr);

    Optional<ComplexConstructed> opt1(5, 3.0f);
    ASSERT(opt1->a == 5);
    ASSERT(opt1->b == 3.0f);
    ASSERT(opt1.Ptr());
  }

  //############################################################################
  void TestOptionalConstAccess(void)
  {
    Optional<int> const opt0(5);

    ASSERT(*opt0 == 5);
    ASSERT(opt0.Ptr());

    Optional<ComplexConstructed> const opt1(5, 3.0f);
    ASSERT(opt1->a == 5);
    ASSERT(opt1->b == 3.0f);
    ASSERT(opt1.Ptr());
  }

  //############################################################################
  void TestOptionalInitDeinit(void)
  {
    int count = 0;

    {
      Optional<NontrivialNonleaking> opt;
      ASSERT(count == 0);

      opt.Emplace(&count);
      ASSERT(count == 1);
    }

    ASSERT(count == 0);
  }

  //############################################################################
  void TestOptionalCopy(void)
  {
    Optional<int> opt0;
    ASSERT(opt0.Ptr() == nullptr);
    opt0.Emplace(5);

    Optional<int> opt1 = opt0;
    ASSERT(opt1.Ptr());
    ASSERT(*opt1 == 5);

    int * loc0 = opt1.Ptr();
    opt1.Emplace(3);
    int * loc1 = opt1.Ptr();
    ASSERT(loc0 == loc1);

    opt0 = opt1;
    ASSERT(opt0.Ptr());
    ASSERT(opt1.Ptr());
    ASSERT(opt0.Ptr() != opt1.Ptr());
    ASSERT(*opt0 == 3);
    ASSERT(*opt1 == 3);
  }

  //############################################################################
  void TestOptionalMove(void)
  {
    Optional<MoveTester> opt0;
    opt0.Emplace();

    Optional<MoveTester> opt1 = std::move(opt0);
    ASSERT(opt0.Ptr());
    ASSERT(opt1.Ptr());
    ASSERT(opt0->GetState() == MoveTester::MoveStatePilfered);
    ASSERT(opt1->GetState() == MoveTester::MoveStateMoveConstructed);

    opt0 = std::move(opt1);
    ASSERT(opt0.Ptr());
    ASSERT(opt1.Ptr());
    ASSERT(opt0->GetState() == MoveTester::MoveStateMoved);
    ASSERT(opt1->GetState() == MoveTester::MoveStatePilfered);
  }

  //############################################################################
  void TestTypeSetAccess(void)
  {
    TypeSet<int, float> set0;

    set0.Get<int>() = 2;
    set0.Get<float>() = 4.0f;

    ASSERT(set0.Get<int>() == 2);
    ASSERT(set0.Get<float>() == 4.0f);

    TypeSet<int, float> set1(5, 10.0f);

    ASSERT(set1.Get<int>() == 5);
    ASSERT(set1.Get<float>() == 10.0f);

    TypeSet<MoveTester> set2(std::move(MoveTester()));

    ASSERT(set2.Get<MoveTester>().GetState() ==
      MoveTester::MoveStateMoveConstructed);
  }

  //############################################################################
  void TestTypeSetConstAccess(void)
  {
    TypeSet<int, float> const set0(5, 10.0f);

    ASSERT(set0.Get<int>() == 5);
    ASSERT(set0.Get<float>() == 10.0f);
  }

  //############################################################################
  void TestTypeSetCopy(void)
  {
    TypeSet<int, float> set0(5, 10.0f);
    TypeSet<int, float> set1 = set0;

    ASSERT(set0 == set1);
    ASSERT(set0.Get<int>() == set1.Get<int>());
    ASSERT(set0.Get<float>() == set1.Get<float>());

    set1.Get<int>() = 2;
    set1.Get<float>() = 5.3f;

    set0 = set1;

    ASSERT(set0 == set1);
    ASSERT(set0.Get<int>() == set1.Get<int>());
    ASSERT(set0.Get<float>() == set1.Get<float>());
  }

  //############################################################################
  void TestTypeSetMove(void)
  {
    TypeSet<MoveTester> set0;
    ASSERT(set0.Get<MoveTester>().GetState() ==
      MoveTester::MoveStateDefaultConstructed);

    TypeSet<MoveTester> set1 = std::move(set0);
    ASSERT(set0.Get<MoveTester>().GetState() ==
      MoveTester::MoveStatePilfered);
    ASSERT(set1.Get<MoveTester>().GetState() ==
      MoveTester::MoveStateMoveConstructed);

    set0 = std::move(set1);
    ASSERT(set0.Get<MoveTester>().GetState() ==
      MoveTester::MoveStateMoved);
    ASSERT(set1.Get<MoveTester>().GetState() ==
      MoveTester::MoveStatePilfered);
  }

  //############################################################################
  void TestTokens(void)
  {
    TestTokenCorrectness();
    TestTokenComparisons();
    TestTokenNoLongNames();
    TestTokenAssignment();
  }

  //############################################################################
  void TestSortedArrays(void)
  {
    TestSortedArraySingleValue();
    TestSortedArrayValuesAddedInOrder();
    TestSortedArrayValuesAddedOutOfOrder();
    TestSortedArrayNoDuplicateValues();
    TestSortedArrayFind();
    TestSortedArrayHas();
    TestSortedArrayConstAccess();
    TestSortedArrayAssignment();
  }

  //############################################################################
  void TestArrayMaps(void)
  {
    TestArrayMapSingleValue();
    TestArrayMapValuesAddedInOrder();
    TestArrayMapValuesAddedOutOfOrder();
    TestArrayMapNoDuplicateValues();
    TestArrayMapFind();
    TestArrayMapHas();
    TestArrayMapConstAccess();
  }

  //############################################################################
  void TestDataLayouts(void)
  {
    TestDataLayoutSingleEntry();
    TestDataLayoutNoAddDuplicateEntry();
    TestDataLayoutMultipleEntries();
    TestDataLayoutOrdering();
    TestDataLayoutAssignment();
  }

  //############################################################################
  void TestStrings(void)
  {
    TestStringInfo();
    TestStringMakeOwner();
    TestStringAccess();
    TestStringAppend();
    TestStringCopy();
    TestStringMove();
    TestStringSubstring();
  }

  //############################################################################
  void TestDumbVariants(void)
  {
    TestDumbVariantEmplace();
    TestDumbVariantClear();
    TestDumbVariantGet();
  }


  //############################################################################
  void TestBitsets(void)
  {
    TestBitsetAccess();
    TestBitsetCopy();
    TestBitsetClear();
  }

  //############################################################################
  void TestVariants(void)
  {
    TestVariantAccess();
    TestVariantComplexEmplace();
    TestVariantInitDeinit();
    TestVariantCopy();
    TestVariantMove();
  }

  //############################################################################
  void TestExternals(void)
  {
    TestExternalAccess();
    TestExternalConstAccess();
    TestExternalInitDeinit();
    TestExternalCopy();
    TestExternalMove();
  }

  //############################################################################
  void TestWeakExternals(void)
  {
    TestWeakExternalAccess();
    TestWeakExternalConstAccess();
    TestWeakExternalCopy();
  }

  //############################################################################
  void TestOptionals(void)
  {
    TestOptionalAccess();
    TestOptionalConstAccess();
    TestOptionalInitDeinit();
    TestOptionalCopy();
    TestOptionalMove();
  }

  //############################################################################
  void TestTypeSets(void)
  {
    TestTypeSetAccess();
    TestTypeSetConstAccess();
    TestTypeSetCopy();
    TestTypeSetMove();
  }
}

//##############################################################################
void TestAllUtilities(void)
{
  TestTokens();
  TestSortedArrays();
  TestDataLayouts();
  TestStrings();
  TestDumbVariants();
  TestBitsets();
  TestVariants();
  TestExternals();
  TestWeakExternals();
  TestOptionals();
  TestTypeSets();
}

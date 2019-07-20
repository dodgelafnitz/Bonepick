#ifndef UTILITY_MACROS_H
#define UTILITY_MACROS_H

//macro for expanding __VA_ARGS__ in macros into multiple macro parameters
#define EXPAND(macro) macro

//macro for joining macro parameters with commas in them
#define JOIN(...) __VA_ARGS__

//macro for referencing unused parameters
#define REFERENCE(...) ((void*)(__VA_ARGS__, 0))

//macro for getting the number of elelments in an array
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*arr))

//macro for geting the C string of a macro call
#define CSTRING(string) CStringInner(string)
#define CSTRING_INNER(string) #string

//macros for counting arguments
#define MAX_COUNT_ARGS 32

#define COUNT_ARGS(...) EXPAND(COUNT_ARGS_INNER(__VA_ARGS__, 32, 31, 30, 29, 28, 27, 26, 25, \
                                                             24, 23, 22, 21, 20, 19, 18, 17, \
                                                             16, 15, 14, 13, 12, 11, 10, 9,  \
                                                             8, 7, 6, 5, 4, 3, 2, 1))
#define COUNT_ARGS_INNER( _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8, \
                          _9, _10, _11, _12, _13, _14, _15, _16, \
                         _17, _18, _19, _20, _21, _22, _23, _24, \
                         _25, _26, _27, _28, _29, _30, _31, _32, \
                         N, ...) N

//macros for "overloading" macros with number of arguments
#define MACRO_NAME_COUNT(base, ...) MACRO_NAME(base, COUNT_ARGS(__VA_ARGS__))
#define MACRO_NAME(base, tag) MACRO_NAME_INNER(base, tag)
#define MACRO_NAME_INNER(base, tag) base##tag

//macros for simplifying "overloaded" macro logic
#define EXTRA_ARGS(...) EXPAND(COUNT_ARGS_INNER(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, \
                                                             1, 1, 1, 1, 1, 1, 1, 1, \
                                                             1, 1, 1, 1, 1, 1, 1, 1, \
                                                             1, 1, 1, 1, 1, 1, 1, 0))
#define MACRO_NAME_HAS_EXTRA_ARGS(base, ...) EXPAND(MACRO_NAME(MACRO_HAS_EXTRA_ARGS_, EXTRA_ARGS(__VA_ARGS__)))(base)
#define MACRO_HAS_EXTRA_ARGS_0(base) MACRO_NAME(base, SINGLE)
#define MACRO_HAS_EXTRA_ARGS_1(base) MACRO_NAME(base, MULTI)

//macros for defining iterative macro behavior
#define MAP_WITH_SPLIT(macro, split, ...) EXPAND(MACRO_NAME_COUNT(MAP_WITH_SPLIT_, __VA_ARGS__)(macro, split, __VA_ARGS__))
#define MAP(macro, ...) EXPAND(MAP_WITH_SPLIT(macro, , __VA_ARGS__))

#define MAP_WITH_SPLIT_1(macro, split, input)       macro(input)
#define MAP_WITH_SPLIT_2(macro, split, input, ...)  macro(input) split EXPAND(MAP_WITH_SPLIT_1(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_3(macro, split, input, ...)  macro(input) split EXPAND(MAP_WITH_SPLIT_2(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_4(macro, split, input, ...)  macro(input) split EXPAND(MAP_WITH_SPLIT_3(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_5(macro, split, input, ...)  macro(input) split EXPAND(MAP_WITH_SPLIT_4(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_6(macro, split, input, ...)  macro(input) split EXPAND(MAP_WITH_SPLIT_5(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_7(macro, split, input, ...)  macro(input) split EXPAND(MAP_WITH_SPLIT_6(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_8(macro, split, input, ...)  macro(input) split EXPAND(MAP_WITH_SPLIT_7(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_9(macro, split, input, ...)  macro(input) split EXPAND(MAP_WITH_SPLIT_8(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_10(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_9(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_11(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_10(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_12(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_11(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_13(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_12(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_14(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_13(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_15(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_14(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_16(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_15(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_17(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_16(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_18(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_17(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_19(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_18(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_20(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_19(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_21(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_20(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_22(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_21(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_23(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_22(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_24(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_23(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_25(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_24(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_26(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_25(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_27(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_26(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_28(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_27(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_29(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_28(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_30(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_29(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_31(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_30(macro, split, __VA_ARGS__))
#define MAP_WITH_SPLIT_32(macro, split, input, ...) macro(input) split EXPAND(MAP_WITH_SPLIT_31(macro, split, __VA_ARGS__))

//macros for evaluating macros before combining them
#define COMBINE_ACTUAL(a, b) a##b
#define COMBINE(...) EXPAND(MACRO_NAME_COUNT(COMBINE_, __VA_ARGS__)(__VA_ARGS__))

#define COMBINE_1(a) a
#define COMBINE_2(a, b)    COMBINE_ACTUAL(a, b)
#define COMBINE_3(a, ...)  EXPAND(COMBINE_2(a, COMBINE_2(__VA_ARGS__)))
#define COMBINE_4(a, ...)  EXPAND(COMBINE_2(a, COMBINE_3(__VA_ARGS__)))
#define COMBINE_5(a, ...)  EXPAND(COMBINE_2(a, COMBINE_4(__VA_ARGS__)))
#define COMBINE_6(a, ...)  EXPAND(COMBINE_2(a, COMBINE_5(__VA_ARGS__)))
#define COMBINE_7(a, ...)  EXPAND(COMBINE_2(a, COMBINE_6(__VA_ARGS__)))
#define COMBINE_8(a, ...)  EXPAND(COMBINE_2(a, COMBINE_7(__VA_ARGS__)))
#define COMBINE_9(a, ...)  EXPAND(COMBINE_2(a, COMBINE_8(__VA_ARGS__)))
#define COMBINE_10(a, ...) EXPAND(COMBINE_2(a, COMBINE_9(__VA_ARGS__)))
#define COMBINE_11(a, ...) EXPAND(COMBINE_2(a, COMBINE_10(__VA_ARGS__)))
#define COMBINE_12(a, ...) EXPAND(COMBINE_2(a, COMBINE_11(__VA_ARGS__)))
#define COMBINE_13(a, ...) EXPAND(COMBINE_2(a, COMBINE_12(__VA_ARGS__)))
#define COMBINE_14(a, ...) EXPAND(COMBINE_2(a, COMBINE_13(__VA_ARGS__)))
#define COMBINE_15(a, ...) EXPAND(COMBINE_2(a, COMBINE_14(__VA_ARGS__)))
#define COMBINE_16(a, ...) EXPAND(COMBINE_2(a, COMBINE_15(__VA_ARGS__)))
#define COMBINE_17(a, ...) EXPAND(COMBINE_2(a, COMBINE_16(__VA_ARGS__)))
#define COMBINE_18(a, ...) EXPAND(COMBINE_2(a, COMBINE_17(__VA_ARGS__)))
#define COMBINE_19(a, ...) EXPAND(COMBINE_2(a, COMBINE_18(__VA_ARGS__)))
#define COMBINE_20(a, ...) EXPAND(COMBINE_2(a, COMBINE_19(__VA_ARGS__)))
#define COMBINE_21(a, ...) EXPAND(COMBINE_2(a, COMBINE_20(__VA_ARGS__)))
#define COMBINE_22(a, ...) EXPAND(COMBINE_2(a, COMBINE_21(__VA_ARGS__)))
#define COMBINE_23(a, ...) EXPAND(COMBINE_2(a, COMBINE_22(__VA_ARGS__)))
#define COMBINE_24(a, ...) EXPAND(COMBINE_2(a, COMBINE_23(__VA_ARGS__)))
#define COMBINE_25(a, ...) EXPAND(COMBINE_2(a, COMBINE_24(__VA_ARGS__)))
#define COMBINE_26(a, ...) EXPAND(COMBINE_2(a, COMBINE_25(__VA_ARGS__)))
#define COMBINE_27(a, ...) EXPAND(COMBINE_2(a, COMBINE_26(__VA_ARGS__)))
#define COMBINE_28(a, ...) EXPAND(COMBINE_2(a, COMBINE_27(__VA_ARGS__)))
#define COMBINE_29(a, ...) EXPAND(COMBINE_2(a, COMBINE_28(__VA_ARGS__)))
#define COMBINE_30(a, ...) EXPAND(COMBINE_2(a, COMBINE_29(__VA_ARGS__)))
#define COMBINE_31(a, ...) EXPAND(COMBINE_2(a, COMBINE_30(__VA_ARGS__)))
#define COMBINE_32(a, ...) EXPAND(COMBINE_2(a, COMBINE_31(__VA_ARGS__)))

//macros for inserting parameters into variadic macros
#define INSERT_PARAMETER(index, parameter, ...) EXPAND(MACRO_NAME(INSERT_PARAMETER_, index)(parameter, __VA_ARGS__))

#define INSERT_PARAMETER_0(parameter, ...)         parameter, __VA_ARGS__
#define INSERT_PARAMETER_1(parameter, first, ...)  first, parameter, __VA_ARGS__
#define INSERT_PARAMETER_2(parameter, first, ...)  first, EXPAND(INSERT_PARAMETER_1(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_3(parameter, first, ...)  first, EXPAND(INSERT_PARAMETER_2(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_4(parameter, first, ...)  first, EXPAND(INSERT_PARAMETER_3(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_5(parameter, first, ...)  first, EXPAND(INSERT_PARAMETER_4(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_6(parameter, first, ...)  first, EXPAND(INSERT_PARAMETER_5(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_7(parameter, first, ...)  first, EXPAND(INSERT_PARAMETER_6(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_8(parameter, first, ...)  first, EXPAND(INSERT_PARAMETER_7(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_9(parameter, first, ...)  first, EXPAND(INSERT_PARAMETER_8(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_10(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_9(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_11(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_10(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_12(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_11(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_13(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_12(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_14(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_13(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_15(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_14(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_16(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_15(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_17(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_16(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_18(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_17(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_19(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_18(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_20(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_19(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_21(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_20(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_22(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_21(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_23(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_22(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_24(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_23(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_25(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_24(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_26(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_25(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_27(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_26(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_28(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_27(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_29(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_28(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_30(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_29(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_31(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_30(parameter, __VA_ARGS__))
#define INSERT_PARAMETER_32(parameter, first, ...) first, EXPAND(INSERT_PARAMETER_31(parameter, __VA_ARGS__))

#endif

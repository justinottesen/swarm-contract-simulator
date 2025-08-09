#pragma once

// NOLINTBEGIN(cppcoreguidelines-macro-usage, bugprone-macro-parentheses)

// Silence an unused parameter warning
#define UNUSED(X) (void)(X)

// Rescan macro tokens up to 256 times
#define EXPAND4(...) __VA_OPT__(__VA_ARGS__)
#define EXPAND3(...) __VA_OPT__(EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__)))))
#define EXPAND2(...) __VA_OPT__(EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__)))))
#define EXPAND1(...) __VA_OPT__(EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__)))))
#define EXPAND(...) __VA_OPT__(EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__)))))
#define PARENS ()
#define ID(...) __VA_ARGS__

// For each macro and helpers
#define FOR_EACH_HELPER(MACRO, ARG_1, ...)                                     \
  MACRO(ARG_1) __VA_OPT__(FOR_EACH_AGAIN PARENS(MACRO, __VA_ARGS__))
#define FOR_EACH_AGAIN() FOR_EACH_HELPER
#define FOR_EACH(MACRO, ...)                                                   \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER(MACRO, __VA_ARGS__)))

#define FOR_EACH_P_HELPER(MACRO, ARG_1, ...)                                   \
  MACRO ARG_1 __VA_OPT__(FOR_EACH_P_AGAIN PARENS(MACRO, __VA_ARGS__))
#define FOR_EACH_P_AGAIN() _FOR_EACH_P_HELPER
#define FOR_EACH_P(MACRO, ...)                                                 \
  __VA_OPT__(EXPAND(FOR_EACH_P_HELPER(MACRO, __VA_ARGS__)))

#define FOR_EACH_LIST_HELPER(MACRO, ARG_1, ...)                                \
  MACRO(ARG_1) __VA_OPT__(, FOR_EACH_LIST_AGAIN PARENS(MACRO, __VA_ARGS__))
#define FOR_EACH_LIST_AGAIN() FOR_EACH_LIST_HELPER
#define FOR_EACH_LIST(MACRO, ...)                                              \
  __VA_OPT__(EXPAND(FOR_EACH_LIST_HELPER(MACRO, __VA_ARGS__)))

#define FOR_EACH_LIST_P_HELPER(MACRO, ARG_1, ...)                              \
  MACRO ARG_1 __VA_OPT__(, FOR_EACH_LIST_P_AGAIN PARENS(MACRO, __VA_ARGS__))
#define FOR_EACH_LIST_P_AGAIN() FOR_EACH_LIST_P_HELPER
#define FOR_EACH_LIST_P(MACRO, ...)                                            \
  __VA_OPT__(EXPAND(FOR_EACH_LIST_P_HELPER(MACRO, __VA_ARGS__)))

// Enum with `to_string` and `from_string` methods
#ifndef ENUM_FROM_STRING_TEMPLATE
#define ENUM_FROM_STRING_TEMPLATE
#include <result.hpp>
template <typename EnumT>
[[nodiscard]] constexpr auto from_string(std::string_view _str) noexcept
    -> Result<EnumT>;
#endif
#define ENUM_CASE(NAME, ...)                                                   \
  case NAME:                                                                   \
    return #NAME;
#define ENUM_IF(NAME, ...)                                                     \
  if (_str == #NAME) {                                                         \
    return NAME;                                                               \
  }
#define ENUM_STRING_METHODS(TYPE, LOOPER, ...)                                 \
  [[nodiscard]] constexpr inline auto to_string(TYPE _enum) noexcept           \
      -> Result<std::string_view> {                                            \
    using enum TYPE;                                                           \
    switch (_enum) {                                                           \
      EXPAND(LOOPER(ENUM_CASE, __VA_ARGS__))                                   \
    default:                                                                   \
      return std::unexpected(                                                  \
          std::make_error_code(std::errc::invalid_argument));                  \
    }                                                                          \
  }                                                                            \
  template <>                                                                  \
  [[nodiscard]] constexpr inline auto from_string(                             \
      std::string_view _str) noexcept -> Result<TYPE> {                        \
    using enum TYPE;                                                           \
    EXPAND(LOOPER(ENUM_IF, __VA_ARGS__))                                       \
    return std::unexpected(std::make_error_code(std::errc::invalid_argument)); \
  }
#define ENUM(TYPE, BASE, ...)                                                  \
  enum class TYPE : BASE { __VA_ARGS__ };                                      \
  ENUM_STRING_METHODS(TYPE, FOR_EACH, __VA_ARGS__)

#define ENUM_ASSIGN(NAME, VAL) NAME = VAL
#define VAL_ENUM(TYPE, BASE, ...)                                              \
  enum class TYPE : BASE { FOR_EACH_LIST_P(ENUM_ASSIGN, __VA_ARGS__) };        \
  ENUM_STRING_METHODS(TYPE, FOR_EACH_P, __VA_ARGS__)

// Cconstructors, Assignment, Destrcutors
#define CTOR(CLASSNAME, MODIFIER) CLASSNAME() noexcept = MODIFIER
#define COPY_CTOR_ASSIGN(CLASSNAME, MODIFIER)                                  \
  CLASSNAME(const CLASSNAME &) noexcept = MODIFIER;                            \
  auto operator=(const CLASSNAME &) noexcept -> CLASSNAME & = MODIFIER
#define MOVE_CTOR_ASSIGN(CLASSNAME, MODIFIER)                                  \
  CLASSNAME(CLASSNAME &&) noexcept = MODIFIER;                                 \
  auto operator=(CLASSNAME &&) noexcept -> CLASSNAME & = MODIFIER

#define DEFAULT_DTOR(CLASSNAME) ~CLASSNAME() noexcept = default
#define VIRTUAL_DEFAULT_DTOR(CLASSNAME) virtual DEFAULT_DTOR(CLASSNAME)

#define DEFAULT_CTOR(CLASSNAME) CTOR(CLASSNAME, default)
#define DEFAULT_COPY(CLASSNAME) COPY_CTOR_ASSIGN(CLASSNAME, default)
#define DEFAULT_MOVE(CLASSNAME) MOVE_CTOR_ASSIGN(CLASSNAME, default)

#define DELETE_CTOR(CLASSNAME) CTOR(CLASSNAME, delete)
#define DELETE_COPY(CLASSNAME) COPY_CTOR_ASSIGN(CLASSNAME, delete)
#define DELETE_MOVE(CLASSNAME) MOVE_CTOR_ASSIGN(CLASSNAME, delete)

#define DEFAULT_CTOR_DTOR(CLASSNAME)                                           \
  DEFAULT_CTOR(CLASSNAME);                                                     \
  DEFAULT_DTOR(CLASSNAME)

#define DEFAULT_COPY_MOVE(CLASSNAME)                                           \
  DEFAULT_COPY(CLASSNAME);                                                     \
  DEFAULT_MOVE(CLASSNAME);

#define DELETE_COPY_MOVE(CLASSNAME)                                            \
  DELETE_COPY(CLASSNAME);                                                      \
  DELETE_MOVE(CLASSNAME)

#define DELETE_CTOR_COPY_MOVE(CLASSNAME)                                       \
  DELETE_CTOR(CLASSNAME);                                                      \
  DELETE_COPY_MOVE(CLASSNAME)

// Concept method macros
#define CONCEPT_INTERFACE(CONCEPTNAME, ...)                                    \
  template <typename ConceptType __VA_OPT__(, )                                \
                FOR_EACH_LIST(ID, __VA_ARGS__)>                                \
  concept CONCEPTNAME =                                                        \
    requires

#define WRAP_DECLVAL(TYPE) std::declval<TYPE>()
#define CONCEPT_METHOD(NAME, RETURN, ...)                                      \
  {                                                                            \
    std::declval<ConceptType>().NAME(FOR_EACH_LIST(WRAP_DECLVAL, __VA_ARGS__)) \
  } noexcept -> std::same_as<RETURN>

#define CONCEPT_METHOD_CONST(NAME, RETURN, ...)                                \
  {                                                                            \
    std::declval<const ConceptType>().NAME(                                    \
        FOR_EACH_LIST(WRAP_DECLVAL, __VA_ARGS__))                              \
  } noexcept -> std::same_as<RETURN>

// NOLINTEND(cppcoreguidelines-macro-usage, bugprone-macro-parentheses)

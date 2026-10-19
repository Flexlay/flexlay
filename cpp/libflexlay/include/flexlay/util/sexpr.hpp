// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_UTIL_SEXPR_HPP
#define FLEXLAY_UTIL_SEXPR_HPP

#include <cstdint>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace flexlay {

class SExpr
{
public:
  using List = std::vector<SExpr>;
  using Value = std::variant<std::string, List>;

  SExpr() : m_value(std::string{}) {}
  explicit SExpr(std::string atom) : m_value(std::move(atom)) {}
  explicit SExpr(List list) : m_value(std::move(list)) {}

  bool is_atom() const { return std::holds_alternative<std::string>(m_value); }
  bool is_list() const { return std::holds_alternative<List>(m_value); }

  const std::string& as_atom() const { return std::get<std::string>(m_value); }
  const List& as_list() const { return std::get<List>(m_value); }
  List& as_list() { return std::get<List>(m_value); }

  /** Symbol name if list starts with atom, else empty. */
  std::string tag() const
  {
    if (!is_list() || as_list().empty() || !as_list()[0].is_atom()) {
      return {};
    }
    return as_list()[0].as_atom();
  }

private:
  Value m_value;
};

/** Parse one top-level sexpr from a string (skips comments starting with ;). */
SExpr sexpr_parse(const std::string& text);

/** Read entire file and parse the first top-level sexpr. */
SExpr sexpr_parse_file(const std::string& path);

} // namespace flexlay

#endif

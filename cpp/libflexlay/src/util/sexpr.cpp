// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/util/sexpr.hpp"

#include <cctype>
#include <fstream>
#include <sstream>

namespace flexlay {
namespace {

class Parser
{
public:
  explicit Parser(const std::string& text) : m_text(text) {}

  SExpr parse()
  {
    skip();
    auto v = parse_value();
    return v;
  }

private:
  void skip()
  {
    while (m_pos < m_text.size()) {
      const char c = m_text[m_pos];
      if (c == ';') {
        while (m_pos < m_text.size() && m_text[m_pos] != '\n') {
          ++m_pos;
        }
      } else if (std::isspace(static_cast<unsigned char>(c))) {
        ++m_pos;
      } else {
        break;
      }
    }
  }

  SExpr parse_value()
  {
    skip();
    if (m_pos >= m_text.size()) {
      throw std::runtime_error("sexpr: unexpected end of input");
    }
    if (m_text[m_pos] == '(') {
      ++m_pos;
      SExpr::List list;
      while (true) {
        skip();
        if (m_pos >= m_text.size()) {
          throw std::runtime_error("sexpr: unclosed list");
        }
        if (m_text[m_pos] == ')') {
          ++m_pos;
          break;
        }
        list.push_back(parse_value());
      }
      return SExpr(std::move(list));
    }
    if (m_text[m_pos] == '"') {
      return SExpr(parse_string());
    }
    // atom: until whitespace or paren
    const size_t start = m_pos;
    while (m_pos < m_text.size()) {
      const char c = m_text[m_pos];
      if (std::isspace(static_cast<unsigned char>(c)) || c == '(' || c == ')' || c == ';') {
        break;
      }
      ++m_pos;
    }
    return SExpr(m_text.substr(start, m_pos - start));
  }

  std::string parse_string()
  {
    ++m_pos; // skip "
    std::string out;
    while (m_pos < m_text.size()) {
      const char c = m_text[m_pos++];
      if (c == '"') {
        return out;
      }
      if (c == '\\' && m_pos < m_text.size()) {
        out.push_back(m_text[m_pos++]);
      } else {
        out.push_back(c);
      }
    }
    throw std::runtime_error("sexpr: unclosed string");
  }

  const std::string& m_text;
  size_t m_pos = 0;
};

} // namespace

SExpr sexpr_parse(const std::string& text)
{
  return Parser(text).parse();
}

SExpr sexpr_parse_file(const std::string& path)
{
  std::ifstream in(path);
  if (!in) {
    throw std::runtime_error("sexpr_parse_file: cannot open " + path);
  }
  std::ostringstream ss;
  ss << in.rdbuf();
  return sexpr_parse(ss.str());
}

} // namespace flexlay

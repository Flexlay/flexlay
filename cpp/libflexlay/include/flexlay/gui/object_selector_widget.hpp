// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_GUI_OBJECT_SELECTOR_WIDGET_HPP
#define FLEXLAY_GUI_OBJECT_SELECTOR_WIDGET_HPP

#include "flexlay/object_prototype.hpp"

#include <QListWidget>

#include <vector>

namespace flexlay {

class ObjectSelectorWidget : public QListWidget
{
  Q_OBJECT

public:
  explicit ObjectSelectorWidget(QWidget* parent = nullptr);

  void set_prototypes(std::vector<ObjectPrototype> prototypes);
  const std::vector<ObjectPrototype>& prototypes() const { return m_prototypes; }

  /** Currently selected prototype, or nullptr. */
  const ObjectPrototype* current_prototype() const;

signals:
  void prototype_activated(const ObjectPrototype& proto);

private:
  std::vector<ObjectPrototype> m_prototypes;
};

} // namespace flexlay

#endif

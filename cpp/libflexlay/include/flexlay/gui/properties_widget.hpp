// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_GUI_PROPERTIES_WIDGET_HPP
#define FLEXLAY_GUI_PROPERTIES_WIDGET_HPP

#include "flexlay/objmap_object.hpp"
#include "flexlay/property_bag.hpp"

#include <QWidget>

#include <memory>
#include <vector>

class QFormLayout;
class QLineEdit;
class QLabel;

namespace flexlay {

/**
 * Form editor for an ObjMapObject's name + PropertyBag.
 * Edits write through immediately to the object.
 */
class PropertiesWidget : public QWidget
{
  Q_OBJECT

public:
  explicit PropertiesWidget(QWidget* parent = nullptr);

  void set_object(std::shared_ptr<ObjMapObject> obj);
  std::shared_ptr<ObjMapObject> object() const { return m_object; }

  void clear();
  void refresh();

signals:
  void properties_changed();

private:
  void rebuild_form();

  std::shared_ptr<ObjMapObject> m_object;
  QFormLayout* m_form = nullptr;
  QLabel* m_empty_label = nullptr;
  QLineEdit* m_name_edit = nullptr;
  bool m_block = false;
};

} // namespace flexlay

#endif

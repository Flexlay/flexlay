// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_GUI_PROPERTIES_WIDGET_HPP
#define FLEXLAY_GUI_PROPERTIES_WIDGET_HPP

#include "flexlay/commands/command.hpp"
#include "flexlay/objmap_object.hpp"
#include "flexlay/property_bag.hpp"

#include <QWidget>

#include <functional>
#include <memory>

class QFormLayout;
class QLineEdit;
class QLabel;

namespace flexlay {

/**
 * Form editor for an ObjMapObject's name + PropertyBag.
 * When commit_command is set, edits go through ObjectPropertyCommand (undoable).
 */
class PropertiesWidget : public QWidget
{
  Q_OBJECT

public:
  explicit PropertiesWidget(QWidget* parent = nullptr);

  void set_object(std::shared_ptr<ObjMapObject> obj);
  std::shared_ptr<ObjMapObject> object() const { return m_object; }

  /**
   * If set, property edits are packaged as CommandPtr and passed here
   * (typically EditorMap::execute). If unset, edits apply in-place.
   */
  void set_commit_handler(std::function<void(CommandPtr)> handler)
  {
    m_commit = std::move(handler);
  }

  void clear();
  void refresh();

signals:
  void properties_changed();

private:
  void rebuild_form();
  void commit(CommandPtr cmd);

  std::shared_ptr<ObjMapObject> m_object;
  std::function<void(CommandPtr)> m_commit;
  QFormLayout* m_form = nullptr;
  QLabel* m_empty_label = nullptr;
  QLineEdit* m_name_edit = nullptr;
  bool m_block = false;
};

} // namespace flexlay

#endif

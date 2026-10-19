// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/gui/object_selector_widget.hpp"

#include <QListWidgetItem>

namespace flexlay {

ObjectSelectorWidget::ObjectSelectorWidget(QWidget* parent)
  : QListWidget(parent)
{
  setSelectionMode(QAbstractItemView::SingleSelection);
  connect(this, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
    if (!item) {
      return;
    }
    const int idx = item->data(Qt::UserRole).toInt();
    if (idx >= 0 && idx < static_cast<int>(m_prototypes.size())) {
      emit prototype_activated(m_prototypes[static_cast<size_t>(idx)]);
    }
  });
}

void ObjectSelectorWidget::set_prototypes(std::vector<ObjectPrototype> prototypes)
{
  m_prototypes = std::move(prototypes);
  clear();
  for (size_t i = 0; i < m_prototypes.size(); ++i) {
    auto* item = new QListWidgetItem(QString::fromStdString(m_prototypes[i].label), this);
    item->setData(Qt::UserRole, static_cast<int>(i));
    item->setToolTip(QString::fromStdString(m_prototypes[i].type_id));
  }
  if (!m_prototypes.empty()) {
    setCurrentRow(0);
  }
}

const ObjectPrototype* ObjectSelectorWidget::current_prototype() const
{
  const int row = currentRow();
  if (row < 0 || row >= static_cast<int>(m_prototypes.size())) {
    return nullptr;
  }
  return &m_prototypes[static_cast<size_t>(row)];
}

} // namespace flexlay

// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/gui/layer_selector_widget.hpp"

#include <QListWidgetItem>

namespace flexlay {

LayerSelectorWidget::LayerSelectorWidget(QWidget* parent)
  : QListWidget(parent)
{
  setSelectionMode(QAbstractItemView::SingleSelection);
  connect(this, &QListWidget::itemChanged, this, &LayerSelectorWidget::on_item_changed);
  connect(this, &QListWidget::currentRowChanged, this, &LayerSelectorWidget::on_current_row_changed);
}

void LayerSelectorWidget::set_map(std::shared_ptr<EditorMap> map)
{
  m_map = std::move(map);
  refresh();
}

void LayerSelectorWidget::refresh()
{
  m_block_signals = true;
  clear();
  if (!m_map) {
    m_block_signals = false;
    return;
  }
  int first_tilemap_row = -1;
  const auto& layers = m_map->layers();
  for (size_t i = 0; i < layers.size(); ++i) {
    const auto& layer = layers[i];
    QString label = QString::fromStdString(layer->get_name());
    QString kind;
    if (dynamic_cast<TilemapLayer*>(layer.get())) {
      kind = QStringLiteral("tilemap");
      if (first_tilemap_row < 0) {
        first_tilemap_row = static_cast<int>(i);
      }
    } else if (dynamic_cast<ObjectLayer*>(layer.get())) {
      kind = QStringLiteral("objects");
    } else {
      kind = QStringLiteral("layer");
    }
    if (!kind.isEmpty()) {
      label = QStringLiteral("[%1] %2").arg(kind, label);
    }
    auto* item = new QListWidgetItem(label, this);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    item->setCheckState(layer->is_hidden() ? Qt::Unchecked : Qt::Checked);
    item->setData(Qt::UserRole, static_cast<int>(i));
  }
  m_block_signals = false;
  if (first_tilemap_row >= 0) {
    setCurrentRow(first_tilemap_row);
  }
}

int LayerSelectorWidget::selected_layer_index() const
{
  return currentRow();
}

void LayerSelectorWidget::on_item_changed(QListWidgetItem* item)
{
  if (m_block_signals || !m_map || !item) {
    return;
  }
  const int index = item->data(Qt::UserRole).toInt();
  if (index < 0 || index >= static_cast<int>(m_map->layers().size())) {
    return;
  }
  const bool visible = item->checkState() == Qt::Checked;
  m_map->layers()[static_cast<size_t>(index)]->set_hidden(!visible);
  // layers() returns const ref - problem!
  emit layer_visibility_changed(index, visible);
}

void LayerSelectorWidget::on_current_row_changed(int row)
{
  if (m_block_signals || !m_map || row < 0) {
    return;
  }
  if (row >= static_cast<int>(m_map->layers().size())) {
    return;
  }
  auto layer = m_map->layers()[static_cast<size_t>(row)];
  if (auto tm = std::dynamic_pointer_cast<TilemapLayer>(layer)) {
    emit tilemap_activated(tm);
  } else if (auto ol = std::dynamic_pointer_cast<ObjectLayer>(layer)) {
    emit object_layer_activated(ol);
  }
}

} // namespace flexlay

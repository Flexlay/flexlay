// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_GUI_LAYER_SELECTOR_WIDGET_HPP
#define FLEXLAY_GUI_LAYER_SELECTOR_WIDGET_HPP

#include "flexlay/editor_map.hpp"
#include "flexlay/object_layer.hpp"
#include "flexlay/tilemap_layer.hpp"

#include <QListWidget>

#include <memory>

namespace flexlay {


/**
 * Lists map layers with checkable visibility.
 * Selecting a TilemapLayer emits tilemap_activated with the shared_ptr.
 */
class LayerSelectorWidget : public QListWidget
{
  Q_OBJECT

public:
  explicit LayerSelectorWidget(QWidget* parent = nullptr);

  void set_map(std::shared_ptr<EditorMap> map);
  void refresh();

  /** Index of the currently selected row, or -1. */
  int selected_layer_index() const;

signals:
  void tilemap_activated(std::shared_ptr<TilemapLayer> layer);
  void object_layer_activated(std::shared_ptr<ObjectLayer> layer);
  void layer_visibility_changed(int index, bool visible);

private slots:
  void on_item_changed(QListWidgetItem* item);
  void on_current_row_changed(int row);

private:
  std::shared_ptr<EditorMap> m_map;
  bool m_block_signals = false;
};

} // namespace flexlay

#endif

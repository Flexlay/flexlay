// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_COMMANDS_PAINT_COMMAND_HPP
#define FLEXLAY_COMMANDS_PAINT_COMMAND_HPP

#include "flexlay/commands/command.hpp"
#include "flexlay/tile_brush.hpp"
#include "flexlay/tilemap_layer.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace flexlay {

class PaintCommand : public Command
{
public:
  struct CellChange
  {
    int x = 0;
    int y = 0;
    uint32_t old_id = 0;
    uint32_t new_id = 0;
  };

  explicit PaintCommand(std::shared_ptr<TilemapLayer> layer);

  void paint_cell(int x, int y, uint32_t tile_id);
  /** Stamp brush with top-left at (x, y) in tile coordinates. */
  void paint_brush(int x, int y, const TileBrush& brush);

  bool empty() const { return m_changes.empty(); }

  void execute() override;
  void undo() override;
  void redo() override;
  std::string name() const override { return "Paint"; }

private:
  void record_cell(int x, int y, uint32_t tile_id);

  std::shared_ptr<TilemapLayer> m_layer;
  std::vector<CellChange> m_changes;
  bool m_applied = false;
};

} // namespace flexlay

#endif

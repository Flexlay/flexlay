// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_COMMANDS_OBJECT_TRANSFORM_COMMAND_HPP
#define FLEXLAY_COMMANDS_OBJECT_TRANSFORM_COMMAND_HPP

#include "flexlay/commands/command.hpp"
#include "flexlay/math/point.hpp"
#include "flexlay/math/size.hpp"
#include "flexlay/objmap_rect_object.hpp"

#include <memory>

namespace flexlay {

/** Undoable pos+size change for a single ObjMapRectObject. */
class ObjectTransformCommand : public Command
{
public:
  ObjectTransformCommand(std::shared_ptr<ObjMapRectObject> obj,
                         const Pointf& old_pos, const Sizef& old_size,
                         const Pointf& new_pos, const Sizef& new_size);

  void execute() override;
  void undo() override;
  void redo() override { execute(); }
  std::string name() const override { return "Transform object"; }

  bool changed() const
  {
    return m_old_pos != m_new_pos || m_old_size != m_new_size;
  }

private:
  std::shared_ptr<ObjMapRectObject> m_obj;
  Pointf m_old_pos;
  Sizef m_old_size;
  Pointf m_new_pos;
  Sizef m_new_size;
};

} // namespace flexlay

#endif

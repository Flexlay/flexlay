# Flexlay - A Generic 2D Game Editor
# Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


import math


class WorkspaceMoveTool:

    def __init__(self):
        self.scrolling = false
        self.click_pos = Point(0, 0)
        self.old_trans_offset = Pointf(0, 0)

    def on_mouse_down(self, event):
        print("WorkspaceMoveToolImpl.on_mouse_down")
        self.scrolling = True
        self.old_trans_offset = EditorMapComponent.current().get_gc_state().get_pos()
        self.click_pos = event.mouse_pos
        EditorMapComponent.current().capture_mouse()

    def on_mouse_up(self, event):
        print("WorkspaceMoveToolImpl.on_mouse_up")
        self.scrolling = false
        self.update(event)
        self.old_trans_offset = EditorMapComponent.current().get_gc_state().get_pos()
        EditorMapComponent.current().release_mouse()

    def on_mouse_move(self, event):
        if scrolling:
            self.update(event)

    def update(self, event):
        gc_state = EditorMapComponent.current().get_gc_state()

        sa = math.sin(-gc_state.get_rotation() / 180.0 * math.pi)
        ca = math.cos(-gc_state.get_rotation() / 180.0 * math.pi)

        dx = ca * (click_pos.x - event.mouse_pos.x) - sa * (click_pos.y - event.mouse_pos.y)
        dy = sa * (click_pos.x - event.mouse_pos.x) + ca * (click_pos.y - event.mouse_pos.y)

        gc_state.set_pos(Pointf(old_trans_offset.x
                                + dx / EditorMapComponent.current().get_gc_state().get_zoom(),
                                old_trans_offset.y
                                + dy / EditorMapComponent.current().get_gc_state().get_zoom()))


# EOF #
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


from flexlay.tools import Tool
from flexlay.math import Pointf, Rectf, Rect
from flexlay.gui import EditorMapComponent
from flexlay import Color, ObjectLayer, InputEvent, Workspace, ObjectMoveCommand


class ObjMapSelectTool(Tool):

    STATE_NONE = 0
    STATE_DRAG = 1
    STATE_SELECT = 2

    def __init__(self):
        self.state = ObjMapSelectTool.STATE_NONE
        self.drag_start = Pointf(0, 0)
        self.selection_rect = Rectf(0, 0, 0, 0)
        self.offset = Pointf(0, 0)
        self.move_command = None

        self.control_point = None
        self.selection = None

        self.on_popup_menu_display = None
        self.on_right_click = None

    def clear_selection(self):
        self.selection.clear()
        self.on_selection_change()

    def get_selection(self):
        return self.selection

    def set_selection(self, selection):
        self.selection = selection

    def sig_on_popup_menu_display(self):
        return self.on_popup_menu_display

    def sig_on_right_click(self):
        return self.on_right_click

    def draw(self, gc):
        for obj in self.selection:
            gc.draw_rect(Rect(obj.get_bound_rect()), Color(255, 0, 0))

        if self.state == ObjMapSelectTool.STATE_DRAG:
            pass
        elif self.state == ObjMapSelectTool.STATE_SELECT:
            gc.draw_rect(Rect(self.selection_rect),
                         Color(255, 255, 255))

    def on_mouse_up(self, event):
        print("ObjMapSelectToolImpl.on_mouse_up ", event.kind, event.mouse_pos.x, event.mouse_pos.y)

        objmap = ObjectLayer.current()
        parent = EditorMapComponent.current()
        pos = parent.screen2world(event.mouse_pos)

        if event.kind == InputEvent.MOUSE_LEFT:
            if self.state == ObjMapSelectTool.DRAG_STATE:
                if self.move_command:
                    Workspace.current().get_map().execute(self.move_command)
                    self.move_command = None
                self.state = ObjMapSelectTool.NONE_STATE
                parent.release_mouse()

            elif self.state == ObjMapSelectTool.SELECT_STATE:
                self.state = ObjMapSelectTool.NONE_STATE

                self.selection_rect.right = pos.x
                self.selection_rect.bottom = pos.y
                self.selection_rect.normalize()

                self.selection = objmap.get_selection(self.selection_rect)
                self.on_selection_change()
                parent.release_mouse()

        elif event.kind == InputEvent.MOUSE_RIGHT:
            # GRUMBEL on_right_click(event.mouse_pos.x + parent.get_screen_rect().left,
            #               event.mouse_pos.y + parent.get_screen_rect().top)
            pass

        print("-- Selection: ", self.selection.size())

    def on_mouse_down(self, event):
        print("ObjMapSelectToolImpl.on_mouse_down ", event.kind, event.mouse_pos.x, event.mouse_pos.y)
        objmap = ObjectLayer.current()
        parent = EditorMapComponent.current()
        pos = parent.screen2world(event.mouse_pos)

        if event.kind == InputEvent.MOUSE_LEFT:
            control_point = objmap.find_control_point(pos)

            if control_point:
                self.state = ObjMapSelectTool.DRAG_STATE
                parent.capture_mouse()
                self.offset = pos - control_point.get_pos()
                self.drag_start = pos
            else:
                obj = objmap.find_object(pos)
                if obj:
                    if event.mod & InputEvent.MOD_SHIFT:
                        if obj not in self.selection:
                            self.selection.append(obj)
                        else:
                            self.selection.remove(obj)

                        self.on_selection_change()
                    else:
                        self.state = ObjMapSelectTool.DRAG_STATE
                        parent.capture_mouse()
                        self.offset = pos - obj.get_pos()
                        self.drag_start = pos

                        if obj in self.selection:
                            # Clicked object is not in the selection, so we add it
                            self.selection.clear()
                            objmap.delete_control_points()
                            self.selection.push_back(obj)
                            self.on_selection_change()

                        move_command = ObjectMoveCommand(objmap)
                        for obj in self.selection:
                            move_command.add_obj(obj)
                else:
                    self.state = SELECT_STATE
                    self.selection_rect = Rectf(pos.x, pos.y, pos.x, pos.y)
                    parent.capture_mouse()

        print("-- Selection: ", selection.size())

    def on_mouse_move(self, event):
        print("ObjMapSelectToolImpl.on_mouse_move ", event.kind, event.mouse_pos.x, event.mouse_pos.y)

        parent = EditorMapComponent.current()
        pos = parent.screen2world(event.mouse_pos)

        if state == DRAG:
            if control_point:
                control_point.set_pos(pos - offset)
            else:
                move_command.move_by(pos - drag_start)
                if selection.size() == 1:
                    selection.front().update_control_points()

        elif state == SELECT:
            self.selection_rect.right = pos.x
            self.selection_rect.bottom = pos.y

    def on_selection_change(self):
        objmap = ObjectLayer.current()
        objmap.delete_control_points()

        if selection.size() == 1:
            selection.front().add_control_points()


# EOF #
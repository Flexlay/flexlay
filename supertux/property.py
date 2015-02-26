# Flexlay - A Generic 2D Game Editor
# Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


from PyQt4.QtGui import QPen, QBrush, QPixmap

from flexlay import Colorf, Config
from flexlay.math import Pointf, Rectf
from flexlay.util import get_value_from_tree

from .sprite import SuperTuxSprite

import os.path


class Property:

    def __init__(self, label, identifier, default, optional=False):
        self.label = label
        self.identifier = identifier
        self.value = default
        self.default = default
        self.optional = optional

    def read(self, sexpr):
        self.value = get_value_from_tree([self.identifier, "_"],  sexpr, self.default)

    def write(self, writer):
        if not self.optional or self.value != self.default:
            writer.write(self.identifier, self.value)

    def init_graphics(self, scene):
        pass

    def deinit_graphics(self, scene):
        pass

    def property_dialog(self, dialog):
        pass

    def on_value_change(self, value):
        self.value = value


class BoolProperty(Property):

    def property_dialog(self, dialog):
        dialog.add_bool(self.label, self.value, self.on_value_change)


class IntProperty(Property):

    def __init__(self, label, identifier, default=0, optional=False):
        super().__init__(label, identifier, default, optional)

    def property_dialog(self, dialog):
        dialog.add_int(self.label, self.value, self.on_value_change)


class FloatProperty(Property):

    def __init__(self, label, identifier, default=0.0, optional=False):
        super().__init__(label, identifier, default, optional)

    def property_dialog(self, dialog):
        dialog.add_int(self.label, self.value, self.on_value_change)


class StringProperty(Property):

    def __init__(self, label, identifier, default="", optional=False, translatable=False):
        super().__init__(label, identifier, default, optional)
        self.translatable = translatable

    def read(self, sexpr):
        self.value = get_value_from_tree([self.identifier, "_"],  sexpr, self.default)

    def write(self, writer):
        if not self.optional or self.value != self.default:
            writer.write_string(self.identifier, self.value, translatable=self.translatable)

    def property_dialog(self, dialog):
        dialog.add_string(self.label, self.value, self.on_value_change)


class EnumProperty(Property):

    def __init__(self, label, identifier, default, optional=False, values=None):
        super().__init__(label, identifier, default, optional=optional)

        if values is None:
            values = []

        self.values = values

    def read(self, sexpr):
        value_name = get_value_from_tree([self.identifier, "_"],  sexpr, self.values[self.default])
        try:
            self.value = self.values.index(value_name)
        except:
            raise RuntimeError("%s: invalid enum value: %r not in %r" % (self.identifier, self.value, self.values))

    def write(self, writer):
        if not self.optional or self.value != self.default:
            writer.write_string(self.identifier, self.values[self.value])

    def property_dialog(self, dialog):
        dialog.add_enum(self.label, self.values, self.value, self.on_value_change)


class DirectionProperty(EnumProperty):

    def __init__(self, label, identifier, default):
        super().__init__(label, identifier, default, optional=True, values=["auto", "left", "right"])


class InlinePosProperty:

    def __init__(self):
        self.pos = Pointf(0, 0)

    def read(self, sexpr):
        self.pos.x = get_value_from_tree(["x", "_"],  sexpr, 0.0)
        self.pos.y = get_value_from_tree(["y", "_"],  sexpr, 0.0)

    def write(self, writer):
        writer.write_inline_pointf(self.pos)

    def property_dialog(self, dialog):
        pass


class InlineRectProperty:

    def __init__(self):
        self.rect_item = None
        self.rect = Rectf(0, 0, 0, 0)

    def read(self, sexpr):
        x = get_value_from_tree(["x", "_"],  sexpr, 0.0)
        y = get_value_from_tree(["y", "_"],  sexpr, 0.0)
        width = get_value_from_tree(["width", "_"],  sexpr, 0.0)
        height = get_value_from_tree(["height", "_"],  sexpr, 0.0)

        self.rect.left = x
        self.rect.top = y
        self.rect.set_size(width, height)

    def write(self, writer):
        writer.write_inline_sizef(self.rect.size)
        writer.write_inline_pointf(Pointf(self.rect.left, self.rect.top))

    def init_graphics(self, scene):
        self.rect_item = scene.addRect(self.rect.to_qt(), QPen(), QBrush())

    def deinit_graphics(self, scene):
        scene.removeItem(self.rect_item)
        self.rect_item = None

    def property_dialog(self, dialog):
        pass


class SpriteProperty(StringProperty):

    def __init__(self):
        self.pixmap_item = None

    def init_graphics(self, scene):
        filename = self.value

        sprite = SuperTuxSprite.from_file(os.path.join(Config.current.datadir, filename))
        pixmap = QPixmap.fromImage(sprite.pixelbuffer.get_qimage())
        self.pixmap_item = scene.addPixmap(pixmap)

    def deinit_graphics(self, scene):
        scene.removeItem(self.pixmap_item)
        self.pixmap_item = None


class ImageProperty(StringProperty):

    def init_graphics(self, scene):
        pass

    def deinit_graphics(self, scene):
        pass


class ColorProperty(StringProperty):

    def __init__(self, label, identifier):
        super().__init__(label, identifier, Colorf())

    def init_graphics(self, scene):
        pass

    def deinit_graphics(self, scene):
        pass

    def read(self, sexpr):
        self.value = Colorf(*get_value_from_tree([self.identifier],  sexpr, [1.0, 1.0, 1.0]))

    def write(self, writer):
        writer.write_color(self.identifier, self.value.to_list()[0:3])

    def property_dialog(self, dialog):
        dialog.add_color(self.label, self.value)


class PathProperty:

    class Node:

        mode_values = ['oneshot', 'pingpong', 'circular']

        def __init__(self, x, y, time):
            self.x = x
            self.y = y
            self.time = time

    def __init__(self, label, identifier):
        self.label = label
        self.identifier = identifier
        self.mode = 2
        self.nodes = []

    def read(self, sexpr):
        self.nodes = []

        sexpr = get_value_from_tree([self.identifier], sexpr, [])
        for node in sexpr:
            if node[0] == 'node':
                x = get_value_from_tree(["x", "_"], node[1:], 0)
                y = get_value_from_tree(["y", "_"], node[1:], 0)
                time = get_value_from_tree(["time", "_"], node[1:], 1)
                self.nodes.append(PathProperty.Node(x, y, time))
            elif node[0] == 'mode':
                if node[1] in PathProperty.Node.mode_values:
                    self.mode = PathProperty.Node.mode_values.index(node[1])
                else:
                    raise RuntimeError("unknown enum value %r" % node[1])
            else:
                raise RuntimeError("unknown tag %r" % node[0])

    def write(self, writer):
        if self.nodes:
            writer.begin_list("path")
            if self.mode != 2:
                writer.write_string("mode", PathProperty.Node.mode_values[self.mode])
            for node in self.nodes:
                writer.begin_list("node")
                writer.write_int("x", node.x)
                writer.write_int("y", node.y)
                if node.time != 1:
                    writer.write_float("time", node.time)
                writer.end_list()
            writer.end_list()

    def property_dialog(self, dialog):
        pass


class SampleProperty(StringProperty):

    def __init__(self, label, identifier, default):
        super().__init__(label, identifier, default, optional=True)


class TilemapProperty(StringProperty):

    def __init__(self, label, identifier):
        super().__init__(label, identifier, "", optional=True)


class SectorProperty(StringProperty):

    def __init__(self, label, identifier, default, optional):
        super().__init__(label, identifier, default, optional=optional)


# EOF #

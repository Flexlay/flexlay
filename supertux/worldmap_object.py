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


class WorldmapObject:

    def __init__(self):
        self.obj = None


class WMSpawnPoint(WorldmapObject):

  def __init__(self):
      self.name = ""
      self.obj = ObjMapSpriteObject.new(
          make_sprite($datadir + "images/worldmap/common/tux.png"),
          Pointf.new(0, 0), make_metadata(self))
      connect_v1_ObjMapObject(self.obj.to_object.sig_move(), method(:on_move))

  def on_move(self, data):
    pos = self.obj.to_object.get_pos()
    pos.x = (((pos.x+16)/32).to_i)*32                               
    pos.y = (((pos.y+16)/32).to_i)*32
    self.obj.to_object.set_pos(pos)
  end

  def parse(self, data):
    x = get_value_from_tree(["x", "_"], data, 0)
    y = get_value_from_tree(["y", "_"], data, 0)
    self.obj.to_object.set_pos(Pointf.new(x * 32, y * 32))
    self.name = get_value_from_tree(["name", "_"], data, "")
                                                                               
  def save(self, writer):
    writer.start_list("spawnpoint")
    pos = self.obj.to_object.get_pos()
    writer.write_int("x", pos.x / 32)
    writer.write_int("y", pos.y / 32)
    writer.write_string("name", self.name)
    writer.end_list("spawnpoint")
  end

  def property_dialog(self):
    dialog = $gui.gui.create_generic_dialog("SpawnPoint Property Dialog")
    dialog.add_string("Name", self.name)
    dialog.set_callback(proc{|name|
          self.name = name
      })                                                          


class WorldmapLevel(WorldmapObject):

  def __init__(self):
    self.name = ""
    self.extro_filename = ""
    self.sprite = ""
    self.quit_worldmap = False
    self.obj = ObjMapSpriteObject.new(
            make_sprite($datadir + "images/worldmap/common/leveldot_green.png"),
            Pointf.new(0, 0), make_metadata(self))
    connect_v1_ObjMapObject(self.obj.to_object.sig_move(), method(:on_move))
  end

  def parse(self, data):
    x = get_value_from_tree(["x", "_"], data, 0)
    y = get_value_from_tree(["y", "_"], data, 0)
    self.obj.to_object.set_pos(Pointf.new(x * 32, y * 32))
    self.name = get_value_from_tree(["name", "_"], data, "")
    self.sprite = get_value_from_tree(["sprite", "_"], data, "")
    self.extro_filename = get_value_from_tree(["extro-filename", "_"], data, "")
    self.quit_worldmap = get_value_from_tree(["quit-worldmap", "_"], data, False)
  end

  def save(self, writer):
    writer.start_list("level")
    pos = self.obj.to_object.get_pos()
    writer.write_int("x", pos.x / 32)
    writer.write_int("y", pos.y / 32)
    if self.sprite != ""
      writer.write_string("sprite", self.sprite)
    end
    writer.write_string("name", self.name)
    if self.extro_filename != ""
      writer.write_string("extro-filename", self.extro_filename)
    end
    if self.quit_worldmap == True
      writer.write_bool("quit-worldmap", self.quit_worldmap)
    end
    writer.end_list("level")
  end

  def on_move(self, data):
    pos = self.obj.to_object.get_pos()
    pos.x = (((pos.x+16)/32).to_i)*32
    pos.y = (((pos.y+16)/32).to_i)*32
    self.obj.to_object.set_pos(pos)
  end

  def property_dialog():
    dialog = $gui.gui.create_generic_dialog("LevelTile Property Dialog")
    dialog.add_string("level", self.name)
    dialog.add_string("sprite", self.sprite)
    dialog.add_string("extro-filename", self.extro_filename)
    dialog.add_bool("quit-worldmap", self.quit_worldmap)
    dialog.set_callback(proc{|name, sprite, extro_filename, quit_worldmap|
          self.name = name
          self.sprite = sprite
          self.extro_filename = extro_filename
          self.quit_worldmap = quit_worldmap
        })
  end
end

class SpecialTile(WorldmapObject):

  def __init__(self):
    self.map_message = ""
    self.apply_to_direction = ""
    self.passive_message = False
    self.teleport_x = 0
    self.teleport_y = 0
    self.invisible_tile = False
    self.obj = ObjMapSpriteObject.new(
            make_sprite($datadir + "images/worldmap/common/teleporterdot.png"),
            Pointf.new(0, 0), make_metadata(self))
    connect_v1_ObjMapObject(self.obj.to_object.sig_move(), method(:on_move))

  def parse(self, data):
    x = get_value_from_tree(["x", "_"], data, 0)
    y = get_value_from_tree(["y", "_"], data, 0)
    self.obj.to_object.set_pos(Pointf.new(x * 32, y * 32))
    self.map_message = get_value_from_tree(["map-message", "_"], data, "")
    self.passive_message = get_value_from_tree(["passive-message", "_"], data, False)
    self.teleport_x = get_value_from_tree(["teleport-to-x", "_"], data, -1)
    self.teleport_y = get_value_from_tree(["teleport-to-y", "_"], data, -1)
    self.invisible_tile = get_value_from_tree(["invisible_tile", "_"], data, False)
    self.apply_to_direction = get_value_from_tree(["apply-to-direction", "_"],
        data, "")
  end

  def save(self, writer):
    writer.start_list("special-tile")
    pos = self.obj.to_object.get_pos()
    writer.write_int("x", pos.x / 32)
    writer.write_int("y", pos.y / 32)
    if(self.map_message != "")
      writer.write_string("map-message", self.map_message, True)
    end
    if(self.passive_message != False)
      writer.write_bool("passive-message", self.passive_message)
    end
    if(self.invisible_tile != False)
      writer.write_bool("invisible-tile", self.invisible_tile)
    end
    if(self.apply_to_direction != "")
      writer.write_string("apply-to-direction", self.apply_to_direction)
    end
    if(self.teleport_x != -1)
      writer.write_int("teleport-to-x", self.teleport_x)
      writer.write_int("teleport-to-y", self.teleport_y)
    end
    writer.end_list("special-tile")
  end

  def on_move(self, data):
    pos = self.obj.to_object.get_pos()
    pos.x = (((pos.x+16)/32).to_i)*32
    pos.y = (((pos.y+16)/32).to_i)*32
    self.obj.to_object.set_pos(pos)
  end                                  

  def property_dialog(self):
    dialog = $gui.gui.create_generic_dialog("SpecialTile Property Dialog")
    dialog.add_string("map-message", self.map_message)
    dialog.add_bool("passive-message", self.passive_message)
    dialog.add_bool("invisible-tile", self.invisible_tile)
    dialog.add_string("apply-to-direction", self.apply_to_direction)
    dialog.add_int("teleport-to-x", self.teleport_x)
    dialog.add_int("teleport-to-y", self.teleport_y)
    
    dialog.set_callback(proc{|map_message, passive_message, invisible_tile,
        apply_to_direction, teleport_x, teleport_y|
          self.map_message = map_message
          self.passive_message = passive_message
          self.invisible_tile = invisible_tile
          self.apply_to_direction = apply_to_direction
          self.teleport_x = teleport_x
          self.teleport_y = teleport_y
        })
  end
end

$worldmap_objects = [
  [:level, "images/worldmap/common/leveldot_green.png", WorldmapLevel],
  [:"special-tile", "images/worldmap/common/teleporterdot.png", SpecialTile],
  [:spawnpoint, "images/worldmap/common/tux.png", WMSpawnPoint],
]

def create_worldmapobject_at_pos(objmap, name, pos)
  objectclass = $worldmap_objects.find {|x| x[0] == name}
  if objectclass == None 
    print "Error: Couldn't resolve object type: " , name, "\n"  
    return
  end

  (name, image, _class) = objectclass
  object = _class.new()
  object.obj.to_object.set_pos(pos)
  cmd = ObjectAddCommand.new(objmap)
  cmd.add_object(object.obj.to_object);
  $gui.workspace.get_map().execute(cmd.to_command());
  return object
end

def create_worldmapobject_from_data(objmap, name, sexpr)
  objectclass = $worldmap_objects.find {|x| x[0] == name}
  if objectclass == None 
    print "Error: Couldn't resolve object type: " , name, "\n"
    return
  end

  (name, image, _class) = objectclass
  object = _class.new()
  object.parse(sexpr)
  cmd = ObjectAddCommand.new(objmap)
  cmd.add_object(object.obj.to_object);
  $gui.workspace.get_map().execute(cmd.to_command());
  return object
end


# EOF #
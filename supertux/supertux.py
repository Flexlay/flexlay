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


$config_file = File.expand_path("~/.flexlay/supertux.rb")


def find_supertux_datadir()
    # try to automatically detect the supertux datadir
    possible_locations = [
      "~/cvs/supertux/supertux/data/",
      "~/cvs/supertux/data/",
      "/usr/share/games/supertux/",
      "/usr/local/share/games/supertux/",
      "/opt/supertux/data/",
      "~/projects/supertux/data/",
      "~/projects/supertux/svn/trunk/supertux/data/",
    ]
    # `which supertux`

# Config file loading hack
if File.exist?($config_file):
    require $config_file

BACKGROUND_LAYER = 1
INTERACTIVE_LAYER = 2
FOREGROUND_LAYER = 3

require "flexlay_wrap"
include Flexlay_wrap

require "flexlay.rb"
require "sexpr.rb"

require_relative "gameobj.rb"

$flexlay = Flexlay()

# Tools
$tilemap_paint_tool = TileMapPaintTool()
$tilemap_select_tool = TileMapSelectTool()
$zoom_tool = ZoomTool()
$zoom2_tool = Zoom2Tool()
$workspace_move_tool = WorkspaceMoveTool()
$objmap_select_tool = ObjMapSelectTool()
# $sketch_stroke_tool = SketchStrokeTool()

$mysprite = make_sprite("../data/images/icons16/stock_paste-16.png")

# $console = Console(Rect(Point(50, 100), Size(400, 200)),
#                        $gui.get_component())
# $console.write("Hello World\n");
# $console.write("blabl\n");
# $console.write("blabl\naoeuau\naeouau");

require_relative "gui.rb"


class SuperTuxConfig
    attr_accessor:
        datadir, :
            recent_files

    def __init__(self):
        self.recent_files = []

    def save(self, filename):
        dir = File.expand_path("~/.flexlay/")
        if not File.exists?(dir):
            Dir.mkdir(dir)
        f = File(filename, "w")
        f.write("# Autogenerated Script, don't edit by hand!\n\n")
        f.write("$datadir      = " + $datadir.inspect() + "\n")
        f.write("$recent_files = " + $recent_files.inspect() + "\n")
        f.write("\n# EOF #\n")

$config = SuperTuxConfig()
if !$datadir:
    $datadir = File.expand_path("~/projects/supertux/trunk/supertux/data/") + "/"

require_relative "data.rb"
require_relative "WorldMap.rb"
require_relative "WorldMapObject.rb"
require_relative "TileMap.rb"
require_relative "LispWriter.rb"
require_relative "tileset.rb"
require_relative "level.rb"
require_relative "sector.rb"
require_relative "sprite.rb"
require_relative "util.rb"

$tileset = Tileset(32)
$tileset.load($datadir + "images/tiles.strf")
$tileset.create_ungrouped_tiles_group()

if !$recent_files:
    $recent_files = []

$gui = SuperTuxGUI()
$recent_files.each do | filename|
    $gui.recent_files_menu.add_item(filename, proc{supertux_load_level(filename)})

if ARGV == []:
    Level(100, 50).activate($gui.workspace)
else:
    supertux_load_level(ARGV[0])

# Init the GUI, so that button state is in sync with internal state
$gui.gui_toggle_minimap()
$gui.gui_toggle_minimap()
$gui.gui_show_interactive()
$gui.gui_show_current()
$gui.set_tilemap_paint_tool()

if not File.exist?($datadir):
    dialog = $gui.gui.create_generic_dialog("Specify the SuperTux data directory and restart")
    dialog.add_label("You need to specify the datadir where SuperTux is located")
    dialog.add_string("Datadir:", $datadir)

    dialog.set_block { | datadir |
      $datadir = datadir
    }

$gui.run()

puts "config save: #{$config_file}"
$config.save($config_file)

# EOF #

%module flexlay_wrap
 
%{
#include <ClanLib/Display/color.h>
#include <ClanLib/GUI/component.h>
#include <ClanLib/GUI/button.h>
#include <ClanLib/GUI/window.h>
#include <ClanLib/Core/Math/rect.h>
#include <ClanLib/Core/Math/point.h>
#include "scripting/editor.hxx"
#include "command.hxx"
#include "paint_command.hxx"
#include "object_move_command.hxx"
#include "object_add_command.hxx"
#include "object_delete_command.hxx"
#include "scripting/editor.hxx"
#include "tile.hxx"
#include "tile_brush.hxx"
#include "editor.hxx"
#include "meta_data.hxx"
#include "python_meta_data.hxx"

#include "layer.hxx"
#include "tilemap_layer.hxx"
#include "object_layer.hxx"

#include "minimap.hxx"
#include "editor_map.hxx"
#include "workspace.hxx"
#include "tileset.hxx"
#include "editor_map_component.hxx"
#include "flexlay.hxx"
#include "globals.hxx"
#include "python_functor.hxx"
#include "gui_manager.hxx"
#include "tile_selector.hxx"
#include "object_brush.hxx"
#include "object_selector.hxx"
#include "sexpr_parser.hxx"
#include "icon.hxx"
#include "window.hxx"
#include "panel.hxx"
#include "directory_view.hxx"
#include "menu.hxx"
#include "menubar.hxx"
#include "scrollbar.hxx"
#include "graphic_context_state.hxx"

#include "tilemap_paint_tool.hxx"
#include "tilemap_select_tool.hxx"
#include "objmap_select_tool.hxx"
#include "objmap_sprite_object.hxx"
#include "objmap_object.hxx"
#include "zoom_tool.hxx"

#include "netpanzer.hxx" 
%}

%include "std_string.i"
%include "std_vector.i"
%template(std_vector_int) std::vector<int>;
%template(std_vector_ObjMapObject) std::vector<ObjMapObject>;

%include "clanlib.i"
%include "scripting/editor.hxx"
%include "command.hxx"
%include "paint_command.hxx"
%include "object_move_command.hxx"
%include "object_add_command.hxx"
%include "object_delete_command.hxx"
%include "scripting/editor.hxx"
%include "tile.hxx"
%include "tile_brush.hxx"
%include "editor.hxx"
%include "meta_data.hxx"
%include "python_meta_data.hxx"
 
%include "layer.hxx"
%include "tilemap_layer.hxx"
%include "object_layer.hxx"

%include "editor_map.hxx"
%include "workspace.hxx"
%include "tileset.hxx"
%include "editor_map_component.hxx"
%include "flexlay.hxx"
%include "globals.hxx"
%include "gui_manager.hxx"
%include "tile_selector.hxx"
%include "object_brush.hxx"
%include "object_selector.hxx"
%include "sexpr_parser.hxx"
%include "icon.hxx"
%include "window.hxx"
%include "panel.hxx"
%include "minimap.hxx"
%include "directory_view.hxx"
%include "menu.hxx"
%include "menubar.hxx"
%include "scrollbar.hxx"
%include "tilemap_paint_tool.hxx"
%include "tilemap_select_tool.hxx"
%include "objmap_select_tool.hxx"
%include "objmap_sprite_object.hxx"
%include "objmap_object.hxx"
%include "zoom_tool.hxx" 
%include "graphic_context_state.hxx"

%include "netpanzer.hxx" 


/* EOF */
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


env = Environment()

sources = (Glob("flexlay/*.py") +
           Glob("flexlay/gui/*.py") +
           Glob("flexlay/tools/*.py") +
           Glob("flexlay/math/*.py") +
           Glob("supertux/*.py") +
           Glob("tests/*.py"))

autopep_cmd = env.Command("autopep", sources,
                          "autopep8  --max-line=120  --in-place $SOURCES")
env.Alias("autopep", autopep_cmd)

test_cmd = env.Command("test.phony", None, "python3 -m unittest discover")
env.Alias("test", test_cmd)

flake_cmd = env.Command("flake.phony", sources,
                        "python3 -m flake8.run --max-line-length=120 $SOURCES")
env.Alias("flake", flake_cmd)

for i in sources:
    pylint_cmd = env.Command(str(i) + ".pylint.phony", i, "epylint $SOURCE")
    Alias("pylint", pylint_cmd)

env.Default(flake_cmd)
env.Alias("all", [autopep_cmd, flake_cmd, test_cmd])


# EOF #
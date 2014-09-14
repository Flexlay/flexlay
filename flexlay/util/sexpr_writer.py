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


def write_sexpr(fout, sexpr, indent=0):
    if isinstance(sexpr, list):
        fout.write("(\n")
        for i, e in enumerate(sexpr):
            write_sexpr(fout, e)
            if i != len(sexpr) - 1:
                fout.write(" \n")
        fout.write(")\n")
    else:
        # if sexpr.is_a?(Symbol):
        #   fout.write(str(sexpr))
        # else:
        fout.write(repr(sexpr))


class SExprWriter:

    def __init__(self, fout):
        self.fout = fout
        self.indent_depth = 0

    def write_comment(self, comment):
        self.fout.write(";; " + comment + "\n")

    def start_list(self, listname):
        self.indent()
        self.fout.write("(" + listname + "\n")
        self.indent_depth += 2

    def end_list(self, listname):
        self.indent_depth -= 2
        self.indent()
        self.fout.write(")\n")

    def write_bool(self, name, value):
        self.indent()
        self.fout.write("(" + name + " ")
        if value:
            self.fout.write("#t")
        else:
            self.fout.write("#f")
        self.fout.write(")\n")

    def write_int(self, name, value):
        self.indent()
        self.fout.write("(%s %d)\n" % (name, value))

    def write_float(self, name, value):
        self.indent()
        # %f makes ugly floats, e.g. 45.000000, %s gives nicely
        # trimmed ones, e.g. "45.0"
        self.fout.write("(%s %s)\n" % (name, value))

    def write_string(self, name, value, translatable=False):
        self.indent()
        self.fout.write("(" + name)
        if translatable:
            self.fout.write(" (_ \"" + value + "\"))\n")
        else:
            self.fout.write(" \"" + value + "\")\n")

    def write_vector(self, name, values):
        self.indent()
        self.fout.write("(" + name)
        for i in values:
            self.fout.write(" %r" % i)
        self.fout.write(")\n")

    def indent(self):
        self.fout.write(" " * self.indent_depth)


# EOF #
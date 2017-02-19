# gtkedit

This is a fairly good little editor written in Gtk+2 in C. The editor will compile and build 
(without warning with -Wall -Wextra -pedantic) on Linux with the Gtk+2 installed. The included
Makefile handles the build. This is a companion project to Gtkwrite (with syntax highlighting 
from GtkSourceView 2.10 -- not yet pushed) Both are Licensed under GPLv2.

The editor has the following menu layout and shortcut key layout (if you are comfortable with
the layout of kwrite/kate, you will be right at home with this Gtk variant):

File
  - New                    Ctrl+N
  - Open                   Ctrl+O
  - Save                   Ctrl+S
  - Save As          Shift+Ctrl+A
  - Reload Saved File          F5
  - Page Setup        Shift+Alt+P *
  - Print Preview    Shift+Ctrl+P *
  - Print                  Ctrl+P *
  - Close                  Ctrl+W
  - Quit                   Ctrl+Q

* (options current not complete, or implemented 
   in the Gtkwrite code with GtkSourceView)

Edit
  - Undo                   Ctrl+Z
  - Redo             Shift+Ctrl+Z
  - Copy                   Ctrl+C
  - Cut                    Ctrl+X
  - Paste                  Ctrl+V
  - Find                   Ctrl+F
  - Find & Replace         Ctrl+R
  - Goto Line              Ctrl+G
  - Preferences             Alt+P
  
View
  - Font Selection          Alt+T

Status
  - (just miscellaneous hooks for testing)

Tools
  - Increase Indent        Ctrl+I
  - Decrease Indent  Shift+Ctrl+I
  - Insert File at Cursor...
  - Uppercase              Ctrl+U
  - Lowercase        Shift+Ctrl+U
  - Titlecase         Shift+Alt+U
  - Join Lines             Ctrl+J

Help
  - About

The editor provides a status bar tracking the line/col Ins/Ovr as well as displaying various
operational messages. The file/save status is provided along with the current filename in the
window title. A good deal of effor has been put into the find/replace dialog functionality with
forward/reverse whole-doc and selection search ability. You can perform case sensitive/case
insensitive searches and limit selection/replacement to whole words, etc. The indent/unindent
will align to softtab settings for code layout.

The code is in relative good shape and reasonably commented. There are parts that are newer
than others, in better shape, worse shape, etc. but on balance it should be relatively easy
to follow along. The option handling is done via a central struct that is passed around as
needed. There are several options that can be trimmed as the goal was to store routinely used
information to cut down on the number of function calls Gtk seems to like so much.

All in all, the code builds into a single executable of roughly 70K stripped and builds
fine on Intel/AMD or raspberryPi/ARM.

I recommend using a build directory to maintain a clean source tree. Simply create a 'gtkedit.build' 
directory at the same level as your repository and then simply soft link the source and Makefile in 
the build directory, e.g.:

$ mkdir gtkedit.build && cd gtkedit.build
$ for i in ../gtkedit/*.[ch] Makefile; do ln -s "$i"; done
$ make

Give it a try, help make it better. I'll be adding a repository for the Gtkwrite version 
with GtkSourceView enhancements as time permits. Drop me a line if you have any questions.
(ignore typos in the Readme -- it doesn't go though the compiler :)

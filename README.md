# gtkwrite

This is a good little editor written in Gtk+2 in C created to work with you rather than against you. The editor is written to compile and build (without warnings with -Wall -Wextra -pedantic incorporating the Gnu C11 standard on Linux with Gtk+2 and optionally GtkSourceView2 installed. The included Makefile handles the build and packages are available for [OpenSuSE 13.1 through Leap 42.2](http://download.opensuse.org/repositories/home:/drankinatty/) and the current release and development branch builds are available for Archlinux [gtkwrite](https://aur.archlinux.org/packages/gtkwrite/) and [gtkwrite_git](https://aur.archlinux.org/packages/gtkwrite_git/). The editor can be compiled *with or without* GtkSourceView. Without GtkSourceView it will compile as `gtkedit`. With GtkSourceView it compiles as `gtkwrite` (requires GtkSourceView 2.x). Both gtkedit and gtkwrite compile and run under win32 as well. (the Win-key accelerators are disabled on windows) gtkwrite is Licensed under GPLv2.

### Menu Layout/Use

The editor has the following menu layout and shortcut key layout (if you are comfortable with the layout of KDE3 kwrite/kate, you will be right at home with this Gtk variant):

    File
      - New                           Ctrl+N
      - Open                          Ctrl+O
      + Open Recent File
          - recent_file_1
          - recent_file_2
          - ...
      - Save                          Ctrl+S
      - Save As                 Shift+Ctrl+A
      - Reload Saved File                 F5
      - Page Setup               Shift+Alt+P
      - Print Preview           Shift+Ctrl+P *
      - Print                         Ctrl+P
      - Close                         Ctrl+W
      - Quit                          Ctrl+Q

    Edit
      - Undo                          Ctrl+Z
      - Redo                    Shift+Ctrl+Z
      - Cut                           Ctrl+X
      - Copy                          Ctrl+C
      - Paste                         Ctrl+V
      - Delete
      - Select All                    Ctrl+A
      - Deselect All            Shift+Ctrl+A
      - Find                          Ctrl+F
      - Find & Replace                Ctrl+R
      - Goto Line                     Ctrl+G
      - Preferences                    Alt+P

    View
      - Font Selection                 Alt+T
      - Show/Hide Toolbar             Ctrl+T
      + Toolbar Appearance
          - Text Only
          - Icons Only
          - Text & Icons
      - Current Line Highlight  Shift+Ctrl+H *
      - Syntax Highlight         Shift+Alt+H *
      + Syntax Language                      *
          - None
          + Markup
          + ...
          + Sources
              - Ada
              - C
              - ...
      + Syntax Color Scheme
          - classic
          - cobalt
          - ...
      - Line Numbers                     F11 *

    Tools
      - Increase Indent               Ctrl+I
      - Decrease Indent         Shift+Ctrl+I
      - Indent Fixed Width           Super+I
      - Unindent Fixed Width   Shift+Super+I
      - Insert File at Cursor...
      - Comment                       Ctrl+D
      - Uncomment               Shift+Ctrl+D
      - Uppercase                     Ctrl+U
      - Lowercase               Shift+Ctrl+U
      - Titlecase                Shift+Alt+U
      - Join Lines                    Ctrl+J
      - Word/Char Statistics           Alt+R

    Help
      - About

    * (option in work, or only available with GtkSourceView)

### Features

The editor has a full menubar and toolbar that can be shown/hidden and set to display icons, text or both as desired. The editor provides robust find/replace dialog functionality with forward/reverse whole-document, from cursor and selection-limited search ability with configurable case sensitive/insensitive searches and the ability to limit selection/replacement to whole words. Search and replace terms are remembered between searches. (planned REGEX search/replace remains to be implemented)

Configurable indent/unindent with align to softtab and separate indent/unindent by an absolute number of characters. Default settings provide auto-indent for code layout and smart_backspace (unindent in leading whitespace). Selected sections of code can be commented/uncommented based on the string set in Preferences->Editing->Single-line comment. (autorecognition of file type remains to be implemented). The editor also provides case-conversion, lower-to-upper, upper-to-lower, and title-case. Join lines will join any number of selected lines replacing all intervening whitespace with a single space.

When compiled with GtkSourceView, syntax highlighting, syntax languange selection and selectable syntax style (color schemes) are enabled in the View menu. The provided custom style in 'styles/gtkwrite.xml' provides an alternative to the default styles and works well for C or bash. It is still a style in work. You may install the syles folder in the normal global search location of '/usr/share/gtksourceview-2.0/' or on a per-user basis in '~/.local/share/gtksourceview-2.0/'.

An active window title displays file/save status along with the current filename. A status bar tracks the `line:col INS/OVR` state as well as displaying various operational messages. Print capabilities used the native print dialog making print/print to .pdf, etc. available. Page Setup provides access to the native printer page setup dialog. Current document content statistics are available though the Tools menu via the 'Word/Char Statistics' menuitem to display the number of whitespace, non-whitespace and other character counts, along with the word and line counts for the current contents of the editor. (this is more useful for general text documents rather than for coding).

### Code Layout/Approach

The code is in relatively good shape and reasonably commented. The code is separated into files by functionality. There are parts that are newer than others leaving some areas of the code that may still be tidied up and separated further. The options handling throughout the code is done via a central struct that is passed by parameter as required. The goal being to store routinely used information to cut down on the number of function calls. All in all, the code builds into a single executable of roughly 80K (stripped), 100K otherwise, and builds fine on Intel/AMD or raspberryPi/ARM (tested on Archlinux, openSuSE and Debian).

### Building

I recommend using a build directory to maintain a clean source tree. (although the object files and executables are generated under separate subdirectores, e.g. `obj/` and `bin/` should you choose to build in the source directory) To perform an **out-of-source** build, simply create a 'gtkwrite.build' directory at the same level as your repository and then soft-link the sources and Makefile in the build directory, e.g.:

    $ mkdir gtkwrite.build && cd gtkwrite.build
    $ for i in ../gtkwrite/*.[ch] ../gtkwrite/Makefile; do ln -s "$i"; done
    $ make

The default build is ***without*** GtkSourceView. If you have **GtkSourceView 2.x** installed, simply pass the `with=-DWGTKSOURCEVIEW2` as an argument to make. For example, to build without GtkSourceView, simply type:

    $ make

To enable GtkSourceView and syntax highlighting (with GtkSourceView 2.x installed), build with:

    $ make clean
    $ make with=-DWGTKSOURCEVIEW2

**Note:** you must use `make clean` before calling `make` when switching back and forth between versions ***and*** after each `git pull`.

To build either on win32 and enable the `-subsystem,windows` link option, add

    $ make ss=-DWIN

Both **GtkEdit** and **GtkWrite** will compile with either MinGW or TDM-MinGW compilers. If you choose TDM-MinGW (separately or as part of CodeBlocks for windows), you will need to adjust the Makefile as the windows version of mkdir is used. On either, you will need to softlink `libxml2.dll` to `libxml2-2.dll` to run `gtkwrite`. After installing MinGW, the packages required to compile on windows are:

 * [https://download.gnome.org/binaries/win32/gtk+/2.24/gtk+-bundle_2.24.10-20120208_win32.zip](https://download.gnome.org/binaries/win32/gtk+/2.24/gtk+-bundle_2.24.10-20120208_win32.zip)
 * [https://download.gnome.org/binaries/win32/gtksourceview/2.10/gtksourceview-2.10.0.zip](https://download.gnome.org/binaries/win32/gtksourceview/2.10/gtksourceview-2.10.0.zip)
 * [https://download.gnome.org/binaries/win32/gtksourceview/2.10/gtksourceview-dev-2.10.0.zip](https://download.gnome.org/binaries/win32/gtksourceview/2.10/gtksourceview-dev-2.10.0.zip)
 * [http://xmlsoft.org/sources/win32/libxml2-2.7.8.win32.zip](http://xmlsoft.org/sources/win32/libxml2-2.7.8.win32.zip)

(Msys has `libxml2` available, but it requires adding it to your path if you use it)

Just unzip `gtk+-bundle_2.24.10-20120208_win32.zip` to a directory of your choice. (e.g. `c:\opt\gtk2`), then unzip and add the files from the other packages to the appropriate directores (e.g. `bin, include, lib, etc..`). Don't forget to **add the path to the MinGW and gtk2** `bin` directores to your windows `PATH` variable. (e.g. `rt-click My Computer -> Properties -> Advanced Settings -> Environment Variables` and add the paths to your personal environment (e.g. **the TOP section**) and then start Msys, Command Prompt (cmd.exe) or powershell to compile.

Regardless which OS you are compiling on, the package should compile without any warnings. If you receive a warning, something is wrong, fix it before proceeding. It is currently compiled on Archlinux, openSuSE, debian/raspberryPi, Ubuntu, Win_7 and Win_10.

### Configuration & Custom Style Locations

On Linux, the default location for the configuration file is `$HOME/.config/gtkwrite`. On windows it is `%LOCALAPPDATA%\gtkwrite`.

On Linux system wide syntax color-schemes (styles) are found in `/usr/share/gtksourceview-2.0/styles` and personal styles can be added in `$HOME/.local/share/gtksourceview-2.0/styles`. On win32, the system wide style directory will depend on where you installed the gtksourceview2 files, any personal styles can be placed in `%LOCALAPPDATA%\gtksourceview-2.0\styles`.

Give it a try, help make it better. Drop me a line if you have any questions.

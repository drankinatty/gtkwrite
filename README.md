# gtkwrite

A small fast standalone text editor written in C using Gtk+2 and optionally GtkSourceView2 to provide syntax highlighting. The editor *does not* rely on any third-party editor component such as Scintilla. The included Makefile handles the build (details below) and packages are available for [OpenSuSE 13.1 through Leap 42.3](http://download.opensuse.org/repositories/home:/drankinatty/) and the current release and development branch builds are available for Archlinux [gtkwrite](https://aur.archlinux.org/packages/gtkwrite/) and [gtkwrite-git](https://aur.archlinux.org/packages/gtkwrite-git/). The editor can be compiled *with or without* GtkSourceView. Without GtkSourceView it will compile as `gtkedit`. With GtkSourceView it compiles as `gtkwrite` (requires GtkSourceView 2.x). Both gtkedit and gtkwrite compile and run under RaspberryPi/ARM and windows as well. (the Win-key accelerators are disabled on windows). gtkwrite is Licensed under GPLv2.

### Feature Summary

Features include Open Recent Files listing, Reload Saved File, Print (with print to .pdf provided by native print dialog), Undo/Redo, Select All, robust Find/Replace, Goto Line, Font Selection, Increase/Decreate Indent, Fixed width Indent/Unindent, current line auto-indent, smart home/end, smart backspace, Current Line Highlight, Syntax Highlight, Line Numbering, Comment/Uncomment, Change Case (upper, lower, title), Join Lines, File Statistics (characters, words, whitespace, non-whitespace and line count), on-save trailing whitespace removal and POSIX end-of-line insertion. Custom keypress handlers for `Ctrl+Shift` selection to avoid default handling annoyances. End-of-line detection and conversion between LF, CRLF and CR with persistent setting to use file EOL or Operating System default EOL with prompt for conversion on file open. External File Insertion within an open buffer, Insert of Date/Time both as a menu-iterm & tool-button. GIO File Monitoring for modification by foreign process. Interface provdes menu, configurable toolbar that can be shown/hidden and statusbar. Syntax language and style selection when compiled with GtkSourceView2. Correctly detects all Byte Order Marks, but currently handles only ASCII/UTF-8 character set.

### Menu Layout/Use

The editor has the following menu layout and shortcut key layout (if you are comfortable with the layout of KDE3 kwrite/kate, you will be right at home with this Gtk variant):

    File
      - New                               Ctrl+N
      - Open                              Ctrl+O
      + Open Recent File
          - recent_file_1
          - recent_file_2
          - ...
      - Save                              Ctrl+S
      - Save As                     Shift+Ctrl+A
      - Reload Saved File                     F5
      - Page Setup                   Shift+Alt+P
      - Print Preview               Shift+Ctrl+P *
      - Print                             Ctrl+P
      - Close                             Ctrl+W
      - Quit                              Ctrl+Q

    Edit
      - Undo                              Ctrl+Z
      - Redo                        Shift+Ctrl+Z
      - Cut                               Ctrl+X
      - Copy                              Ctrl+C
      - Paste                             Ctrl+V
      - Delete
      - Select All                        Ctrl+A
      - Deselect All                Shift+Ctrl+A
      - Find                              Ctrl+F
      - Find & Replace                    Ctrl+R
      - Goto Line                         Ctrl+G
      - Preferences                        Alt+P

    View
      - Font Selection                     Alt+T
      - Show/Hide Toolbar                 Ctrl+T
      + Toolbar Appearance
          - Text Only
          - Icons Only
          - Text & Icons
      - Current Line Highlight      Shift+Ctrl+H *
      - Syntax Highlight             Shift+Alt+H *
      + Syntax Language                          *
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
      - Line Numbers                         F11 *

    Tools
      + End of Line Selection
          - Linux / Unix / OSX
          - DOS / Windows
          - Machintosh (Pre-OSX)
      - Increase Indent                   Ctrl+I
      - Decrease Indent             Shift+Ctrl+I
      - Indent Fixed Width               Super+I
      - Unindent Fixed Width       Shift+Super+I
      - Insert File at Cursor...
      - Insert Date/Time at Cursor  Shift+Ctrl+T
      - Comment                           Ctrl+D
      - Uncomment                   Shift+Ctrl+D
      - Uppercase                         Ctrl+U
      - Lowercase                         Ctrl+L
      - Titlecase                    Shift+Alt+U
      - Join Lines                        Ctrl+J
      - Word/Char Statistics               Alt+R

    Help
      - About

    * (option in work, or only available with GtkSourceView)

### Features

The editor has a full menubar and toolbar that can be shown/hidden and set to display icons, text or both as desired. The editor provides robust find/replace dialog functionality with forward/reverse whole-document, from cursor and selection-limited search ability with configurable case sensitive/insensitive searches and the ability to limit selection/replacement to whole words. Search and replace terms are remembered between searches. (planned REGEX search/replace remains to be implemented)

The editor at present operates in a single-document-interface manner where requesting a 'New' document from the File menu will open another copy of the editor in its very own and separate window. Development is working toward a multi-document-interface where multiple files/buffers can be open within a single application window with the current document determined by selecting a document list on the left side of the edit window.

Configurable indent/unindent with align to softtab and separate indent/unindent by an absolute number of characters are provided. Default settings provide auto-indent for code layout and smart_backspace (unindent in leading whitespace). Selected sections of code can be commented/uncommented based on the string set in Preferences->Editing->Single-line comment. (autorecognition of file type and multi-line comemnt characters remain to be implemented). The editor also provides case-conversion, lower-to-upper, upper-to-lower, and title-case. Join lines will join any number of selected lines replacing all intervening whitespace with a single space. Current window size can be saved (if clicked), otherwise the window size on exit is saved for next use if restore window size at start is selected.

A custom keypress input handler for `Ctrl+Shift+Right-Arrow` and `Ctrl+Shift+Left-Arrow` to correct the annoying default gtk handler that will *select-to-end-of-next-word* when used in leading whitespace instead of the desired behavior of *select-to-beginning-of-next-word*. A configuration option in Settings, `Appearance->Cursor & Selection->Ctrl + Shift + Right-Arrow fix`. This enables a corresponding keypress handler for `Ctrl+Shift+Left-Arrow` and small stack to track sequential `Ctrl+Shift+Left/Right` events. This is set on by default. To restore default gtk handling, simply uncheck the box in settings.

End-of-line detection (`LF`, `CRLF` and `CR`) is provided on file-open and can be selected in the `Tools` menu. Default EOL for new files on each operating system except DOS/Windows is `LF` (line-feed), with the DOS/Windows default being `CRLF`. Pre-OSX `CR` will be detected and set on File-Open, but no specific pre-processor check tests for earlier MacOS. After open, or change of any new file, selecting a different EOL (from the `Tools menu` or within `Settings`) will convert the file to the selected EOL. `Settings->File Load/Save->End-of-Line Handling/Selection` allows for a persistent EOL to be selected along with options for `Use EOL from File` or `Use OS Default` (default). When set to anything other than `Use EOL from File`, a prompt on file-open is presented to convert to the persistent EOL setting if the file EOL differs from the persistent setting. Once open, the `Tools->End of Line Selection` operates independent of the persistent setting allowing conversion to/from any of the line endings without affecting the persistent setting.

Byte Order Mark (BOM) detection of all (UTF-8, UTF-16 (BE/LE), UTF-32 (BE/LE), UTF-7, UTF-1, UTF-EBCDIC, SCSU, BOCU-1, and GB-18030), but currently ASCII/UTF-8 is the only encoding handled by the editor. Work is progressing on character set handling for UTF-16/32 as well as on conversion routines between the different encodings.

When compiled with GtkSourceView, syntax highlighting, syntax languange selection and selectable syntax style (color schemes) are enabled in the View menu. The provided custom styles, 'styles/gtkwrite.xml' and 'styles/gtkwrite_light.xml' provide an alternative to the default styles and works well for C or bash. Both styles are continually being refined. You may install the syles folder in the normal global search location of '/usr/share/gtksourceview-2.0/' or on a per-user basis in '~/.local/share/gtksourceview-2.0/'. (for win32, install in the gtk-runtime directory structure) Single line and block-comemnts are automatically set and functional based on the sourceview language. At present all single-line comments (which can be applied to a selected block of text as well) are automatically handled for `ada asp awk c c-sharp cmake cpp desktop diff dosbatch erlang fortran gtkrc haskell java js lua makefile octave perl php python rpmspec ruby sql sh vbnet`. (all multi-line block comment start and end strings are also set, but their use is not yet implementd -- this includes the foregoing and `html objc pascal xml` which have no single-line comment strings)

(*Note:* for builds without GtkSourceView2, the single line comment can be set in Settings and it will be applied with the comment/uncomment commands. The automatic table lookup provided by GtkSourceView2 simple overrides this setting for the current file based on the language)

An active window title displays file/save status along with the current filename. A status bar tracks the `line x/total col: y | INS/OVR | EOL | CharSet` state as well as displaying various operational messages. Print capabilities used the native print dialog making print/print to .pdf, etc. available. Page Setup provides access to the native printer page setup dialog. Current document content statistics are available though the Tools menu via the 'Word/Char Statistics' menuitem to display the number of whitespace, non-whitespace and other character counts, along with the word and line counts for the current contents of the editor. (this is more useful for general text documents rather than for coding).

GIO File Monitoring added to monitor for source modification by a foreign process. File modify, move and delete are currently monitored. Attribute changes are ignored. If a file modification occurs not commanded by the user (e.g. by issuing a file-save or save-as), the user is prompted to either save the current document overwriting any changes or to reload and incorporate the foreign modifications to allow source additions/modifications from the command-line.

### Code Layout/Approach

The code is in relatively good shape and reasonably commented. The code is separated into files by functionality, but a more modular rewrite is planned. There are parts that are newer than others leaving some areas of the code that may still be tidied up and separated further. The options handling throughout the code is done via a central struct that is passed by parameter as required. The goal being to store routinely used information to cut down on the number of function calls. All in all, the code builds into a single executable of roughly 130K (stripped), 170K otherwise, and builds Intel/AMD, raspberryPi/ARM and Windows (tested on Archlinux, openSuSE, Debian, Ubuntu, Windows 7 & 10).

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

Both **GtkEdit** and **GtkWrite** will compile with either MinGW or TDM-MinGW compilers. If you choose TDM-MinGW (separately or as part of CodeBlocks for windows), you will need to adjust the Makefile as the windows version of mkdir is used. After installing MinGW, the packages required to compile on windows are:

 * [https://download.gnome.org/binaries/win32/gtk+/2.24/gtk+-bundle_2.24.10-20120208_win32.zip](https://download.gnome.org/binaries/win32/gtk+/2.24/gtk+-bundle_2.24.10-20120208_win32.zip)
 * [https://download.gnome.org/binaries/win32/gtksourceview/2.10/gtksourceview-2.10.0.zip](https://download.gnome.org/binaries/win32/gtksourceview/2.10/gtksourceview-2.10.0.zip)
 * [https://download.gnome.org/binaries/win32/gtksourceview/2.10/gtksourceview-dev-2.10.0.zip](https://download.gnome.org/binaries/win32/gtksourceview/2.10/gtksourceview-dev-2.10.0.zip)
 * [http://xmlsoft.org/sources/win32/libxml2-2.7.8.win32.zip](http://xmlsoft.org/sources/win32/libxml2-2.7.8.win32.zip)
 * [https://download.gnome.org/binaries/win32/dependencies/libxml2_2.9.0-1_win32.zip](https://download.gnome.org/binaries/win32/dependencies/libxml2_2.9.0-1_win32.zip)

(Msys has `libxml2` available, but it requires adding it to your path if you use it)

Just unzip `gtk+-bundle_2.24.10-20120208_win32.zip` to a directory of your choice. (e.g. `c:\opt\gtk2`), then unzip and add the files from the other packages to the appropriate directores (e.g. `bin, include, lib, etc..`). Don't forget to **add the path to the MinGW and gtk2** `bin` directores to your windows `PATH` variable. (e.g. `rt-click My Computer -> Properties -> Advanced Settings -> Environment Variables` and add the paths to your personal environment (e.g. **the TOP section**) and then start Msys, Command Prompt (cmd.exe) or powershell to compile.

Regardless which OS you are compiling on, the package should compile without any warnings. If you receive a warning, something is wrong, fix it before proceeding. It is currently compiled on Archlinux, openSuSE, debian/raspberryPi, Ubuntu, Win_7 and Win_10. (**note:** on windows, after mingw update to gcc 6.3, a deprecated warning is displayed from the gobject library (due to its use of `1 << 31` not being an integer value) -- it's not our warning, remove -pedantic to suppress)

### Configuration & Custom Style Locations

On Linux, the default location for the configuration file is `$HOME/.config/gtkwrite`. On windows it is `%LOCALAPPDATA%\gtkwrite`.

On Linux system wide syntax color-schemes (styles) are found in `/usr/share/gtksourceview-2.0/styles` and personal styles can be added in `$HOME/.local/share/gtksourceview-2.0/styles`. On win32, the system wide style directory will depend on where you installed the gtksourceview2 files, any personal styles can be placed in `%LOCALAPPDATA%\gtksourceview-2.0\styles`.

On Linux, a `gtkrc-2.0_gtkwrite` file to reduce the button-spacing for GtkInfoBar is installed to /usr/share/gtkwrite.

On Windows, where you install the gtkwrite.exe executable is irrelevant to the operation of the editor. However, until an installer is created, to have the GPL license and logo displayed in help-about you should install to `C:/Program Files/GtkWrite` (on 32-bit versions) or to `C:/Program Files (x86)/GtkWrite` for 64-bit versions. (include the `gtkrc-2.0_gtkwrite`, `gpl-2.0.txt` files and the `img` directory in that the same location)

Give it a try, help make it better. Drop me a line if you have any questions.

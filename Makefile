# application name
APPNAME := gtkedit
SYMNAME := $(APPNAME)_dev
# source/output directories
BINDIR := bin
OBJDIR := obj
SRCDIR := .
# FLAGS	:=
CFLAGS  := -Wall -Wextra -pedantic -finline-functions -std=gnu11 -Ofast
# CFLAGS  := -Wall -Wextra -pedantic -finline-functions -std=gnu11 -O3
CFLAGS  += `pkg-config --cflags gtk+-2.0`
CFLAGS  += -MP -MD
CFLAGS  += $(with)
# auto dependency generation above (remember to add to clean:)
SOURCES	:= $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
# SONMVER	:= 1
# VER	:= $(SONMVER).0
# linking
CCLD    := $(CC)
LDFLAGS := `pkg-config --libs gtk+-2.0`
# LDFLAGS :=
# DEBUGFLAGS	:= -O0 -DDEBUG
# RELEASEFLAGS	:= -O2
ifeq ($(with),-DWGTKSOURCEVIEW2)
CFLAGS  += -I/usr/include/gtksourceview-2.0/
LIBS    := -lgtksourceview-2.0
else
LIBS    :=
endif

$(APPNAME):	$(OBJECTS)
	@mkdir -p $(@D)/bin
	$(CCLD) -o $(BINDIR)/$(SYMNAME) $(OBJECTS) $(CFLAGS) $(LDFLAGS) $(LIBS)
	strip -s -o $(BINDIR)/$(APPNAME) $(BINDIR)/$(SYMNAME)

$(OBJECTS):	$(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# $(OBJDIR)/gtk_common_dlg.o	: gtk_common_dlg.h
# $(OBJDIR)/gtk_findreplace.o	: gtk_findreplace.h
# $(OBJDIR)/gtk_windef.o		: gtk_windef.h gtk_findreplace.h
# $(OBJDIR)/mygtkapp.o		: gtk_windef.h

clean:
	rm -rf $(BINDIR) $(OBJDIR)
#	rm -f bin/mygtkappmk bin/mygtkappmk_final $(OBJECTS)

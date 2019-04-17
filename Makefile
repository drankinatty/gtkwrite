# application name
APPNAME := gtkedit
SYMNAME := $(APPNAME)_dev
# source/output directories
BINDIR := bin
OBJDIR := obj
SRCDIR := .
CFLAGS  := -Wall -Wextra -pedantic -finline-functions -std=gnu11 -Ofast
CFLAGS  += `pkg-config --cflags gtk+-2.0`
CFLAGS  += -MP -MD
CFLAGS  += $(with)
CFLAGS  += $(defn)
# auto dependency generation above (remember to add to clean:)
SOURCES	:= $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
# linking
CCLD    := $(CC)
LDFLAGS := `pkg-config --libs gtk+-2.0`
# conditional naming, includes and libraries
ifeq ($(with),-DWGTKSOURCEVIEW2)
APPNAME := gtkwrite
SYMNAME := $(APPNAME)_dev
CFLAGS	+= `pkg-config --cflags gtksourceview-2.0`
LDFLAGS	+= `pkg-config --libs gtksourceview-2.0`
LIBS    :=
else
LIBS    :=
endif
ifeq ($(ss),-DWIN)
APPNAME := $(APPNAME).exe
SYMNAME := $(SYMNAME).exe
LIBS    += -Wl,-subsystem,windows
endif

$(APPNAME):	$(OBJECTS)
	@mkdir -p $(@D)/bin
	$(CCLD) -o $(BINDIR)/$(SYMNAME) $(OBJECTS) $(CFLAGS) $(LDFLAGS) $(LIBS)
	strip -s -o $(BINDIR)/$(APPNAME) $(BINDIR)/$(SYMNAME)

$(OBJECTS):	$(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(DEFS) -c -o $@ $<

clean:
	rm -rf $(BINDIR) $(OBJDIR)

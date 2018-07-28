#ifndef __completionsv_h__
#define __completionsv_h__  1

#if defined (WGTKSOURCEVIEW2) || defined (WGTKSOURCEVIEW3) || defined (WGTKSOURCEVIEW4)

#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcecompletion.h>
#include <gtksourceview/gtksourcecompletioninfo.h>
#include <gtksourceview/gtksourcecompletionitem.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/completion-providers/words/gtksourcecompletionwords.h>

#include "gtk_appdata.h"

void create_completion (kwinst *app);

#endif

#endif

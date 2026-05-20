#pragma once

#include <adwaita.h>
#include "engine/game-assets.h"

#define SELECT_COMBINATION_TYPE_WINDOW (select_combination_window_get_type())

struct _SelectCombinationWindow
{
	AdwApplicationWindow  parent_instance;

	/* Template widgets */
    GtkListView        *list_view;
};

G_DECLARE_FINAL_TYPE (SelectCombinationWindow, select_combination_window,
                       SELECTCOMBINATION, WINDOW, AdwApplicationWindow)

int select_combination_get_index(void);
void add_combinations(SelectCombinationWindow *self,
                      struct CombinationNode  *node);


G_END_DECLS

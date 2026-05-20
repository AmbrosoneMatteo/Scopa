#include <gtk/gtk.h>
#include "config.h"
#include "client/client.h"

#include "select-combination.h"

#define COMBO_TYPE_ITEM (combo_item_get_type())
G_DECLARE_FINAL_TYPE(ComboItem, combo_item, COMBO, ITEM, GObject)

struct _ComboItem {
    GObject parent;
    guint   index;
    struct CombinationList *list; /* borrowed pointer – owned by the linked list */
};

G_DEFINE_TYPE(ComboItem, combo_item, G_TYPE_OBJECT)

static void combo_item_class_init(ComboItemClass *klass) { (void)klass; }
static void combo_item_init(ComboItem *self)              { (void)self;  }
static void on_combination_clicked(GtkButton *button, gpointer user_data);
int selected_index = 0;

static ComboItem *
combo_item_new(guint index, struct CombinationList *list)
{
    ComboItem *item = g_object_new(COMBO_TYPE_ITEM, NULL);
    item->index = index;
    item->list  = list;
    return item;
}

G_DEFINE_FINAL_TYPE (SelectCombinationWindow, select_combination_window,
                     ADW_TYPE_APPLICATION_WINDOW)

static guint signals[1];

static void
select_combination_window_class_init (SelectCombinationWindowClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class,
                          "/io/github/AmbrosoneMatteo/Scopa/select-combination.ui");

    gtk_widget_class_bind_template_child (widget_class, SelectCombinationWindow, list_view);

    signals[0] = g_signal_new ("return-index",
                                G_TYPE_FROM_CLASS (klass),
                                G_SIGNAL_RUN_LAST,
                                0, NULL, NULL, NULL,
                                G_TYPE_NONE, 0);
}

static GtkWidget *
build_card_row(struct CombinationList *combo_list)
{
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_vexpand(hbox, TRUE);
    gtk_widget_set_hexpand(hbox, TRUE);

    for (struct CombinationList *cl = combo_list; cl != NULL; cl = cl->next) {
        if (cl->node == NULL) continue;

        struct Card *card = cl->node->card;
        char *path;
        int value = card->value;
        char suit = suit_strings[card->suit];
        asprintf(&path, "/io/github/AmbrosoneMatteo/Scopa/images/DalNegro_Cards/%d_%c.png",
                 value, suit);

        GtkWidget *image = gtk_image_new_from_resource(path);

        gtk_widget_set_vexpand (image, true);
        gtk_widget_set_hexpand (image, true);
        gtk_widget_set_vexpand_set (image, true);
        gtk_widget_set_hexpand_set (image, true);
        gtk_image_set_pixel_size ((GtkImage*)image, 160);
        gtk_box_append(GTK_BOX(hbox), image);
    }
    return hbox;
}

static void
on_combination_clicked(GtkButton *button, gpointer user_data)
{
    SelectCombinationWindow *self  = (SelectCombinationWindow *)user_data;
    guint index = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "combo-index"));

    g_print("Combination %u clicked\n", index);
    selected_index = index;
    // If the game is played over the network, the selected combination needs
    // to be sent to the client thread via the dedicated queue
    if(is_network_game){
        int *combo_index = malloc(sizeof(int));
        *combo_index = index;
        g_async_queue_push(player_combo_queue, combo_index);
        gtk_window_destroy(GTK_WINDOW(self));
    }else{
        g_signal_emit(self, signals[0], 0);
    }
}

int
select_combination_get_index(void) {
    return selected_index;
}

static void
factory_setup(GtkListItemFactory *factory,
              GtkListItem        *list_item,
              gpointer            user_data)
{
    (void)factory; (void)user_data;

    /* Wrap the card-image box in a GtkButton so the row is clickable */
    GtkWidget *button = gtk_button_new();
    gtk_button_set_has_frame(GTK_BUTTON(button), FALSE);
    gtk_list_item_set_child(list_item, button);
}

static void
factory_bind(GtkListItemFactory *factory,
             GtkListItem        *list_item,
             gpointer            user_data)
{
    (void)factory;
    g_print("factory_bind called\n");

    SelectCombinationWindow *self = (SelectCombinationWindow *)user_data;

    ComboItem *item   = COMBO_ITEM(gtk_list_item_get_item(list_item));
    GtkWidget *button = gtk_list_item_get_child(list_item);

    /* Replace any previous card-image box with a fresh one */
    GtkWidget *old_child = gtk_button_get_child(GTK_BUTTON(button));
    if (old_child) gtk_button_set_child(GTK_BUTTON(button), NULL);

    GtkWidget *card_row = build_card_row(item->list);
    gtk_button_set_child(GTK_BUTTON(button), card_row);

    /*
     * Store the index on the button so the click handler can read it.
     * g_object_set_data copies nothing – the guint is cast to a pointer.
     */
    g_object_set_data(G_OBJECT(button),
                      "combo-index",
                      GUINT_TO_POINTER(item->index));

    /* Connect click signal (guard against double-connecting on recycle) */
    if (!g_signal_handler_find(button,
                               G_SIGNAL_MATCH_FUNC,
                               0, 0, NULL,
                               on_combination_clicked, self)) {
        g_signal_connect(button, "clicked",
                         G_CALLBACK(on_combination_clicked), self);
    }
}


void add_combinations(SelectCombinationWindow * self, struct CombinationNode * node) {
    /* Build a GListStore that GtkListView can consume */
    GListStore *store = g_list_store_new(COMBO_TYPE_ITEM);

    guint index = 0;
    for (struct CombinationNode *cn = node; cn != NULL; cn = cn->next, ++index) {
        ComboItem *item = combo_item_new(index, cn->list);
        g_list_store_append(store, item);
        g_object_unref(item); /* store holds the ref */
    }

    /* Wire up the factory */
    GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(factory_setup), NULL);
    g_signal_connect(factory, "bind",  G_CALLBACK(factory_bind),  self);

    GtkNoSelection *selection = gtk_no_selection_new(G_LIST_MODEL(store));

    gtk_list_view_set_factory(self->list_view, factory);
    gtk_list_view_set_model(self->list_view, GTK_SELECTION_MODEL(selection));
    g_object_unref(selection);

    g_object_unref(factory);
}

static void
select_combination_window_init (SelectCombinationWindow *self)
{
    gtk_widget_init_template (GTK_WIDGET (self));
    g_assert(self->list_view != NULL);
}

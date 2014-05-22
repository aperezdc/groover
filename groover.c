/*
 * groover.c
 * Copyright (C) 2014 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#ifndef WEB_EXTENSIONS_DIRECTORY
# ifndef PREFIX
#  error Either WEB_EXTENSIONS_DIRECTORY or PREFIX must be defined
# endif /* !PREFIX */
# define WEB_EXTENSIONS_DIRECTORY (PREFIX "/lib/groover")
#endif /* !WEB_EXTENSIONS_DIRECTORY */

#define GROOVER_GRESOURCE(name)  ("/org/perezdecastro/groover/" name)


static void
add_transport_buttons (GtkContainer *container)
{
    const struct {
        const gchar *icon;
        const gchar *action;
    } button_map[] = {
        { "media-skip-backward-symbolic",  "app.previous-song" },
        { "media-playback-start-symbolic", "app.toggle-play"   },
        { "media-skip-forward-symbolic",   "app.next-song"     },
    };

    for (guint i = 0; i < G_N_ELEMENTS (button_map); i++) {
        GtkWidget *button = gtk_button_new_from_icon_name (button_map[i].icon,
                                                           GTK_ICON_SIZE_BUTTON);
        gtk_actionable_set_action_name (GTK_ACTIONABLE (button),
                                        button_map[i].action);
        gtk_container_add (container, button);
    }
}


static void
application_window_setup_header_bar (GtkWindow *window)
{
    GtkWidget *header = gtk_header_bar_new ();
    gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (header), TRUE);
    gtk_header_bar_set_has_subtitle (GTK_HEADER_BAR (header), TRUE);
    gtk_header_bar_set_title (GTK_HEADER_BAR (header), "Groover");

    gtk_window_set_titlebar (window, header);

    GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    add_transport_buttons (GTK_CONTAINER (hbox));
    gtk_header_bar_pack_start (GTK_HEADER_BAR (header), hbox);
}


static GtkWidget *
application_window_new (GtkApplication *application)
{
    GtkWidget *window = gtk_application_window_new (application);
    gtk_window_set_title (GTK_WINDOW (window), "Groover");
    gtk_window_set_has_resize_grip (GTK_WINDOW (window), TRUE);
    // TODO: Save and restore window sizes
    gtk_widget_set_size_request (window, 900, 680);
    application_window_setup_header_bar (GTK_WINDOW (window));

    GtkWidget *web_view = webkit_web_view_new ();
    // TODO: Save the server URL in a setting
    webkit_web_view_load_uri (WEBKIT_WEB_VIEW (web_view),
                              "http://127.0.0.1:16242");

    gtk_container_add (GTK_CONTAINER (window), web_view);
    return window;
}


static void
toggle_play_action_activated (GSimpleAction *action,
                              GVariant      *parameter,
                              gpointer       userdata)
{
    // TODO: Implement
}


static void
about_action_activated (GSimpleAction *action,
                        GVariant      *parameter,
                        gpointer       userdata)
{
    static const gchar *authors[] = {
        "Adrián Pérez de Castro",
        NULL,
    };

    gtk_show_about_dialog (NULL,
                           "authors",        authors,
                           "logo-icon-name", "gnome-music",
                           "license-type",   GTK_LICENSE_MIT_X11,
                           "comments",       "A simple Groove Basin remote using its web-based UI",
                           "website",        "https://github.com/aperezdc/groover",
                           NULL);
}


static void
quit_action_activated (GSimpleAction *action,
                        GVariant      *parameter,
                        gpointer       userdata)
{
    g_application_quit (G_APPLICATION (userdata));
}


static const GActionEntry app_actions[] = {
    { "toggle-play", toggle_play_action_activated, NULL, NULL, NULL },
    { "about",       about_action_activated,       NULL, NULL, NULL },
    { "quit",        quit_action_activated,        NULL, NULL, NULL },
};


static void
application_started (GtkApplication *application)
{
    WebKitWebContext *context = webkit_web_context_get_default ();
    webkit_web_context_set_process_model (context,
        WEBKIT_PROCESS_MODEL_SHARED_SECONDARY_PROCESS);
    webkit_web_context_set_web_extensions_directory (
        context, WEB_EXTENSIONS_DIRECTORY);

    gtk_window_set_default_icon_name ("gnome-music");
    g_object_set (gtk_settings_get_default (),
                  "gtk-application-prefer-dark-theme",
                  TRUE, NULL);

    g_action_map_add_action_entries (G_ACTION_MAP (application), app_actions,
                                     G_N_ELEMENTS (app_actions), application);

    GtkBuilder *builder = gtk_builder_new_from_resource (GROOVER_GRESOURCE ("menus.xml"));
    gtk_application_set_app_menu (application,
        G_MENU_MODEL (gtk_builder_get_object (builder, "app-menu")));
    g_object_unref (builder);

    const struct {
        const gchar *action;
        const gchar *accel;
        GVariant    *param;
    } accel_map[] = {
        { "app.toggle-play",   "<Ctrl>m",      NULL },
        { "app.previous-song", "<Ctrl>comma",  NULL },
        { "app.next-song",     "<Ctrl>period", NULL },
    };

    for (guint i = 0; i < G_N_ELEMENTS (accel_map); i++) {
        gtk_application_add_accelerator (application,
                                         accel_map[i].accel,
                                         accel_map[i].action,
                                         accel_map[i].param);
    }
}


static void
application_activated (GtkApplication *application)
{
    gtk_widget_show_all (application_window_new (application));
}


static const gchar *
get_application_id (void)
{
    static const gchar *app_id = NULL;
    if (!app_id) {
        if (!(app_id = g_getenv ("GROOVER_APPLICATION_ID")))
            app_id = "org.perezdecastro.groover";
    }
    return app_id;
}


int
main (int argc, char **argv)
{
    GtkApplication *application =
        gtk_application_new (get_application_id (),
                             G_APPLICATION_FLAGS_NONE);

    g_signal_connect (G_OBJECT (application), "startup",
                      G_CALLBACK (application_started), NULL);
    g_signal_connect (G_OBJECT (application), "activate",
                      G_CALLBACK (application_activated), NULL);

    gint status = g_application_run (G_APPLICATION (application), argc, argv);
    g_object_unref (application);
    return status;
}

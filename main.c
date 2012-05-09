#include <stdlib.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>

static gboolean delete(GtkWindow*, GdkEvent*);

gboolean
delete(GtkWindow *w, GdkEvent *e) {
    (void)w; (void)e;

    gtk_main_quit();
    return TRUE;
}

int
main(int argc, char *argv[]) {
    GtkWindow *win;
    WebKitWebView *web;
    GtkScrolledWindow *scroll;

    if(argc < 2)
        return EXIT_FAILURE;
    gtk_init(&argc, &argv);
    win = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    scroll = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    web = WEBKIT_WEB_VIEW(webkit_web_view_new());
    g_signal_connect(win, "delete_event", G_CALLBACK(delete), NULL);
    gtk_container_add(GTK_CONTAINER(scroll), GTK_WIDGET(web));
    gtk_container_add(GTK_CONTAINER(win), GTK_WIDGET(scroll));
    gtk_widget_grab_focus(GTK_WIDGET(web));
    gtk_widget_show_all(GTK_WIDGET(win));
    webkit_web_view_load_uri(web, argv[1]);
    gtk_main();
    gtk_widget_destroy(GTK_WIDGET(web));
    gtk_widget_destroy(GTK_WIDGET(scroll));
    gtk_widget_destroy(GTK_WIDGET(win));
    return EXIT_SUCCESS;
}

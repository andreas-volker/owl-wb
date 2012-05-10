Win*
win_create(void) {
    Win *w;

    w = emalloc(sizeof(Win));
    w->win = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    w->scroll = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    w->web = WEBKIT_WEB_VIEW(webkit_web_view_new());
    data.wins = g_list_append(data.wins, w);
    gtk_container_add(GTK_CONTAINER(w->scroll), GTK_WIDGET(w->web));
    gtk_container_add(GTK_CONTAINER(w->win), GTK_WIDGET(w->scroll));
    event_init(w);
    gtk_scrolled_window_set_policy(w->scroll,
                                   GTK_POLICY_NEVER, GTK_POLICY_NEVER);
    gtk_widget_show_all(GTK_WIDGET(w->win));
    return w;
}

void
win_destroy(void *ptr) {
    Win *w;
    GList *l;

    w = (Win*)ptr;
    if(!(l = g_list_find(data.wins, w)))
        return;
    data.wins = g_list_remove_link(data.wins, l);
    g_list_free_1(l);
    gtk_widget_destroy(GTK_WIDGET(w->web));
    gtk_widget_destroy(GTK_WIDGET(w->scroll));
    gtk_widget_destroy(GTK_WIDGET(w->win));
    free(w);
    if(!data.wins)
        gtk_main_quit();
}

void
win_load(Win *w, char *str) {
    char *c, *scheme, buf[BUFSIZ];

    c = estrdup(!str ? "ddg.gg" : str);
    scheme = g_file_test(c, G_FILE_TEST_EXISTS) ? "file://" :
            (!strstr(c, ":") ? "http://" : "");
    sprintf(buf, "%s%s", scheme, c);
    webkit_web_view_load_uri(w->web, buf);
    free(c);
}

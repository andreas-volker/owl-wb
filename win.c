Win*
win_create(void) {
    Win *w;
    GdkGeometry g;
    GtkPolicyType h, v;

    w = emalloc(sizeof(Win));
    w->win = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    w->scroll = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    w->web = WEBKIT_WEB_VIEW(webkit_web_view_new());
    w->ignore = w->zoom = FALSE;
    g.min_width = g.min_height = 1;
    data.wins = g_list_append(data.wins, w);
    webkit_web_view_set_full_content_zoom(w->web, TRUE);
    webkit_web_view_set_zoom_level(w->web, ZOOM);
    gtk_window_set_geometry_hints(w->win, NULL, &g, GDK_HINT_MIN_SIZE);
    gtk_window_set_default_size(w->win, WIDTH, HEIGHT);
    gtk_container_add(GTK_CONTAINER(w->scroll), GTK_WIDGET(w->web));
    gtk_container_add(GTK_CONTAINER(w->win), GTK_WIDGET(w->scroll));
    gtk_widget_show_all(GTK_WIDGET(w->win));
    gdk_window_set_events(GWIN(w), GDK_PROPERTY_CHANGE_MASK);
    gdk_window_add_filter(GWIN(w), atom_propertynotify, w);
    event_init(w);
    h = SHOW_HSCROLL ? GTK_POLICY_AUTOMATIC : GTK_POLICY_NEVER;
    v = SHOW_VSCROLL ? GTK_POLICY_AUTOMATIC : GTK_POLICY_NEVER;
    gtk_scrolled_window_set_policy(w->scroll, h, v);
    gtk_widget_grab_focus(GTK_WIDGET(w->web));
    return w;
}

void
win_destroy(void *ptr) {
    Win *w;
    GList *l;

    w = (Win*)ptr;
    gtk_widget_destroy(GTK_WIDGET(w->web));
    gtk_widget_destroy(GTK_WIDGET(w->scroll));
    gtk_widget_destroy(GTK_WIDGET(w->win));
    if((l = g_list_find(data.wins, w))) {
        data.wins = g_list_remove_link(data.wins, l);
        g_list_free_1(l);
    }
    free(w);
    if(!data.wins)
        gtk_main_quit();
}

void
win_find(Win *w, gboolean b) {
    char buf[BUFSIZ];
    unsigned char *p;

    webkit_web_view_unmark_text_matches(w->web);
    if(!atom_get(w, atoms[_OWL_FIND], XA_STRING, sizeof(buf), &p) || !p)
        return;
    memset(&buf, '\0', sizeof(buf));
    strncpy(buf, (char*)p, sizeof(buf));
    XFree(p);
    webkit_web_view_mark_text_matches(w->web, buf, CASEFIND, 0);
    webkit_web_view_set_highlight_text_matches(w->web, TRUE);
    webkit_web_view_search_text(w->web, buf, CASEFIND, b, TRUE);
}

void
win_load(Win *w) {
    char *c, *s, buf[BUFSIZ];
    unsigned int i;
    unsigned char *p;
    struct { char *id, *c; } alias[] = {
        { "wiki",   "%s.wikipedia.org"          },
        { "ddg",    "duckduckgo.com/?q=%s"      },
        { "git",    "github.com/andreas-volker" },
    };

    if(!atom_get(w, atoms[_OWL_URI], XA_STRING, sizeof(buf), &p))
        return;
    c = !p ? "about:blank" : (!strcmp((char*)p, "") ? HOMEPAGE : (char*)p);
    s = estrdup(c);
    for(i = 0; s[i] && s[i] != ' '; i++);
    s[i] = '\0';
    c = strstr(c, ":") || strstr(c, "/") || strstr(c, ".") ? g_strdup(c) :
        g_strdup_printf(SEARCH, c);
    XFree(p);
    for(i = 0; i < LEN(alias); i++) {
        if(strcmp(s, alias[i].id) != 0)
            continue;
        g_free(c);
        c = strchr(s, '\0');
        c = strstr(alias[i].c, "%s") ? g_strdup_printf(alias[i].c, ++c) :
            g_strdup(alias[i].c);
    }
    free(s);
    s = g_file_test(c, G_FILE_TEST_EXISTS) ? "file://" :
        (!strstr(c, ":") ? "http://" : "");
    snprintf(buf, sizeof(buf), "%s%s", s, c);
    g_free(c);
    webkit_web_view_load_uri(w->web, buf);
}

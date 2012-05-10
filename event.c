void
event_init(Win *w) {
    g_signal_connect(G_OBJECT(webkit_web_view_get_main_frame(w->web)),
    "scrollbars-policy-changed",    G_CALLBACK(scrollbars), NULL);
    g_object_connect(G_OBJECT(w->win),
    "signal::delete_event",         G_CALLBACK(delwin),     w, NULL);
    g_object_connect(G_OBJECT(w->scroll),
    "signal::key-press-event",      G_CALLBACK(keypress),   w, NULL);
    g_object_connect(G_OBJECT(w->web),
    "signal::notify::load-status",  G_CALLBACK(status),     w,
    "signal::create-web-view",      G_CALLBACK(newwin),  NULL,
    "signal::console-message",      G_CALLBACK(message), NULL, NULL);
}

gboolean
delwin(GtkWidget *gw, GdkEvent *e, Win *w) {
    (void)gw; (void)e;

    win_destroy(w);
    return TRUE;
}

void
grabdom(GObject *o, WebKitDOMEvent *e, void *ptr) {
    (void)o; (void)e;

    gtk_widget_grab_focus(GTK_WIDGET(ptr));
}

gboolean
keypress(GtkWidget *gw, GdkEvent *e, Win *w) {
    static gboolean b = FALSE;
    (void)gw;

    if(!(e->key.state & GDK_CONTROL_MASK))
        return FALSE;
    b = !b;
    gtk_widget_grab_focus(b ? GTK_WIDGET(w->scroll) : GTK_WIDGET(w->web));
    return TRUE;
}

gboolean
message(WebKitWebView *v, char *msg, int line, char *src) {
    (void)v; (void)msg; (void)line; (void)src;

    return TRUE;
}

WebKitWebView*
newwin(WebKitWebView *v, WebKitWebFrame *f) {
    Win *w;
    (void)v; (void)f;

    w = win_create();
    return w->web;
}

gboolean
scrollbars(WebKitWebFrame *f) {
    (void)f;

    return TRUE;
}

void
status(GObject *o, GParamSpec *ps, Win *w) {
    unsigned long i, len;
    WebKitDOMNode *n;
    WebKitLoadStatus s;
    WebKitDOMDocument *d;
    WebKitDOMNodeList *e;
    (void)o; (void)ps;

    s = webkit_web_view_get_load_status(w->web);
    if(s == WEBKIT_LOAD_FINISHED) {
        d = webkit_web_view_get_dom_document(w->web);
        e = webkit_dom_document_get_elements_by_tag_name(d, "*");
        len = webkit_dom_node_list_get_length(e);
        for(i = 0; i < len; i++) {
            n = webkit_dom_node_list_item(e, i);
            if(!WEBKIT_DOM_IS_HTML_INPUT_ELEMENT(n))
                continue;
            webkit_dom_event_target_add_event_listener(
                WEBKIT_DOM_EVENT_TARGET(n), "focus",
                G_CALLBACK(grabdom), FALSE, w->web);
            webkit_dom_event_target_add_event_listener(
                WEBKIT_DOM_EVENT_TARGET(n), "blur",
                G_CALLBACK(grabdom), FALSE, w->scroll);
        }
        g_object_unref(G_OBJECT(e));
    } else if(s == WEBKIT_LOAD_COMMITTED)
        gtk_widget_grab_focus(GTK_WIDGET(w->web));
}

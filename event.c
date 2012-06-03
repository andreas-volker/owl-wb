void
event_init(Win *w) {
    g_signal_connect(G_OBJECT(webkit_web_view_get_main_frame(w->web)),
    "scrollbars-policy-changed",    G_CALLBACK(scrollbars), NULL);
    g_signal_connect(G_OBJECT(data.jar), "changed", G_CALLBACK(cookie), NULL);
    g_object_connect(G_OBJECT(w->win),
    "signal::delete_event",         G_CALLBACK(delwin),     w,
    "signal::size-allocate",        G_CALLBACK(resize),     w, NULL);
    g_object_connect(G_OBJECT(w->scroll),
    "signal::key-press-event",      G_CALLBACK(keypress),   w, NULL);
    g_object_connect(G_OBJECT(w->web),
    "signal::notify::load-status",  G_CALLBACK(status),     w,
    "signal::create-web-view",      G_CALLBACK(newwin),  NULL,
    "signal::console-message",      G_CALLBACK(message), NULL, NULL);
}

void
cookie(SoupCookieJar *j, SoupCookie *old, SoupCookie *new) {
    (void)j;

    soup_cookie_set_max_age((new ? new : old), SOUP_COOKIE_MAX_AGE_ONE_YEAR);
}
gboolean
delwin(GtkWidget *gw, GdkEvent *e, void *ptr) {
    (void)gw; (void)e;

    win_destroy((Win*)ptr);
    return TRUE;
}

void
grabdom(GObject *o, WebKitDOMEvent *e, void *ptr) {
    (void)o; (void)e;

    gtk_widget_grab_focus(GTK_WIDGET(ptr));
}

gboolean
keypress(GtkWidget *gw, GdkEvent *e, void *ptr) {
    int i;
    char a[7], *c;
    GdkKeymap *km;
    unsigned int kv;
    GdkKeymapKey *kk;
    (void)gw;

    km = gdk_keymap_get_default();
    gdk_keymap_get_entries_for_keyval(km, e->key.keyval, &kk, &i);
    kk[0].level = 0;
    kk[0].keycode = e->key.hardware_keycode;
    kv = gdk_keymap_lookup_key(km, kk);
    c = gdk_keyval_name(kv);
    i = g_unichar_to_utf8(gdk_keyval_to_unicode(kv), a);
    a[i] = '\0';
    for(i = 0; i < (int)LEN(keys); i++)
        if((!strcmp(keys[i].key, c) || !strcmp(keys[i].key, a)) &&
           keys[i].func && ((e->key.state & ~LOCK & MOD) == keys[i].mod) &&
           (!((Win*)ptr)->ignore || keys[i].func == keyfocus)) {
            keys[i].func((Win*)ptr, &(keys[i].arg));
            return TRUE;
        }
    return FALSE;
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

void
resize(GtkWidget *gw, GdkRectangle *r, void *ptr) {
    float f;
    (void)gw;

    if(((Win*)ptr)->zoom)
        return;
    f = CLAMP(r->width * 0.0015, 0.1, ZOOM);
    webkit_web_view_set_zoom_level(((Win*)ptr)->web, f);
}

gboolean
scrollbars(WebKitWebFrame *f) {
    (void)f;

    return TRUE;
}

void
status(GObject *o, GParamSpec *ps, void *ptr) {
    Win *w;
    unsigned long i, len;
    WebKitDOMNode *n;
    WebKitLoadStatus s;
    WebKitDOMDocument *d;
    WebKitDOMNodeList *e;
    (void)o; (void)ps;

    w = (Win*)ptr;
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

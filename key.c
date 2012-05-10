#define CTRL GDK_CONTROL_MASK
#define SHIFT GDK_SHIFT_MASK
static Key keys[] = {
    { 0,    "u",    keyprevnext,    { .i = -1       }},
    { 0,    "i",    keyreload,      { .b = FALSE    }},
    { SHIFT,"i",    keyreload,      { .b = TRUE     }},
    { 0,    "m",    keyprevnext,    { .i = +1       }},
    { 0,    ",",    keystop,        { 0             }},
};

void
keyprevnext(Win *w, Arg *a) {
    webkit_web_view_go_back_or_forward(w->web, a->i);
}

void
keyreload(Win *w, Arg *a) {
    if(a->b)
        webkit_web_view_reload_bypass_cache(w->web);
    else
        webkit_web_view_reload(w->web);
}

void
keystop(Win *w, Arg *a) {
    (void)a;

    webkit_web_view_stop_loading(w->web);
}

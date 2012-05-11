#define URI  "`printf \"%s\\n%s\" $owlhome $owluri | dmenu`"
#define FIND "`xprop -id $owlxid _OWL_FIND | grep '\"' | cut -d '\"' -f 2 | "\
             "dmenu`"
#define SET(a, b) "str=\""a"\" && xprop -id $owlxid -f "b" 8s -set "b" \"$str\""
#define DEL(x) "xprop -id $owlxid -remove "x""

#define CTRL (GDK_CONTROL_MASK)
#define SHIFT (GDK_SHIFT_MASK)
static Key keys[] = {
    { 0,            "u",    keyprevnext,    { .i = -1                       }},
    { 0,            "i",    keyreload,      { .b = FALSE                    }},
    { SHIFT,        "i",    keyreload,      { .b = TRUE                     }},
    { 0,            "o",    keyfind,        { .b = TRUE                     }},
    { SHIFT,        "o",    keyfind,        { .b = FALSE                    }},
    { CTRL,         "o",    keyexec,        { .c = SET(FIND, "_OWL_FIND")   }},
    { CTRL|SHIFT,   "o",    keyexec,        { .c = DEL("_OWL_FIND")         }},
    { 0,            "f",    keyexec,        { .c = SET(URI, "_OWL_URI")     }},
    { CTRL|SHIFT,   "f",    keyexec,        { .c = DEL("_OWL_URI")          }},
    { 0,            "m",    keyprevnext,    { .i = +1                       }},
    { 0,            ",",    keystop,        { 0                             }},
};
#undef URI
#undef FIND
#undef DEL
#undef SET

void
keyexec(Win *w, Arg *a) {
    char buf[BUFSIZ];

    snprintf(buf, sizeof(buf), "owlxid=\"%u\" && owluri=\"%s\" && "
             "owlhome=\"%s\" && %s", (unsigned int)XWIN(w),
             webkit_web_view_get_uri(w->web), HOMEPAGE, a->c);
    system(buf);
}

void
keyfind(Win *w, Arg *a) {
    win_find(w, a->b);
}

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

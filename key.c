enum { LEFT = -4, RIGHT, UP, DOWN };

#define URI  "`printf \"%s\\n%s\" $owlhome $owluri | dmenu`"
#define FIND "`xprop -id $owlxid _OWL_FIND | grep '\"' | cut -d '\"' -f 2 | "\
             "dmenu`"
#define SET(a, b) "str=\""a"\" && xprop -id $owlxid -f "b" 8s -set "b\
                  " \"$str\""
#define DEL(x) "xprop -id $owlxid -remove "x""

#define CTRL (GDK_CONTROL_MASK)
#define SHIFT (GDK_SHIFT_MASK)
static Key keys[] = {
    { 0,            "0",    keyscroll,      { .i = 0                        }},
    { 0,            "1",    keyscroll,      { .i = 11                       }},
    { 0,            "2",    keyscroll,      { .i = 22                       }},
    { 0,            "3",    keyscroll,      { .i = 33                       }},
    { 0,            "4",    keyscroll,      { .i = 44                       }},
    { 0,            "5",    keyscroll,      { .i = 55                       }},
    { 0,            "6",    keyscroll,      { .i = 66                       }},
    { 0,            "7",    keyscroll,      { .i = 77                       }},
    { 0,            "8",    keyscroll,      { .i = 88                       }},
    { 0,            "9",    keyscroll,      { .i = 99                       }},
    { 0,            "u",    keyprevnext,    { .i = -1                       }},
    { 0,            "i",    keyreload,      { .b = FALSE                    }},
    { SHIFT,        "i",    keyreload,      { .b = TRUE                     }},
    { 0,            "o",    keyfind,        { .b = TRUE                     }},
    { SHIFT,        "o",    keyfind,        { .b = FALSE                    }},
    { CTRL,         "o",    keyexec,        { .c = SET(FIND, "_OWL_FIND")   }},
    { CTRL|SHIFT,   "o",    keyexec,        { .c = DEL("_OWL_FIND")         }},
    { 0,            "f",    keyexec,        { .c = SET(URI, "_OWL_URI")     }},
    { SHIFT,        "f",    keynewwin,      { 0                             }},
    { CTRL|SHIFT,   "f",    keyexec,        { .c = DEL("_OWL_URI")          }},
    { 0,            "h",    keyscroll,      { .i = LEFT                     }},
    { 0,            "j",    keyscroll,      { .i = DOWN                     }},
    { 0,            "k",    keyscroll,      { .i = UP                       }},
    { 0,            "l",    keyscroll,      { .i = RIGHT                    }},
    { 0,            "m",    keyprevnext,    { .i = +1                       }},
    { 0,            ",",    keystop,        { 0                             }},
};
#undef CTRL
#undef SHIFT
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
keynewwin(Win *w, Arg *a) {
    Win *n;
    (void)w; (void)a;

    n = win_create();
    XChangeProperty(data.dpy, XWIN(n), atoms[_OWL_URI], XA_STRING, 8,
                    PropModeReplace, (unsigned char*)"", 1);
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
keyscroll(Win *w, Arg *a) {
    double d, v, u;
    GtkAdjustment *adj;

    adj = a->i == LEFT || a->i == RIGHT ?
          gtk_scrolled_window_get_hadjustment(w->scroll) :
          gtk_scrolled_window_get_vadjustment(w->scroll);
    v = gtk_adjustment_get_value(adj);
    u = gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj);
    d = a->i == RIGHT || a->i == DOWN ? v + STEP :
        (a->i == LEFT || a->i == UP   ? v - STEP : (u / 99) * a->i);
    d = CLAMP(d, 0, u);
    gtk_adjustment_set_value(adj, d);
}

void
keystop(Win *w, Arg *a) {
    (void)a;

    webkit_web_view_stop_loading(w->web);
}

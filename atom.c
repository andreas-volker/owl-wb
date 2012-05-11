enum { _OWL_URI, _OWL_FIND, LASTAtom };

static Atom atoms[LASTAtom];

void
atom_init(void) {
    unsigned int i;

    for(i = 0; i < LASTAtom; i++)
        atoms[i] = XInternAtom(data.dpy, atom_str(i), False);
}

gboolean
atom_get(Win *w, Atom prop, Atom type, long l, unsigned char **p) {
    int di;
    Atom da;
    unsigned long dl;

    return(XGetWindowProperty(data.dpy, XWIN(w), prop, 0L, l, False, type, &da,
                              &di, &dl, &dl, p) == Success);
}

GdkFilterReturn
atom_propertynotify(GdkXEvent *e, GdkEvent *ge, void *ptr) {
    XPropertyEvent *x;
    (void)ge;

    if(((XEvent*)e)->type != PropertyNotify)
        return GDK_FILTER_CONTINUE;
    x = &((XEvent*)e)->xproperty;
    if(x->atom == atoms[_OWL_URI])
        win_load((Win*)ptr);
    else if(x->atom == atoms[_OWL_FIND])
        win_find((Win*)ptr, TRUE);
    else
        return GDK_FILTER_CONTINUE;
    return GDK_FILTER_REMOVE;
}

char*
atom_str(int i) {
    char *c[LASTAtom] = {
        [_OWL_URI]      = "_OWL_URI",
        [_OWL_FIND]     = "_OWL_FIND",
    };
    return c[i];
}

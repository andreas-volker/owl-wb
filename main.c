#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>

typedef struct {
    GtkWindow           *win;
    GtkScrolledWindow   *scroll;
    WebKitWebView       *web;
} Win;

struct {
    GList   *wins;
} data;

/* event.c */
static void event_init(Win*);
static gboolean delwin(GtkWidget*, GdkEvent*, Win*);
static void grabdom(GObject*, WebKitDOMEvent*, void*);
static gboolean keypress(GtkWidget*, GdkEvent*, Win*);
static gboolean message(WebKitWebView*, char*, int, char*);
static WebKitWebView* newwin(WebKitWebView*, WebKitWebFrame*);
static gboolean scrollbars(WebKitWebFrame*);
static void status(GObject*, GParamSpec*, Win*);
/* main.c */
static void clean(void);
static void init(int*, char***);
static void sighdl(int);
/* util.c */
static void *emalloc(size_t);
static void eprintf(char*, ...);
static char *estrdup(char*);
/* win.c */
static Win *win_create(void);
static void win_destroy(void*);
static void win_load(Win*, char*);

#include "util.c"
#include "win.c"
#include "event.c"

void
clean(void) {
    return;
}

void
init(int *argc, char ***argv) {
    Win *w;

    gtk_init(&(*argc), &(*argv));
    data.wins = NULL;
    w = win_create();
    win_load(w, (*argv)[1]);
    signal(SIGINT, sighdl);
    signal(SIGTERM, sighdl);
}

void
sighdl(int i) {
    (void)i;

    g_list_free_full(data.wins, win_destroy);
}

int
main(int argc, char **argv) {
    init(&argc, &argv);
    gtk_main();
    clean();
    return EXIT_SUCCESS;
}

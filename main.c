#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>

#define LEN(x)  (sizeof (x) / sizeof ((x)[0]))
#define LOCK    (GDK_LOCK_MASK|GDK_MOD2_MASK|GDK_MOD3_MASK)
#define MOD     (GDK_SHIFT_MASK|GDK_CONTROL_MASK|\
                 GDK_MOD1_MASK|GDK_MOD4_MASK|GDK_MOD5_MASK)

typedef struct {
    int         i;
    char        *c;
    gboolean    b;
} Arg;

typedef struct {
    GtkWindow           *win;
    GtkScrolledWindow   *scroll;
    WebKitWebView       *web;
} Win;

typedef struct {
    unsigned int    mod;
    char            *key;
    void            (*func)(Win*, Arg*);
    Arg             arg;
} Key;

struct {
    GList   *wins;
    Key     *keys;
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
/* key.c */
static void keyprevnext(Win*, Arg*);
static void keyreload(Win*, Arg*);
static void keystop(Win*, Arg*);
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
#include "key.c"
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

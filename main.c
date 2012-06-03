#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>

#define GWIN(X) (GTK_WIDGET((X)->win)->window)
#define XWIN(X) (GDK_WINDOW_XID(GWIN(X)))
#define LEN(X)  (sizeof (X) / sizeof ((X)[0]))
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
    gboolean            ignore, zoom;
} Win;

typedef struct {
    unsigned int    mod;
    char            *key;
    void            (*func)(Win*, Arg*);
    Arg             arg;
} Key;

struct {
    SoupCookieJar   *jar;
    Display         *dpy;
    GList           *wins;
    Key             *keys;
} data;

/* atom.c */
static void atom_init(void);
static gboolean atom_get(Win*, Atom, Atom, long, unsigned char**);
static GdkFilterReturn atom_propertynotify(GdkXEvent*, GdkEvent*, void*);
static char *atom_str(int);
/* event.c */
static void event_init(Win*);
static void cookie(SoupCookieJar*, SoupCookie*, SoupCookie*);
static gboolean delwin(GtkWidget*, GdkEvent*, void*);
static void grabdom(GObject*, WebKitDOMEvent*, void*);
static gboolean keypress(GtkWidget*, GdkEvent*, void*);
static gboolean message(WebKitWebView*, char*, int, char*);
static WebKitWebView* newwin(WebKitWebView*, WebKitWebFrame*);
static void resize(GtkWidget*, GdkRectangle*, void*);
static gboolean scrollbars(WebKitWebFrame*);
static void status(GObject*, GParamSpec*, void*);
/* key.c */
static void keyexec(Win*, Arg*);
static void keyfind(Win*, Arg*);
static void keyfocus(Win*, Arg*);
static void keynewwin(Win*, Arg*);
static void keyprevnext(Win*, Arg*);
static void keyreload(Win*, Arg*);
static void keyscroll(Win*, Arg*);
static void keystop(Win*, Arg*);
static void keyzoom(Win*, Arg*);
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
static void win_find(Win*, gboolean);
static void win_load(Win*);

#include "config.h"
#include "util.c"
#include "atom.c"
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
    int i;
    char *dir, *c, buf[BUFSIZ], tmp[BUFSIZ];
    SoupSession *s;

    memset(&buf, '\0', sizeof(buf));
    if(*argc > 1) {
        strncpy(buf, (*argv)[1], sizeof(buf));
        for(i = 2; i < *argc; i++) {
            snprintf(tmp, sizeof(tmp), "%s %s", buf, (*argv)[i]);
            strncpy(buf, tmp, sizeof(buf));
        }
    }
    gtk_init(&(*argc), &(*argv));
    data.dpy = gdk_x11_get_default_xdisplay();
    data.wins = NULL;
    dir = g_str_has_prefix(OWLDIR, "/") ?
          g_strdup(OWLDIR) : g_build_filename(g_get_home_dir(), OWLDIR, NULL);
    if(!g_file_test(dir, G_FILE_TEST_EXISTS))
        g_mkdir_with_parents(dir, 0700);
    else if(!g_file_test(dir, G_FILE_TEST_IS_DIR))
        eprintf("\"%s\" is not a directory.\n", dir);
    s = webkit_get_default_session();
    c = g_build_filename(dir, COOKIEFILE, NULL);
    g_free(dir);
    data.jar = soup_cookie_jar_text_new(c, FALSE);
    g_free(c);
    soup_session_add_feature(s, SOUP_SESSION_FEATURE(data.jar));
    soup_cookie_jar_set_accept_policy(data.jar, (USECOOKIE ?
                                      SOUP_COOKIE_JAR_ACCEPT_ALWAYS :
                                      SOUP_COOKIE_JAR_ACCEPT_NEVER));
    atom_init();
    w = win_create();
    XChangeProperty(data.dpy, XWIN(w), atoms[_OWL_URI], XA_STRING, 8,
                    PropModeReplace, (unsigned char*)buf, strlen(buf) + 1);
    win_load(w);
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

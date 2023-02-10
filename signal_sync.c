
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <glib-object.h>
#include <gio/gio.h>


GMainLoop *main_loop;

void signal_sigint_handler(int signal) {
  fprintf(stderr, "... Get SIGINT signal!\n");

  if(main_loop) {
    g_main_loop_quit(main_loop);
    g_main_loop_unref(main_loop);
    main_loop = NULL;
  }
}


void dbus_signal_callback(void) {
  printf("Get signal!\n");
}

int main(int argc, char *argv[]) {

  signal(SIGINT, signal_sigint_handler);

  main_loop = g_main_loop_new(NULL, FALSE);

  GDBusConnection * conn = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, NULL);

  GDBusProxy* proxy = g_dbus_proxy_new_sync(conn, G_DBUS_PROXY_FLAGS_NONE, NULL,
                                       "com.test.signal", "/com/test/signal", "org.test.signal", 
                                       NULL, NULL);


  g_signal_connect(proxy, "Test", dbus_signal_callback, NULL);

  g_main_loop_run(main_loop);

  return 0;
}


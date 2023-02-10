
#include <stdio.h>
#include <stdlib.h>


#include <glib-object.h>
#include <gio/gio.h>


// typedef void (*GAsyncReadyCallback) (GObject *source_object,
//              GAsyncResult *res,
//              gpointer user_data);
//

GDBusConnection *global_connection;

static void dbus_proxy_call_ready(GObject *source_object, GAsyncResult* res, gpointer user_data) {

  GError *error = NULL;

  GVariant *variant = g_dbus_proxy_call_finish((GDBusProxy *)source_object, res, &error);
  if(error) {
    fprintf(stderr, "g_dbus_proxy_call failed: %s\n", error->message);
    g_error_free(error);
    return ;
  }

  // g_print("%s\n", g_variant_print(variant, TRUE));

  GVariant *hostname;
  g_variant_get(variant, "(v)", &hostname);
  g_print("Hostname: %s\n", g_variant_get_string(hostname, NULL));
  g_variant_unref(variant);

  g_print("g_dbus_proxy_call succeed!\n");
  g_main_loop_quit((GMainLoop*)user_data);
}

static void dbus_proxy_ready(GObject *source_object, GAsyncResult* res, gpointer user_data) {

  GError *error = NULL;
  GDBusProxy *proxy = g_dbus_proxy_new_finish(res, &error);
  if(error) {
    fprintf(stderr, "g_dbus_proxy failed: %s\n", error->message);
    g_error_free(error);
    return ;
  }
  g_print("g_dbus_proxy succeed!\n");

  g_dbus_proxy_call( proxy,
                    "Get",
                    g_variant_new("(ss)", "org.freedesktop.hostname1", "Hostname"),
                    G_DBUS_CALL_FLAGS_NONE,
                    -1,
                    NULL,
                    dbus_proxy_call_ready,
                    user_data);
}

static void  bus_get_ready(GObject *source_object, GAsyncResult* res, gpointer user_data) {

  GError *error = NULL;
  global_connection = g_bus_get_finish(res, &error);

  if(error) {
    fprintf(stderr, "g_dbus_get failed: %s\n", error->message);
    g_error_free(error);
    return ;
  }
  g_print("g_dbus_get succeed!\n");

  g_dbus_proxy_new(global_connection, G_DBUS_PROXY_FLAGS_NONE, NULL, 
                   "org.freedesktop.hostname1", 
                   "/org/freedesktop/hostname1", 
                   "org.freedesktop.DBus.Properties", NULL, 
                   dbus_proxy_ready, user_data);

}


int main(int argc, char *argv[]) {

  GMainLoop *loop;

  loop = g_main_loop_new(NULL, FALSE);

  g_bus_get(G_BUS_TYPE_SYSTEM, NULL, (GAsyncReadyCallback)bus_get_ready, loop);
  g_main_loop_run(loop);
  return 0;
}






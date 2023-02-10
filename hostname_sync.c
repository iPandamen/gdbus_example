
#include <glib-object.h>
#include <gio/gio.h>

int main(int argc, char *argv[]) {

  GError *error = NULL;
  GDBusConnection *conn;

  conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);

  if(error != NULL) {
    g_print("Get SystemBus Failed: %s\n", error->message);
    g_error_free(error);
    return -1;
  }

  if(conn == NULL) {
    return -1;
  }

  GDBusProxy *proxy = g_dbus_proxy_new_sync(conn, G_DBUS_PROXY_FLAGS_NONE, NULL,
                   "org.freedesktop.hostname1",
                   "/org/freedesktop/hostname1",
                   "org.freedesktop.DBus.Properties",
                   NULL,
                   NULL);

  GVariant *variant = g_dbus_proxy_call_sync(proxy, 
                                             "Get", 
                                             g_variant_new("(ss)", "org.freedesktop.hostname1", "Hostname"), 
                                             0, 
                                             -1, 
                                             NULL, 
                                             NULL);
  if(variant == NULL) {
    g_print("Call failed!\n");
    return 1;
  }

  g_print("%s\n", g_variant_print(variant, TRUE));

  GVariant *hostname;
  g_variant_get(variant, "(v)", &hostname);
  g_variant_unref(variant);
  g_print("%s\n", g_variant_get_string(hostname, NULL));
}




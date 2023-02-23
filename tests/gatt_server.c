
#include <glib.h>
#include <gio/gio.h>


int main(int argc ,char *argv[]) {


  GError *error = NULL;
  GMainLoop *loop;
  loop = g_main_loop_new(NULL, FALSE);

  GDBusConnection *conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
  if(error != NULL) {
    g_print("Error get system bus: %s\n", error->message);
    g_error_free(error);
    return 1;
  }

  GDBusProxy *proxy = g_dbus_proxy_new_sync(conn, G_DBUS_PROXY_FLAGS_NONE, NULL, "org.bluze", "/org/bluez/hci0", "org.bluez.GattManager1", NULL, &error);
  if(error != NULL) {
    g_print("Error get proxy: %s\n", error->message);
    g_error_free(error);
    return 1;
  }

  char string[] = "{'/org/bluez/example/service0': { 'org.bluez.GattService1': { 'UUID': '0000180d-0000-1000-8000-00805f9b34fb'}}}";
  GVariant *variant;
  variant = g_variant_new_parsed(string);
  g_print("%s\n", g_variant_print(variant, TRUE));

  // g_dbus_proxy_call_sync(proxy, "RegisterApplication", variant,G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
  g_dbus_proxy_call_sync(proxy, "RegisterApplication", g_variant_new_string("/org/bluez/example/service0"),G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
  if(error != NULL) {
    g_print("Error proxy call: %s\n", error->message);
    g_error_free(error);
    return 1;
  }

  g_main_loop_run(loop);
  g_variant_unref(variant);
  g_main_loop_unref(loop);

  return 0;
}




#include <glib.h>
#include <gio/gio.h>

#include <stdio.h>
#include <signal.h>

#define AD_OBJECT_PATH  "/org/bluez/example/advertisement0"

static GDBusNodeInfo *ad_introspection_data = NULL;
static const gchar ad_introspection_xml[] =
  "<node>"
  "  <interface name='org.freedesktop.DBus.Properties'>"
  "    <method name='GetAll'>"
  "      <arg name='interface' direction='in' type='s' />"
  "      <arg name='property' direction='out' type='a{sv}' />"
  "    </method>"
  "  </interface>"
  "  <interface name='org.bluez.LEAdvertisement1'>"
  "    <method name='Release'>"
  "    </method>"
  "    <property name='Type' type='s' access='read'/>"
  "    <property name='ServiceUUIDs' type='as' access='read'/>"
  "    <property name='ManufacturerData' type='a{sv}' access='read'/>"
  "    <property name='SolicitUUIDs' type='as' access='read'/>"
  "    <property name='ServiceData' type='a{sv}' access='read'/>"
  "  </interface>"
  "</node>";

static void handle_method_call (GDBusConnection       *connection,
                                const gchar           *sender,
                                const gchar           *object_path,
                                const gchar           *interface_name,
                                const gchar           *method_name,
                                GVariant              *parameters,
                                GDBusMethodInvocation *invocation,
                                gpointer               user_data) {

  g_print("sender: %s, object_path: %s, interface_name: %s, method_name: %s\n", sender, object_path, interface_name, method_name);

  if(g_strcmp0(interface_name, "org.freedesktop.DBus.Properties") == 0) {
    if(g_strcmp0(method_name, "GetAll") == 0) {
      GVariant *variant = NULL;
      gchar string[] = 
        "({"
        "  'Type': <'peripheral'>, "
        "  'ServiceUUIDs': <['180D', '180F']>, "
        "  'LocalName': <'TestAdvertisement'>, "
        "  'Includes': <['tx-power']>"
        "},)";
      variant = g_variant_new_parsed(string);
      g_print("variant: %s\n", g_variant_print(variant, TRUE));
      g_dbus_method_invocation_return_value(invocation, variant);
    }
  }
}


GVariant *handle_get_property (GDBusConnection       *connection,
                               const gchar           *sender,
                               const gchar           *object_path,
                               const gchar           *interface_name,
                               const gchar           *property_name,
                               GError               **error,
                               gpointer               user_data) {

  g_print("sender: %s, object_path: %s, interface_name: %s, property_name: %s\n", sender, object_path, interface_name, property_name);

  GVariant *ret = NULL;

  if(g_strcmp0(object_path, "/org/bluez/BatteryService/service000") == 0) {
    if(g_strcmp0(property_name, "UUID") == 0) {
      ret = g_variant_new_string("0000180d-0000-1000-8000-00805f9b34fb");
    } else if(g_strcmp0(property_name, "Primary") == 0) {
      ret = g_variant_new_boolean(TRUE);
    }
  }
  return ret;
}

gboolean  handle_set_property (GDBusConnection       *connection,
                               const gchar           *sender,
                               const gchar           *object_path,
                               const gchar           *interface_name,
                               const gchar           *property_name,
                               GVariant              *value,
                               GError               **error,
                               gpointer               user_data) {

  g_print("sender: %s, object_path: %s, interface_name: %s, property_name: %s\n", sender, object_path, interface_name, property_name);

  return *error == NULL;
}

GDBusInterfaceVTable ad_interface_table = {
  handle_method_call,
  handle_get_property,
  handle_set_property,
  {0}
};


static void on_ad_register_ready(GDBusProxy* proxy,
                                 GAsyncResult *res,
                                 gpointer user_data) {

  GError *error = NULL;
  GVariant *result = g_dbus_proxy_call_finish(proxy, res, &error);

  if(error != NULL) {
    g_print("Error calling le_ad_proxy RegisterAdvertisement : %s\n", error->message);
    g_error_free(error);
    return ;
  }
  g_print("result: %s\n", g_variant_print(result, TRUE));
  g_variant_unref(result);
}


void register_advertisement(GDBusProxy *le_ad_proxy, gchar *advertisement) {

  GError * error = NULL;
  GVariant *parameters = NULL;

  parameters = g_variant_new("(oa{sv})", advertisement, NULL);
  g_print("parameters: %s\n", g_variant_print(parameters, TRUE));

  g_dbus_proxy_call(le_ad_proxy, "RegisterAdvertisement",
                    parameters,
                    G_DBUS_CALL_FLAGS_NONE,
                    -1,
                    NULL,
                    (GAsyncReadyCallback)on_ad_register_ready, NULL);
}

static void on_ad_unregister_ready(GDBusProxy* proxy,
                                   GAsyncResult *res,
                                   gpointer user_data) {

  GError *error = NULL;
  GVariant *result = g_dbus_proxy_call_finish(proxy, res, &error);

  if(error != NULL) {
    g_print("Error calling le_ad_proxy RegisterAdvertisement : %s\n", error->message);
    g_error_free(error);
    return ;
  }

  g_print("result: %s\n", g_variant_print(result, TRUE));
  g_variant_unref(result);
}

void unregister_advertisement(GDBusProxy *le_ad_proxy, gchar *advertisement) {

  GError * error = NULL;
  GVariant *parameters = NULL;

  parameters = g_variant_new("(o)", advertisement);
  g_dbus_proxy_call(le_ad_proxy, "UnregisterAdvertisement",
                    parameters,
                    G_DBUS_CALL_FLAGS_NONE,
                    -1,
                    NULL,
                    (GAsyncReadyCallback)on_ad_unregister_ready, 
                    NULL);

  g_variant_unref(parameters);
}

GMainLoop *main_loop = NULL;

void on_signal_sigint (int signal) {

  printf("--------------------------------> Get SIGINT signal!\n");
  g_main_loop_quit(main_loop);
}

int main(int argc, char *argv[]) {

  signal(SIGINT, on_signal_sigint);

  GError *error = NULL;
  GDBusConnection *connection;

  connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
  if(error != NULL) {
    g_print("Error get system bus: %s\n", error->message);
    g_error_free(error);
    exit(1);
  }
  g_print("unique name: %s\n", g_dbus_connection_get_unique_name(connection));

  ad_introspection_data = g_dbus_node_info_new_for_xml(ad_introspection_xml, &error);
  if(error != NULL) {
    g_print("Error get ad node info: %s\n", error->message);
    g_error_free(error);
    exit(1);
  }

  guint registeration_id = g_dbus_connection_register_object(connection, 
                                                             AD_OBJECT_PATH,
                                                             ad_introspection_data->interfaces[0],
                                                             &ad_interface_table, 
                                                             NULL,
                                                             NULL,
                                                             &error);

  if(error != NULL) {
    g_print("Error registering object: %s\n", error->message);
    g_error_free(error);
    exit(1);
  }

  GDBusProxy * le_ad_proxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, 
                                                           G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES,
                                                           NULL, 
                                                           "org.bluez",
                                                           "/org/bluez/hci0",
                                                           "org.bluez.LEAdvertisingManager1",
                                                           NULL, 
                                                           &error);
  if(error != NULL) {
    g_print("Error get dubs proxy: %s\n", error->message);
    g_error_free(error);
    exit(1);
  }

  register_advertisement(le_ad_proxy, AD_OBJECT_PATH );
  main_loop = g_main_loop_new(NULL, FALSE);
  g_main_loop_run(main_loop);

  unregister_advertisement(le_ad_proxy, AD_OBJECT_PATH);
  g_dbus_connection_unregister_object(connection, registeration_id);
  g_object_unref(connection);

  return 0;
}








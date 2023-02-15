
#include <glib.h>
#include <gio/gio.h>


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

  if(g_strcmp0(object_path, "/org/bluez/BatteryService") == 0) {
    if(g_strcmp0(interface_name, "org.freedesktop.DBus.ObjectManager") == 0) {
      if(g_strcmp0(method_name, "GetManagedObjects") == 0) {
        GVariant *variant = NULL;
        gchar variant_string[] = 
          "({"
          "  objectpath '/org/bluez/example/service0': { "
          "    'org.bluez.GattService1': { "
          "      'UUID': <'0000180d-0000-1000-8000-00805f9b34fb'>, "
          "      'Primary': <true>, "
          "      'Characteristics': <['/org/bluez/example/service0/char0', '/org/bluez/example/service0/char1','/org/bluez/example/service0/char2']>"
          "    }"
          "  }, "
          "  objectpath '/org/bluez/example/service0/char0': { "
          "    'org.bluez.GattCharacteristic1': {"
          "      'Service': <'/org/bluez/example/service0'>, "
          "      'UUID': <'00002a37-0000-1000-8000-00805f9b34fb'>, "
          "      'Flags': <['notify']>, "
          "      'Descriptors': <@as []>"
          "    } "
          "  },"
          "  objectpath '/org/bluez/example/service0/char1': {"
          "    'org.bluez.GattCharacteristic1': {"
          "      'Service': <'/org/bluez/example/service0'>, "
          "      'UUID': <'00002a38-0000-1000-8000-00805f9b34fb'>, "
          "      'Flags': <['read']>, "
          "      'Descriptors': <@as []>"
          "    }"
          "  }, "
          "  objectpath '/org/bluez/example/service0/char2': {"
          "    'org.bluez.GattCharacteristic1': {"
          "      'Service': <'/org/bluez/example/service0'>, "
          "      'UUID': <'00002a39-0000-1000-8000-00805f9b34fb'>, "
          "      'Flags': <['write']>, "
          "      'Descriptors': <@as []>"
          "    }"
          "  }"
          "},)";
        variant = g_variant_new_parsed(variant_string);
        g_print("variant: %s\n", g_variant_print(variant, TRUE));
        g_dbus_method_invocation_return_value(invocation, variant);
        // g_variant_unref(variant);
      }
    }
  }
}


static gchar *_global_title = NULL;

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

  if(g_strcmp0(property_name, "Title") == 0) {
    if(_global_title == NULL) {
      _global_title = g_strdup("Back To C!");
    }
    ret = g_variant_new_string(_global_title);
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
  if(g_strcmp0(property_name, "Title") == 0) {
    if(g_strcmp0(_global_title, g_variant_get_string(value, NULL)) != 0) {
      GVariantBuilder *builder;
      GError *local_error;

      g_free(_global_title);
      _global_title = g_variant_dup_string(value, NULL);

      local_error = NULL;
      builder = g_variant_builder_new(G_VARIANT_TYPE_ARRAY);
      g_variant_builder_add(builder, "{sv}", "Title", g_variant_new_string(_global_title));
      g_dbus_connection_emit_signal(
                                    connection, 
                                    NULL, 
                                    object_path, 
                                    "org.freedesktop.DBus.Properties", 
                                    "PropertiesChanaged", 
                                    g_variant_new("(sa{sv}as)", interface_name, builder, NULL), 
                                    &local_error);
      g_assert_no_error(local_error);
    }
  }

  return *error == NULL;
}




GDBusInterfaceVTable ad_interface_table = {
  handle_method_call,
  handle_get_property,
  handle_set_property,
  {0}
};

int main(int argc, char *argv[]) {

  GError *error = NULL;
  GMainLoop *loop;
  GDBusConnection *connection;

  connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
  if(error != NULL) {
    g_print("Error get system bus: %s\n", error->message);
    g_error_free(error);
    exit(1);
  }

  ad_introspection_data = g_dbus_node_info_new_for_xml(ad_introspection_xml, &error);
  if(error != NULL) {
    g_print("Error get  ad node info: %s\n", error->message);
    g_error_free(error);
    exit(1);
  }

  g_dbus_connection_register_object(connection, 
                                    "/org/bluez/example/advertisement0",
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



  g_print("unique name: %s\n", g_dbus_connection_get_unique_name(connection));

  loop = g_main_loop_new(NULL, FALSE);
  g_main_loop_run(loop);

  return 0;
}








#include <glib.h>
#include <gio/gio.h>

#define DBUS_INTERFACE_OBJECT_MANAGER "org.freedesktop.DBus.ObjectManager"

#ifndef DBUS_ERROR_UNKNOWN_PROPERTY
#define DBUS_ERROR_UNKNOWN_PROPERTY "org.freedesktop.DBus.Error.UnknownProperty"
#endif

#ifndef DBUS_ERROR_PROPERTY_READ_ONLY
#define DBUS_ERROR_PROPERTY_READ_ONLY "org.freedesktop.DBus.Error.PropertyReadOnly"
#endif

/* ---------------------------------------------------------------------------------------------------- */


static GDBusNodeInfo *object_introspection_data = NULL;
static const gchar object_introspection_xml[] =
  "<node>"
  "  <interface name='org.freedesktop.DBus.ObjectManager'>"
  "    <method name='GetManagedObjects'>"
  "      <arg name='objects' type='a{oa{sa{sv}}}' direction='out'/>"
  "    </method>"
  "  </interface>"
  "</node>";


static GDBusNodeInfo *service_introspection_data = NULL;
static const gchar service_introspection_xml[] =
  "<node>"
  "  <interface name='org.bluez.GattService1'>"
  "    <property type='s' name='UUID' access='read'/>"
  "    <property type='b' name='Primary' access='read'/>"
  "  </interface>"
  "</node>";

static GDBusNodeInfo *characteristic_introspection_data = NULL;
static const gchar characteristic_introspection_xml[] = 
  "<node>"
  "  <interface name='org.bluez.GattCharacteristic1'>"
  "    <method name='ReadValue'>"
  "      <arg name='options' type='a{sv}' direction='in'/>"
  "      <arg name='value' type='ay' direction='out'/>"
  "    </method>"
  "    <method name='WriteValue'>"
  "      <arg name='options' type='a{sv}' direction='in'/>"
  "      <arg name='value' type='ay' direction='in'/>"
  "    </method>"
  "    <method name='StartNotify'>"
  "    </method>"
  "    <method name='StopNotify'>"
  "    </method>"
  "    <property type='s' name='UUID' access='read'/>"
  "    <property type='o' name='Service' access='read'/>"
  "    <property type='ay' name='Value' access='read'/>"
  "    <property type='b' name='Notifying' access='read'/>"
  "    <property type='as' name='Flags' access='read'/>"
  "  </interface>"
  "</node>";

/* ---------------------------------------------------------------------------------------------------- */

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

  if(g_strcmp0(method_name, "HelloWorld") == 0) {
    const gchar *greeting;
    g_variant_get(parameters, "(&s)", &greeting);
    if(g_strcmp0(greeting, "Return Unregistered") == 0) {
      g_dbus_method_invocation_return_error(invocation, G_IO_ERROR, G_IO_ERROR_FAILED_HANDLED, "As requested, here's a GError not registered(G_IO_ERROR_FAILED_HANDLED)");
    } else if(g_strcmp0(greeting, "Return Registered") == 0) {
      g_dbus_method_invocation_return_error(invocation, G_DBUS_ERROR, G_DBUS_ERROR_MATCH_RULE_NOT_FOUND, "As requested, here's a GError not registered(G_DBUS_ERROR_MATCH_RULE_NOT_FOUND)");
    } else if(g_strcmp0(greeting," Retrun Raw") == 0) {
      g_dbus_method_invocation_return_dbus_error(invocation,"org.bluez.Services.Battery.SomeErrorName","As requested, here's a GError not registered(G_DBUS_ERROR_MATCH_RULE_NOT_FOUND)");
    } else {
      gchar *response;
      response = g_strdup_printf("Your greeted me with '%s'. Thanks!", greeting);
      g_dbus_method_invocation_return_value(invocation, g_variant_new("(s)", response));
      g_free(response);
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

GDBusInterfaceVTable interface_vtable = {
  handle_method_call,
  handle_get_property,
  handle_set_property,
  {0}
};

void register_service_object(GDBusConnection *connection) {

  guint registration_id;
  registration_id = g_dbus_connection_register_object(connection, 
                                                      "/org/bluez/BatteryService",
                                                      object_introspection_data->interfaces[0],
                                                      &interface_vtable,
                                                      NULL,
                                                      NULL,
                                                      NULL);

  registration_id = g_dbus_connection_register_object(connection, 
                                                      "/org/bluez/BatteryService/service000",
                                                      service_introspection_data->interfaces[0],
                                                      &interface_vtable,
                                                      NULL,
                                                      NULL,
                                                      NULL);
  g_assert(registration_id > 0);
}

static void 
on_bus_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data) {
  register_service_object(connection);
}

static void 
on_name_acquired(GDBusConnection *connect, const gchar *name, gpointer user_data) {
  g_print("bus name acquired!\n");
}

static void 
on_name_lost(GDBusConnection *connect, const gchar *name, gpointer user_data) {

  g_print("bus name lost: name: %s!\n", name);
  exit(1);
}


int main(int argc, char *argv[]) {

  guint owner_id;
  GMainLoop *loop;

  object_introspection_data = g_dbus_node_info_new_for_xml(object_introspection_xml, NULL);
  g_assert(object_introspection_data != NULL);

  service_introspection_data = g_dbus_node_info_new_for_xml(service_introspection_xml, NULL);
  g_assert(service_introspection_data != NULL);

  // owner_id = g_bus_own_name(G_BUS_TYPE_SESSION,
  //                           "org.bluez.BatteryService",
  //                           G_BUS_NAME_OWNER_FLAGS_NONE,
  //                           on_bus_acquired,
  //                           on_name_acquired,
  //                           on_name_lost,
  //                           NULL,
  //                           NULL);

  GDBusConnection *conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, NULL);

  register_service_object(conn);


  loop = g_main_loop_new(NULL, FALSE);
  g_main_loop_run(loop);

  g_bus_unown_name(owner_id);
  g_dbus_node_info_unref(service_introspection_data);

  return 0;
}

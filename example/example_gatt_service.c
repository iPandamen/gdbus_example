
#include <stdio.h>
#include <signal.h>

#include <glib.h>
#include <gio/gio.h>

#define APP_OBJECT_PATH "/org/bluez/example/app0"

GMainLoop *main_loop = NULL;
GDBusConnection *connection = NULL;

GDBusProxy *gatt_manager_proxy;

GDBusNodeInfo  *app_introspection_data = NULL;
gchar app_introspection_xml[] = 
  "<node>"
  "  <interface name='org.freedesktop.DBus.ObjectManager'>"
  "    <method name='GetManagedObjects'>"
  "      <arg name='objects' direction='out' type='a{oa{sa{sv}}}' />"
  "    </method>"
  "  </interface>"
  "</node>";

GDBusNodeInfo  *service_introspection_data = NULL;
gchar service_introspection_xml[] = 
  "<node>"
  "  <interface name='org.bluez.GattService1'>"
  "    <method name='GetManagedObjects'>"
  "      <arg name='objects' direction='out' type='a{oa{sa{sv}}}' />"
  "    </method>"
  "    <property name='UUID' type='s' access='read' />"
  "    <property name='Priamry' type='b' access='read' />"
  // "    <property name='Device' type='o' acess='read'>"
  // "    <property name='Includes' type='ao' acess='read'>"
  // "    <property name='Handle' type='q' acess='read'>"
  "  </interface>"
  "</node>";

GDBusNodeInfo  *characteristic_introspection_data = NULL;
gchar characteristic_introspection_xml[] = 
  "<node>"
  "  <interface name='org.bluez.GattCharacteristic1'>"
  "    <method name='ReadValue'>"
  "      <arg name='options' direction='in' type='a{sv}' />"
  "      <arg name='value' direction='out' type='ay' />"
  "    </method>"
  "    <method name='WriteValue'>"
  "      <arg name='value' direction='in' type='ay' />"
  "      <arg name='options' direction='in' type='a{sv}' />"
  "    </method>"
  // "    <method name='AcquireWrite'>"
  // "    </method>"
  // "    <method name='AcquireNotify'>"
  // "    </method>"
  "    <method name='StartNotify'>"
  "    </method>"
  "    <method name='StopNotify'>"
  "    </method>"
  // "    <method name='Confirm'>"
  // "    </method>"
  "    <property name='UUID' type='s' access='read' />"
  "    <property name='Service' type='o' access='read' />"
  // "    <property name='Value' type='ay' acess='read' />"
  // "    <property name='WriteAcquired' type='b' acess='read' />"
  // "    <property name='NotifyAcquired' type='b' acess='read' />"
  // "    <property name='Notifying' type='b' acess='read' />"
  "    <property name='Flags' type='as' access='read' />"
  // "    <property name='Handle' type='q' acess='read' />"
  "    <property name='MTU' type='q' access='read' />"
  "  </interface>"
  "</node>";

GDBusNodeInfo  *desc_introspection_data = NULL;
gchar desc_introspection_xml[] = 
  "<node>"
  "  <interface name='org.bluez.GattDescriptor1'>"
  "    <method name='ReadValue'>"
  "      <arg name='flags' direction='in' type='a{sv}' />"
  "      <arg name='value' direction='out' type='ay' />"
  "    </method>"
  "    <method name='WriteValue'>"
  "      <arg name='value' direction='in' type='ay' />"
  "      <arg name='flags' direction='in' type='a{sv}' />"
  "    </method>"
  "    <property name='UUID' type='s' access='read' />"
  // "    <property name='Value' type='ay' access='read'>"
  "    <property name='Flags' type='as' access='read' />"
  // "    <property name='Handle' type='q' access='read'>"
  "  </interface>"
  "</node>";


gboolean is_notify = FALSE;
gboolean hr_source_func(gpointer user_data) {

  printf("-------- %s\n", __FUNCTION__);
  if(is_notify) {

    GRand *rand = g_rand_new_with_seed(g_get_real_time());
    guint16 val = g_rand_int_range(rand, 0, 0xFFFF);

    GVariant *variant = g_variant_new_parsed("('org.bluez.GattCharacteristic1', {'Value': <@ay [%y, %y, %y]>}, @as [])", 0x06, val & 0xFF, val >> 8);
    g_print("---- hr_source_func variant: %s\n", g_variant_print(variant, TRUE));

    GError *error = NULL;
    g_dbus_connection_emit_signal(connection, 
                                  NULL, 
                                  APP_OBJECT_PATH "/service0/char0", 
                                  // "org.bluez.GattCharacteristic1",
                                  "org.freedesktop.DBus.Properties",
                                  "PropertiesChanged", 
                                  variant, 
                                  &error);
    if(error != NULL) {
      fprintf(stderr, "Error emitting PropertiesChanged: %s\n", error->message);
      g_error_free(error);
      return FALSE;
    }
  }

  return TRUE;
}

static void handle_method_call (GDBusConnection       *connection,
                                const gchar           *sender,
                                const gchar           *object_path,
                                const gchar           *interface_name,
                                const gchar           *method_name,
                                GVariant              *parameters,
                                GDBusMethodInvocation *invocation,
                                gpointer               user_data) {

  g_print("sender: %s, object_path: %s, interface_name: %s, method_name: %s\n", sender, object_path, interface_name, method_name);

  if(g_strcmp0(object_path, APP_OBJECT_PATH) == 0) {
    if(g_strcmp0(interface_name, "org.freedesktop.DBus.ObjectManager") == 0) {
      if(g_strcmp0(method_name, "GetManagedObjects") == 0) {
        GVariant *variant = NULL;
        gchar string[] = 
          "({"
          "  objectpath '/org/bluez/example/app0/service0': {"
          "    'org.bluez.GattService1': { "
          "      'UUID': <'0000180d-0000-1000-8000-00805f9b34fb'>,"
          "      'Primary': <true>,"
          "      'Characteristics': <['/org/bluez/example/app0/service0/char0', '/org/bluez/example/app0/service0/char1', '/org/bluez/example/app0/service0/char2']>"
          "    }"
          "  },"
          "  objectpath '/org/bluez/example/app0/service0/char0': {"
          "    'org.bluez.GattCharacteristic1': { "
          "      'Service': <@o '/org/bluez/example/app0/service0'>,"
          "      'UUID': <'00002a37-0000-1000-8000-00805f9b34fb'>,"
          "      'Flags': <['notify']>,"
          "      'Descriptors': <@ao []>"
          "    }"
          "  },"
          "  objectpath '/org/bluez/example/app0/service0/char1': {"
          "    'org.bluez.GattCharacteristic1': { "
          "      'Service': <@o '/org/bluez/example/app0/service0'>,"
          "      'UUID': <'00002a38-0000-1000-8000-00805f9b34fb'>,"
          "      'Flags': <['read']>,"
          "      'Descriptors': <@ao []>"
          "    }"
          "  },"
          "  objectpath '/org/bluez/example/app0/service0/char2': {"
          "    'org.bluez.GattCharacteristic1': { "
          "      'Service': <@o '/org/bluez/example/app0/service0'>,"
          "      'UUID': <'00002a39-0000-1000-8000-00805f9b34fb'>,"
          "      'Flags': <['write']>,"
          "      'Descriptors': <@ao []>"
          "    }"
          "  }"
          "},)";
        variant = g_variant_new_parsed(string);
        g_print("variant: %s\n", g_variant_print(variant, TRUE));
        g_dbus_method_invocation_return_value(invocation, variant);
      }
    }
  } else if(g_strcmp0(object_path, APP_OBJECT_PATH "/service0") == 0) {

  } else if(g_strcmp0(object_path, APP_OBJECT_PATH "/service0/char0") == 0) {

    if(g_strcmp0(method_name, "StartNotify") == 0) {
      is_notify = TRUE;
      g_dbus_method_invocation_return_value(invocation, g_variant_new_parsed("()"));
    } else if(g_strcmp0(method_name, "StopNotify") == 0) {
      is_notify = FALSE;
      g_dbus_method_invocation_return_value(invocation, g_variant_new_parsed("()"));
    }

  } else if(g_strcmp0(object_path, APP_OBJECT_PATH "/service0/char1") == 0) {

    if(g_strcmp0(method_name, "ReadValue") == 0) {
      GVariant *variant = g_variant_new_parsed("(@ay [ 0x01 ], )");
      g_dbus_method_invocation_return_value(invocation, variant);
    }
  } else if(g_strcmp0(object_path, APP_OBJECT_PATH "/service0/char2") == 0) {
    if(g_strcmp0(method_name, "WriteValue") == 0) {

      g_print("parameters: %s\n", g_variant_print(parameters, TRUE));
      g_dbus_method_invocation_return_value(invocation, g_variant_new_parsed("()"));
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

GDBusInterfaceVTable app_interface_table = {
  handle_method_call,
  handle_get_property,
  handle_set_property,
  {0}
};

static void on_register_application (GObject *source_object,
				     GAsyncResult *res,
             gpointer user_data) {

  GError *error = NULL;
  GDBusProxy *proxy = (GDBusProxy *)source_object;
  GVariant *result = g_dbus_proxy_call_finish(proxy , res, &error);
  if(error != NULL) {
    fprintf(stderr, "Error register_application: %s\n", error->message);
    g_error_free(error);
    return ;
  }

  fprintf(stdout, "result: %s\n", g_variant_print(result, TRUE));
  g_variant_unref(result);
}


void register_application(GDBusProxy *gatt_manager_proxy, gchar *application) {

  GVariant *parameters = NULL;
  printf("-------- %s, %s\n", __FUNCTION__, application);

  parameters = g_variant_new("(oa{sv})", application, NULL);
  g_print("parameters: %s\n", g_variant_print(parameters, TRUE));

  g_dbus_proxy_call(gatt_manager_proxy, 
                                  "RegisterApplication",
                                  parameters,
                                  G_DBUS_CALL_FLAGS_NONE,
                                  -1,
                                  NULL,
                                  on_register_application,
                                  NULL);
}


static void on_unregister_application (GObject *source_object,
				     GAsyncResult *res,
             gpointer user_data) {
  GError *error = NULL;
  GDBusProxy *proxy = (GDBusProxy *)source_object;
  GVariant *result = g_dbus_proxy_call_finish(proxy , res, &error);
  if(error != NULL) {
    fprintf(stderr, "Error unregister_application: %s\n", error->message);
    g_error_free(error);
    return ;
  }

  fprintf(stdout, "result: %s\n", g_variant_print(result, TRUE));
  g_variant_unref(result);
}


void unregister_application (GDBusProxy* gatt_manageer_proxy, gchar *application) {

  GVariant *parameters = NULL;

  printf("-------- %s, %s\n", __FUNCTION__, application);
  parameters = g_variant_new("(o)", application);

  g_dbus_proxy_call(gatt_manager_proxy, 
                    "UnregisterApplication",
                    parameters,
                    G_DBUS_CALL_FLAGS_NONE, 
                    -1,
                    NULL,
                    on_unregister_application,
                    NULL);
}




void on_signal(int signal) {

  g_main_loop_quit(main_loop);
  switch (signal) {
    case SIGINT:
      printf("\n---------------- Get SIGINT signal!\n");
      break;
    case SIGSEGV:
      printf("\n---------------- Get SIGSEGV signal!\n");
      exit(1);
      break;
    default:
      printf("\n---------------- Get unprocessed signal: %d!\n", signal);
      break;
  }
}

int main(int argc, char* argv[]) {

  signal(SIGINT, on_signal);
  signal(SIGSEGV, on_signal);

  GError * error = NULL;

  main_loop = g_main_loop_new(NULL, FALSE);

  printf("-------- Get System Bus.\n");
  // Get system bus
  connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error); 
  if(error != NULL) {
    fprintf(stderr, "Error getting system bus: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  printf("-------- Get node info.\n");
  app_introspection_data = g_dbus_node_info_new_for_xml(app_introspection_xml, &error);
  if(error != NULL) {
    fprintf(stderr, "Erro geting app_introspection_data: %s\n ", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  service_introspection_data = g_dbus_node_info_new_for_xml(service_introspection_xml, &error);
  if(error != NULL) {
    fprintf(stderr, "Erro geting service_introspection_data: %s\n ", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  characteristic_introspection_data = g_dbus_node_info_new_for_xml(characteristic_introspection_xml, &error);
  if(error != NULL) {
    fprintf(stderr, "Erro geting characteristic_introspection_data: %s\n ", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  desc_introspection_data = g_dbus_node_info_new_for_xml(desc_introspection_xml, &error);
  if(error != NULL) {
    fprintf(stderr, "Erro geting desc_introspection_data: %s\n ", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  printf("-------- Register app object.\n");
  guint app_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH,
                                    app_introspection_data->interfaces[0],
                                    &app_interface_table,
                                    NULL, 
                                    NULL,
                                    &error);
  if(error != NULL) {
    fprintf(stderr, "Errorr registering object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  guint service_0_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH "/service0",
                                    service_introspection_data->interfaces[0],
                                    &app_interface_table,
                                    NULL, 
                                    NULL,
                                    &error);
  if(error != NULL) {
    fprintf(stderr, "Errorr registering object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  guint char_0_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH "/service0/char0",
                                    characteristic_introspection_data->interfaces[0],
                                    &app_interface_table,
                                    NULL, 
                                    NULL,
                                    &error);
  if(error != NULL) {
    fprintf(stderr, "Errorr registering object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  guint char_1_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH "/service0/char1",
                                    characteristic_introspection_data->interfaces[0],
                                    &app_interface_table,
                                    NULL, 
                                    NULL,
                                    &error);
  if(error != NULL) {
    fprintf(stderr, "Errorr registering object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  guint char_2_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH "/service0/char2",
                                    characteristic_introspection_data->interfaces[0],
                                    &app_interface_table,
                                    NULL, 
                                    NULL,
                                    &error);
  if(error != NULL) {
    fprintf(stderr, "Errorr registering object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  printf("-------- Get proxy.\n");
  gatt_manager_proxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, 
                                                     G_DBUS_PROXY_FLAGS_NONE,
                                                     NULL,
                                                     "org.bluez",
                                                     "/org/bluez/hci0",
                                                     "org.bluez.GattManager1",
                                                     NULL,
                                                     &error);

  if(error != NULL) {
    fprintf(stderr, "Errorr get GattManager1 proxy: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  g_timeout_add_seconds(1, hr_source_func, NULL);

  register_application(gatt_manager_proxy, APP_OBJECT_PATH);
  g_main_loop_run(main_loop);

  unregister_application(gatt_manager_proxy, APP_OBJECT_PATH);
  g_dbus_connection_unregister_object(connection, app_register_id);
  return 0;
}








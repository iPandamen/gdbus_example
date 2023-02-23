
#include <stdio.h>
#include <signal.h>

#include <glib.h>
#include <gio/gio.h>

#define APP_OBJECT_PATH "/org/bluez/example/app"

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

static void app_method_call (GDBusConnection       *connection,
                                const gchar           *sender,
                                const gchar           *object_path,
                                const gchar           *interface_name,
                                const gchar           *method_name,
                                GVariant              *parameters,
                                GDBusMethodInvocation *invocation,
                                gpointer               user_data) {

  if(g_str_equal(object_path, APP_OBJECT_PATH)) {
    if(g_str_equal(interface_name, "org.freedesktop.DBus.ObjectManager")) {
      if(g_str_equal(method_name, "GetManagedObjects")) {
        GVariant *variant = NULL;
        gchar string[] = 
          "({"
          // Battery Service
          "  objectpath '/org/bluez/example/app/service0': {"
          "    'org.bluez.GattService1': { "
          "      'UUID': <'180F'>,"
          "      'Primary': <true>,"
          "      'Characteristics': <@ao ['/org/bluez/example/app/service0/char0']>"
          "    }"
          "  },"
          // Battery Level
          "  objectpath '/org/bluez/example/app/service0/char0': {"
          "    'org.bluez.GattCharacteristic1': { "
          "      'Service': <@o '/org/bluez/example/app/service0'>,"
          "      'UUID': <'2A19'>,"
          "      'Flags': <['read', 'notify']>,"
          "      'Descriptors': <@ao []>"
          "    }"
          "  }"
          "},)";
        variant = g_variant_new_parsed(string);
        g_print("variant: %s\n", g_variant_print(variant, TRUE));
        g_dbus_method_invocation_return_value(invocation, variant);
      }
    }
  }
}

GDBusInterfaceVTable app_interface_table = {
  app_method_call,
  NULL,
  NULL,
  {0}
};


gboolean is_notify_battery = FALSE;

guint8 battery_level = 99;

gboolean battery_notify_func(gpointer user_data) {

  GRand *rand = g_rand_new_with_seed(g_get_real_time());
  battery_level =  g_rand_int_range(rand, 50, 100);

  GVariant *variant = g_variant_new_parsed("('org.bluez.GattCharacteristic1', {'Value': <@ay [%y]>}, @as [])", battery_level);
  g_print("---- %s variant: %s\n", __FUNCTION__,  g_variant_print(variant, TRUE));

  GError *error = NULL;
  g_dbus_connection_emit_signal(connection, 
                                NULL, 
                                APP_OBJECT_PATH "/service0/char0", 
                                "org.freedesktop.DBus.Properties",
                                "PropertiesChanged", 
                                variant, 
                                &error);
  if(error != NULL) {
    fprintf(stderr, "Error emitting PropertiesChanged: %s\n", error->message);
    g_error_free(error);
    return FALSE;
  }
  return is_notify_battery;
}


static void battery_method_call (GDBusConnection       *connection,
                                const gchar           *sender,
                                const gchar           *object_path,
                                const gchar           *interface_name,
                                const gchar           *method_name,
                                GVariant              *parameters,
                                GDBusMethodInvocation *invocation,
                                gpointer               user_data) {

  if(g_str_equal(object_path, APP_OBJECT_PATH "/service0")) {

    if(g_str_equal(method_name, "GetAll")) {
      gchar value_string[] = 
        "  objectpath '/org/bluez/example/app/service0': {"
        "    'org.bluez.GattService1': { "
        "      'UUID': <'180F'>,"
        "      'Primary': <true>,"
        "      'Characteristics': <@ao ['/org/bluez/example/app/service0/char0']>"
        "    }"
        "  }";
      GVariant* variant = g_variant_new_parsed(value_string);
      g_dbus_method_invocation_return_value(invocation, variant);
    }
  } else if(g_str_equal(object_path, APP_OBJECT_PATH "/service0/char0")) {
    if(g_str_equal(method_name, "ReadValue")) {

      GVariant *variant = g_variant_new_parsed("(@ay [ %y ], )", battery_level);
      g_dbus_method_invocation_return_value(invocation, variant);
    } else if(g_str_equal(method_name, "StartNotify")) {
      is_notify_battery = TRUE;
      g_timeout_add_seconds(1, battery_notify_func, NULL);
    } else if(g_str_equal(method_name, "StopNotify")) {
      is_notify_battery = FALSE;
    }
  }
}

GDBusInterfaceVTable battery_interface_table = {
  battery_method_call,
  NULL,
  NULL,
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
                                    &battery_interface_table,
                                    NULL, 
                                    NULL,
                                    &error);
  if(error != NULL) {
    fprintf(stderr, "Errorr registering object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  guint service_0_char_0_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH "/service0/char0",
                                    characteristic_introspection_data->interfaces[0],
                                    &battery_interface_table,
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


  register_application(gatt_manager_proxy, APP_OBJECT_PATH);
  g_main_loop_run(main_loop);

  unregister_application(gatt_manager_proxy, APP_OBJECT_PATH);
  g_dbus_connection_unregister_object(connection, app_register_id);

  g_dbus_connection_unregister_object(connection, service_0_register_id);
  g_dbus_connection_unregister_object(connection, service_0_char_0_register_id);

  return 0;
}








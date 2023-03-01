
#include <stdio.h>

#include <glib.h>
#include <gio/gio.h>

GMainLoop *main_loop = NULL;
GDBusConnection *connection = NULL;

guint interface_added_subscribe_id = 0;
guint interface_removed_subscribe_id = 0;
guint properties_changes_subscribe_id = 0;

#define AD_OBJECT_PATH "/org/bluez/app/advertisement0"

guint ad_register_id = 0;
GDBusProxy *ad_manager_proxy = NULL;
GDBusNodeInfo * ad_node_info = NULL;
gchar *ad_xml = 
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

static void ad_method_call (GDBusConnection       *connection,
                                const gchar           *sender,
                                const gchar           *object_path,
                                const gchar           *interface_name,
                                const gchar           *method_name,
                                GVariant              *parameters,
                                GDBusMethodInvocation *invocation,
                                gpointer               user_data) {

  g_print("sender: %s, object_path: %s, interface_name: %s, method_name: %s\n", sender, object_path, interface_name, method_name);

  if(g_str_equal(object_path, AD_OBJECT_PATH)) {
    if(g_str_equal(interface_name, "org.freedesktop.DBus.Properties")) {
      if(g_str_equal(method_name, "GetAll")) {
        GVariant *variant = NULL;
        gchar string[] = 
          "({"
          "  'Type': <'peripheral'>, "
          "  'ServiceUUIDs': <['1812']>, "
          "  'LocalName': <'TestMouse'>, "
          "  'Appearance': <@q 0x03C0>,"
          "  'Discoverable': <true>, "
          "  'Includes': <['tx-power']>"
          "},)";
        variant = g_variant_new_parsed(string);
        g_print("variant: %s\n", g_variant_print(variant, TRUE));
        g_dbus_method_invocation_return_value(invocation, variant);
      }
    }
  }
}

static GDBusInterfaceVTable ad_interface_table = {
  ad_method_call,
  NULL,
  NULL,
  {0}
};


#define AGENT_OBJECT_PATH "/org/bluez/app/agent0"

guint agent_register_id = 0;
GDBusProxy *agent_manager_proxy = NULL;

GDBusNodeInfo *agent_node_info;
gchar *agent_xml =
  "<node>"
  "  <interface name='org.bluez.Agent1'>"
  "    <method name='Release'>"
  "    </method>"
  "    <method name='RequestPinCode'>"
  "      <arg name='device' direction='in' type='o' />"
  "      <arg name='pincode' direction='out' type='s' />"
  "    </method>"
  "    <method name='DisplayPinCode'>"
  "      <arg name='device' direction='in' type='o' />"
  "      <arg name='pincode' direction='in' type='s' />"
  "    </method>"
  "    <method name='RequestPasskey'>"
  "      <arg name='device' direction='in' type='o' />"
  "      <arg name='passkey' direction='out' type='u' />"
  "    </method>"
  "    <method name='DisplayPasskey'>"
  "      <arg name='device' direction='in' type='o' />"
  "      <arg name='passkey' direction='in' type='u' />"
  "      <arg name='entered' direction='in' type='q' />"
  "    </method>"
  "    <method name='RequestConfirmation'>"
  "      <arg name='device' direction='in' type='o' />"
  "      <arg name='passkey' direction='in' type='u' />"
  "    </method>"
  "    <method name='RequestAuthorization'>"
  "      <arg name='device' direction='in' type='o' />"
  "    </method>"
  "    <method name='AuthorizeService'>"
  "      <arg name='device' direction='in' type='o' />"
  "      <arg name='uuid' direction='in' type='s' />"
  "    </method>"
  "    <method name='Cancel'>"
  "    </method>"
  "  </interface>"
  "</node>";


static void agent_method_call (GDBusConnection       *connection,
                                const gchar           *sender,
                                const gchar           *object_path,
                                const gchar           *interface_name,
                                const gchar           *method_name,
                                GVariant              *parameters,
                                GDBusMethodInvocation *invocation,
                                gpointer               user_data) {

  g_print("sender: %s, object_path: %s, interface_name: %s, method_name: %s\n", sender, object_path, interface_name, method_name);
  g_print("parameters: %s\n", g_variant_print(parameters, TRUE));

  if(g_str_equal(object_path, AGENT_OBJECT_PATH)) {
    if(g_str_equal(interface_name, "org.bluez.Agent1")) {

      GVariant *variant = NULL;
      variant = g_variant_new("()");
      g_print("variant: %s\n", g_variant_print(variant, TRUE));
      g_dbus_method_invocation_return_value(invocation, variant);

      if(g_str_equal(method_name, "RequestConfirmation")) {

      }
    }
  }
}

static GDBusInterfaceVTable agent_interface_table = {
  agent_method_call,
  NULL,
  NULL,
  {0}
};



static void on_register_ad_ready (GObject *source_object,
                       GAsyncResult *res,
                       gpointer user_data) {

  GError *error  = NULL;
  GDBusProxy *proxy = (GDBusProxy *)source_object;
  GVariant *result = g_dbus_proxy_call_finish(proxy, res, &error);
  if(error != NULL) {
    g_print("Error register advertisement: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
    return ;
  }

  g_print("result: %s\n", g_variant_print(result, TRUE));
}

void start_advertisement(void) {

  g_print("---------------- Start advertisement!\n");
  GVariant *parameters = g_variant_new("(oa{sv})", AD_OBJECT_PATH, NULL);
  g_print("parameters: %s\n", g_variant_print(parameters, TRUE));
  g_dbus_proxy_call(ad_manager_proxy,
                    "RegisterAdvertisement",
                    parameters,
                    G_DBUS_CALL_FLAGS_NONE,
                    -1,
                    NULL,
                    on_register_ad_ready,
                    NULL);
}


static void on_unregister_ad_ready (GObject *source_object,
                       GAsyncResult *res,
                       gpointer user_data) {

  GError *error  = NULL;
  GDBusProxy *proxy = (GDBusProxy *)source_object;
  GVariant *result  = g_dbus_proxy_call_finish(proxy, res, &error);
  if(error != NULL) {
    g_print("Error unregister advertisement: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
    return ;
  }

  g_print("result: %s\n", g_variant_print(result, TRUE));
}

void stop_advertisement(void) {

  g_print("---------------- Stop advertisement!\n");
  GVariant *parameters = g_variant_new("(o)", AD_OBJECT_PATH);
  g_dbus_proxy_call(ad_manager_proxy,
                    "UnregisterAdvertisement",
                    parameters,
                    G_DBUS_CALL_FLAGS_NONE,
                    -1,
                    NULL,
                    on_unregister_ad_ready,
                    NULL);
}

static void on_register_agent_ready (GObject *source_object,
                       GAsyncResult *res,
                       gpointer user_data) {
  GError *error = NULL;
  GDBusProxy *proxy = (GDBusProxy *)source_object;
  GVariant* result = g_dbus_proxy_call_finish(proxy, res, &error);
  if(error != NULL) {
    g_print("Error register agent: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
    return ;
  }

  g_print("result: %s\n", g_variant_print(result, TRUE));

}

void register_agent(void) {

  g_print("---------------- Register Agent!\n");
  GVariant *parameters = g_variant_new("(os)", AGENT_OBJECT_PATH, "KeyboardDisplay");
  g_dbus_proxy_call(agent_manager_proxy,
                    "RegisterAgent",
                    parameters,
                    G_DBUS_CALL_FLAGS_NONE,
                    -1,
                    NULL,
                    on_register_agent_ready,
                    NULL);

}

static void on_unregister_agent_ready (GObject *source_object,
                       GAsyncResult *res,
                       gpointer user_data) {

  GError *error = NULL;
  GDBusProxy *proxy = (GDBusProxy *)source_object;
  GVariant* result = g_dbus_proxy_call_finish(proxy, res, &error);
  if(error != NULL) {
    g_print("Error unregister agent: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
    return ;
  }

  g_print("result: %s\n", g_variant_print(result, TRUE));

}

void unregister_agent(void) {

  g_print("---------------- Unregister Agent!\n");
  GVariant *parameters = g_variant_new("(o)", AGENT_OBJECT_PATH);
  g_dbus_proxy_call(agent_manager_proxy,
                    "UnregisterAgent",
                    parameters,
                    G_DBUS_CALL_FLAGS_NONE,
                    -1,
                    NULL,
                    on_register_agent_ready,
                    NULL);
}


void on_get_ad_manager_proxy_ready (GObject *source_object,
                       GAsyncResult *res,
                       gpointer user_data) {
  GError *error = NULL;

  ad_manager_proxy = g_dbus_proxy_new_finish(res, &error);
  if(error != NULL) {
    g_print("Error getting ad_manager_proxy: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
    return ;
  }

  g_print("---------------- Get advertisement object node info!\n");
  ad_node_info = g_dbus_node_info_new_for_xml(ad_xml, &error);
  if(error != NULL) {
    g_print("Error get advertisement object node info: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
    return ;
  }

  g_print("---------------- Register advertisement object!\n");
  ad_register_id = g_dbus_connection_register_object(connection,
                                    AD_OBJECT_PATH,
                                    ad_node_info->interfaces[0],
                                    &ad_interface_table,
                                    NULL, 
                                    NULL, 
                                    &error);
  if(error != NULL) {
    g_print("Error registering advertisement object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
    return ;
  }

  start_advertisement();
}

void on_get_agent_manager_proxy_ready (GObject *source_object,
                       GAsyncResult *res,
                       gpointer user_data) {
  GError *error = NULL;

  agent_manager_proxy = g_dbus_proxy_new_finish(res, &error);
  if(error != NULL) {
    g_print("Error getting agent_manager_proxy: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
    return ;
  }

  g_print("---------------- Get agent object node info!\n");
  agent_node_info = g_dbus_node_info_new_for_xml(agent_xml, &error);
  if(error != NULL) {
    g_print("Error get advertisement object node info: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
    return ;
  }

  g_print("---------------- Register agent object!\n");
  agent_register_id = g_dbus_connection_register_object(connection,
                                    AGENT_OBJECT_PATH,
                                    agent_node_info->interfaces[0],
                                    &agent_interface_table,
                                    NULL, 
                                    NULL, 
                                    &error);
  if(error != NULL) {
    g_print("Error registering advertisement object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
    return ;
  }

  register_agent();
}

void on_bluez_signal (GDBusConnection  *connection,
                const gchar      *sender_name,
                const gchar      *object_path,
                const gchar      *interface_name,
                const gchar      *signal_name,
                GVariant         *parameters,
                gpointer          user_data) {

  // g_print("\n");
  // g_print("sender_name: %s, object_path: %s, interface_name: %s, signal_name: %s\n", sender_name, object_path, interface_name, signal_name);
  // g_print("parameters: %s\n", g_variant_print(parameters, TRUE));

  if(g_str_equal(interface_name, "org.freedesktop.DBus.ObjectManager")) {
    if(g_str_equal(signal_name, "InterfacesAdded")) {

    } else if(g_str_equal(signal_name, "InterfacesRemoved")) {

    }
  } else if(g_str_equal(interface_name, "org.freedesktop.DBus.Properties")){ 
    if(g_str_equal(signal_name, "PropertiesChanged")) {
      
    }
  }
}


void on_bus_get_ready (GObject *source_object,
                       GAsyncResult *res,
                       gpointer user_data) {

  GError *error = NULL;
  g_print("---------------- Get system_bus_ready!\n");

  connection = g_bus_get_finish(res, &error);
  if(error != NULL) {
    g_print("Error getting bus: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
    return ;
  }

  g_print("---------------- Subscribe signal InterfacesAdded!\n");
  interface_added_subscribe_id = 
    g_dbus_connection_signal_subscribe(connection,
                                       NULL, 
                                       "org.freedesktop.DBus.ObjectManager", 
                                       "InterfacesAdded", 
                                       NULL,
                                       NULL,
                                       G_DBUS_SIGNAL_FLAGS_NONE,
                                       on_bluez_signal,
                                       NULL,
                                       NULL);

  g_print("---------------- Subscribe signal InterfacesRemoved!\n");
  interface_removed_subscribe_id = 
    g_dbus_connection_signal_subscribe(connection,
                                       NULL, 
                                       "org.freedesktop.DBus.ObjectManager", 
                                       "InterfacesRemoved", 
                                       NULL,
                                       NULL,
                                       G_DBUS_SIGNAL_FLAGS_NONE,
                                       on_bluez_signal,
                                       NULL,
                                       NULL);

  g_print("---------------- Subscribe signal PropertiesChanged!\n");
  properties_changes_subscribe_id =
    g_dbus_connection_signal_subscribe(connection,
                                       NULL,
                                       "org.freedesktop.DBus.Properties",
                                       "PropertiesChanged",
                                       NULL,
                                       NULL,
                                       G_DBUS_SIGNAL_FLAGS_NONE,
                                       on_bluez_signal,
                                       NULL,
                                       NULL);

  g_print("---------------- Get ad_manager_proxy !\n");
  g_dbus_proxy_new(connection,
                   G_DBUS_PROXY_FLAGS_NONE,
                   NULL,
                   "org.bluez",
                   "/org/bluez/hci0",
                   "org.bluez.LEAdvertisingManager1",
                   NULL,
                   on_get_ad_manager_proxy_ready,
                   NULL);


  g_print("---------------- Get agent_manager_proxy !\n");
  g_dbus_proxy_new(connection,
                   G_DBUS_PROXY_FLAGS_NONE,
                   NULL,
                   "org.bluez",
                   "/org/bluez",
                   "org.bluez.AgentManager1",
                   NULL,
                   on_get_agent_manager_proxy_ready,
                   NULL);

}

void on_system_signal(int signal) {

  g_main_loop_quit(main_loop);
  switch (signal) {
    case SIGINT:
      g_print("\n-------------------------------- Get signal SIGINT!\n");
      break;
    case SIGSEGV:
      g_print("\n-------------------------------- Get signal SIGSEGV!\n");
      exit(1);
      break;
    default:
      break;
  }
}

int main(int argc, char *argv[]) {

  signal(SIGINT, on_system_signal);
  signal(SIGSEGV, on_system_signal);

  g_bus_get(G_BUS_TYPE_SYSTEM, NULL, on_bus_get_ready, NULL);

  main_loop = g_main_loop_new(NULL, FALSE);
  g_main_loop_run(main_loop);

  g_print("==> Exit main_loop...\n");
  g_dbus_connection_signal_unsubscribe(connection, interface_added_subscribe_id);
  g_dbus_connection_signal_unsubscribe(connection, interface_removed_subscribe_id);

  stop_advertisement();
  g_dbus_connection_unregister_object(connection, ad_register_id);

  unregister_agent();
  g_dbus_connection_unregister_object(connection, agent_register_id);

  return 0;
}





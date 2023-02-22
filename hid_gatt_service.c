
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
          "  objectpath '/org/bluez/example/app0/service0': {"
          "    'org.bluez.GattService1': { "
          "      'UUID': <'00001812-0000-1000-8000-00805f9b34fb'>,"
          "      'Primary': <true>,"
          "      'Characteristics': <['/org/bluez/example/app0/service0/char0', '/org/bluez/example/app0/service0/char1', '/org/bluez/example/app0/service0/char2', '/org/bluez/example/app0/service0/char3', '/org/bluez/example/app0/service0/char4', '/org/bluez/example/app0/service0/char5']>"
          "    }"
          "  },"
          // HID Information
          "  objectpath '/org/bluez/example/app0/service0/char0': {"
          "    'org.bluez.GattCharacteristic1': { "
          "      'Service': <@o '/org/bluez/example/app0/service0'>,"
          "      'UUID': <'00002a4a-0000-1000-8000-00805f9b34fb'>,"
          "      'Flags': <['read']>,"
          "      'Descriptors': <@ao []>"
          "    }"
          "  },"
          // ReportMap
          "  objectpath '/org/bluez/example/app0/service0/char1': {"
          "    'org.bluez.GattCharacteristic1': { "
          "      'Service': <@o '/org/bluez/example/app0/service0'>,"
          "      'UUID': <'00002a4b-0000-1000-8000-00805f9b34fb'>,"
          "      'Flags': <['read']>,"
          "      'Descriptors': <@ao []>"
          "    }"
          "  },"
          // Control Point
          "  objectpath '/org/bluez/example/app0/service0/char2': {"
          "    'org.bluez.GattCharacteristic1': { "
          "      'Service': <@o '/org/bluez/example/app0/service0'>,"
          "      'UUID': <'00002a4c-0000-1000-8000-00805f9b34fb'>,"
          "      'Flags': <['write']>,"
          "      'Descriptors': <@ao []>"
          "    }"
          "  },"
          // Report 
          "  objectpath '/org/bluez/example/app0/service0/char3': {"
          "    'org.bluez.GattCharacteristic1': { "
          "      'Service': <@o '/org/bluez/example/app0/service0'>,"
          "      'UUID': <'00002a4d-0000-1000-8000-00805f9b34fb'>,"
          "      'Flags': <['read', 'write', 'notify']>,"
          "      'Descriptors': <@ao []>"
          "    }"
          "  },"
          "  objectpath '/org/bluez/example/app0/service0/char3/descriptor0': {"
          "    'org.bluez.GattDescriptor1': { "
          "      'Characteristic': <@o '/org/bluez/example/app0/service0/char3'>,"
          "      'UUID': <'00002908-0000-1000-8000-00805f9b34fb'>,"
          "      'Flags': <['read']>,"
          "      'Descriptors': <@ao []>"
          "    }"
          "  },"
          // Report
          "  objectpath '/org/bluez/example/app0/service0/char4': {"
          "    'org.bluez.GattCharacteristic1': { "
          "      'Service': <@o '/org/bluez/example/app0/service0'>,"
          "      'UUID': <'00002a4d-0000-1000-8000-00805f9b34fb'>,"
          "      'Flags': <['read','write', 'notify']>,"
          "      'Descriptors': <@ao []>"
          "    }"
          "  },"
          "  objectpath '/org/bluez/example/app0/service0/char4/descriptor0': {"
          "    'org.bluez.GattDescriptor1': { "
          "      'Characteristic': <@o '/org/bluez/example/app0/service0/char4'>,"
          "      'UUID': <'00002908-0000-1000-8000-00805f9b34fb'>,"
          "      'Flags': <['read']>,"
          "      'Descriptors': <@ao []>"
          "    }"
          "  },"
          // Report
          "  objectpath '/org/bluez/example/app0/service0/char5': {"
          "    'org.bluez.GattCharacteristic1': { "
          "      'Service': <@o '/org/bluez/example/app0/service0'>,"
          "      'UUID': <'00002a4d-0000-1000-8000-00805f9b34fb'>,"
          "      'Flags': <['read','write', 'notify']>,"
          "      'Descriptors': <@ao []>"
          "    }"
          "  },"
          "  objectpath '/org/bluez/example/app0/service0/char5/descriptor0': {"
          "    'org.bluez.GattDescriptor1': { "
          "      'Characteristic': <@o '/org/bluez/example/app0/service0/char5'>,"
          "      'UUID': <'00002908-0000-1000-8000-00805f9b34fb'>,"
          "      'Flags': <['read']>,"
          "      'Descriptors': <@ao []>"
          "    }"
          "  },"
          // Protocol Mode
          "  objectpath '/org/bluez/example/app0/service0/char6': {"
          "    'org.bluez.GattCharacteristic1': { "
          "      'Service': <@o '/org/bluez/example/app0/service0'>,"
          "      'UUID': <'00002a4e-0000-1000-8000-00805f9b34fb'>,"
          "      'Flags': <['read','write']>,"
          "      'Descriptors': <@ao []>"
          "    }"
          "  }, "
          // Battery Service
          "  objectpath '/org/bluez/example/app0/service1': {"
          "    'org.bluez.GattService1': { "
          "      'UUID': <'0000180f-0000-1000-8000-00805f9b34fb'>,"
          "      'Primary': <true>,"
          "      'Characteristics': <@ao ['/org/bluez/example/app0/service1/char0']>"
          "    }"
          "  },"
          // Battery Level
          "  objectpath '/org/bluez/example/app0/service1/char0': {"
          "    'org.bluez.GattCharacteristic1': { "
          "      'Service': <@o '/org/bluez/example/app0/service1'>,"
          "      'UUID': <'00002a19-0000-1000-8000-00805f9b34fb'>,"
          "      'Flags': <['read', 'notify']>,"
          "      'Descriptors': <@ao []>"
          "    }"
          "  },"
          // Device Info Service
          "  objectpath '/org/bluez/example/app0/service2': {"
          "    'org.bluez.GattService1': { "
          "      'UUID': <'0000180a-0000-1000-8000-00805f9b34fb'>,"
          "      'Primary': <true>,"
          "      'Characteristics': <@ao ['/org/bluez/example/app0/service2/char0', '/org/bluez/example/app0/service2/char1', '/org/bluez/example/app0/service2/char2']>"
          "    }"
          "  },"
          // Vendor
          "  objectpath '/org/bluez/example/app0/service2/char0': {"
          "    'org.bluez.GattCharacteristic1': { "
          "      'Service': <@o '/org/bluez/example/app0/service2'>,"
          "      'UUID': <'00002a29-0000-1000-8000-00805f9b34fb'>,"
          "      'Flags': <['read']>,"
          "      'Descriptors': <@ao []>"
          "    }"
          "  },"
          // Product
          "  objectpath '/org/bluez/example/app0/service2/char1': {"
          "    'org.bluez.GattCharacteristic1': { "
          "      'Service': <@o '/org/bluez/example/app0/service2'>,"
          "      'UUID': <'00002a24-0000-1000-8000-00805f9b34fb'>,"
          "      'Flags': <['read']>,"
          "      'Descriptors': <@ao []>"
          "    }"
          "  },"
          // Version
          "  objectpath '/org/bluez/example/app0/service2/char2': {"
          "    'org.bluez.GattCharacteristic1': { "
          "      'Service': <@o '/org/bluez/example/app0/service2'>,"
          "      'UUID': <'00002a28-0000-1000-8000-00805f9b34fb'>,"
          "      'Flags': <['read']>,"
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







gboolean is_notify = FALSE;
gboolean hid_report_func(gpointer user_data) {

  printf("-------- %s\n", __FUNCTION__);
  if(is_notify) {

    GRand *rand = g_rand_new_with_seed(g_get_real_time());
    guint16 val = g_rand_int_range(rand, 0, 0xFFFF);

    guchar data[4] = { 0 };

    data[0] = 0x00;
    data[1] = 0x10;
    data[2] = 0x10;
    data[3] = 0x00;

    GVariant *variant = g_variant_new_parsed("('org.bluez.GattCharacteristic1', {'Value': <@ay [%y, %y, %y, %y]>}, @as [])", data[0], data[1], data[2], data[3]);
    g_print("---- %s variant: %s\n", __FUNCTION__,  g_variant_print(variant, TRUE));

    GError *error = NULL;
    g_dbus_connection_emit_signal(connection, 
                                  NULL, 
                                  APP_OBJECT_PATH "/service0/char3", 
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
    return TRUE;
  }
  return FALSE;
}

static guchar protocol_mode = 1; 

static void hid_method_call (GDBusConnection       *connection,
                                const gchar           *sender,
                                const gchar           *object_path,
                                const gchar           *interface_name,
                                const gchar           *method_name,
                                GVariant              *parameters,
                                GDBusMethodInvocation *invocation,
                                gpointer               user_data) {

  g_print("sender: %s, object_path: %s, interface_name: %s, method_name: %s\n", sender, object_path, interface_name, method_name);

  if(g_str_equal(object_path, APP_OBJECT_PATH "/service0")) {

    if(g_str_equal(method_name, "GetAll")) {
      gchar value_string[] = 
        "("
        "  objectpath '/org/bluez/example/app0/service0': {"
        "    'org.bluez.GattService1': { "
        "      'UUID': <'00001812-0000-1000-8000-00805f9b34fb'>,"
        "      'Primary': <true>,"
        "      'Characteristics': <['/org/bluez/example/app0/service0/char0', '/org/bluez/example/app0/service0/char1', '/org/bluez/example/app0/service0/char2', '/org/bluez/example/app0/service0/char3', '/org/bluez/example/app0/service0/char4', '/org/bluez/example/app0/service0/char5']>"
        "    }"
        "  },)";

      GVariant *variant = g_variant_new_parsed(value_string);
      g_dbus_method_invocation_return_value(invocation, variant);
    }
  } else if(g_str_equal(object_path, APP_OBJECT_PATH "/service0/char0")) {
    // HID Information
    // 01110002
    if(g_str_equal(method_name, "ReadValue")) {
      // GVariant *variant = g_variant_new_parsed("(@ay [ 0x01, 0x01, 0x00, 0x02 ], )");
      GVariant *variant = g_variant_new_parsed("(@ay [ 0x01, 0x11, 0x00, 0x02 ], )");
      g_dbus_method_invocation_return_value(invocation, variant);
    }

  } else if(g_str_equal(object_path, APP_OBJECT_PATH "/service0/char1")) {
    // Report Map
    static const guchar hids_report_descriptor[] =
    {
      0x05, 0x01,   // USAGE_PAGE (Generic Desktop)
      0x09, 0x02,   // USAGE (Mouse)
      0xa1, 0x01,   // COLLECTION (Application)
      0x85, 0x01,   //   REPORT_ID (1)
      0x09, 0x01,   //   USAGE (Pointer)
      0xa1, 0x00,   //   COLLECTION (Physical)
      0x05, 0x09,   //         Usage Page (Buttons)
      0x19, 0x01,   //         Usage Minimum (1)
      0x29, 0x03,   //         Usage Maximum (3)
      0x15, 0x00,   //         Logical Minimum (0)
      0x25, 0x01,   //         Logical Maximum (1)
      0x95, 0x03,   //         Report Count (3)
      0x75, 0x01,   //         Report Size (1)
      0x81, 0x02,   //         Input(Data, Variable, Absolute); 3 button bits
      0x95, 0x01,   //         Report Count(1)
      0x75, 0x05,   //         Report Size(5)
      0x81, 0x03,   //         Input(Constant);                 5 bit padding
      0x05, 0x01,   //         Usage Page (Generic Desktop)
      0x09, 0x30,   //         Usage (X)
      0x09, 0x31,   //         Usage (Y)
      0x09, 0x38,   //         Usage (Wheel)
      0x15, 0x81,   //         Logical Minimum (-127)
      0x25, 0x7F,   //         Logical Maximum (127)
      0x75, 0x08,   //         Report Size (8)
      0x95, 0x03,   //         Report Count (3)
      0x81, 0x06,   //         Input(Data, Variable, Relative); 3 position bytes (X,Y,Wheel)
      0xc0,         //   END_COLLECTION
      0xc0          // END_COLLECTION
    };

    // static const guchar hids_report_descriptor[] = {
    //   0x05, 0x01,    // USAGE_PAGE (Generic Desktop)
    //   0x09, 0x02,    // USAGE (Mouse)
    //   0xA1, 0x01,    // COLLECTION (Application)
    //   0x09, 0x01,    //   USAGE (Pointer)
    //   0xA1, 0x00,    //   COLLECTION (Physical)
    //   0x05, 0x09,    //     USAGE_PAGE (Button)
    //   0x19, 0x01,    //     USAGE_MINIMUM (Button 1)
    //   0x29, 0x03,    //     USAGE_MAXIMUM (Button 3)
    //   0x15, 0x00,    //     LOGICAL_MINIMUM (0)
    //   0x25, 0x01,    //     LOGICAL_MAXIMUM (1)
    //   0x95, 0x03,    //     REPORT_COUNT (3)
    //   0x75, 0x01,    //     REPORT_SIZE (1)
    //   0x81, 0x02,    //     INPUT (Data,Var,Abs)
    //   0x95, 0x01,    //     REPORT_COUNT (1)
    //   0x75, 0x05,    //     REPORT_SIZE (5)
    //   0x81, 0x03,    //     INPUT (Cnst,Var,Abs)
    //   0x05, 0x01,    //     USAGE_PAGE (Generic Desktop)
    //   0x09, 0x30,    //     USAGE (X)
    //   0x09, 0x31,    //     USAGE (Y)
    //   0x15, 0x81,    //     LOGICAL_MINIMUM (-127)
    //   0x25, 0x7F,    //     LOGICAL_MAXIMUM (127)
    //   0x75, 0x08,    //     REPORT_SIZE (8)
    //   0x95, 0x02,    //     REPORT_COUNT (2)
    //   0x81, 0x06,    //     INPUT (Data,Var,Rel)
    //   0xC0,        //  END_COLLECTION
    //   0xC0         // END_COLLECTION
    // };

    if(g_str_equal(method_name, "ReadValue")) {

      int map_len = 0;
      GVariantBuilder map_builder;
      map_len = sizeof(hids_report_descriptor);
      g_variant_builder_init(&map_builder, G_VARIANT_TYPE_ARRAY);
      for(int i = 0; i < map_len; i++) {
        g_variant_builder_add(&map_builder, "y", hids_report_descriptor[i]);
      }
      GVariant *variant = g_variant_builder_end(&map_builder);
      g_print("%s\n", g_variant_print(variant, TRUE));

      GVariant *variant_0 = g_variant_new("(@ay)", variant);
      g_print("%s\n", g_variant_print(variant_0, TRUE));

      g_dbus_method_invocation_return_value(invocation, variant_0);
    }
  } else if(g_str_equal(object_path, APP_OBJECT_PATH "/service0/char2")) {
    // Control Point
    if(g_str_equal(method_name, "WriteValue")) {

      g_print("parameters: %s\n", g_variant_print(parameters, TRUE));
      g_dbus_method_invocation_return_value(invocation, g_variant_new_parsed("()"));
    }
  } else if(g_str_equal(object_path, APP_OBJECT_PATH "/service0/char3")) {
    // Report 1
    if(g_str_equal(method_name, "ReadValue")) {
      GVariant *variant = g_variant_new_parsed("(@ay [ 0x01 ], )");
      g_dbus_method_invocation_return_value(invocation, variant); 
    } else if(g_str_equal(method_name, "StartNotify")) {
      is_notify = TRUE;
      g_timeout_add_seconds(1, hid_report_func, NULL);
    } else if(g_str_equal(method_name, "StopNotify")) {
      is_notify = FALSE;
    }
  } else if(g_str_equal(object_path, APP_OBJECT_PATH "/service0/char3/descriptor0")) {
    if(g_str_equal(method_name, "ReadValue")) {
      GVariant *variant = g_variant_new_parsed("(@ay [ 0x01 0x01], )");
      g_dbus_method_invocation_return_value(invocation, variant); 
    }
  } else if(g_str_equal(object_path, APP_OBJECT_PATH "/service0/char4")) {
    // Report write
    if(g_str_equal(method_name, "ReadValue")) {
      GVariant *variant = g_variant_new_parsed("(@ay [ 0x01 ], )");
      g_dbus_method_invocation_return_value(invocation, variant); 
    } else if(g_str_equal(method_name, "WriteValue")) {
      g_print("parameters: %s\n", g_variant_print(parameters, TRUE));
      g_dbus_method_invocation_return_value(invocation, g_variant_new_parsed("()"));
    }
  } else if(g_str_equal(object_path, APP_OBJECT_PATH "/service0/char4/descriptor0")) {

    if(g_str_equal(method_name, "ReadValue")) {
      GVariant *variant = g_variant_new_parsed("(@ay [ 0x02 0x01], )");
      g_dbus_method_invocation_return_value(invocation, variant); 
    }
  } else if(g_str_equal(object_path, APP_OBJECT_PATH "/service0/char5")) {
    // Report
    if(g_str_equal(method_name, "ReadValue")) {
      GVariant *variant = g_variant_new_parsed("(@ay [ 0x01 ], )");
      g_dbus_method_invocation_return_value(invocation, variant); 
    } else if(g_str_equal(method_name, "WriteValue")) {
      g_print("parameters: %s\n", g_variant_print(parameters, TRUE));
      g_dbus_method_invocation_return_value(invocation, g_variant_new_parsed("()"));
    }
  } else if(g_str_equal(object_path, APP_OBJECT_PATH "/service0/char5/descriptor0")) {
    if(g_str_equal(method_name, "ReadValue")) {
      GVariant *variant = g_variant_new_parsed("(@ay [ 0x03 0x01], )");
      g_dbus_method_invocation_return_value(invocation, variant); 
    }

  } else if(g_str_equal(object_path, APP_OBJECT_PATH "/service0/char6")) {

    // Protocol Mode
    if(g_str_equal(method_name, "ReadValue")) {

      g_print("protocol_mode: %d\n", protocol_mode);
      GVariant *variant = g_variant_new_parsed("(@ay [ %y ], )", protocol_mode);
      g_dbus_method_invocation_return_value(invocation, variant);

    } else if(g_str_equal(method_name, "WriteValue")) {

      g_print("parameters: %s\n", g_variant_print(parameters, TRUE));

      GVariant *variant;
      g_variant_get(parameters, "(@ay@a{sv})", &variant, NULL);

      GVariantIter *iter;
      g_variant_get(variant, "ay", &iter);

      guchar val = 0;
      while(g_variant_iter_loop(iter, "y", &val)) {
        protocol_mode = val;
        g_print("protocol_mode: %d\n", protocol_mode);
      }

      g_dbus_method_invocation_return_value(invocation, g_variant_new_parsed("()"));
    }
  } 
}

GDBusInterfaceVTable hid_interface_table = {
  hid_method_call,
  NULL,
  NULL,
  {0}
};

static void battery_method_call (GDBusConnection       *connection,
                                const gchar           *sender,
                                const gchar           *object_path,
                                const gchar           *interface_name,
                                const gchar           *method_name,
                                GVariant              *parameters,
                                GDBusMethodInvocation *invocation,
                                gpointer               user_data) {

  if(g_str_equal(object_path, APP_OBJECT_PATH "/service1")) {

    gchar value_string[] = 
      "  objectpath '/org/bluez/example/app0/service1': {"
      "    'org.bluez.GattService1': { "
      "      'UUID': <'0000180f-0000-1000-8000-00805f9b34fb'>,"
      "      'Primary': <true>,"
      "      'Characteristics': <@ao ['/org/bluez/example/app0/service1/char0']>"
      "    }"
      "  }";
    GVariant* variant = g_variant_new_parsed(value_string);
    g_dbus_method_invocation_return_value(invocation, variant);
    
  } else if(g_str_equal(object_path, APP_OBJECT_PATH "/service1/char0")) {
    if(g_str_equal(method_name, "ReadValue")) {

      GVariant *variant = g_variant_new_parsed("(@ay [ %y ], )", 99);
      g_dbus_method_invocation_return_value(invocation, variant);
    }
  }
}

GDBusInterfaceVTable battery_interface_table = {
  battery_method_call,
  NULL,
  NULL,
  {0}
};

static void dev_info_method_call (GDBusConnection       *connection,
                                const gchar           *sender,
                                const gchar           *object_path,
                                const gchar           *interface_name,
                                const gchar           *method_name,
                                GVariant              *parameters,
                                GDBusMethodInvocation *invocation,
                                gpointer               user_data) {

  g_print("sender: %s, object_path: %s, interface_name: %s, method_name: %s\n", sender, object_path, interface_name, method_name);
  if(g_str_equal(object_path, APP_OBJECT_PATH "/service2")) {

  } else if(g_str_equal(object_path, APP_OBJECT_PATH "/service2/char0")) {
    if(g_str_equal(method_name, "ReadValue")) {
      const guchar *ptr = NULL;
      const guchar vender[] = "HodgeCode";

      GVariantBuilder vendor_builder;
      g_variant_builder_init(&vendor_builder, G_VARIANT_TYPE_ARRAY);

      ptr = vender;
      while(*ptr != '\0') {
        g_variant_builder_add(&vendor_builder, "y", *ptr);
        ptr++;
      }
      GVariant* array_variant = g_variant_builder_end(&vendor_builder);
      g_print("array_variant: %s\n", g_variant_print(array_variant, TRUE));

      GVariant *variant = g_variant_new("(@ay)", array_variant);
      g_print("variant: %s\n", g_variant_print(array_variant, TRUE));
      g_dbus_method_invocation_return_value(invocation, variant);
    }
  } else if(g_str_equal(object_path, APP_OBJECT_PATH "/service2/char1")) {
    if(g_str_equal(method_name, "ReadValue")) {
      const guchar *ptr = NULL;
      const guchar vender[] = "smartRemotes";

      GVariantBuilder product_builder;
      g_variant_builder_init(&product_builder, G_VARIANT_TYPE_ARRAY);

      ptr = vender;
      while(*ptr != '\0') {
        g_variant_builder_add(&product_builder, "y", *ptr);
        ptr++;
      }
      GVariant* array_variant = g_variant_builder_end(&product_builder);
      g_print("array_variant: %s\n", g_variant_print(array_variant, TRUE));

      GVariant *variant = g_variant_new("(@ay)", array_variant);
      g_print("variant: %s\n", g_variant_print(array_variant, TRUE));

      g_dbus_method_invocation_return_value(invocation, variant);
    }
  } else if(g_str_equal(object_path, APP_OBJECT_PATH "/service2/char2")) {
    if(g_str_equal(method_name, "ReadValue")) {
      const guchar *ptr = NULL;
      const guchar vender[] = "version 1.0.0";

      GVariantBuilder product_builder;
      g_variant_builder_init(&product_builder, G_VARIANT_TYPE_ARRAY);

      ptr = vender;
      while(*ptr != '\0') {
        g_variant_builder_add(&product_builder, "y", *ptr);
        ptr++;
      }
      GVariant* array_variant = g_variant_builder_end(&product_builder);
      g_print("array_variant: %s\n", g_variant_print(array_variant, TRUE));

      GVariant *variant = g_variant_new("(@ay)", array_variant);
      g_print("variant: %s\n", g_variant_print(array_variant, TRUE));

      g_dbus_method_invocation_return_value(invocation, variant);
    }
  }
}

GDBusInterfaceVTable dev_info_interface_table = {
  dev_info_method_call,
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
                                    &hid_interface_table,
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
                                    &hid_interface_table,
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
                                    &hid_interface_table,
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
                                    &hid_interface_table,
                                    NULL, 
                                    NULL,
                                    &error);
  if(error != NULL) {
    fprintf(stderr, "Errorr registering object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  guint char_3_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH "/service0/char3",
                                    characteristic_introspection_data->interfaces[0],
                                    &hid_interface_table,
                                    NULL, 
                                    NULL,
                                    &error);
  if(error != NULL) {
    fprintf(stderr, "Errorr registering object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  guint char_3_desc_0_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH "/service0/char3/descriptior0",
                                    characteristic_introspection_data->interfaces[0],
                                    &hid_interface_table,
                                    NULL, 
                                    NULL,
                                    &error);
  if(error != NULL) {
    fprintf(stderr, "Errorr registering object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  guint char_4_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH "/service0/char4",
                                    characteristic_introspection_data->interfaces[0],
                                    &hid_interface_table,
                                    NULL, 
                                    NULL,
                                    &error);
  if(error != NULL) {
    fprintf(stderr, "Errorr registering object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  guint char_4_desc_0_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH "/service0/char4/descriptor0",
                                    characteristic_introspection_data->interfaces[0],
                                    &hid_interface_table,
                                    NULL, 
                                    NULL,
                                    &error);
  if(error != NULL) {
    fprintf(stderr, "Errorr registering object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  guint char_5_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH "/service0/char5",
                                    characteristic_introspection_data->interfaces[0],
                                    &hid_interface_table,
                                    NULL, 
                                    NULL,
                                    &error);
  if(error != NULL) {
    fprintf(stderr, "Errorr registering object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  guint char_5_desc_0_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH "/service0/char5/descriptor0",
                                    characteristic_introspection_data->interfaces[0],
                                    &hid_interface_table,
                                    NULL, 
                                    NULL,
                                    &error);
  if(error != NULL) {
    fprintf(stderr, "Errorr registering object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  guint char_6_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH "/service0/char6",
                                    characteristic_introspection_data->interfaces[0],
                                    &hid_interface_table,
                                    NULL, 
                                    NULL,
                                    &error);
  if(error != NULL) {
    fprintf(stderr, "Errorr registering object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  guint service_1_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH "/service1",
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

  guint service_1_char_0_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH "/service1/char0",
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


  guint service_2_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH "/service2",
                                    service_introspection_data->interfaces[0],
                                    &dev_info_interface_table,
                                    NULL, 
                                    NULL,
                                    &error);
  if(error != NULL) {
    fprintf(stderr, "Errorr registering object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  guint service_2_char_0_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH "/service2/char0",
                                    characteristic_introspection_data->interfaces[0],
                                    &dev_info_interface_table,
                                    NULL, 
                                    NULL,
                                    &error);
  if(error != NULL) {
    fprintf(stderr, "Errorr registering object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  guint service_2_char_1_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH "/service2/char1",
                                    characteristic_introspection_data->interfaces[0],
                                    &dev_info_interface_table,
                                    NULL, 
                                    NULL,
                                    &error);
  if(error != NULL) {
    fprintf(stderr, "Errorr registering object: %s\n", error->message);
    g_error_free(error);
    raise(SIGINT);
  }

  guint service_2_char_2_register_id = g_dbus_connection_register_object(connection, 
                                    APP_OBJECT_PATH "/service2/char2",
                                    characteristic_introspection_data->interfaces[0],
                                    &dev_info_interface_table,
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
  g_dbus_connection_unregister_object(connection, char_0_register_id);
  g_dbus_connection_unregister_object(connection, char_1_register_id);
  g_dbus_connection_unregister_object(connection, char_2_register_id);
  g_dbus_connection_unregister_object(connection, char_3_register_id);
  g_dbus_connection_unregister_object(connection, char_4_register_id);

  g_dbus_connection_unregister_object(connection, service_1_register_id);
  g_dbus_connection_unregister_object(connection, service_1_char_0_register_id);

  g_dbus_connection_unregister_object(connection, service_2_register_id);
  g_dbus_connection_unregister_object(connection, service_2_char_0_register_id);
  g_dbus_connection_unregister_object(connection, service_2_char_1_register_id);
  g_dbus_connection_unregister_object(connection, service_2_char_2_register_id);
  return 0;
}








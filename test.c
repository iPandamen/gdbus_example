
#include <glib.h>
#include <string.h>

gchar variant_string[] = 
  "{"
  "  '/org/bluez/example/service0': { "
  "    'org.bluez.GattService1': { "
  "      'UUID': <'0000180d-0000-1000-8000-00805f9b34fb'>, "
  "      'Primary': <true>, "
  "      'Characteristics': <['/org/bluez/example/service0/char0', '/org/bluez/example/service0/char1','/org/bluez/example/service0/char2']>"
  "    }"
  "  }, "
  "  '/org/bluez/example/service0/char0': { "
  "    'org.bluez.GattCharacteristic1': {"
  "      'Service': <'/org/bluez/example/service0'>, "
  "      'UUID': <'00002a37-0000-1000-8000-00805f9b34fb'>, "
  "      'Flags': <['notify']>, "
  "      'Descriptors': <@as []>"
  "    } "
  "  },"
  "  '/org/bluez/example/service0/char1': {"
  "    'org.bluez.GattCharacteristic1': {"
  "      'Service': <'/org/bluez/example/service0'>, "
  "      'UUID': <'00002a38-0000-1000-8000-00805f9b34fb'>, "
  "      'Flags': <['read']>, "
  "      'Descriptors': <@as []>"
  "    }"
  "  }, "
  "  '/org/bluez/example/service0/char2': {"
  "    'org.bluez.GattCharacteristic1': {"
  "      'Service': <'/org/bluez/example/service0'>, "
  "      'UUID': <'00002a39-0000-1000-8000-00805f9b34fb'>, "
  "      'Flags': <['write']>, "
  "      'Descriptors': <@as []>"
  "    }"
  "  }"
  "}";


int main(int argc, char *argv[])
{

  GVariant *data;
  gint value = 1;
  gint max = 3;

  /* type (oa{sa{sv}) */
  data = g_variant_new_parsed ("(%o, {'brightness': {'value': <%i>, 'max': <%i>}})",
                               "/object/path", value, max);
  {
    GVariant *params;
    GVariant *p_brightness;
    gchar *obj;
    gint p_max;

    g_variant_get (data, "(o@a{?*})", &obj, &params);
    g_print ("object_path: %s\n", obj);

    p_brightness = g_variant_lookup_value (params, "brightness", G_VARIANT_TYPE_VARDICT);
    g_variant_lookup (p_brightness, "max", "i", &p_max);
    g_print ("max: %d\n", p_max);

  }

  g_print("data: %s\n", g_variant_print(data, TRUE));

  data = g_variant_new_parsed(g_variant_print(data, TRUE));
  g_print("data: %s\n", g_variant_print(data, TRUE));

  // a{oa{sa{sv}}}

  GVariant *variant = NULL;

  variant = g_variant_new_parsed("<[<'string'>, <'asdfs'>, <222>, <true>]>");
  g_print("variant: %s\n", g_variant_print(variant, TRUE));
  g_variant_unref(variant);

  const gchar *string = 
    "{"
    "  '/org/bluez/BatteryService/service000': {"
    "    'UUID': <'0000180d-0000-1000-8000-00805f9b34fb'>,"
    "    'Primary': <true>"
    "  }"
    "}";
  variant = g_variant_new_parsed(string);
  g_print("variant: %s\n", g_variant_print(variant, TRUE));
  g_variant_unref(variant);

  g_print("string length: %d\n", (int)strlen(variant_string));
  variant = g_variant_new_parsed(variant_string);
  g_print("variant: %s\n", g_variant_print(variant, TRUE));
  g_variant_unref(variant);



  return 0;
}

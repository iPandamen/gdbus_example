#include <stdio.h>

#include <glib-object.h>
#include <gio/gio.h>

void numeric_type_test(void) {

  printf("\n-------- %s --------\n", __FUNCTION__);
  /* b gboolean
   * y guchar
   * n gint16
   * q guint16
   * i gint32
   * u guint32
   * x gint64
   * t guint64
   * h gint32
   * d gdouble
   */

  GVariant * value0;
  value0 = g_variant_new("y", 200);

  int a = 0;
  printf("%d\n", a);
  g_variant_get(value0, "y", &a);
  printf("%d\n", a);

  guchar b;
  printf("%d\n", b);
  g_variant_get(value0, "y", &b);
  printf("%d\n", b);

  g_variant_unref(value0);
}

void string_type_test(void) {
  printf("\n-------- %s --------\n", __FUNCTION__);
  GVariant *value1, *value2, *value3;

  value1 = g_variant_new("s", "hello world");
  value2 = g_variant_new("o", "/must/be/a/valid/path");
  value3 = g_variant_new("g", "iias");

  gchar *result;
  g_variant_get(value1, "s", &result);
  g_print("It was '%s'\n", result);
  g_free(result);

  g_variant_get(value2, "o", &result);
  g_print("It was '%s'\n", result);
  g_free(result);

  g_variant_get(value3, "g", &result);
  g_print("It was '%s'\n", result);
  g_free(result);

  g_variant_unref(value1);
  g_variant_unref(value2);
  g_variant_unref(value3);
}


void array_type_test(void) {
  printf("\n-------- %s --------\n", __FUNCTION__);

  GVariantBuilder *builder;
  GVariant *value;

  builder = g_variant_builder_new(G_VARIANT_TYPE("as"));
  g_variant_builder_add(builder, "s", "when");
  g_variant_builder_add(builder, "s", "in");
  g_variant_builder_add(builder, "s", "the");
  g_variant_builder_add(builder, "s", "course");

  value = g_variant_new("as", builder);
  g_variant_builder_unref(builder);

  GVariantIter *iter;
  gchar *str;

  g_variant_get(value, "as", &iter);

  while(g_variant_iter_loop(iter, "s", &str)) {
    g_print("%s\n", str);
  }
  g_variant_iter_free(iter);

  printf("%s\n", g_variant_print(value, TRUE));
  g_variant_unref(value);
}

void maybe_type_test(void) {

  printf("\n-------- %s --------\n", __FUNCTION__);

  GVariant *value1, *value2;
  value1 = g_variant_new("m(ii)", TRUE,  123, 456);
  value2 = g_variant_new("m(ii)", FALSE,  789, 654);


  gboolean bb;
  gint32 i1 = 0, i2 = 0; 
  g_variant_get(value1, "m(ii)", &bb, &i1, &i2);
  g_print("%d, %d, %d\n", bb, i1, i2);

  g_variant_get(value2, "m(ii)", &bb, &i1, &i2);
  g_print("%d, %d, %d\n", bb, i1, i2);

  printf("%s\n", g_variant_print(value1, TRUE));
}


void parsed_test(void) {

  printf("\n-------- %s --------\n", __FUNCTION__);

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
  g_variant_unref(variant);
}



int main(int argc, char *argv[]) {

  numeric_type_test();
  string_type_test();
  array_type_test();
  maybe_type_test();
  parsed_test();
  return 0;
}


#include <stdio.h>

#include <glib-object.h>
#include <gio/gio.h>

void numeric_type_test(void){

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




static const gchar* bluez_adapter_get_property(GDBusConnection* conn, const gchar* path, const char *name) {

  GVariant *result;
  GVariant *prop = NULL;
  GError *error = NULL;
  const gchar *address = NULL;

  result = g_dbus_connection_call_sync(conn, 
                                      "org.bluez",
                                      path,
                                      "org.freedesktop.DBus.Properties",
                                      "Get",
                                      g_variant_new("(ss)", "org.bluez.Adapter1", name),
                                      NULL,
                                      G_DBUS_CALL_FLAGS_NONE,
                                      -1,
                                      NULL, 
                                      &error);

  if(error != NULL) {
    g_print("%s", error->message);
    return NULL;
  }

  g_print("%s\n", g_variant_print(result, TRUE));

  g_variant_get(result,"(v)", &prop);
  g_variant_unref(result);
  return g_variant_get_string(prop, NULL);
}


static void bluez_list_controllers(GDBusConnection *conn, GAsyncResult *res, gpointer data) {

  GVariant *result = NULL;
  GMainLoop* loop = NULL;
  gchar *s = NULL;
  GVariantIter iter;

  const gchar *object_path;

  GVariant *ifaces_and_properties;

  loop = (GMainLoop*)data;

  result = g_dbus_connection_call_finish(conn, res, NULL);
  if(result == NULL) {
    g_print("Unable ot get result for GetManagedObjects\n");
  }

  /* Parse the result */
  if(result) {

    // g_print(g_variant_print(result, TRUE));

    result = g_variant_get_child_value(result, 0);
    
    g_variant_iter_init(&iter, result);
    while(g_variant_iter_next(&iter, "{&o@a{sa{sv}}}", &object_path, &ifaces_and_properties)) {
      const gchar *interface_name;
      GVariant *properties;
      GVariantIter ii;

      g_variant_iter_init(&ii, ifaces_and_properties);
      while(g_variant_iter_next(&ii, "{&s@a{sv}}", &interface_name, &properties)) {
        if(g_strstr_len(g_ascii_strdown(interface_name, -1), -1, "adapter")) {
          g_print("HCI Name: %s Address: %s\n", bluez_adapter_get_property(conn, object_path, "Name"), bluez_adapter_get_property(conn, object_path, "Address"));
        }
        g_variant_unref(properties);
      }
      g_variant_unref(ifaces_and_properties);
    }
    g_variant_unref(result);
  }
  g_main_loop_quit(loop);
}


int gdbus_test(void) {

  GMainLoop *loop;
  GDBusConnection *conn;

  conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, NULL);
  if(conn == NULL) {
    g_print("Not able to get conneciton to system bus\n");
    return -1;
  }

  loop = g_main_loop_new(NULL, FALSE);
  g_dbus_connection_call(conn, "org.bluez", "/", "org.freedesktop.DBus.ObjectManager", "GetManagedObjects", NULL,  G_VARIANT_TYPE("(a{oa{sa{sv}}})"), G_DBUS_CALL_FLAGS_NONE, -1, NULL, (GAsyncReadyCallback)bluez_list_controllers, loop);
  g_main_loop_run(loop);

  g_object_unref(conn);
  return 0;
  // g_dbus_proxy_new_for_bus(DBUS_BUS_SYSTEM, 0, NULL, )
  // g_dbus_proxy_call()
  // g_dbus_proxy_get_connection()
}

int main(int argc, char *argv[]) {

  // numeric_type_test();
  // string_type_test();
  // array_type_test();
  // maybe_type_test();
  gdbus_test();
  return 0;
}


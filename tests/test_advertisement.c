#include <gio/gio.h>
#include <stdio.h>

int main() {
    GError *error = NULL;

    // 连接到 D-Bus
    GDBusConnection *connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if (error) {
        g_print("Failed to get D-Bus connection: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    // 创建代理
    GDBusProxy *proxy = g_dbus_proxy_new_sync(connection, G_DBUS_PROXY_FLAGS_NONE, NULL, "org.bluez",
                                              "/org/bluez/hci0", "org.bluez.LEAdvertisingManager1", NULL, &error);
    if (error) {
        g_print("Failed to create proxy: %s\n", error->message);
        g_error_free(error);
        g_object_unref(connection);
        return 1;
    }

    // 构造参数
    GVariantBuilder options_builder;
    g_variant_builder_init(&options_builder, G_VARIANT_TYPE_DICTIONARY);
    GVariant *options = g_variant_builder_end(&options_builder);

    // 调用 RegisterAdvertisement 方法
    GVariant *result = g_dbus_proxy_call_sync(proxy, "RegisterAdvertisement", g_variant_new("(oa{sa{sv}})", "/", options),
                                              G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
    if (error) {
        g_print("Failed to call RegisterAdvertisement: %s\n", error->message);
        g_error_free(error);
        g_object_unref(proxy);
        g_object_unref(connection);
        return 1;
    }

    g_variant_unref(result);
    g_object_unref(proxy);
    g_object_unref(connection);
    return 0;
}


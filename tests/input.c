#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/select.h>
#include <linux/input.h>

#include <glib.h>
#include <gio/gio.h>


#define APP_OBJECT_PATH "/org/bluez/example/app/hid"
GDBusConnection *connection = NULL;

void update(uint8_t btn,uint8_t rel_x, uint8_t rel_y, uint8_t rel_wheel) {

  GVariant *variant = g_variant_new_parsed("('org.bluez.GattCharacteristic1', {'Value': <@ay [%y, %y, %y, %y]>}, @as [])", btn, rel_x, rel_y, rel_wheel);
  g_print("---- %s variant: %s\n", __FUNCTION__,  g_variant_print(variant, TRUE));

  GError *error = NULL;
  g_dbus_connection_emit_signal(connection, 
                                NULL, 
                                APP_OBJECT_PATH "/service0/char5", 
                                "org.freedesktop.DBus.Properties",
                                "PropertiesChanged", 
                                variant, 
                                &error);
  if(error != NULL) {
    fprintf(stderr, "Error emitting PropertiesChanged: %s\n", error->message);
    g_error_free(error);
  }
}

int main(int argc, char *argv[]) {

  int fd = open("/dev/input/event1", O_RDONLY);
  if(fd < 0 ) {
    printf("Open file failed!\n");
    exit(1);
  }

  connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, NULL);

  fd_set rd_set;
  uint8_t rel_x = 0, rel_y = 0, rel_wheel = 0, btn = 0;
  do {
    struct timeval tv =  {
      .tv_sec = 1,
      .tv_usec = 0
    };

    FD_ZERO(&rd_set);
    FD_SET(fd, &rd_set);

    int rc = select(fd+1, &rd_set, NULL, NULL, &tv);
    if(rc < 0) {
      printf("Select failed\n");
    } else if(rc == 0) {
      printf("timeout...\n");
    } else {

      if(FD_ISSET(fd, &rd_set)) {
        struct input_event ev;
        rc = read(fd, &ev, sizeof(ev));
        if(rc < 0) {
          printf("read failed!\n");
        } else {
          switch(ev.type) {
            case EV_REL:
              if(ev.code == REL_X)  {
                rel_x = ev.value;
              } else if(ev.code == REL_Y) {
                rel_y = ev.value;
              } else if(ev.code == REL_WHEEL) {
                rel_wheel = ev.value;
              }
            case EV_KEY:
              if(ev.code == BTN_LEFT) {
                if(ev.value) {
                  btn |= 0x01;
                } else {
                  btn &= ~0x01;
                }
              } else if(ev.code == BTN_RIGHT) {
                if(ev.value) {
                  btn |= 0x04;
                } else {
                  btn &= ~0x04;
                }
              } else if(ev.code == BTN_MIDDLE) {
                if(ev.value) {
                  btn |= 0x02;
                } else {
                  btn &= ~0x02;
                }
              }
              printf("ev.value: %d\n", ev.value);
              printf("btn: %#x, rel_x: %d, rel_y: %d, rel_wheel: %d\n", btn, rel_x, rel_y, rel_wheel);
              break;
            case EV_SYN:
              // printf("btn: %#x, rel_x: %d, rel_y: %d, rel_wheel: %d\n", btn, rel_x, rel_y, rel_wheel);
              update(btn, rel_x, rel_y, rel_wheel);
              rel_x = 0;
              rel_y = 0;
              rel_wheel = 0;
              break;
            default:
              break;
          }
        }
      }
    }
  }while(1);
  close(fd);
  return 0;
}








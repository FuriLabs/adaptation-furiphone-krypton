#define GNOME_DESKTOP_USE_UNSTABLE_API
#include <gtk/gtk.h>
#include <libgnome-desktop/gnome-rr.h>
#include <glib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// this is a hack/workaround to fix the initial screen frame times of our ili7807s driver
int main(void) {
    GnomeRRScreen *rr_screen;
    GError *error = NULL;
    gboolean ret;
    char brightness[10];
    FILE *brightness_file, *pipe;
    int saved_brightness;
    char cmd_output[10];

    gtk_init(NULL, NULL);

    while (1) {
        // this is not good but it works for now
        pipe = popen("batman-helper wlrdisplay", "r");
        if (!pipe) {
            perror("Failed to run command");
            return -1;
        }

        if (fgets(cmd_output, sizeof(cmd_output), pipe) == NULL) {
            perror("Failed to read command output");
            pclose(pipe);
            return -1;
        }
        pclose(pipe);

        if (strcmp(cmd_output, "yes\n") == 0) {
            fprintf(stderr, "output from 'batman-helper wlrdisplay': %s", cmd_output);
            break;
        } else {
            usleep(500000);
            fprintf(stderr, "Unexpected output from 'batman-helper wlrdisplay': %s", cmd_output);
            continue;
        }
    }

    //sleep(2);

    // Get the default screen to operate on
    rr_screen = gnome_rr_screen_new(gdk_screen_get_default(), &error);
    if (!rr_screen) {
        g_warning("Unable to create GnomeRRScreen: %s", error->message);
        g_clear_error(&error);
        return -1;
    }

    brightness_file = fopen("/sys/class/leds/lcd-backlight/brightness", "r");
    if (!brightness_file) {
        perror("Failed to open brightness file for reading");
        return -1;
    }

    if (fgets(brightness, sizeof(brightness), brightness_file) == NULL) {
        perror("Failed to read brightness");
        fclose(brightness_file);
        return -1;
    }

    fclose(brightness_file);

    saved_brightness = atoi(brightness);

    brightness_file = fopen("/sys/class/leds/lcd-backlight/brightness", "w");
    if (!brightness_file) {
        perror("Failed to open brightness file for writing");
        return -1;
    }

    if (fputs("0", brightness_file) == EOF) {
        perror("Failed to set brightness to 0");
        fclose(brightness_file);
        return -1;
    }

    fclose(brightness_file);

    ret = gnome_rr_screen_set_dpms_mode(rr_screen, GNOME_RR_DPMS_OFF, &error);
    if (!ret) {
        g_warning("Failed to set DPMS mode to OFF: %s", error->message);
        g_clear_error(&error);
    }

    usleep(500000);

    ret = gnome_rr_screen_set_dpms_mode(rr_screen, GNOME_RR_DPMS_ON, &error);
    if (!ret) {
        g_warning("Failed to set DPMS mode to ON: %s", error->message);
        g_clear_error(&error);
    }

    sprintf(brightness, "%d", saved_brightness > 0 ? saved_brightness : 127);
    brightness_file = fopen("/sys/class/leds/lcd-backlight/brightness", "w");
    if (!brightness_file) {
        perror("Failed to open brightness file for writing");
        return -1;
    }

    if (fputs(brightness, brightness_file) == EOF) {
        perror("Failed to restore brightness");
        fclose(brightness_file);
        return -1;
    }

    fclose(brightness_file);

    g_object_unref(rr_screen);

    return 0;
}

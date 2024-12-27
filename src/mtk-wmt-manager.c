// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2018 MediaTek
// Copyright (C) 2024 Bardia Moshiri <bardia@furilabs.com>

#include <sys/inotify.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <hybris/properties/properties.h>

#define MAX_WAIT_SECOND              0xefffffff
#define MAX_RETRY_COUNT              5
#define NVRAM_MAC_ADDRESS_OFFSET     4
#define BUF_SIZE                     1024
#define WIFI_LOADER_DEV              "/dev/wmtWifi"
#define WIFI_NVRAM_PATH              "/mnt/vendor/nvdata/APCFG/APRDEB"
#define WIFI_NVRAM_INI_FILE          "/data/vendor/nvramwifi"
#define WIFI_MACADDR_FILE            "/data/vendor/macwifi"

#define FILE_REMOVE_MASK (IN_DELETE_SELF | IN_MOVE_SELF)
#define FILE_MODIFY_MASK IN_MODIFY
#define WATCH_FILE_MASK (FILE_REMOVE_MASK | FILE_MODIFY_MASK)
#define WATCH_PATH_MASK (IN_MOVED_TO | IN_CREATE)

#define NVRAM_CHANGED    4

int write_data_to_driver(char *data, size_t length) {
    int ret = -1;
    int fd = -1;

    printf("[DRIVER] Writing %zu bytes to driver\n", length);

    if (!data || !length) {
        printf("[DRIVER] Error: Invalid input - data=%p, length=%zu\n", (void*)data, length);
        return ret;
    }

    printf("[DRIVER] Opening device: %s\n", WIFI_LOADER_DEV);
    fd = open(WIFI_LOADER_DEV, O_RDWR);
    if (fd == -1) {
        printf("[DRIVER] Error: Can't open device node(%s), error: %s\n", WIFI_LOADER_DEV, strerror(errno));
        return ret;
    }

    /* write data to kernel */
    ret = write(fd, data, length);

    if (ret < 0)
        printf("[DRIVER] Error: Write failed, error: %s\n", strerror(errno));
    else
        printf("[DRIVER] Successfully wrote %d bytes\n", ret);

    close(fd);
    return ret;
}

int get_custom_mac_address(char mac[]) {
    int fd = -1;
    char buf[BUF_SIZE] = {0};
    int read_len = 0, i = 0;
    int ret_val = 1;

    memset(buf, 0, BUF_SIZE);
    fd = open(WIFI_MACADDR_FILE, O_RDONLY);
    if (fd == -1) {
        printf("[MAC] Unable to access mac file\n");
        return 0;
    }

    read_len = read(fd, buf, BUF_SIZE - 1);
    if (read_len >= 17) {
        printf("[MAC] MAC ADDR = %s\n", buf);
        for(i = 0; i < 17; i++)
            if (buf[i] == ':')
                buf[i] = 0;
        for(i = 0; i < 6; i++) {
            mac[i] = strtol(&buf[i * 3], NULL, 16);
            printf("[MAC] mac[%d] = %x\n", i, mac[i]);
        }
    } else {
        ret_val = 0;
    }

    close(fd);
    return ret_val;
}

int write_nvram(char *filename) {
    int ret = -1;
    int i = 0, fd = -1;
    char *acnvram;
    struct stat stat_nvram;
    int nvram_size = 0;
    char mac[6] = {0};
    int read_len = 0;

    printf("[NVRAM] Starting NVRAM write process for %s\n", filename);

    /* sleep 1 more second in case that daemon is still writing */
    for (i = 0; i < MAX_RETRY_COUNT; i++) {
        printf("[NVRAM] Attempt %d/%d: Checking file status\n", i + 1, MAX_RETRY_COUNT);

        if (stat(filename, &stat_nvram) == -1) {
            printf("[NVRAM] Error: Cannot stat %s - %s\n", filename, strerror(errno));
            sleep(1);
            continue;
        }

        nvram_size = stat_nvram.st_size - 2;
        printf("[NVRAM] File size: %ld, NVRAM size: %d\n", stat_nvram.st_size, nvram_size);

        if (nvram_size > 0 && (nvram_size & 0x0ff) == 0) {
            printf("[NVRAM] Valid NVRAM size found\n");
            break;
        }

        printf("[NVRAM] Invalid size, retrying...\n");
        sleep(1);
    }

    if (nvram_size <= 0 || (nvram_size & 0x0ff) != 0) {
        printf("[NVRAM] Error: Invalid NVRAM size %d\n", nvram_size);
        return ret;
    }

    printf("[NVRAM] Allocating %d bytes for NVRAM data\n", nvram_size + 12);
    acnvram = (char *)malloc(nvram_size + 12);
    if (!acnvram) {
        printf("[NVRAM] Error: Failed to allocate memory\n");
        return ret;
    }

    memset(acnvram, 0, nvram_size + 12);
    strncpy(acnvram, "WR-BUF:NVRAM", 12);

    printf("[NVRAM] Opening NVRAM file for reading\n");
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("[NVRAM] Error: Cannot open file - %s\n", strerror(errno));
        free(acnvram);
        return ret;
    }

    read_len = read(fd, acnvram + 12, nvram_size);
    printf("[NVRAM] Read %d bytes from NVRAM file\n", read_len);
    close(fd);

    if (read_len <= 0) {
        printf("[NVRAM] Error: Failed to read NVRAM data\n");
        free(acnvram);
        return ret;
    }

    printf("[NVRAM] Attempting to get custom MAC address\n");
    if (get_custom_mac_address(mac)) {
        printf("[NVRAM] Successfully got MAC address, copying to NVRAM buffer\n");
        memcpy(acnvram + 12 + NVRAM_MAC_ADDRESS_OFFSET, mac, sizeof(mac));
    } else {
        printf("[NVRAM] Warning: Failed to get custom MAC address\n");
    }

    printf("[NVRAM] Writing NVRAM data to driver (%d bytes)\n", 12 + nvram_size);
    ret = write_data_to_driver(acnvram, 12 + nvram_size);

    if (ret < 0) {
        printf("[NVRAM] Error: Failed to write NVRAM to driver\n");
    } else {
        printf("[NVRAM] Successfully wrote NVRAM to driver\n");
        printf("[NVRAM] Setting vendor.mtk.nvram.ready property\n");
        property_set("vendor.mtk.nvram.ready", "1");
    }

    free(acnvram);
    return ret;
}

int file_event_handler(int inot_fd, int wd, struct inotify_event *event, char *path, char *file_name) {
    char file_path[512];
    int ret;

    ret = snprintf(file_path, sizeof(file_path), "%s/%s", path, file_name);
    if (ret <= 0)
        return -1;

    if ((event->mask & WATCH_PATH_MASK) && !strcmp(event->name, file_name)) {
        inotify_rm_watch(inot_fd, wd);
        wd = inotify_add_watch(inot_fd, file_path, WATCH_FILE_MASK);
    } else if (event->mask & FILE_REMOVE_MASK) {
        inotify_rm_watch(inot_fd, wd);
        if (access(file_path, R_OK) >= 0)
            wd = inotify_add_watch(inot_fd, file_path, WATCH_FILE_MASK);
        else
            wd = inotify_add_watch(inot_fd, path, WATCH_PATH_MASK);
    }

    return wd;
}

void get_custom_nvram_file_name(char *filename) {
    int fd = -1;
    char buf[BUF_SIZE] = {0};
    int read_len = 0;
    size_t remaining_space;

    memset(buf, 0, BUF_SIZE);
    fd = open(WIFI_NVRAM_INI_FILE, O_RDONLY);
    if (fd == -1) {
        remaining_space = BUF_SIZE - strlen(filename);
        if (remaining_space > 4)
            strcat(filename, "WIFI");
    } else {
        read_len = read(fd, buf, BUF_SIZE - 1);
        if (read_len > 0 && read_len < BUF_SIZE - 1) {
            buf[read_len] = 0;
            remaining_space = BUF_SIZE - strlen(filename);
            if (remaining_space > read_len)
                strcat(filename, buf);
        } else {
            remaining_space = BUF_SIZE - strlen(filename);
            if (remaining_space > 4)
                strcat(filename, "WIFI");
        }

        close(fd);
    }

    printf("[MAC] Custom NVRAM filename = %s\n", filename);
}

void wmt_start_monitor() {
    int inot_fd = 0;
    int dev_wd = -1;
    int nvram_wd = -1;
    int readlen = 0;
    ssize_t readbytes = 0;
    int changed = 0;
    char buf[BUF_SIZE] = {0};
    struct stat stat_buf;
    struct inotify_event *event = NULL;
    int running = 1;
    fd_set inot_fd_set;
    struct timeval timeout;
    long timeout_sec = MAX_WAIT_SECOND;
    char nvram_filename[BUF_SIZE] = {0};

    printf("[MONITOR] Starting wmt_start_monitor\n");

    printf("[MONITOR] Waiting for device %s to be accessible\n", WIFI_LOADER_DEV);
    while (access(WIFI_LOADER_DEV, R_OK | W_OK) < 0) {
        usleep(100000);
        printf("[MONITOR] Still waiting for device...\n");
    }
    printf("[MONITOR] Device is now accessible\n");

    printf("[MONITOR] Checking device properties\n");
    if (stat(WIFI_LOADER_DEV, &stat_buf) == -1) {
        printf("[MONITOR] Error: stat on %s failed: %s\n", WIFI_LOADER_DEV, strerror(errno));
        return;
    }

    if (!S_ISCHR(stat_buf.st_mode)) {
        printf("[MONITOR] Error: %s is not a char device\n", WIFI_LOADER_DEV);
        return;
    }

    printf("[MONITOR] Device properties verified\n");

    printf("[MONITOR] Initializing inotify\n");
    inot_fd = inotify_init();
    if (inot_fd < 0) {
        printf("[MONITOR] Error: inotify_init failed: %s\n", strerror(errno));
        return;
    }

    printf("[MONITOR] Adding watch for device removal\n");
    dev_wd = inotify_add_watch(inot_fd, WIFI_LOADER_DEV, FILE_REMOVE_MASK);
    if (dev_wd < 0)
        printf("[MONITOR] Error: Failed to add device watch: %s\n", strerror(errno));
    else
        printf("[MONITOR] Device watch added successfully\n");

    printf("[MONITOR] Preparing NVRAM filename\n");
    memset(nvram_filename, 0, sizeof(nvram_filename));
    snprintf(nvram_filename, sizeof(nvram_filename), "%s/", WIFI_NVRAM_PATH);
    get_custom_nvram_file_name(nvram_filename);

    printf("[MONITOR] Waiting for NVRAM file to be readable\n");
    while (access(nvram_filename, R_OK) < 0) {
        printf("[MONITOR] NVRAM file not yet accessible, waiting...\n");
        sleep(1);
    }

    printf("[MONITOR] NVRAM file is now accessible\n");

    printf("[MONITOR] Adding watch for NVRAM file\n");
    nvram_wd = inotify_add_watch(inot_fd, nvram_filename, WATCH_FILE_MASK);
    if (nvram_wd < 0)
        printf("[MONITOR] Error: Failed to add NVRAM watch: %s\n", strerror(errno));
    else
        printf("[MONITOR] NVRAM watch added successfully\n");

    printf("[MONITOR] Performing initial NVRAM write\n");
    write_nvram(nvram_filename);

    printf("[MONITOR] Entering main monitoring loop\n");
    while (running) {
        timeout.tv_sec = timeout_sec;
        timeout.tv_usec = 0;
        FD_ZERO(&inot_fd_set);
        FD_SET(inot_fd, &inot_fd_set);

        printf("[MONITOR] Waiting for events (timeout: %ld sec)\n", timeout_sec);
        readlen = select(inot_fd + 1, &inot_fd_set, NULL, NULL, &timeout);

        if (readlen < 0 && errno == EINTR) {
            printf("[MONITOR] Select interrupted, continuing...\n");
            continue;
        }

        if (!readlen) {
            printf("[MONITOR] Timeout reached, sync status: changed=%#x\n", changed);
            if (changed & NVRAM_CHANGED) {
                printf("[MONITOR] NVRAM changes detected, initiating write\n");
                write_nvram(nvram_filename);
            }
            changed = 0;
            timeout_sec = MAX_WAIT_SECOND;
            continue;
        }

        printf("[MONITOR] Reading events\n");
        readbytes = read(inot_fd, buf, sizeof(buf)-1);
        readlen = 0;
        while (readbytes > readlen) {
            event = (struct inotify_event *)&buf[readlen];

            readlen = readlen + sizeof(struct inotify_event) + event->len;
            if (event->mask & IN_IGNORED)
                continue;

            if (event->wd == dev_wd && (event->mask & FILE_REMOVE_MASK)) {
                printf("[MONITOR] Device removal detected, exiting\n");
                running = 0;
                break;
            }

            if (event->wd == nvram_wd) {
                printf("[MONITOR] NVRAM change detected\n");
                nvram_wd = file_event_handler(inot_fd, nvram_wd, event, WIFI_NVRAM_PATH,
                                              nvram_filename + strlen(WIFI_NVRAM_PATH) + 1);
                changed |= NVRAM_CHANGED;
            }
        }
        if (changed > 0)
            timeout_sec = 1;
        memset(buf, 0, sizeof(buf));
    }

    printf("[MONITOR] Cleaning up watches\n");
    if (dev_wd > 0) {
        printf("[MONITOR] Removing device watch\n");
        inotify_rm_watch(inot_fd, dev_wd);
    }

    if (nvram_wd > 0) {
        printf("[MONITOR] Removing NVRAM watch\n");
        inotify_rm_watch(inot_fd, nvram_wd);
    }

    printf("[MONITOR] Monitor shutdown complete\n");
    return;
}

int main(void) {
    wmt_start_monitor();
    return 0;
}

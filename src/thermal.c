#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>

#define PROCFS_TM_PID "/proc/driver/thermal/clwmt_pid"
#define SOCKET_RECV_LEN (128)

struct thermal_socket {
    int socket;
    int is_connected;
    char *path;
    pthread_mutex_t mutex;
};

static struct thermal_socket g_thermal = {
    .socket = -1,
    .is_connected = 0,
    .path = "/dev/socket/rild-oem",
    .mutex = PTHREAD_MUTEX_INITIALIZER
};

static int thermal_connect(void) {
    pthread_mutex_lock(&g_thermal.mutex);
    if (g_thermal.is_connected && g_thermal.socket >= 0) {
        printf("[DEBUG] thermal_connect: Already connected with socket fd: %d\n", g_thermal.socket);
        pthread_mutex_unlock(&g_thermal.mutex);
        return g_thermal.socket;
    }

    printf("[DEBUG] thermal_connect: Starting connection to %s\n", g_thermal.path);
    struct sockaddr_un addr;

    g_thermal.socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (g_thermal.socket < 0) {
        printf("[ERROR] thermal_connect: Failed to create socket: %s\n", strerror(errno));
        pthread_mutex_unlock(&g_thermal.mutex);
        return -1;
    }
    printf("[DEBUG] thermal_connect: Socket created successfully with fd: %d\n", g_thermal.socket);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, g_thermal.path, sizeof(addr.sun_path) - 1);
    printf("[DEBUG] thermal_connect: Attempting connection to socket path: %s\n", addr.sun_path);

    if (connect(g_thermal.socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("[ERROR] thermal_connect: Failed to connect: %s\n", strerror(errno));
        close(g_thermal.socket);
        g_thermal.socket = -1;
        g_thermal.is_connected = 0;
        pthread_mutex_unlock(&g_thermal.mutex);
        return -1;
    }

    g_thermal.is_connected = 1;
    printf("[DEBUG] thermal_connect: Successfully connected to socket\n");
    pthread_mutex_unlock(&g_thermal.mutex);
    return g_thermal.socket;
}

static int thermal_disconnect(void) {
    pthread_mutex_lock(&g_thermal.mutex);
    if (g_thermal.socket < 0) {
        printf("[DEBUG] thermal_disconnect: Socket already disconnected\n");
        pthread_mutex_unlock(&g_thermal.mutex);
        return 0;
    }

    printf("[DEBUG] thermal_disconnect: Starting with socket fd: %d\n", g_thermal.socket);
    int result = close(g_thermal.socket);
    if (result < 0) {
        printf("[ERROR] thermal_disconnect: Failed to close socket: %s\n", strerror(errno));
    } else {
        printf("[DEBUG] thermal_disconnect: Successfully closed socket\n");
    }

    g_thermal.socket = -1;
    g_thermal.is_connected = 0;
    pthread_mutex_unlock(&g_thermal.mutex);
    return result;
}

static void thermal_cleanup(void) {
    printf("[DEBUG] thermal_cleanup: Cleaning up thermal socket\n");
    thermal_disconnect();
}

static int thermal_ensure_connected(void) {
    pthread_mutex_lock(&g_thermal.mutex);
    int need_connect = !g_thermal.is_connected || g_thermal.socket < 0;
    pthread_mutex_unlock(&g_thermal.mutex);

    if (need_connect) {
        printf("[DEBUG] thermal_ensure_connected: Connection needed, attempting to connect\n");
        return thermal_connect();
    }

    return g_thermal.socket;
}

static int setMdTpThreshold(int slotId, int sensorType, int threshold)
{
    printf("[DEBUG] setMdTpThreshold: Starting (slotId=%d, sensorType=%d, threshold=%d)\n",
           slotId, sensorType, threshold);

    int sock = thermal_ensure_connected();
    if (sock < 0) {
        printf("[ERROR] setMdTpThreshold: No valid socket connection\n");
        return -1;
    }

    int ret = -1, strLen = 0, count = 1;
    char strParam[64] = {0};

    snprintf(strParam, sizeof(strParam), "THERMAL,%d,0,%d,%d\n", slotId, sensorType, threshold);
    strLen = strlen(strParam) + 1;
    printf("[DEBUG] setMdTpThreshold: Preparing message (len=%d): %s\n", strLen, strParam);

    ret = send(sock, (const void *)&count, sizeof(int), 0);
    printf("[DEBUG] setMdTpThreshold: First send result: %d\n", ret);
    if (sizeof(int) != ret) {
        printf("[ERROR] setMdTpThreshold: Failed to send count\n");
        ret = -4;
        goto failed;
    }

    ret = send(sock, &strLen, sizeof(strLen), 0);
    printf("[DEBUG] setMdTpThreshold: Second send result: %d\n", ret);
    if (sizeof(strLen) != ret) {
        printf("[ERROR] setMdTpThreshold: Failed to send length\n");
        ret = -3;
        goto failed;
    }

    ret = send(sock, strParam, strLen, 0);
    printf("[DEBUG] setMdTpThreshold: Final send result: %d\n", ret);
    if (strLen != ret) {
        printf("[ERROR] setMdTpThreshold: Failed to send full message\n");
        ret = -5;
        goto failed;
    }

    printf("[DEBUG] setMdTpThreshold: Completed successfully\n");
    return 0;

failed:
    printf("[ERROR] setMdTpThreshold: Operation failed with error code %d: %s\n", ret, strerror(errno));
    return ret;
}

static int queryMdThermalInfo(int slotId, int opcode)
{
    printf("[DEBUG] queryMdThermalInfo: Starting (slotId=%d, opcode=%d)\n", slotId, opcode);

    int sock = thermal_ensure_connected();
    if (sock < 0) {
        printf("[ERROR] queryMdThermalInfo: No valid socket connection\n");
        return -1;
    }

    int ret = -1, strLen = 0, count = 1;
    char strParam[32] = {0};

    snprintf(strParam, sizeof(strParam), "THERMAL,%d,%d\n", slotId, opcode);
    strLen = strlen(strParam) + 1;
    printf("[DEBUG] queryMdThermalInfo: Preparing message (len=%d): %s\n", strLen, strParam);

    ret = send(sock, (const void *)&count, sizeof(int), 0);
    printf("[DEBUG] queryMdThermalInfo: First send result: %d\n", ret);
    if (sizeof(int) != ret) {
        printf("[ERROR] queryMdThermalInfo: Failed to send count\n");
        ret = -4;
        goto failed;
    }

    ret = send(sock, (const void *)&strLen, sizeof(strLen), 0);
    printf("[DEBUG] queryMdThermalInfo: Second send result: %d\n", ret);
    if (sizeof(strLen) != ret) {
        printf("[ERROR] queryMdThermalInfo: Failed to send length\n");
        ret = -3;
        goto failed;
    }

    ret = send(sock, strParam, strLen, 0);
    printf("[DEBUG] queryMdThermalInfo: Final send result: %d\n", ret);
    if (strLen != ret) {
        printf("[ERROR] queryMdThermalInfo: Failed to send full message\n");
        ret = -5;
        goto failed;
    }

    printf("[DEBUG] queryMdThermalInfo: Completed successfully\n");
    return 0;

failed:
    printf("[ERROR] queryMdThermalInfo: Operation failed with error code %d: %s\n", ret, strerror(errno));
    return ret;
}

static int recvMdThermalInfo(int slotId, int opcode)
{
    printf("[DEBUG] recvMdThermalInfo: Starting (slotId=%d, opcode=%d)\n", slotId, opcode);

    int sock = thermal_ensure_connected();
    if (sock < 0) {
        printf("[ERROR] recvMdThermalInfo: No valid socket connection\n");
        return -1;
    }

    int ret = -1, strLen = SOCKET_RECV_LEN-1;
    uint8_t strParm[SOCKET_RECV_LEN] = {0};

    memset(strParm, '\0', strLen+1);
    strParm[SOCKET_RECV_LEN-1] = '\0';

    printf("[DEBUG] recvMdThermalInfo: Attempting to receive data\n");
    ret = recv(sock, strParm, strLen, 0);
    printf("[DEBUG] recvMdThermalInfo: Received %d bytes: %s\n", ret, strParm);

    if (0 < ret) {
        if (strncmp("ERROR", (const char *)strParm, 5) != 0) {
            if (strncmp("URC", (const char *)strParm, 3) == 0) {
                printf("[DEBUG] recvMdThermalInfo: Processing URC message\n");
                int sensorType = -1, temp = -127;
                int tok;
                tok = sscanf((const char *)strParm, "URC,%d,%d", &sensorType, &temp);

                if (tok == 2) {
                    int mdinfoex_idx = -1;
                    printf("[DEBUG] recvMdThermalInfo: Parsed URC data - sensor=%d temp=%d\n",
                           sensorType, temp);

                    switch (sensorType) {
                        case 0: mdinfoex_idx = 1; break;
                        case 1: mdinfoex_idx = 0; break;
                        case 2: mdinfoex_idx = 2; break;
                        default:
                            printf("[ERROR] recvMdThermalInfo: Unknown sensor type %d\n", sensorType);
                            break;
                    }

                    if (mdinfoex_idx > -1) {
                        printf("[DEBUG] recvMdThermalInfo: Writing to mdm_mdinfoex\n");
                        int fd = open("/proc/driver/thermal/mdm_mdinfoex", O_RDWR);
                        char mdinfo_string[SOCKET_RECV_LEN] = {0};
                        if (fd >= 0) {
                            sprintf(mdinfo_string, "%d,%d", mdinfoex_idx, temp);
                            ret = write(fd, mdinfo_string, sizeof(char) * strlen(mdinfo_string));
                            if (ret <= 0) {
                                printf("[ERROR] recvMdThermalInfo: Failed to write to mdm_mdinfoex: %s\n",
                                       strerror(errno));
                            } else {
                                printf("[DEBUG] recvMdThermalInfo: Successfully wrote to mdm_mdinfoex\n");
                            }
                            close(fd);
                        } else {
                            printf("[ERROR] recvMdThermalInfo: Failed to open mdm_mdinfoex\n");
                        }
                    }
                    printf("[DEBUG] recvMdThermalInfo: URC processing completed successfully\n");
                    return 0;
                }
                printf("[ERROR] recvMdThermalInfo: Failed to parse URC message\n");
                return -1;
            }

            if (opcode == -1) {
                printf("[DEBUG] recvMdThermalInfo: Processing opcode -1 response\n");
                int md = 0, temp = 32767, tx = -127, opval0 = 0, opval1 = 0, opval2 = 0;
                int tok = sscanf((const char *)strParm, "%d,%d,%d,%d,%d,%d",
                               &md, &temp, &tx, &opval0, &opval1, &opval2);

                if (tok == 2) {
                    printf("[DEBUG] recv	MdThermalInfo: Parsing alternative format\n");
                    tok = sscanf((const char *)strParm, "%d,%d,,%d,%d,%d",
                               &md, &temp, &opval0, &opval1, &opval2);
                }

                printf("[DEBUG] recvMdThermalInfo: Parsed values - md=%d temp=%d tx=%d opval0=%d opval1=%d opval2=%d\n",
                       md, temp, tx, opval0, opval1, opval2);

                // Writing to mdm_mdinfo
                printf("[DEBUG] recvMdThermalInfo: Writing to mdm_mdinfo\n");
                int fd = open("/proc/driver/thermal/mdm_mdinfo", O_RDWR);
                if (fd >= 0) {
                    char mdinfo_string[SOCKET_RECV_LEN];
                    sprintf(mdinfo_string, "%d,%d,%d,%d", slotId, md, temp, ((tok>2)?tx:-127));
                    ret = write(fd, mdinfo_string, sizeof(char) * strlen(mdinfo_string));
                    if (ret <= 0)
                        printf("[ERROR] recvMdThermalInfo: Failed to write to mdm_mdinfo\n");
                    else
                        printf("[DEBUG] recvMdThermalInfo: Successfully wrote to mdm_mdinfo\n");
                    close(fd);
                } else {
                    printf("[ERROR] recvMdThermalInfo: Failed to open mdm_mdinfo\n");
                }

                // Writing to mdm_mdinfoex
                printf("[DEBUG] recvMdThermalInfo: Writing to mdm_mdinfoex\n");
                fd = open("/proc/driver/thermal/mdm_mdinfoex", O_RDWR);
                if (fd >= 0) {
                    char mdinfo_string[SOCKET_RECV_LEN];

                    // Writing opval0
                    sprintf(mdinfo_string, "%d,%d", 0, opval0);
                    ret = write(fd, mdinfo_string, sizeof(char) * strlen(mdinfo_string));
                    if (ret <= 0)
                        printf("[ERROR] recvMdThermalInfo: Failed to write opval0\n");
                    else
                        printf("[DEBUG] recvMdThermalInfo: Successfully wrote opval0\n");

                    // Writing opval1
                    memset(mdinfo_string, 0, SOCKET_RECV_LEN);
                    sprintf(mdinfo_string, "%d,%d", 1, opval1);
                    ret = write(fd, mdinfo_string, sizeof(char) * strlen(mdinfo_string));
                    if (ret <= 0)
                        printf("[ERROR] recvMdThermalInfo: Failed to write opval1\n");
                    else
                        printf("[DEBUG] recvMdThermalInfo: Successfully wrote opval1\n");

                    // Writing opval2
                    memset(mdinfo_string, 0, SOCKET_RECV_LEN);
                    sprintf(mdinfo_string, "%d,%d", 2, opval2);
                    ret = write(fd, mdinfo_string, sizeof(char) * strlen(mdinfo_string));
                    if (ret <= 0)
                        printf("[ERROR] recvMdThermalInfo: Failed to write opval2\n");
                    else
                        printf("[DEBUG] recvMdThermalInfo: Successfully wrote opval2\n");
                    close(fd);
                } else {
                    printf("[ERROR] recvMdThermalInfo: Failed to open mdm_mdinfoex\n");
                }
            }
        } else {
            printf("[ERROR] recvMdThermalInfo: Received ERROR response\n");
        }
        ret = -1;
    }

    printf("[DEBUG] recvMdThermalInfo: Completed with ret=%d\n", ret);
    return ret;
}

static int new_md_tp_flow(void)
{
    printf("[DEBUG] new_md_tp_flow: Starting\n");
    int count = 0;
    int i = 0;
    static int thresholds[3] = { 85, 85, 85 };
    static int new_thresholds[3] = { 85, 85, 85 };

    // Initial connection
    if (thermal_connect() < 0) {
        printf("[ERROR] new_md_tp_flow: Initial thermal connection failed\n");
        return -1;
    }

    while (1) {
        printf("[DEBUG] new_md_tp_flow: Main loop iteration %d starting\n", count + 1);

        if (0 == access("/proc/driver/thermal/mdm_mdinfoex_thre", R_OK)) {
            printf("[DEBUG] new_md_tp_flow: Reading threshold values\n");
            char buf[80];
            int fd = open("/proc/driver/thermal/mdm_mdinfoex_thre", O_RDONLY);
            if (fd < 0) {
                printf("[ERROR] new_md_tp_flow: Can't open mdinfoex_thre: %s\n", strerror(errno));
            } else {
                buf[79] = (char) 0x0;
                int len = read(fd, buf, sizeof(buf) - 1);
                if (len < 0)
                    printf("[ERROR] new_md_tp_flow: Can't read mdinfoex_thre: %s\n", strerror(errno));
                close(fd);
                if (3 == sscanf(buf, "%d,%d,%d,", &new_thresholds[1], &new_thresholds[0], &new_thresholds[2]))
                    printf("[DEBUG] new_md_tp_flow: New thresholds: %d,%d,%d\n",
                           new_thresholds[0], new_thresholds[1], new_thresholds[2]);
                else
                    printf("[ERROR] new_md_tp_flow: Failed to parse thresholds: %s\n", buf);
            }
        }

        for (i = 0; i < 3; i++) {
            if (new_thresholds[i] == thresholds[i]) {
                printf("[DEBUG] new_md_tp_flow: Threshold %d unchanged (%d)\n", i, thresholds[i]);
                continue;
            }

            printf("[DEBUG] new_md_tp_flow: Updating threshold %d from %d to %d\n",
                   i, thresholds[i], new_thresholds[i]);
            thresholds[i] = new_thresholds[i];

            if (setMdTpThreshold(0, i, thresholds[i]) < 0) {
                printf("[ERROR] new_md_tp_flow: Failed to set threshold\n");
                thermal_disconnect();
                sleep(5);
                continue;
            }

            if (recvMdThermalInfo(0, 0) < 0) {
                printf("[ERROR] new_md_tp_flow: Failed to receive confirmation\n");
                thermal_disconnect();
                sleep(5);
                continue;
            }
        }

        count++;
        printf("[DEBUG] new_md_tp_flow: Processing count %d\n", count);

        printf("[DEBUG] new_md_tp_flow: Querying MD thermal info\n");
        if (queryMdThermalInfo(0, -1) < 0 ||
            recvMdThermalInfo(0, -1) < 0) {
            printf("fail here0");
            thermal_disconnect();
            sleep(5);
            continue;
        }

        fd_set rfds;
        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        printf("[DEBUG] new_md_tp_flow: Waiting for URC, count %d timeout %ld sec\n",
               count, timeout.tv_sec);

        FD_ZERO(&rfds);
        pthread_mutex_lock(&g_thermal.mutex);
        FD_SET(g_thermal.socket, &rfds);
        int ret = select(g_thermal.socket + 1, &rfds, NULL, NULL, &timeout);
        pthread_mutex_unlock(&g_thermal.mutex);

        if (ret < 0) {
            if (errno == EINTR) {
                printf("[ERROR] new_md_tp_flow: Select interrupted, retrying\n");
                continue;
            }
            printf("[ERROR] new_md_tp_flow: Select failed with error %d\n", errno);
            thermal_disconnect();
            return -1;
        }

        if (ret == 0) {
            printf("[DEBUG] new_md_tp_flow: Select timed out, continuing\n");
            sleep(5);
            continue;
        }

        if (FD_ISSET(g_thermal.socket, &rfds)) {
            printf("[DEBUG] new_md_tp_flow: Data available on socket, receiving\n");
            if (recvMdThermalInfo(0, -1) < 0) {
                printf("fail here");
                thermal_disconnect();
                sleep(5);
                continue;
            }
        }

        sleep(5);
    }

    thermal_disconnect();
    return 0;
}

int main(int argc, char *argv[])
{
    printf("[DEBUG] main: Starting process (PID: %d)\n", getpid());

    int fd = open(PROCFS_TM_PID, O_RDWR);
    int pid = getpid();
    int ret = 0;
    char pid_string[32] = {0};
    struct sigaction act;

    printf("[DEBUG] main: Setting up signal handler\n");
    memset(&act, 0, sizeof(act));
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    ret = sigaction(SIGIO, &act, NULL);
    if (ret != 0) {
        printf("[ERROR] main: Failed to set up signal handler: %s\n", strerror(errno));
    }

    sprintf(pid_string, "%d", pid);

    printf("[DEBUG] main: Writing PID to %s\n", PROCFS_TM_PID);
    if (fd >= 0) {
        ret = write(fd, pid_string, sizeof(char) * strlen(pid_string));
        if (ret <= 0)
            printf("[ERROR] main: Failed to write PID to %s: %s\n", PROCFS_TM_PID, strerror(errno));
        else
            printf("[DEBUG] main: Successfully wrote PID to %s\n", PROCFS_TM_PID);
        close(fd);
    } else {
        printf("[ERROR] main: Failed to open %s: %s\n", PROCFS_TM_PID, strerror(errno));
    }

    atexit(thermal_cleanup);

    printf("[DEBUG] main: Starting MD TP flow\n");
    new_md_tp_flow();

    printf("[DEBUG] main: Process ending\n");
    return 0;
}

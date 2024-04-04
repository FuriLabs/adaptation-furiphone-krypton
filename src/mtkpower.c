#include <stdio.h>
#include <gbinder.h>
#include <stdlib.h>
#include <stdint.h>

enum {
    //MTK pre-defined hint
    MTKPOWER_HINT_BASE                              = 20,

    MTKPOWER_HINT_PROCESS_CREATE                    = 21,
    MTKPOWER_HINT_PACK_SWITCH                       = 22,
    MTKPOWER_HINT_ACT_SWITCH                        = 23,
    MTKPOWER_HINT_APP_ROTATE                        = 24,
    MTKPOWER_HINT_APP_TOUCH                         = 25,
    MTKPOWER_HINT_GALLERY_BOOST                     = 26,
    MTKPOWER_HINT_GALLERY_STEREO_BOOST              = 27,
    MTKPOWER_HINT_WFD                               = 28,
    MTKPOWER_HINT_PMS_INSTALL                       = 29,
    MTKPOWER_HINT_EXT_LAUNCH                        = 30,
    MTKPOWER_HINT_WHITELIST_LAUNCH                  = 31,
    MTKPOWER_HINT_WIPHY_SPEED_DL                    = 32,
    MTKPOWER_HINT_SDN                               = 33,
    MTKPOWER_HINT_WHITELIST_ACT_SWITCH              = 34,
    MTKPOWER_HINT_BOOT                              = 35,
    MTKPOWER_HINT_AUDIO_LATENCY_DL                  = 36,
    MTKPOWER_HINT_AUDIO_LATENCY_UL                  = 37,
    MTKPOWER_HINT_AUDIO_POWER_DL                    = 38,
    MTKPOWER_HINT_AUDIO_DISABLE_WIFI_POWER_SAVE     = 39,
    MTKPOWER_HINT_MULTI_DISPLAY_WITH_GPU_FPS_60     = 40,
    MTKPOWER_HINT_MULTI_DISPLAY_WITH_GPU_FPS_90     = 41,
    MTKPOWER_HINT_MULTI_DISPLAY_WITH_GPU_FPS_120    = 42,
    MTKPOWER_HINT_UX_SCROLLING                      = 43,
    MTKPOWER_HINT_AUDIO_POWER_UL                    = 44,
    MTKPOWER_HINT_UX_MOVE_SCROLLING                 = 45,
    MTKPOWER_HINT_AUDIO_POWER_HI_RES                = 46,

    MTKPOWER_HINT_NUM,
};

void mtk_power_hint_hidl(GBinderClient* client, const int choice) {
    int status;
    GBinderLocalRequest* req = gbinder_client_new_request(client);
    GBinderWriter writer;

    // mtkPowerHint
    gbinder_local_request_init_writer(req, &writer);
    gbinder_writer_append_int32(&writer, choice);
    gbinder_writer_append_int32(&writer, 1);
    gbinder_client_transact_sync_reply(client, 1, req, &status);
    gbinder_local_request_unref(req);
}

int init_hidl(const int mode) {
    GBinderServiceManager* sm = gbinder_servicemanager_new("/dev/hwbinder");
    if (!sm) return 1;

    GBinderRemoteObject* remote = gbinder_servicemanager_get_service_sync(sm, "vendor.mediatek.hardware.mtkpower@1.0::IMtkPower/default", NULL);
    if (!remote) {
        gbinder_servicemanager_unref(sm);
        return 1;
    }

    GBinderClient* client = gbinder_client_new(remote, "vendor.mediatek.hardware.mtkpower@1.0::IMtkPower");
    if (!client) {
        gbinder_remote_object_unref(remote);
        gbinder_servicemanager_unref(sm);
        return 1;
    }

    mtk_power_hint_hidl(client, mode);

    gbinder_client_unref(client);
    gbinder_remote_object_unref(remote);

    return 0;
}

int main(int argc, char *argv[]) {
    int choice = atoi(argv[1]);

    if (choice >= 21 && choice <= 46) {
        int ret = init_hidl(choice);

        if (ret != 0) {
           printf("None of the backends are available for mtkpower. Exiting.\n");
           return 1;
        } else {
           printf("Using HIDL backend\n");
        }
    } else {
        return 1;
    }

    return 0;
}

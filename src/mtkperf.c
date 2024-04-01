#include <stdio.h>
#include <gbinder.h>
#include <stdlib.h>
#include <stdint.h>

enum {
    /* MAJOR = 1 CPU freq*/
    PERF_RES_CPUFREQ_MIN_CLUSTER_0                   = 0x00400000, // set the frequency
    PERF_RES_CPUFREQ_MIN_CLUSTER_1                   = 0x00400100,
    PERF_RES_CPUFREQ_MIN_CLUSTER_2                   = 0x00400200,
    PERF_RES_CPUFREQ_MAX_CLUSTER_0                   = 0x00404000,
    PERF_RES_CPUFREQ_MAX_CLUSTER_1                   = 0x00404100,
    PERF_RES_CPUFREQ_MAX_CLUSTER_2                   = 0x00404200,
    PERF_RES_CPUFREQ_MIN_HL_CLUSTER_0                = 0x00408000,
    PERF_RES_CPUFREQ_MIN_HL_CLUSTER_1                = 0x00408100,
    PERF_RES_CPUFREQ_MIN_HL_CLUSTER_2                = 0x00408200,
    PERF_RES_CPUFREQ_MAX_HL_CLUSTER_0                = 0x0040c000,
    PERF_RES_CPUFREQ_MAX_HL_CLUSTER_1                = 0x0040c100,
    PERF_RES_CPUFREQ_MAX_HL_CLUSTER_2                = 0x0040c200,
    PERF_RES_CPUFREQ_CCI_FREQ                        = 0x00410000,
    PERF_RES_CPUFREQ_PERF_MODE                       = 0x00414000, // force all cpu run at the highest freq

    /* MAJOR = 2 CPU core*/
    PERF_RES_CPUCORE_MIN_CLUSTER_0                   = 0x00800000, // set the core number
    PERF_RES_CPUCORE_MIN_CLUSTER_1                   = 0x00800100,
    PERF_RES_CPUCORE_MIN_CLUSTER_2                   = 0x00800200,
    PERF_RES_CPUCORE_MAX_CLUSTER_0                   = 0x00804000,
    PERF_RES_CPUCORE_MAX_CLUSTER_1                   = 0x00804100,
    PERF_RES_CPUCORE_MAX_CLUSTER_2                   = 0x00804200,
    PERF_RES_CPUCORE_PERF_MODE                       = 0x00808000,
    PERF_RES_CPUCORE_FORCE_PAUSE_CPU                 = 0x0080c000,

    /* MAJOR = 3 GPU*/
    PERF_RES_GPU_FREQ_MIN                            = 0x00c00000,
    PERF_RES_GPU_FREQ_MIN_HL                         = 0x00c00100,
    PERF_RES_GPU_FREQ_MAX                            = 0x00c04000,
    PERF_RES_GPU_FREQ_MAX_HL                         = 0x00c04100,
    PERF_RES_GPU_FREQ_LOW_LATENCY                    = 0x00c08000,
    PERF_RES_GPU_GED_BENCHMARK_ON                    = 0x00c0c000,
    PERF_RES_GPU_GED_MARGIN_MODE                     = 0x00c0c100,
    PERF_RES_GPU_GED_TIMER_BASE_DVFS_MARGIN          = 0x00c0c200,
    PERF_RES_GPU_GED_LOADING_BASE_DVFS_STEP          = 0x00c0c300,
    PERF_RES_GPU_GED_CWAITG                          = 0x00c0c400,
    PERF_RES_GPU_GED_GX_BOOST                        = 0x00c0c500,
    PERF_RES_GPU_GED_DVFS_LOADING_MODE               = 0x00c0c600,
    PERF_RES_GPU_GED_DVFS_WORKLOAD_MODE              = 0x00c0c700,
    PERF_RES_GPU_POWER_POLICY                        = 0x00c10000,
    PERF_RES_GPU_POWER_ONOFF_INTERVAL                = 0x00c10100,
    PERF_RES_GPU_ACP_HINT                            = 0x00c14000,
    PERF_RES_GPU_BW_MODE_HINT                        = 0x00c18000,
    PERF_RES_GPU_DCS_POLICY                          = 0x00c1c000,
    PERF_RES_GPU_FW_IDLE                             = 0x00c20000,
    PERF_RES_GPU_BLOCK_API_BOOST                     = 0x00c24000,
    PERF_RES_GPU_GED_LOADING_BASE_WINDOW_SIZE        = 0x00c28100,
    PERF_RES_GPU_GED_LOADING_BASE_STRIDE_SIZE        = 0x00c28200,
    PERF_RES_GPU_GED_FALLBACK_TIMING                 = 0x00c2c100,
    PERF_RES_GPU_GED_FALLBACK_INTERVAL               = 0x00c2c200,
    PERF_RES_GPU_GED_FRAME_BASE_THRESHOLD            = 0x00c30100,

    /* MAJOR = 4 DRAM (VCORE, CM MGR, EMI)*/
    PERF_RES_DRAM_OPP_MIN                            = 0x01000000, // force DDR run at the highest freq
    PERF_RES_DRAM_OPP_MIN_LP5                        = 0x01000100,
    PERF_RES_DRAM_OPP_MIN_LP5_HFR                    = 0x01000200,
    PERF_RES_DRAM_VCORE_MIN                          = 0x01004000,
    PERF_RES_DRAM_VCORE_MIN_LP3                      = 0x01004100,
    PERF_RES_DRAM_VCORE_BW_ENABLE                    = 0x01008000,
    PERF_RES_DRAM_VCORE_BW_THRES                     = 0x01008100,
    PERF_RES_DRAM_VCORE_BW_THRESH_LP3                = 0x01008200,
    PERF_RES_DRAM_VCORE_POLICY                       = 0x0100c000,
    PERF_RES_DRAM_CM_MGR                             = 0x01010000,
    PERF_RES_DRAM_CM_MGR_CAM_ENABLE                  = 0x01010100,
    PERF_RES_DRAM_CM_MGR_MAP_DRAM_ENABLE             = 0x01010200,
    PERF_RES_DRAM_CM_MGR_AGGRESSIVE                  = 0x01010300,
    PERF_RES_DRAM_CM_MGR_CM_HINT_MASK                = 0x01010400,
    PERF_RES_DRAM_CM_MGR_DSU_HINT                    = 0x01010500,
    PERF_RES_DRAM_CM_MGR_DSU_LMODE                   = 0x01010600,
    PERF_RES_DRAM_CM_MGR_LMODE                       = 0x01010700,
    PERF_RES_DRAM_CM_RATIO_UP_X_0                    = 0x01014000,
    PERF_RES_DRAM_CM_RATIO_UP_X_1                    = 0x01014100,
    PERF_RES_DRAM_CM_RATIO_UP_X_2                    = 0x01014200,
    PERF_RES_DRAM_CM_RATIO_UP_X_3                    = 0x01014300,
    PERF_RES_DRAM_CM_RATIO_UP_X_4                    = 0x01014400,
    PERF_RES_DRAM_CM_PASSIVE                         = 0x01014500,

    PERF_RES_DRAM_VM_SWAPINESS                       = 0x0102c000,
    PERF_RES_DRAM_VM_DROP_CACHES                     = 0x0102c100,
    PERF_RES_DRAM_VM_WATERMARK_EXTRAKBYTESADJ        = 0x0102c200,
    PERF_RES_DRAM_VM_WATERMARK_SCALEFACTOR           = 0x0102c300,
    PERF_RES_DRAM_LMK_KILL_TIMEOUT                   = 0x01030000,
    PERF_RES_DRAM_LMK_KILL_HEAVIEST_TASK             = 0x01030100,
    PERF_RES_DRAM_LMK_RELEASE_MEM                    = 0x01030200,
    PERF_RES_DRAM_LMK_THRASHING_LIMIT                = 0x01030300,
    PERF_RES_DRAM_LMK_SWAP_FREE_LOW_PERCENT          = 0x01030400,
    PERF_RES_DRAM_LMK_MINFREE_SCALEFACTOR            = 0x01030500,
    PERF_RES_DRAM_DURASPEED_CPUTHRESHOLD             = 0x01054000,
    PERF_RES_DRAM_DURASPEED_CPUTARGET                = 0x01054100,
    PERF_RES_DRAM_DURASPEED_POLICYLEVEL              = 0x01054200,

    /* MAJOR = 5 Scheduler*/
    PERF_RES_SCHED_BOOST_VALUE_ROOT                  = 0x01400000,
    PERF_RES_SCHED_BOOST_VALUE_FG                    = 0x01400100,
    PERF_RES_SCHED_BOOST_VALUE_BG                    = 0x01400200,
    PERF_RES_SCHED_BOOST_VALUE_TA                    = 0x01400300,
    PERF_RES_SCHED_BOOST_VALUE_RT                    = 0x01400400,
    PERF_RES_SCHED_PREFER_IDLE_ROOT                  = 0x01404000,
    PERF_RES_SCHED_PREFER_IDLE_FG                    = 0x01404100,
    PERF_RES_SCHED_PREFER_IDLE_BG                    = 0x01404200,
    PERF_RES_SCHED_PREFER_IDLE_TA                    = 0x01404300,
    PERF_RES_SCHED_PREFER_IDLE_RT                    = 0x01404400,
    PERF_RES_SCHED_PREFER_IDLE_PER_TASK              = 0x01404500,
    PERF_RES_SCHED_PREFER_IDLE_SYS                   = 0x01404600,
    PERF_RES_SCHED_PREFER_IDLE_SYSBG                 = 0x01404700,
    PERF_RES_SCHED_UCLAMP_MIN_ROOT                   = 0x01408000,
    PERF_RES_SCHED_UCLAMP_MIN_FG                     = 0x01408100,
    PERF_RES_SCHED_UCLAMP_MIN_BG                     = 0x01408200,
    PERF_RES_SCHED_UCLAMP_MIN_TA                     = 0x01408300,
    PERF_RES_SCHED_UCLAMP_MIN_RT                     = 0x01408400,
    PERF_RES_SCHED_UCLAMP_MIN_SYS                    = 0x01408500,
    PERF_RES_SCHED_UCLAMP_MAX_ROOT                   = 0x01408600,
    PERF_RES_SCHED_UCLAMP_MAX_FG                     = 0x01408700,
    PERF_RES_SCHED_UCLAMP_MAX_BG                     = 0x01408800,
    PERF_RES_SCHED_UCLAMP_MAX_TA                     = 0x01408900,
    PERF_RES_SCHED_UCLAMP_MAX_RT                     = 0x01408a00,
    PERF_RES_SCHED_UCLAMP_MAX_SYS                    = 0x01408b00,
    PERF_RES_SCHED_UCLAMP_MIN_SYSBG                  = 0x01408c00,
    PERF_RES_SCHED_UCLAMP_MAX_SYSBG                  = 0x01408d00,
    PERF_RES_SCHED_UCLAMP_MIN_SF                     = 0x01408e00,
    PERF_RES_SCHED_UCLAMP_MAX_SF                     = 0x01408f00,
    PERF_RES_SCHED_TUNE_THRES                        = 0x0140c000,
    PERF_RES_SCHED_BOOST                             = 0x01410000, // big core first
    PERF_RES_SCHED_MIGRATE_COST                      = 0x01414000,
    PERF_RES_SCHED_CPU_PREFER_TASK_1_BIG             = 0x01418000,
    PERF_RES_SCHED_CPU_PREFER_TASK_1_LITTLE          = 0x01418100,
    PERF_RES_SCHED_CPU_PREFER_TASK_1_RESERVED        = 0x01418200,
    PERF_RES_SCHED_CPU_PREFER_TASK_2_BIG             = 0x01418300,
    PERF_RES_SCHED_CPU_PREFER_TASK_2_LITTLE          = 0x01418400,
    PERF_RES_SCHED_CPU_PREFER_TASK_2_RESERVED        = 0x01418500,
    PERF_RES_SCHED_CPUSET_TA                         = 0x01418600,
    PERF_RES_SCHED_BTASK_ROTATE                      = 0x0141c000,
    PERF_RES_SCHED_CACHE_AUDIT                       = 0x01420000,
    PERF_RES_SCHED_CACHE_PART_CTRL                   = 0x01420100,
    PERF_RES_SCHED_CACHE_SET_CT_ROOT                 = 0x01420200,
    PERF_RES_SCHED_CACHE_SET_CT_TA                   = 0x01420300,
    PERF_RES_SCHED_CACHE_SET_CT_FG                   = 0x01420400,
    PERF_RES_SCHED_CACHE_SET_CT_BG                   = 0x01420500,
    PERF_RES_SCHED_CACHE_SET_CT_SYSBG                = 0x01420600,
    PERF_RES_SCHED_CACHE_SET_CT_RT                   = 0x01420700,
    PERF_RES_SCHED_CACHE_SET_CT_SYS                  = 0x01420800,
    PERF_RES_SCHED_CACHE_SET_CT_TASK                 = 0x01420900,
    PERF_RES_SCHED_CACHE_SET_NCT_TASK                = 0x01420a00,
    PERF_RES_SCHED_CACHE_CPUQOS_MODE                 = 0x01420b00,
    PERF_RES_SCHED_PLUS_DOWN_THROTTLE_NS             = 0x01424000,
    PERF_RES_SCHED_PLUS_UP_THROTTLE_NS               = 0x01424100,
    PERF_RES_SCHED_PLUS_SYNC_FLAG                    = 0x01424200,
    PERF_RES_SCHED_PLUS_DOWN_THROTTLE_US             = 0x01424300,
    PERF_RES_SCHED_PLUS_UP_THROTTLE_US               = 0x01424400,
    PERF_RES_SCHED_MTK_PREFER_IDLE                   = 0x01428000, // mtkcam calls this and sets it to 1 (???)
    PERF_RES_SCHED_HEAVY_TASK_THRES                  = 0x0142c000,
    PERF_RES_SCHED_HEAVY_TASK_AVG_HTASK_AC           = 0x0142c100,
    PERF_RES_SCHED_HEAVY_TASK_AVG_HTASK_THRES        = 0x0142c200,
    PERF_RES_SCHED_WALT                              = 0x01430000,
    PERF_RES_SCHED_PELT_HALFLIFE                     = 0x01430100,
    PERF_RES_SCHED_PELT_THROTTLE_MS                  = 0x01430200,
    PERF_RES_SCHED_PELT_HALFLIFE_HIGH_PRIORITY       = 0x01430300,
    PERF_RES_SCHED_PREFER_CPU_ROOT                   = 0x01434000,
    PERF_RES_SCHED_PREFER_CPU_FG                     = 0x01434100,
    PERF_RES_SCHED_PREFER_CPU_BG                     = 0x01434200,
    PERF_RES_SCHED_PREFER_CPU_TA                     = 0x01434300,
    PERF_RES_SCHED_PREFER_CPU_RT                     = 0x01434400,
    PERF_RES_SCHED_PREFER_CPU_SYSBG                  = 0x01434500,
    PERF_RES_SCHED_UTIL_RATE_LIMIT_US_CLUSTER_0      = 0x01438000,
    PERF_RES_SCHED_UTIL_RATE_LIMIT_US_CLUSTER_1      = 0x01438100,
    PERF_RES_SCHED_UTIL_RATE_LIMIT_US_CLUSTER_2      = 0x01438200,
    PERF_RES_SCHED_UTIL_UP_RATE_LIMIT_US_CLUSTER_0   = 0x01438300,
    PERF_RES_SCHED_UTIL_UP_RATE_LIMIT_US_CLUSTER_1   = 0x01438400,
    PERF_RES_SCHED_UTIL_UP_RATE_LIMIT_US_CLUSTER_2   = 0x01438500,
    PERF_RES_SCHED_UTIL_DOWN_RATE_LIMIT_US_CLUSTER_0 = 0x01438600,
    PERF_RES_SCHED_UTIL_DOWN_RATE_LIMIT_US_CLUSTER_1 = 0x01438700,
    PERF_RES_SCHED_UTIL_DOWN_RATE_LIMIT_US_CLUSTER_2 = 0x01438800,
    PERF_RES_SCHED_ISOLATION_CPU                     = 0x0143c000,
    PERF_RES_SCHED_CORE_CTL_POLICY_ENABLE            = 0x0143c100,
    PERF_RES_SCHED_OFFLINE_THROTTLE_MS_CLUSTER_0     = 0x0143c200,
    PERF_RES_SCHED_OFFLINE_THROTTLE_MS_CLUSTER_1     = 0x0143c300,
    PERF_RES_SCHED_OFFLINE_THROTTLE_MS_CLUSTER_2     = 0x0143c400,
    PERF_RES_SCHED_BTASK_UP_THRESH_CLUSTER_0         = 0x0143c500,
    PERF_RES_SCHED_BTASK_UP_THRESH_CLUSTER_1         = 0x0143c600,
    PERF_RES_SCHED_CPU_CORE_NOT_PREFERRED            = 0x0143c700,
    PERF_RES_SCHED_NEWLY_IDLE_BALANCE_INTERVAL       = 0x01440000,
    PERF_RES_SCHED_THERMAL_HEADROOM_INTERVAL_TICK    = 0x01440100,

    /* MAJOR = 6 AI*/
    PERF_RES_AI_VPU_FREQ_MIN_CORE_0                  = 0x01800000,
    PERF_RES_AI_VPU_FREQ_MIN_CORE_1                  = 0x01800100,
    PERF_RES_AI_VPU_FREQ_MAX_CORE_0                  = 0x01804000,
    PERF_RES_AI_VPU_FREQ_MAX_CORE_1                  = 0x01804100,
    PERF_RES_AI_MDLA_FREQ_MIN                        = 0x01808000,
    PERF_RES_AI_MDLA_FREQ_MAX                        = 0x0180c000,
    PERF_RES_AI_APUSYS_BOOST_IPU_IF                  = 0X01810000,

    /* MAJOR = 7 Low power policy */
    PERF_RES_POWER_CPUFREQ_HISPEED_FREQ              = 0x01c00000,
    // mtkcam sets these for scenario video recoring if its less than 1080p to save on power
    PERF_RES_POWER_CPUFREQ_MIN_SAMPLE_TIME           = 0x01c04000, // set sample time i.e: for a quick fall 20000 is 20ms
    PERF_RES_POWER_CPUFREQ_ABOVE_HISPEED_DELAY       = 0x01c08000, // set the hispeed delay i.e: for a slow rise 80000 is 80ms
    PERF_RES_POWER_CPUFREQ_POWER_MODE                = 0x01c0c000,
    PERF_RES_POWER_HPS_THRESH_UP                     = 0x01c10000,
    PERF_RES_POWER_HPS_THRESH_DOWN                   = 0x01c10100,
    PERF_RES_POWER_HPS_TIMES_UP                      = 0x01c14000,
    PERF_RES_POWER_HPS_TIMES_DOWN                    = 0x01c14100,
    PERF_RES_POWER_HPS_RUSH_BOOST_ENABLE             = 0x01c18000,
    PERF_RES_POWER_HPS_RUSH_BOOST_THRESH             = 0x01c18100,
    PERF_RES_POWER_HPS_HEAVY_TASK                    = 0x01c1c000,
    PERF_RES_POWER_HPS_POWER_MODE                    = 0x01c20000,
    PERF_RES_POWER_PPM_ROOT_CLUSTER                  = 0x01c24000,
    PERF_RES_POWER_PPM_MODE                          = 0x01c28000,
    PERF_RES_POWER_PPM_HICA_VAR                      = 0x01c2c000,
    PERF_RES_POWER_PPM_LIMIT_BIG                     = 0x01c30000,
    PERF_RES_POWER_PPM_SPORTS_MODE                   = 0x01c34000,
    PERF_RES_POWER_PPM_USERLIMIT_BOOST               = 0x01c38000,
    PERF_RES_POWER_CPUIDLE_MCDI_ENABLE               = 0x01c3c000,
    PERF_RES_PM_QOS_CPUIDLE_MCDI_ENABLE              = 0x01c3c100,
    PERF_RES_PM_QOS_CPU_DMA_LATENCY_VALUE            = 0x01c3c200,
    PERF_RES_POWER_SYSLIMITER_60                     = 0x01c40000,
    PERF_RES_POWER_SYSLIMITER_90                     = 0x01c40100,
    PERF_RES_POWER_SYSLIMITER_120                    = 0x01c40200,
    PERF_RES_POWER_SYSLIMITER_144                    = 0x01c40300,
    PERF_RES_POWER_SYSLIMITER                        = 0x01c40400,
    PERF_RES_POWER_SYSLIMITER_DISABLE                = 0x01c44000,
    PERF_RES_POWER_SYSLIMITER_TARGET_FPS_TOLERANCE_PERCENT
                                                     = 0x01c44100,

    /* MAJOR = 8 FPS policy (FPSGO) */
    PERF_RES_FPS_FSTB_FPS_LOWER                      = 0x02000000,
    PERF_RES_FPS_FSTB_FPS_UPPER                      = 0x02000100,
    PERF_RES_FPS_FSTB_TARGET_FPS_PID                 = 0x02000200,
    PERF_RES_FPS_FPSGO_CTL                           = 0x02000300,
    PERF_RES_FPS_FPSGO_NOCTL                         = 0x02000400,
    PERF_RES_FPS_FPSGO_ENABLE                        = 0x02004000,
    PERF_RES_FPS_FPSGO_FORCE_ONOFF                   = 0x02004100,
    PERF_RES_FPS_FPSGO_STOP_BOOST                    = 0x02004200,
    PERF_RES_FPS_FSTB_FORCE_VAG                      = 0x02008000,
    PERF_RES_FPS_FSTB_JUMP_CHECK_NUM                 = 0x02008100,
    PERF_RES_FPS_FSTB_JUMP_CHECK_Q_PCT               = 0x02008200,
    PERF_RES_FPS_FSTB_ADOPT_LOW_FPS                  = 0x02008300,
    PERF_RES_FPS_FPSGO_CAM                           = 0x02008400,
    PERF_RES_FPS_CAM_ACTIVE_FPSGO_OFF                = 0x02008500,
    PERF_RES_FPS_FBT_SHORT_RESCUE_NS                 = 0x0200c000,
    PERF_RES_FPS_FBT_MIN_RESCUE_PERCENT              = 0x0200c100,
    PERF_RES_FPS_FBT_RESCUE_PERCENT_90               = 0x0200c200,
    PERF_RES_FPS_FBT_RESCUE_PERCENT_120              = 0x0200c300,
    PERF_RES_FPS_FBT_DEQTIME_BOUND                   = 0x02010000,
    PERF_RES_FPS_FBT_FLOOR_BOUND                     = 0x02014000,
    PERF_RES_FPS_FBT_KMIN                            = 0x02018000,
    PERF_RES_FPS_FBT_VARIANCE                        = 0x02018100,
    PERF_RES_FPS_FSTB_SOFT_FPS_LOWER                 = 0x0201c000,
    PERF_RES_FPS_FSTB_SOFT_FPS_UPPER                 = 0x0201c100,
    PERF_RES_FPS_FBT_BHR_OPP                         = 0x02020000,
    PERF_RES_FPS_FBT_BHR                             = 0x02024000,
    PERF_RES_FPS_EARA_BENCH                          = 0x02028000,
    PERF_RES_FPS_FPSGO_GPU_BLOCK_BOOST               = 0x0202c000,
    PERF_RES_FPS_FBT_BOOST_TA                        = 0x02030000,
    PERF_RES_FPS_FPSGO_MARGIN_MODE                   = 0x02034000,
    PERF_RES_FPS_FPSGO_MARGIN_MODE_DBNC_A            = 0x02034100,
    PERF_RES_FPS_FPSGO_MARGIN_MODE_DBNC_B            = 0x02034200,
    PERF_RES_FPS_EARA_THERMAL_ENABLE                 = 0x02038000,
    PERF_RES_FPS_FBT_RESCUE_F                        = 0x0203c000,
    PERF_RES_FPS_FBT_RESCUE_PERCENT                  = 0x0203c100,
    PERF_RES_FPS_FBT_ULTRA_RESCUE                    = 0x0203c200,
    PERF_RES_FPS_FBT_RESCUE_C                        = 0x0203c300,
    PERF_RES_FPS_FBT_RESCUE_CHECK                    = 0x0203c400,
    PERF_RES_FPS_FBT_RESCUE_SECOND_ENABLE            = 0x0203c500,
    PERF_RES_FPS_FBT_RESCUE_SECOND_TIME              = 0x0203c600,
    PERF_RES_FPS_FBT_RESCUE_SECOND_GROUP             = 0x0203c700,
    PERF_RES_FPS_FBT_RESCUE_SECOND_RESCUE_F          = 0x0203c800,
    PERF_RES_FPS_FBT_RESCUE_F_OPP                    = 0x0203c900,
    PERF_RES_FPS_FBT_RESCUE_SECOND_GAME_ENABLE       = 0x0203ca00,
    PERF_RES_FPS_FPSGO_ADJ_LOADING                   = 0x02040000,
    PERF_RES_FPS_FPSGO_ADJ_CNT                       = 0x02040100,
    PERF_RES_FPS_FPSGO_ADJ_DEBNC_CNT                 = 0x02040200,
    PERF_RES_FPS_FPSGO_ADJ_LOADING_TIMEDIFF          = 0x02040300,
    PERF_RES_FPS_FPSGO_ADJ_LOADING_HWUI_HINT         = 0x02040400,
    PERF_RES_FPS_FPSGO_LLF_TH                        = 0x02044000,
    PERF_RES_FPS_FPSGO_LLF_POLICY                    = 0x02044100,
    PERF_RES_FPS_FPSGO_LLF_LIGHT_LOADING_POLICY      = 0x02044200,
    PERF_RES_FPS_FPSGO_LLF_POLICY_90                 = 0x02044300,
    PERF_RES_FPS_FPSGO_LLF_LIGHT_LOADING_POLICY_90   = 0x02044400,
    PERF_RES_FPS_FPSGO_LLF_POLICY_120                = 0x02044500,
    PERF_RES_FPS_FPSGO_LLF_LIGHT_LOADING_POLICY_120  = 0x02044600,
    PERF_RES_FPS_FPSGO_LLF_LOADING_IGNORE_ENABLE     = 0x02044700,
    PERF_RES_FPS_FBT_BLC_BOOST                       = 0x02045b00,
    PERF_RES_FPS_FPSGO_DEP_FRAMES                    = 0x02048000,
    PERF_RES_FPS_FPSGO_SP_NAME_ID                    = 0x02048100,
    PERF_RES_FPS_FPSGO_SP_SUB                        = 0x02048200,
    PERF_RES_FPS_FPSGO_SP_CK_PERIOD                  = 0x02048300,
    PERF_RES_FPS_FPSGO_SUB_QUE_DEQUE_PERIOD          = 0x02048400,
    PERF_RES_FPS_FPSGO_EMA_DIVIDEND                  = 0x02048500,
    PERF_RES_FPS_FPSGO_STDDEV_MULTI                  = 0x02048600,
    PERF_RES_FPS_FPSGO_UBOOST                        = 0x02048700,
    PERF_RES_FPS_FPSGO_UBOOST_F                      = 0x02048800,
    PERF_RES_FPS_GBE1_ENABLE                         = 0x0204c000,
    PERF_RES_FPS_GBE2_ENABLE                         = 0x0204c100,
    PERF_RES_FPS_GBE2_TIMER2_MS                      = 0x0204c200,
    PERF_RES_FPS_GBE2_LOADING_TH                     = 0x0204c300,
    PERF_RES_FPS_GBE2_MAX_BOOST_CNT                  = 0x0204c400,
    PERF_RES_FPS_GBE_POLICY_MASK                     = 0x0204c500,
    PERF_RES_FPS_GBE2_TIMER1_MS                      = 0x0204c600,
    PERF_RES_FPS_GBE_THRM_HDRM_THRS                  = 0x0204c700,
    PERF_RES_FPS_GBE_CPU_1                           = 0x0204c800,
    PERF_RES_FPS_GBE_CPU_0                           = 0x0204c900,
    PERF_RES_FPS_FPSGO_IDLEPREFER                    = 0x02050000,
    PERF_RES_FPS_FPSGO_THRM_TEMP_TH                  = 0x02054000,
    PERF_RES_FPS_FPSGO_THRM_LIMIT_CPU                = 0x02054100,
    PERF_RES_FPS_FPSGO_THRM_SUB_CPU                  = 0x02054200,
    PERF_RES_FPS_FPSGO_THRM_ACTIVATE_FPS             = 0x02054300,
    PERF_RES_FPS_FPSGO_THRM_ENABLE                   = 0x02054400,
    PERF_RES_FPS_FPSGO_CM_BIG_CAP                    = 0x02058000,
    PERF_RES_FPS_FPSGO_CM_TDIFF                      = 0x02058100,
    PERF_RES_FPS_FPSGO_CM_BIG_CAP_90                 = 0x02058200,
    PERF_RES_FPS_FPSGO_CM_TDIFF_90                   = 0x02058300,
    PERF_RES_FPS_FPSGO_CM_BIG_CAP_120                = 0x02058400,
    PERF_RES_FPS_FPSGO_CM_TDIFF_120                  = 0x02058500,
    PERF_RES_FPS_FPSGO_BOOST_AFFINITY                = 0x0205c000,
    PERF_RES_FPS_FPSGO_BOOST_AFFINITY_90             = 0x0205c100,
    PERF_RES_FPS_FPSGO_BOOST_AFFINITY_120            = 0x0205c200,
    PERF_RES_FPS_FPSGO_BOOST_LR                      = 0x0205c300,
    PERF_RES_FPS_FPSGO_SPID                          = 0x02060000,
    PERF_RES_FPS_FBT_LIMIT_CFREQ                     = 0x02064000,
    PERF_RES_FPS_FBT_LIMIT_RFREQ                     = 0x02064100,
    PERF_RES_FPS_FBT_LIMIT_CFREQ_M                   = 0x02064200,
    PERF_RES_FPS_FBT_LIMIT_RFREQ_M                   = 0x02064300,
    PERF_RES_FPS_FBT_CEILING_ENABLE                  = 0x02064400,
    PERF_RES_FPS_FBT_MINITOP_ENABLE                  = 0x02064500,
    PERF_RES_FPS_FBT_MINITOP_THRS_HEAVY              = 0x02064600,
    PERF_RES_FPS_FBT_QR_ENABLE                       = 0x02068000,
    PERF_RES_FPS_FBT_QR_HWUI_HINT                    = 0x02068100,
    PERF_RES_FPS_FBT_QR_T2WNT_X                      = 0x02068200,
    PERF_RES_FPS_FBT_QR_T2WNT_Y_P                    = 0x02068300,
    PERF_RES_FPS_FBT_QR_T2WNT_Y_N                    = 0x02068400,
    PERF_RES_FPS_FBT_QR_T2WNT_X_90                   = 0x02068500,
    PERF_RES_FPS_FBT_QR_T2WNT_Y_P_90                 = 0x02068600,
    PERF_RES_FPS_FBT_QR_T2WNT_Y_N_90                 = 0x02068700,
    PERF_RES_FPS_FBT_QR_T2WNT_X_120                  = 0x02068800,
    PERF_RES_FPS_FBT_QR_T2WNT_Y_P_120                = 0x02068900,
    PERF_RES_FPS_FBT_QR_T2WNT_Y_N_120                = 0x02068A00,
    PERF_RES_FPS_FBT_GCC_ENABLE                      = 0x0206c000,
    PERF_RES_FPS_FBT_GCC_ENQ_BOUND_THRS              = 0x0206c100,
    PERF_RES_FPS_FBT_GCC_ENQ_BOUND_QUOTA             = 0x0206c200,
    PERF_RES_FPS_FBT_GCC_DEQ_BOUND_THRS              = 0x0206c300,
    PERF_RES_FPS_FBT_GCC_DEQ_BOUND_QUOTA             = 0x0206c400,
    PERF_RES_FPS_FBT_GCC_FPS_MARGIN                  = 0x0206c500,
    PERF_RES_FPS_FBT_GCC_UP_SEC_PCT                  = 0x0206c600,
    PERF_RES_FPS_FBT_GCC_DOWN_STEP                   = 0x0206c700,
    PERF_RES_FPS_FBT_GCC_UPPER_CLAMP                 = 0x0206c800,
    PERF_RES_FPS_FBT_GCC_UP_QUOTA_PCT                = 0x0206c900,
    PERF_RES_FPS_FBT_GCC_DOWN_QUOTA_PCT              = 0x0206cA00,
    PERF_RES_FPS_FBT_GCC_POSITIVE_CLAMP              = 0x0206cB00,
    PERF_RES_FPS_FBT_GCC_DOWN_SEC_PCT 				 = 0x0206cc00,
    PERF_RES_FPS_FBT_GCC_UP_STEP					 = 0x0206cd00,
    PERF_RES_FPS_FBT_GCC_HWUI_HINT					 = 0x0206ce00,
    PERF_RES_FPS_FPSGO_FPS_BYPASS_MIN                = 0x02088000,

    PERF_RES_FRS_ENABLE                       = 0x02070000,
    PERF_RES_FRS_THRESHOLD                    = 0x02070100,
    PERF_RES_FRS_TARGET_TEMP                  = 0x02070200,
    PERF_RES_FRS_MAX_FPS                      = 0x02070300,
    PERF_RES_FRS_MIN_FPS                      = 0x02070400,
    PERF_RES_FRS_CEILING                      = 0x02070500,
    PERF_RES_FRS_CEILING_TH                   = 0x02070600,
    PERF_RES_FRS_PTIME                        = 0x02070700,
    PERF_RES_FRS_OT_PTIME                     = 0x02070800,
    PERF_RES_FRS_THROTTLED                    = 0x02070900,
    PERF_RES_FRS_OT_THROTTLED                 = 0x02070a00,
    PERF_RES_FRS_MAX_THROTTLED_FPS            = 0x02070b00,
    PERF_RES_FRS_FPS_RATIO                    = 0x02070c00,
    PERF_RES_FRS_MAX_FPS_DIFF                 = 0x02070f00,
    PERF_RES_FRS_TZ_INDEX                     = 0x02071200,
    PERF_RES_FPS_FPSGO_MARGIN_MODE_GPU               = 0x02074000,
    PERF_RES_FPS_FPSGO_MARGIN_MODE_GPU_DBNC_A        = 0x02074100,
    PERF_RES_FPS_FPSGO_MARGIN_MODE_GPU_DBNC_B        = 0x02074200,
    PERF_RES_FPS_FPSGO_EMA2_ENABLE                   = 0x02078000,
    PERF_RES_FPS_FSTB_SELF_CTRL_FPS_ENABLE           = 0x0207c000,
    PERF_RES_FPS_FSTB_CONSIDER_DEQ                   = 0x0207c100,
    PERF_RES_FPS_FSTB_NO_R_TIMER                     = 0x0207c200,
    PERF_RES_FPS_FSTB_TFPS_TO_POWERHAL_ENABLE        = 0x02080000,
    PERF_RES_FPS_FPSGO_SET_VIDEO_PID                 = 0x02084000,
    PERF_RES_FPS_FPSGO_CLEAR_VIDEO_PID               = 0x02084100,

    /* MAJOR = 9 Display (DFPS, display, video mode) */
    PERF_RES_DISP_DFPS_MODE                          = 0x02400000,
    PERF_RES_DISP_DFPS_FPS                           = 0x02400100,
    PERF_RES_DISP_VIDEO_MODE                         = 0x02404000,
    PERF_RES_DISP_DECOUPLE                           = 0x02408000,
    PERF_RES_DISP_IDLE_TIME                          = 0x0240c000, // mtkcam calls this and sets it to 1 (???)
    PERF_RES_VIDEO_COLOR_CONVERT_MODE                = 0x02410000,
    PERF_RES_DISP_LPHRT_MODE                         = 0x02414000,

    /* MAJOR = 10 CONSYS + MD */
    PERF_RES_NET_WIFI_CAM                            = 0x02800000,
    PERF_RES_NET_WIFI_LOW_LATENCY                    = 0x02804000,
    PERF_RES_NET_WIFI_SMART_PREDICT                  = 0x02804100,
    PERF_RES_NET_WIFI_TWT_SMART_STA                  = 0x02804200,
    PERF_RES_NET_WIFI_INFO_DELAY_BOUND_UDP           = 0x02804300,
    PERF_RES_NET_WIFI_INFO_DELAY_BOUND_TCP           = 0x02804400,
    PERF_RES_NET_WIFI_INFO_PHY_RATE                  = 0x02804500,
    PERF_RES_NET_WIFI_INFO_PRIORITY_UDP              = 0x02804600,
    PERF_RES_NET_WIFI_INFO_PRIORITY_TCP              = 0x02804700,
    PERF_RES_NET_WIFI_INFO_MONITOR_PROTOCOL          = 0x02804800,
    PERF_RES_NET_NETD_BOOST_UID                      = 0x02808000,
    PERF_RES_NET_NETD_BLOCK_UID                      = 0x02808100,
    PERF_RES_NET_MD_LOW_LATENCY                      = 0x0280c000,
    PERF_RES_NET_MD_GAME_MODE                        = 0x0280c100,
    PERF_RES_NET_MD_CERT_PID                         = 0x0280c200,
    PERF_RES_NET_MD_CRASH_PID                        = 0x0280c300,
    PERF_RES_NET_MD_WEAK_SIG_OPT                     = 0x0280c400,
    PERF_RES_NET_MD_HSR_MODE                         = 0x0280c500,
    PERF_RES_NET_BT_AUDIO_LOW_LATENCY                 = 0x02810000,

    /* MAJOR = 11 IO */
    PERF_RES_IO_BOOST_VALUE                          = 0x02c00000,
    PERF_RES_IO_UCLAMP_MIN                           = 0x02c00100,
    PERF_RES_IO_F2FS_UFS_BOOST                       = 0x02c04000,
    PERF_RES_IO_F2FS_UFS_BOOST_ULTRA                 = 0x02c04100,
    PERF_RES_IO_F2FS_EMMC_BOOST                      = 0x02c08000,
    PERF_RES_IO_BLKDEV_READAHEAD                     = 0x02c0c000,
    PERF_RES_IO_EXT4_DATA_BOOST                      = 0x02c10000,
    PERF_RES_IO_DATA_FS_BOOST                        = 0x02c14000,

    /* MAJOR = 12 Misc. (thermal, touch, sys perf) */
    PERF_RES_THERMAL_POLICY                          = 0x03000000,
    PERF_RES_UX_PREDICT_LOW_LATENCY                  = 0x03004000,
    PERF_RES_UX_PREDICT_GAME_MODE                    = 0x03004100,
    PERF_RES_UX_SCROLL_SAVE_POWER                    = 0x03004200,
    PERF_RES_CFP_ENABLE                              = 0x03008000,
    PERF_RES_CFP_POLLING_MS                          = 0x03008100,
    PERF_RES_CFP_UP_LOADING                          = 0x03008200,
    PERF_RES_CFP_DOWN_LOADING                        = 0x03008300,
    PERF_RES_CFP_UP_TIME                             = 0x03008400,
    PERF_RES_CFP_DOWN_TIME                           = 0x03008500,
    PERF_RES_CFP_UP_OPP                              = 0x03008600,
    PERF_RES_CFP_DOWN_OPP                            = 0x03008700,
    PERF_RES_PERF_TASK_TURBO                         = 0x0300c000,
    PERF_RES_TOUCH_CHANGE_RATE                       = 0x03010000,
    PERF_RES_THERMAL_SPORTS_CG_POLICY                = 0x03014000,
    PERF_RES_MPTS_ENABLE                             = 0x03014100,
    PERF_RES_MAGT_THERMAL_AWARE_THRESHOLD            = 0x03018000,
    PERF_RES_MAGT_FPSDROP_AWARE_THRESHOLD            = 0x03018100,
    PERF_RES_MAGT_ADVICE_BAT_AVG_CURRENT             = 0x03018200,
    PERF_RES_MAGT_ADVICE_BAT_MAX_CURRENT             = 0x03018300,

    /* MAJOR = 13 PowerHAL internal control */
    PERF_RES_POWERHAL_SCREEN_OFF_STATE               = 0x03400000,
    PERF_RES_POWERHAL_SCN_CRASH                      = 0x03400100,
    PERF_RES_POWERHAL_SPORTS_MODE                    = 0x03404000,
    PERF_RES_POWERHAL_SPORTS_MODE_APP_SMART_MODE     = 0x03404100,
    PERF_RES_POWERHAL_TOUCH_BOOST_OPP                = 0x03408000,
    PERF_RES_POWERHAL_TOUCH_BOOST_DURATION           = 0x03408100,
    PERF_RES_POWERHAL_TOUCH_BOOST_ACTIVE_TIME        = 0x03408200,
    PERF_RES_POWERHAL_TOUCH_BOOST_EAS_BOOST          = 0x03408300,
    PERF_RES_POWERHAL_TOUCH_BOOST_TIME_TO_LAST_TOUCH = 0x03408400,
    PERF_RES_POWERHAL_TOUCH_BOOST_ENABLE             = 0x03408500,
    PERF_RES_POWERHAL_TOUCH_BOOST_NOTIFY_FBC         = 0x03408600,
    PERF_RES_POWERHAL_TOUCH_BOOST_DEBOOST_WHEN_RENDER= 0x03408700,
    PERF_RES_POWERHAL_TOUCH_BOOST_CLUSTER_0_OPP      = 0x03408800,
    PERF_RES_POWERHAL_TOUCH_BOOST_CLUSTER_1_OPP      = 0x03408900,
    PERF_RES_POWERHAL_TOUCH_BOOST_CLUSTER_2_OPP      = 0x03408a00,
    PERF_RES_POWERHAL_TOUCH_BOOST_NOTIFY_TOUCH_EVENT = 0x03408b00,
    PERF_RES_POWERHAL_WHITELIST_APP_LAUNCH_TIME_COLD = 0x0340c000,
    PERF_RES_POWERHAL_WHITELIST_APP_LAUNCH_TIME_WARM = 0x0340c100,
    PERF_RES_POWERHAL_WHITELIST_ACT_SWITCH_TIME      = 0x0340c200,
    PERF_RES_POWER_HINT_HOLD_TIME                    = 0x03410000,
    PERF_RES_POWER_HINT_EXT_HINT                     = 0x03410100,
    PERF_RES_POWER_HINT_EXT_HINT_HOLD_TIME           = 0x03410200,
    PERF_RES_POWER_END_HINT_HOLD_TIME                = 0x03410300,
    PERF_RES_POWER_HINT_INSTALL_MAX_DURATION         = 0x03410400,
    PERF_RES_POWERHAL_HIDE_LOG                       = 0x03414000,
    PERF_RES_APPLIST_FPS_TOLERANCE_PERCENT           = 0x03418000,

    /* MAJOR = 64 Reserved for customization */
    PERF_RES_CUSTOM_RESOURCE_1                       = 0x10000000,
};

enum{
    MTKPOWER_HINT_ALWAYS_ENABLE = 0x0FFFFFFF,
};

void perf_lock_acquire_hidl(GBinderClient* client, const int choice) {
    int status;
    GBinderLocalRequest* req = gbinder_client_new_request(client);
    GBinderWriter writer;

    int cmdlist[] = {PERF_RES_CPUFREQ_PERF_MODE, 1, PERF_RES_DRAM_OPP_MIN, 0,
                     PERF_RES_SCHED_BOOST, 1, PERF_RES_FPS_GBE1_ENABLE, 1,
                     PERF_RES_FPS_GBE2_ENABLE, 1, PERF_RES_DISP_DFPS_MODE, 1,
                     PERF_RES_DISP_DFPS_FPS, 120};
    int cmdlist_size = sizeof(cmdlist) / sizeof(cmdlist[0]);
    int elem_size = sizeof(cmdlist[0]);

    // perfLockAcquire
    gbinder_local_request_init_writer(req, &writer);
    gbinder_writer_append_int32(&writer, choice);
    gbinder_writer_append_int32(&writer, 30000); // 30 seconds
    gbinder_writer_append_hidl_vec(&writer, cmdlist, cmdlist_size, elem_size);
    gbinder_writer_append_int32(&writer, 1);
    gbinder_client_transact_sync_reply(client, 1, req, &status);
    gbinder_local_request_unref(req);
}

int init_hidl(const int mode) {
    GBinderServiceManager* sm = gbinder_servicemanager_new("/dev/hwbinder");
    if (!sm) return 1;

    GBinderRemoteObject* remote = gbinder_servicemanager_get_service_sync(sm, "vendor.mediatek.hardware.mtkpower@1.0::IMtkPerf/default", NULL);
    if (!remote) {
        gbinder_servicemanager_unref(sm);
        return 1;
    }

    GBinderClient* client = gbinder_client_new(remote, "vendor.mediatek.hardware.mtkpower@1.0::IMtkPerf");
    if (!client) {
        gbinder_remote_object_unref(remote);
        gbinder_servicemanager_unref(sm);
        return 1;
    }

    perf_lock_acquire_hidl(client, mode);

    gbinder_client_unref(client);
    gbinder_remote_object_unref(remote);

    return 0;
}

int main(int argc, char *argv[]) {
    int choice = atoi(argv[1]);

    if (choice >= 21 && choice <= 46) {
        int ret = init_hidl(choice);

        if (ret != 0) {
           printf("None of the backends are available for wakelock. Exiting.\n");
           return 1;
        } else {
           printf("Using HIDL backend\n");
        }
    } else {
        return 1;
    }

    return 0;
}

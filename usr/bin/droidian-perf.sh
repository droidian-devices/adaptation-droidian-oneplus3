#!/bin/bash

CORE_FIRST=$(awk '$1 == "processor" {print $3; exit}' /proc/cpuinfo)
CORE_LAST=$(awk '$1 == "processor" {print $3}' /proc/cpuinfo | tail -1)

for ((i=$CORE_FIRST; i<=$CORE_LAST; i++))
do
    MAXFREQ=$(cat /sys/devices/system/cpu/cpu${i}/cpufreq/scaling_available_frequencies | awk '{print $NF}')
    if [ -f "/var/lib/batman/default_cpu_governor" ]; then
        GOVERNOR=$(</var/lib/batman/default_cpu_governor)
    else
        GOVERNOR=$(</sys/devices/system/cpu/cpu${i}/cpufreq/scaling_governor)
    fi

    if [ -d "/sys/devices/system/cpu/cpu${i}/cpufreq/$GOVERNOR" ]; then
        if [ -f "/sys/devices/system/cpu/cpu${i}/cpufreq/${GOVERNOR}/hispeed_freq" ]; then
            echo "$MAXFREQ" > /sys/devices/system/cpu/cpu${i}/cpufreq/${GOVERNOR}/hispeed_freq
            echo -n "/sys/devices/system/cpu/cpu${i}/cpufreq/${GOVERNOR}/hispeed_freq "
            echo "$MAXFREQ"
        fi
    fi
done

[ -f /proc/sys/kernel/sched_autogroup_enabled ] && echo 0 > /proc/sys/kernel/sched_autogroup_enabled
[ -f /sys/module/cpu_boost/parameters/input_bost_ms ] && echo 156 > /sys/module/cpu_boost/parameters/input_boost_ms
[ -f /sys/class/kgsl/kgsl-3d0/bus_split ] && echo 0 > /sys/class/kgsl/kgsl-3d0/bus_split
[ -f /sys/class/kgsl/kgsl-3d0/min_pwrlevel ] && echo 0 > /sys/class/kgsl/kgsl-3d0/min_pwrlevel
[ -f /sys/class/kgsl/kgsl-3d0/force_bus_on ] && echo 1 > /sys/class/kgsl/kgsl-3d0/force_bus_on
[ -f /sys/class/kgsl/kgsl-3d0/force_clk_on ] && echo 1 > /sys/class/kgsl/kgsl-3d0/force_clk_on
[ -f /sys/class/kgsl/kgsl-3d0/force_rail_on ] && echo 1 > /sys/class/kgsl/kgsl-3d0/force_rail_on
[ -f /sys/class/kgsl/kgsl-3d0/popp ] && echo 0 > /sys/class/kgsl/kgsl-3d0/popp
[ -f /proc/sys/kernel/sched_child_runs_first ] && echo 0 > /proc/sys/kernel/sched_child_runs_first
[ -f /proc/sys/kernel/perf_cpu_time_max_percent ] && echo 20 > /proc/sys/kernel/perf_cpu_time_max_percent
[ -f /proc/sys/kernel/sched_tunable_scaling ] && echo 0 > /proc/sys/kernel/sched_tunable_scaling
[ -f /proc/sys/kernel/sched_migration_cost_ns ] && echo 0 > /proc/sys/kernel/sched_migration_cost_ns
[ -f /proc/perfmgr/boost_ctrl/eas_ctrl/m_sched_migrate_cost_n ] && echo 0 > /proc/perfmgr/boost_ctrl/eas_ctrl/m_sched_migrate_cost_n
[ -f /proc/sys/kernel/sched_min_task_util_for_colocation ] && echo 0 > /proc/sys/kernel/sched_min_task_util_for_colocation
[ -f /proc/sys/kernel/sched_min_task_util_for_boost ] && echo 0 > /proc/sys/kernel/sched_min_task_util_for_boost
[ -f /proc/sys/kernel/sched_nr_migrate ] && echo 128 > /proc/sys/kernel/sched_nr_migrate
[ -f /proc/sys/kernel/sched_schedstats ] && echo 0 > /proc/sys/kernel/sched_schedstats
[ -f /proc/sys/kernel/sched_cstate_aware ] && echo 1 > /proc/sys/kernel/sched_cstate_aware
[ -f /proc/sys/kernel/timer_migration ] && echo 0 > /proc/sys/kernel/timer_migration
[ -f /proc/sys/kernel/sched_boost ] && echo 0 > /proc/sys/kernel/sched_boost
[ -f /sys/devices/system/cpu/eas/enable ] && echo 0 > /sys/devices/system/cpu/eas/enable
[ -f /proc/sys/kernel/sched_walt_rotate_big_tasks ] && echo 1 > /proc/sys/kernel/sched_walt_rotate_big_tasks
[ -f /proc/sys/kernel/sched_prefer_sync_wakee_to_waker ] && echo 1 > /proc/sys/kernel/sched_prefer_sync_wakee_to_waker
[ -f /proc/sys/kernel/sched_boost_top_app ] && echo 1 > /proc/sys/kernel/sched_boost_top_app
[ -f /proc/sys/kernel/sched_init_task_load ] && echo 30 > /proc/sys/kernel/sched_init_task_load
[ -f /proc/sys/kernel/sched_migration_fixup ] && echo 0 > /proc/sys/kernel/sched_migration_fixup
[ -f /proc/sys/kernel/sched_energy_aware ] && echo 0 > /proc/sys/kernel/sched_energy_aware
[ -f /proc/sys/kernel/hung_task_timeout_secs ] && echo 0 > /proc/sys/kernel/hung_task_timeout_secs
[ -f /proc/sys/kernel/sched_conservative_pl ] && echo 0 > /proc/sys/kernel/sched_conservative_pl
[ -f /sys/kernel/debug/debug_enabled ] && echo 0 > /sys/kernel/debug/debug_enabled
[ -f /sys/kernel/debug/msm_vidc/fw_debug_mode ] && echo 0 > /sys/kernel/debug/msm_vidc/fw_debug_mode
[ -f /sys/module/spurious/parameters/noirqdebug ] && echo 1 > /sys/module/spurious/parameters/noirqdebug


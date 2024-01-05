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
[ -f /sys/class/kgsl/kgsl-3d0/bus_split ] && echo 0 > /sys/class/kgsl/kgsl-3d0/bus_split 
[ -f /sys/class/kgsl/kgsl-3d0/min_pwrlevel ] && echo 0 > /sys/class/kgsl/kgsl-3d0/min_pwrlevel 
[ -f /sys/class/kgsl/kgsl-3d0/force_bus_on ] && echo 1 > /sys/class/kgsl/kgsl-3d0/force_bus_on 
[ -f /sys/class/kgsl/kgsl-3d0/force_clk_on ] && echo 1 > /sys/class/kgsl/kgsl-3d0/force_clk_on 
[ -f /sys/class/kgsl/kgsl-3d0/force_rail_on ] && echo 1 > /sys/class/kgsl/kgsl-3d0/force_rail_on 
[ -f /sys/class/kgsl/kgsl-3d0/popp ] && echo 0 > /sys/class/kgsl/kgsl-3d0/popp 


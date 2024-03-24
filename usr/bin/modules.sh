#!/bin/bash

modules=(
trace_mmstat
met
conninfra
wmt_chrdev_wifi
wlan_drv_gen4m
bt_drv_6877
gps_drv
fmradio_drv_connac2x
connfem
fpsgo
)

for name in "${modules[@]}"; do
    modprobe -f "$name"
done

while [ "`getprop vendor.connsys.driver.ready`" != "yes" ]; do
    sleep 1
done

cd /lib/modules/$(uname -r)

rmmod wlan_drv_gen4m
rmmod wmt_chrdev_wifi

sleep 10

insmod wmt_chrdev_wifi.ko
insmod wlan_drv_gen4m.ko

echo 0 > /dev/wmtWifi
echo 1 > /dev/wmtWifi

exit 0

#!/bin/bash

mkdir -p /lib/modules/$(uname -r)
cp /vendor/lib/modules/* /lib/modules/$(uname -r)

depmod -a

modules=(
trace_mmstat
connfem
conninfra
bt_drv_6877
fmradio_drv_connac2x
gps_drv
wmt_chrdev_wifi
wlan_drv_gen4m
fpsgo
met
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

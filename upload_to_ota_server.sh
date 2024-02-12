#!/bin/bash
set -e

#Dependencies: -RSync
# http://192.168.178.143/fota/210911-zisterne.version

[ ! -e platformio.ini ] && echo "Script must be executed in PlatformIO Root Directory." && exit 1


esp_id=`grep 'const char\* ESP_ID' src/* | cut -d'"' -f 2`
old_fw_version=`ssh pi cat /var/www/html/fota/$esp_id.version`
new_fw_version=`grep 'const int FW_VERSION' src/* | tr -d -c 0-9`

echo "ESP_ID:               $esp_id"
echo "Old Firmware Version: $old_fw_version"
echo "New Firmware Version: $new_fw_version"

if [ $new_fw_version -le  $old_fw_version ]
then
echo "New fw version number is lower or equal to the old one."
exit 1
fi

pio run || exit

read -p "Are you sure you want to upload binaries to server and update version number? (y/n)" -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
rsync --rsync-path="sudo rsync" -v .pio/build/*/firmware.bin pi:/var/www/html/fota/$esp_id.bin &&
  ssh pi "echo $new_fw_version | sudo tee /var/www/html/fota/$esp_id.version" > /dev/null
fi


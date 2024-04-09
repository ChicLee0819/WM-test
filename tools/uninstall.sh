#!/bin/bash
WESTON_INI="/etc/xdg/weston/weston.ini"
WESTON_INI_BAK="/etc/xdg/weston/weston.ini.bak"
TEMP_CRONTAB_FILE="/tmp/.cronfile"

function remove_files() {
    rm /usr/bin/settings
    rm /usr/bin/start_*.sh
    rm /usr/share/icons/hicolor/24x24/apps/settings_icon.png
}

function remove_reboot_cronjob() {
    # write out current crontab without reboot cronjob
    crontab -l | grep -v "@reboot /usr/bin/start_after_boot.sh" >$TEMP_CRONTAB_FILE
    # restore cron file
    crontab $TEMP_CRONTAB_FILE
    rm $TEMP_CRONTAB_FILE
}

function remove_weston_shortcut() {
    # restore weston config
    cp $WESTON_INI_BAK $WESTON_INI
    # restart weston to take effect
    systemctl restart weston@root.service
}

remove_files
remove_reboot_cronjob
remove_weston_shortcut
exit 0

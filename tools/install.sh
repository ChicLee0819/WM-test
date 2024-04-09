#!/bin/bash
LAUNCHER_SETTING="[launcher]\nicon=/usr/share/icons/hicolor/24x24/apps/settings_icon.png\npath=/usr/bin/start_settings.sh\n"
WESTON_INI="/etc/xdg/weston/weston.ini"
WESTON_INI_BAK="/etc/xdg/weston/weston.ini.bak"
SETTINGS_CONFIG_INI_PATH="/etc/settings"

function install_files() {
    echo "Installing files..."
    mkdir -p /usr/local/lib
    mkdir -p /usr/local/bin
    cp settings /usr/bin/
    cp start_*.sh /usr/bin/
    cp settings_icon.png /usr/share/icons/hicolor/24x24/apps/
    # create settings config ini path
    mkdir -p $SETTINGS_CONFIG_INI_PATH
    # copy settings config ini if not exist
    if [ ! -f "$SETTINGS_CONFIG_INI_PATH/settings_config.ini" ]; then
        cp settings_config.ini $SETTINGS_CONFIG_INI_PATH
    fi
}

function create_reboot_cronjob() {
    echo "Creating reboot cronjob..."
    cp settings_cron /etc/cron.d
    chmod 0644 /etc/cron.d/settings_cron
}

function create_weston_shortcut() {
    # create top bar shortcut
    if ! grep -q "/usr/bin/settings" $WESTON_INI; then
        cp $WESTON_INI $WESTON_INI_BAK
        echo -en "$LAUNCHER_SETTING" >>$WESTON_INI
        # restart weston to take effect
        systemctl restart weston@root.service
    fi
}

install_files
create_reboot_cronjob
create_weston_shortcut
exit 0

#!/bin/bash
WESTON_SERVICE_NAME=$(systemctl list-units --type=service | grep weston | awk -F ' ' '{print $1}')
WESTON_RUNNING_STATUS=$(systemctl status ${WESTON_SERVICE_NAME} | grep 'running')
EXECUTE_COMMAND=$(awk -F "=" '/startup_command/ {print $2}' /etc/settings/settings_config.ini)
if [ -z "$EXECUTE_COMMAND" ]; then
    echo "command is empty!"
    exit 1
fi

# wait until weston service is running
while [ -z "$WESTON_RUNNING_STATUS" ]; do
    echo "advantech startup script: weston service is not ready"
    sleep 1
    WESTON_RUNNING_STATUS=$(systemctl status ${WESTON_SERVICE_NAME} | grep 'running')
done

# check graphical/desktop runtime directory is set
if test -z "$XDG_RUNTIME_DIR"; then
    export XDG_RUNTIME_DIR=/run/user/$(id -u)
    if ! test -d "$XDG_RUNTIME_DIR"; then
        mkdir --parents $XDG_RUNTIME_DIR
        chmod 0700 $XDG_RUNTIME_DIR
    fi
fi

# wait for weston creates its unix socket file
while [ ! -e $XDG_RUNTIME_DIR/wayland-0 ]; do
    sleep 0.1
done

# wait for weston listen on display
WESTON_XSERVER_STATUS=$(cat $XDG_RUNTIME_DIR/weston.log | grep 'xserver listening on display')
while [ -z "$WESTON_XSERVER_STATUS" ]; do
    echo "advantech startup script: weston xserver is not ready"
    sleep 1
    WESTON_XSERVER_STATUS=$(cat $XDG_RUNTIME_DIR/weston.log | grep 'xserver listening on display')
done
echo "advantech startup script: weston is ready"

# source environment
source /etc/profile
# export the name of the display of an running xserver
export DISPLAY=:0
# export the name of the display of an running wayland server
export WAYLAND_DISPLAY=wayland-0

# start startup application
$EXECUTE_COMMAND &

exit 0

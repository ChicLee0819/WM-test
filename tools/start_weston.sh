#!/bin/bash
WESTON_SERVICE_NAME=$(systemctl list-units --type=service | grep weston | awk -F ' ' '{print $1}')

case "$1" in
'start')
    systemctl start ${WESTON_SERVICE_NAME}
    ;;
'stop')
    systemctl stop ${WESTON_SERVICE_NAME}
    ;;
'restart')
    systemctl restart ${WESTON_SERVICE_NAME}
    ;;
*)
    echo "usage: $0 {start|stop|restart}"
    exit 1
    ;;
esac

exit 0

#!/bin/bash
### BEGIN INIT INFO
# Provides: client-timer
# Required-Start: $local_fs $remote_fs $network $time $rtc $all
# Required-Stop: $local_fs $remote_fs $network
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Short-Description: Start up the Client-timer at boot time
# Description: Main QT program.
### END INIT INFO

PATH='/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin'
export PATH

CLIENT_TIMER_NAME='client-timer'
CLIENT_TIMER_CMD='client-timer' #TODO add config file : -c /etc/client-timer/configuration.ini
CLIENT_TIMER_PID='/var/run/client-timer.pid'
CLIENT_TIMER_USER='root'
CLIENT_TIMER_LOG='/var/log/client-timer.log'

CLIENT_TIMER_BIN="$(which "${CLIENT_TIMER_NAME}")"

do_start() {
    test -f "${CLIENT_TIMER_BIN}" || exit 0
    echo -n $"Start main program QT : ${CLIENT_TIMER_NAME}"
    su - "${CLIENT_TIMER_USER}" -s '/bin/bash' -c "${CLIENT_TIMER_CMD} >> ${CLIENT_TIMER_LOG} 2>&1 & echo \$! > ${CLIENT_TIMER_PID}"
    echo '.'
}

do_stop() {
    test -f "${CLIENT_TIMER_BIN}" || exit 0
    if ! is_up
    then
        echo $'Main program QT daemon already stopped.'
        return 0
    fi
    echo -n $"Stopping main program QT : ${CLIENT_TIMER_NAME}"
    do_kill
    while is_up
    do
        echo -n '.'
        sleep 1
    done
    echo '.'
}

client_timer_pid() {
    tail -1 "${CLIENT_TIMER_PID}" 2> /dev/null
}

do_kill() {
    PID="$(client_timer_pid)"
    [ x"${PID}" != x ] && su - "${CLIENT_TIMER_USER}" -c "kill -TERM ${PID}"
}

do_restart() {
    test -f "${CLIENT_TIMER_BIN}" || exit 0
    do_stop
    sleep 1
    do_start
}

is_up() {
    PID="$(client_timer_pid)"
    [ x"${PID}" != x ] && ps -p "${PID}" -o comm h 2> /dev/null | grep -qFw "${CLIENT_TIMER_NAME}"
}

do_status() {
    test -f "${CLIENT_TIMER_BIN}" || exit 0
    if is_up
    then
        echo "${CLIENT_TIMER_NAME} is running."
        exit 0
    else
        echo "${CLIENT_TIMER_NAME} is not running."
        exit 1
    fi
}

do_usage() {
    echo $"Usage: $0 {start | stop | restart | status}"
    exit 1
}

case "$1" in
start)
    do_start
    exit "$?"
    ;;
stop)
    do_stop
    exit "$?"
    ;;
restart|force-reload)
    do_restart
    exit "$?"
    ;;
status)
    do_status
    ;;
*)
    do_usage
    ;;
esac

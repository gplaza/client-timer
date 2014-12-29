#!/bin/bash
### BEGIN INIT INFO
# Provides: rtc 
# Required-Start: $local_fs $remote_fs $network $time
# Required-Stop: $local_fs $remote_fs $network
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Short-Description: Init the hardware Real time clock
# Description: Real time Clock.
### END INIT INFO

PATH='/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin'
export PATH

RTC_MODEL='pcf8563' # ds1307 or pcf8563
RTC_I2C_ID='0x51' # 0x68 or 0x51
RTC_USER='root'

do_start() {

    # TODO : check probe rtc-ds1307/rtc-pcf8563
    # TODO : check i2c ID

    echo -n $"Init hardware Real Time Clock : ${CLIENT_TIMER_NAME}"
    su - "${RTC_USER}" -s '/bin/bash' -c "echo ${RTC_MODEL} ${RTC_I2C_ID} > /sys/class/i2c-adapter/i2c-1/new_device"
    su - "${RTC_USER}" -s '/bin/bash' -c "hwclock -s"
    echo '.'
}

do_stop() {
    # TODO ...
    echo '.'
}

do_restart() {
    do_stop
    sleep 1
    do_start
}

do_status() {
    # TODO ...
    echo '.'
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

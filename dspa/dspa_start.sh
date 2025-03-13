#!/bin/sh

sudo /usr/bin/mkdir /tmp/app_log
sudo chmod 777 -R /tmp/app_log

sudo rm -rf /var/run/pigpio.pid

sudo /usr/bin/pigpiod &

while :
do
	/usr/bin/ps -ae | grep -v grep | grep 'dspa' > /dev/null
	result_dspa=$?

	if [ "${result_dspa}" -ne "0" ]; then
		sudo /opt/dspa/dspa &
	fi

	sleep 5

done

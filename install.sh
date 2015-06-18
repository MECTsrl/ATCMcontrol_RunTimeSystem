#! /bin/sh

ROOT_FS=~/ltib/rootfs
ROOT=/root
PLC_RUNTIME=fcrts
PLC_HMI=vistestqt

if [ ! -e bin/$PLC_RUNTIME ]
then
	echo "cannot find the executable 'bin/$PLC_RUNTIME'"
	exit 1
fi

echo -n "preparing start script ..."

cat <<EOF > /tmp/fcrts
#! /bin/sh


start() {

if [ ! -e \$TSLIB_CALIBFILE ]; then
	/usr/bin/ts_calibrate
fi

if [ -x $ROOT/$PLC_RUNTIME ]; then
		echo -n "Starting PLC runtime '$PLC_RUNTIME'... "
		$ROOT/$PLC_RUNTIME &
		echo "Done."
		if [ -x $ROOT/$PLC_HMI ]; then
			echo -n "Starting PLC HMI '$PLC_HMI'... "
			$ROOT/$PLC_HMI -qws &
			echo "Done."
		fi
fi
}

stop() {
echo -n "Shutting down PLC HMI '$PLC_HMI'... "
killall $PLC_HMI 2> /dev/null
echo "Done."
echo -n "Shutting down PLC runtime '$PLC_RUNTIME'... "
killall $PLC_RUNTIME 2> /dev/null
echo "Done."
echo "Done."
}

restart() {
stop
sleep 1
start
}

case "\$1" in
start)
    start
;;
stop)
    stop
;;
restart)
    restart
;;
*)
  echo $"Usage: \$0 {start|stop|restart}"
        exit 1
    ;;
    
esac
EOF
echo "Done."

echo -n "installing $PLC_RUNTIME ..."
chmod +x /tmp/$PLC_RUNTIME
sudo chown root:root /tmp/$PLC_RUNTIME
sudo mv /tmp/fcrts $ROOT_FS/etc/rc.d/init.d/$PLC_RUNTIME

sudo cp bin/$PLC_RUNTIME $ROOT_FS/$ROOT/$PLC_RUNTIME
sudo chmod +x $ROOT_FS/$ROOT/$PLC_RUNTIME
sudo chown root:root $ROOT_FS/$ROOT/$PLC_RUNTIME

echo "Done."

if [ -e bin/$PLC_HMI ]
then
	echo -n "installing $PLC_HMI ..."
	sudo cp bin/$PLC_HMI $ROOT_FS/$ROOT/$PLC_HMI
	sudo chmod +x $ROOT_FS/$ROOT/$PLC_HMI
	sudo chown root:root $ROOT_FS/$ROOT/$PLC_HMI
fi

echo "Done."


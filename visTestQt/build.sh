#!/bin/sh

QMAKE=/usr/local/Trolltech/Qt-qvfb-version/bin/qmake 
TARGET="4CPC"
DEBUG="0"


cd ..
make -f _fcrts.mak   TARGET=$TARGET DEBUG=$DEBUG all
make -f _vistest.mak TARGET=$TARGET DEBUG=$DEBUG all
cd lib
if [ -e osKernel.a ]
then
	[ ! -e libosKernel.a ] && ln -s osKernel.a libosKernel.a
else
	echo "cannot found 'osKernel.a'"
	return 1;
fi

if [ -e osShared.a ]
then
	[ ! -e libosShared.a ] && ln -s osShared.a libosShared.a
else
	echo "cannot found 'libosShared.a'"
	return 1;
fi

if [ -e visLib.a ]
then
	[ ! -e libvisLib.a ] && ln -s visLib.a libvisLib.a
else
	echo "cannot found 'libvisLib.a'"
	return 1;
fi

cd -
cd visTestQt

$QMAKE -spec qws/linux-arm-gnueabi-g++
make install


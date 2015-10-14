#!/bin/sh

QUALE1=/opt/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-gcc
QUALE2=/opt/freescale/usr/local/gcc-4.3.3-glibc-2.8-cs2009q1-203/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-gcc
QUALE3=/opt/freescale/usr/local/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/bin/arm-linux-gcc

echo $QUALE1
cd ~/sources/imx28_rt/svn_realtime/base/prove/1 && $QUALE1 -march=armv5te -S -Wcast-align -D_GNU_SOURCE -c ../aligned.c ../attribute.c ../prova.c ../unaligned.c
echo $QUALE2
cd ~/sources/imx28_rt/svn_realtime/base/prove/2 && $QUALE2 -march=armv5te -S -Wcast-align -D_GNU_SOURCE -c ../aligned.c ../attribute.c ../prova.c ../unaligned.c
echo $QUALE3
cd ~/sources/imx28_rt/svn_realtime/base/prove/3 && $QUALE3 -march=armv5te -S -Wcast-align -D_GNU_SOURCE -c ../aligned.c ../attribute.c ../prova.c ../unaligned.c

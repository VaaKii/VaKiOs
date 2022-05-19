#!/bin/sh
set -e
. ./iso.sh

. ./config.sh

for PROJECT in $PROJECTS; do
  (cd $PROJECT && $MAKE clean)
done

qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom myos.iso # -monitor /dev/stdout -S -D ./log.txt

rm -rf sysroot
rm -rf isodir
rm -rf myos.iso
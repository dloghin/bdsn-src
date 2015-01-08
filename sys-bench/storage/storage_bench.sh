#!/bin/bash
#
# Storage benchmarking:
# - throughput (dd)
# - latency (ioping)
#
# Copyright (C) 2014 Dumitrel Loghin
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

BS=1M
CNT=2048
TMPF=tempf
TMP1=tmp_1

function throughput_bench {
	echo "*** running throughput bench using dd version `dd --version | head -1`"
        # 1. get write speed
        echo "Write"
        dd if=/dev/zero of=$TMPF bs=$BS count=$CNT conv=fdatasync,notrunc

        # 2. get read speed by dropping the OS cache
        echo "Read"
        echo 1 > $TMP1
        cp $TMP1 /proc/sys/vm/drop_caches
        dd if=$TMPF of=/dev/null bs=$BS count=$CNT

        # 3. get buffered read speed
        echo "Buffered Read"
        dd if=$TMPF of=/dev/null bs=$BS count=$CNT

        rm -rf $TMPF
	rm -rf $TMP1
	echo "*** throughput bench done."
}

function latency_bench {
	echo "*** running latency bench using ioping version `ioping -v`"
	echo "Write"
	ioping -c 10 -W .
	echo "Read"
	ioping -c 10 -D .
	echo "*** latency bench done."
}

# check root
if [ $UID -ne 0 ]; then 
	echo "Please run this benchmark as root!"
	exit
fi

#check memory
MEM=`free -m | grep Mem: | tr -s ' ' | cut -d ' ' -f 2`
# set threshhold as 1/3 of the memory
MEM=$((MEM / 3))
if [ $CNT -ge $MEM ]; then
	while [ $CNT -ge $MEM ]; do
		CNT=$(($CNT / 2))
	done
	echo "New dd count value: $CNT"
fi

throughput_bench
latency_bench

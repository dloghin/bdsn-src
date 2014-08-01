# Run script for Java ubench. 
# 
# Copyright (C) 2013 B. M. Tudor, D. Loghin
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

#!/bin/bash

ARCH=$(uname -m)

BASEDIR=$(dirname $0)

JVM="java"

CP=$BASEDIR

PRG="FloatInt"

IT=5000000000

if [ $# -lt 1 ]; then
	echo "Usgae: $0 <cores>"
	exit 1
fi

cores=$(($1-1))

for core in `seq 0 $cores` ; do
	MASK=$((2**$core))
	taskset $MASK $JVM -cp $CP $PRG $IT & 
done

wait

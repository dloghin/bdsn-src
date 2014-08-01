#!/bin/bash

ARCH=$(uname -m)

BASEDIR=$(dirname $0)

IT=1000000000

if [ $# -lt 1 ]; then
        echo "Usage: $0 <cores>"
        exit 1
fi

cores=$(($1-1))

for core in `seq 0 $cores` ; do
        $BASEDIR/dhrystone-$ARCH $IT $core &
done

wait

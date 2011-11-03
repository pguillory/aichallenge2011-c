#!/bin/bash
cd `dirname $0`
bot=./MyBot
while [ -f $bot ]; do
    nice -16 python ../tcpclient.py 213.88.39.97 2081 $bot pguillory z3B8R9VBfD7Bmpbx 1
    sleep 1
done

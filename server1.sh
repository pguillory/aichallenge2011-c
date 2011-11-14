#!/bin/bash
cd `dirname $0`
bot=./MyBot
while [ -f $bot ]; do
    echo "new game" | growlnotify
    nice -16 python ../tcpclient.py ants.fluxid.pl 2081 $bot pguillory 8xR8UQBYWjb75Ut9 1
    sleep 1
done

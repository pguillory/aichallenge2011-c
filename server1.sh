#!/bin/bash
cd `dirname $0`
while true; do
    python ../tcpclient.py ants.fluxid.pl 2081 "./MyBot" pguillory 8xR8UQBYWjb75Ut9 1
    sleep 1
done

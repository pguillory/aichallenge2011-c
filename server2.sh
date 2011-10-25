#!/bin/bash
cd `dirname $0`
while true; do
    python ../tcpclient.py 213.88.39.97 2081 "./MyBot" pguillory z3B8R9VBfD7Bmpbx 1
    sleep 1
done

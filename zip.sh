#!/bin/bash
cd `dirname $0`
pwd=`pwd`
name=`basename $pwd`
filename=../$name.tgz

rm $filename
export COPYFILE_DISABLE=true
tar zcvf $filename *.c *.h

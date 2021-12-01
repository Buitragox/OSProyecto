#!/bin/bash

dir="./website"
port="7070"
buffers="5"
schedalg="SFF"

cleanStr="make clean"
makeStr="make"
executeStr="./wserver -d $dir -p $port -b $buffers -s $schedalg"

echo $cleanStr
eval $cleanStr

echo $makeStr
eval $makeStr

echo $executeStr
eval $executeStr

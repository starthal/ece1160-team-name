#!/bin/bash

FILE=../functional/bitmap.c

if [ -f $1.pnm ]
then
    echo the file exists
else
    png2pnm $1.png $1.pnm
fi

echo " " >> $FILE
./imgconv $1.pnm $1 >> /dev/null && ./imgconv $1.pnm $1 >> $FILE || echo "error"


#!/bin/bash

# arguments $1 and $2 are the old/new project names

cp -r $1 $2

# Fix geany project file
cat ${2}/${1}.geany | sed -e "s/${1}/${2}/g" > ${2}/${2}.geany && rm ${2}/${1}.geany 

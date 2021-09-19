#!/bin/bash
rm -r build 
rm -r errors
mkdir build
mkdir errors
file=Topic5McLaughlin.c
builds=()

IFS=$'\n'
while read line;
do
    builds+=($line)
done < builds.txt
len=${#builds[*]}


for ((j=0; j<${len}; j=j+3));
do
    compiler=${builds[$j]}
    flags=${builds[(j+1)]}
    output=${builds[(j+2)]}
    #line="$compiler $flags $file -o build/$output 2> errors/$output"
    line="$compiler $flags $file -o build/$output"
    eval $line
    if [ $? -eq 0 ] 
    then 
        success="success"
    else
        success="failed"
    fi
    echo $compiler $flags $output ... $success
done


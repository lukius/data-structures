#!/bin/bash

for dir in *
do
    if [ -d $dir ] && [ "$dir" != "gtest" ];
    then
        cd $dir;
        echo "Compiling ${dir} tests...";
        make clean;
        make test;
        ./$dir;
        cd ..;
    fi;
done
echo "Done!";
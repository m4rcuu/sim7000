#!/bin/bash

project_name='project'
sources_list='main.cpp sim7000.cpp'

if test -f $project_name; then
    rm -f $project_name
fi

g++ -Wall -o $project_name $sources_list -lwiringPi
echo -e 'starting program\n'
./$project_name
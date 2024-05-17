#!/bin/bash

# undo the changes made in editor.sh

# delete the new files in src
cd src
    rm -f new_file1.c new_file2.c bugga/new_file3.c
cd ..

# create the deleted files in src
cd src
    touch extra_1.cpp
    mkdir extra_dir
    touch extra_dir/extra_2.cpp
cd ..

# reset the permissions of the files in src

cd src
    chmod u+w server.c
    chmod u+w bugga/2.c
cd ..

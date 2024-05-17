#!bin/bash

echo "Making some edits in src"

# create some new files in src
cd src
    touch new_file1.c new_file2.c bugga/new_file3.c
cd ..

# touch some files in src
cd src
    touch a.cpp b.cpp
    touch D1/D2/D3/D4/4_1.txt
cd ..

# delete some files
cd src
    rm -rf extra_1.cpp
    rm -rf extra_dir/extra_2.cpp
    rmdir extra_dir
cd ..


# change permissions of some files in src remove the write permission from user

cd src
    chmod u-w server.c
    chmod u-w bugga/2.c
cd ..

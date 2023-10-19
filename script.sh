#!bin/bash

# g++ run.cpp
g++ blockchain.cpp main.cpp
if [ ! -d "output" ]; then
    mkdir "output"
fi
for ((i=1; i<=$1; i++))
do
    cd output
    if [ ! -d "node$i" ]; then
        mkdir "node$i"
    fi
    cd ..
done


    ./a.out $1 $2 $3 30 25


for ((i=1; i<=$1; i++))
do
    if [ -e "tree$i.dot" ]; then
        dot -Tpng "tree$i.dot" -o "./output/node$i/tree$i.png"
        rm "tree$i.dot"
    fi
done

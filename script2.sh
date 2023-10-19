#!bin/bash


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

for ((k=25; k<=85; k+=30))
do
    for ((j=1;j<=15;j+=1))
    do
        echo "$j"
        for ((i=10;i<=50;i+=5))
        do
            ./a.out $1 $2 $3 $i $k
        done
        python3 script.py
        rm output.csv
        touch output.csv
    done
    python3 plot.py $k
    rm data.csv
    touch data.csv
done 

python3 plot1.py
mv *.csv ./data
mv *.png ./plots

for ((i=1; i<=$1; i++))
do
    if [ -e "tree$i.dot" ]; then
        dot -Tpng "tree$i.dot" -o "./output/node$i/tree$i.png"
        rm "tree$i.dot"
    fi
done

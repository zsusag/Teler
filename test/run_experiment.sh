#!/usr/bin/bash

# Generate initial random directory tree
BYTES_GIT="$(rndtree.sh ./git 1 3 4 6)"
cd ./git
git init

for(( i=0; i<$1; i++)); do
    START=$(date +%s.%N)
    git add . 2>&1 1>/dev/null
    git commit -m "test $i" 2>&1 1>/dev/null
    END=$(date +%s.%N)
    DIFF=$(echo "$END - $START" | bc)
    echo "$DIFF $BYTES_GIT" >> "../test1_git.txt"
    BYTES_GIT="$(rndtree.sh "./$i" 1 3 4 6)"

done

cd ..
rm -rf ./git

# Generate initial random directory tree
BYTES_TELER="$(rndtree.sh ./teler 1 3 4 6)"
cd ./teler
teler init
for(( i=0; i<$1; i++)); do
    START=$(date +%s.%N)
    echo $i | teler push 2>&1 1>/dev/null
    END=$(date +%s.%N)
    DIFF=$(echo "$END - $START" | bc)
    echo "$DIFF $BYTES_TELER" >> "../test1_teler.txt"
    BYTES_TELER="$(rndtree.sh "./$i" 1 3 4 6)"
done

cd ..
rm -rf ./teler

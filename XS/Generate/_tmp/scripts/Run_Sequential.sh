#!/bin/bash

# Script to train MLPs
#@author BaL
#

cd ../..
echo "----------------------------------"
echo "--- Run Training from MLP 0 to 689---"
echo "----------------------------------"

Start=$((0))
End=$((690))

for ((reaction=$Start; reaction<$End; reaction=reaction+1 )) ;
do
     ./Train_XS $reaction
done
cd -

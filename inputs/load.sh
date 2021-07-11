#!/bin/bash
for i in {1..106} 
do
	echo "Downloading $i..."
	wget https://poses.live/problems/$i/download -O $i.problem
done	
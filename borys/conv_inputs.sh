#!/bin/bash
for i in {1..132}
do
	echo "Convert $i..."
	target/debug/conv_input <  ../inputs/$i.problem > ../inputs_conv/$i.problem
done	
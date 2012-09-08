#!/bin/bash

for i in */Makefile */firmware/Makefile; do
	project=$(dirname $i)
	echo ==================== $project
	if ! make -C $project $1; then
		echo "FAILED: $project"
		exit
	fi
done

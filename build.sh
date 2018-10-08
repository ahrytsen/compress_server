#! /usr/bin/env bash

if [ -z $@ ]
then
	make server
else
	if ! make $@
	then
		echo $'\n'"usage: ./build.sh [server|client|clean|fclean]"$'\n'
	fi
fi

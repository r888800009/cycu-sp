#!/bin/bash
name="10612150"

rm $name -r
mkdir $name
cp -r * $name

zip -rl $name.zip $(cat list.txt |xargs -I % echo $name/%)

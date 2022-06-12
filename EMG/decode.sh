#! bin/bash

openssl des3 -iter 100000 -d -k 518520 -in /home/pi/Downloads/game/key | tar xzPf - > /dev/null 2>&1

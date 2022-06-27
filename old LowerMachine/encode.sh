#! bin/bash

tar -czvPf /home/pi/Downloads/game/keytemp.tar /home/pi/Downloads/game/keyy.txt > /dev/null 2>&1
openssl des3 -iter 100000 -salt -k 518520 -in /home/pi/Downloads/game/keytemp.tar -out /home/pi/Downloads/game/key > /dev/null 2>&1
rm -f /home/pi/Downloads/game/keyy.txt > /dev/null 2>&1
rm -f /home/pi/Downloads/game/keytemp.tar > /dev/null 2>&1

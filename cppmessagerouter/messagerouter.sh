#!/bin/csh

while (1)
  /usr/local/sbin/messageserver >>& /var/log/messagerouter.log &
  echo "$!" > messagerouter.pid
  while (-e /proc/$!) 
    sleep 5
  end   
  echo MESSAGEROUTER DIED >> /var/log/messagerouter.log
end

#!/bin/csh

while (1)
  /usr/local/sbin/messageserver -d >> /var/log/messagerouter.log &
  echo "$!" > messagerouter.pid
  while (-e /proc/$!) 
    sleep 5
  end   
  echo MESSAGEROUTER DIED >> /var/log/messagerouter.log
  mv /var/log/messagerouter.log /var/log/messagerouter.crash.`date +%s`
end

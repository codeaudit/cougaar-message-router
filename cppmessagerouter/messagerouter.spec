# Spec file for Message Server
#
Summary: RPM for installing the Cougaar society message server
Name: messagerouter
Version: 1
Release: 7.40
Copyright: Cougaar
Group: Applications/UltraLog
Source: messagerouter
URL: http://www.ultralog.new
Distribution: RedHat Linux 
Vendor: InfoEther, LLC
Packager: David Craine

%undefine __check_files
%description 
This file contains the Cougaar society message server executable 

%prep
%build
%install
%files
/usr/local/sbin/messageserver
/usr/local/sbin/messagerouter.sh
/etc/init.d/messagerouter
%clean
%post
chkconfig --add messagerouter

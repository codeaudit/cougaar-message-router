# Spec file for Message Server
#
Summary: RPM for installing the Cougaar society message server
Name: messageserver
Version: 1
Release: 1 
Copyright: Cougaar
Group: Applications/UltraLog
Source: messageserver
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
/etc/init.d/messageserver
%clean
%post

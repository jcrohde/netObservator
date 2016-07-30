Copyright (C) 2015-2016 Jan Christian Rohde

This file is part of netObservator.

netObservator is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

netObservator is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
netObservator. If not, see http://www.gnu.org/licenses/.



                       LIBRARIES

netObservator is written in QT 5.2.0 for 32bit Windows systems (and runs also
on 64bit Windows machines).

netObservator uses the third party software WinPcap. netObservator was
developed with WinPcap 4.1.2 Developer's Pack. With this package you get an
installer for WinPcap 4.1.3, which should be compatible with the Developer's
pack.



                       DISCLAIMER
	
The third party software WinPcap_4_1_3 installer is redistributed WITHOUT ANY
WARRANTY.


	
                       INSTALL / START
					
By the following 3 steps, you can run netObservator:

1.) You need WinPcap to run netObservator. If you have an older version of
WinPcap or not have WinPcap installed, you can install a new version of WinPcap
with the installer you got with this package.

2.) Click on "start netObservator". That's it! 

3.) Have fun!



                       GENERAL INFORMATION

netObservator is a simple to use sniffer app. The goal is to give you information
about the connections of your computer.

When you start to sniff, only information about the first packet of a
connection with another host will be displayed. This information will
disappear some time after the last packet from or to this host has been
sniffed. This will give you an overview of the hosts you are currently
connected with.

After sniffing information about all packets can be displayed and stored in
XML files. These slices can be displayed after sniffing.

You can search on these files. Moreover you can also edit filters for sniffing.

For a quick start just select the device of your internet connection and click
the "Run"-button. Then the program starts to sniff. If you press this button a
second time, the program will stop to sniff.
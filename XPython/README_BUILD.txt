4/12/2020
pbuck@avnwx.com

This directory XPPython3 contains code for python plugin, as modified to support
Python3 and _only_ SDK 303+ (X-Plane 11.50+)

XPPython3/
   *.[ch]
   lin/
      Makefile
   win/
      Makefile
   mac/
      Makefile
   SDK/
      CHeaders/
      Libraries/
      Delphi/
   Resources/plugins/

To build:
a) verify ``python-config'' returns info for python 3 (this is used by makefile)
b) Copy / link current SDK as "SDK" directory immediately under XPPython3. I DO NOT INCLUDE SDK with this plugin source
c) To build:
     make linux
     make darwin
     make windows
   
d) Build objects are in (lin|mac|win) subdirectory: .o and <target>.xpl.
   AND python plugin copied <target>.xpl copied to Resources/plugsin/XPPython3/(mac|win|lin)_x64/

Quick non-X-Plane test, from XPPython3 directory  ##!! Only partially supported 5/29/2020
$ make test
... This will call mac/main_302, with the plugin location correctly set.
   
Set your X-Plane to point to build product location
  X-Plane/Resources/plugins/XPPython3 -> <code>/XPPython3/Resources/plugins/XPPython3
  That way, starting X-Plane, will use latest built plugin

For Windows.. I installed 64-bit version of mingw, updated makefiles to support
cd /z

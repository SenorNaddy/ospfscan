ospfscan
========

An OSPF Routing Protocol Monitor capable of scanning OSPFv2 for changes to network topology

## Requirements

<<<<<<< HEAD
* libtrace 3.10 available from [http://www.wand.net.nz](http://www.wand.net.nz)
=======
* libtrace 3.10 available from [http://www.wand.net.nz][http://www.wand.net.nz]
>>>>>>> 74190caaae55005a70c412eb6bbe975af658465f
* libwandevent
* libsqlite3

libtrace now includes ospf functions (originally inspired by this project), so it's entirely possible a new version of libtrace may break this code. In which case I suggest using an older version.

I could upgrade the code to make use of the libtrace functions, but I don't have time
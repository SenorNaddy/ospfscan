ospfscan
========

An OSPF Routing Protocol Monitor capable of scanning OSPFv2 for changes to network topology

## Requirements

* libtrace 3.0.10 available from [http://www.wand.net.nz](http://www.wand.net.nz)
* libwandevent
* libsqlite3
* argtable2

You need to use the new.db SQLite3 database as a base. Copy this file before use. (It provides the base schema that the application expects. Without this file as the base for the database, the application will crash)

libtrace now includes ospf functions (originally inspired by this project), so it's entirely possible a new version of libtrace may break this code. In which case I suggest using an older version. I can however confirm that it works fine with libtrace 3.0.14.

I could upgrade the code to make use of the libtrace functions, but I don't have time

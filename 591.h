#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <sqlite3.h>
#include <time.h>
#include "libtrace.h"
#include "libwandevent.h"
#include "ospf.h"
#include "sql.h"
#include "daemons.h"
#include "logger.h"
#include "event.h"
#include <signal.h>

using namespace std;

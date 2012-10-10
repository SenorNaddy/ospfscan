/*
 * ospf_scan.cpp
 *
 *  Created on: 16/03/2011
 *      Author: Simon Wadsworth
 */
#include "591.h"
#include <list>

char *prog = NULL;
char *pidfile = NULL;
int verbose = 0;
sqlite3 *db;

int per_packet(libtrace_packet_t *packet);
static int process_mon_event(mon_env_t *env, libtrace_eventobj_t event);


void version()
{
    printf("OSPF Route Monitor %.2f alpha\nSimon Wadsworth <sw162@cs.waikato.ac.nz>\n", 0.1);
    printf("Compiled against libtrace v%i.%i\n", LIBTRACE_API_VERSION >> 16, LIBTRACE_API_VERSION & 0xF);
    printf("Using SQLite v%s\n", SQLITE_VERSION);
}

void usage(char *pname)
{
    logger(LOG_DAEMON | LOG_DEBUG, "usage: %s [args] filename\n", pname);
}


static void mon_event(mon_env_t *env)
{
	struct libtrace_eventobj_t event;
	libtrace_t *trace = env->trace;
	int poll_again = 1;

	do
	{
		if (!env->packet)
		{
			env->packet = trace_create_packet();
		}

		event = trace_event(trace, env->packet);
		poll_again = process_mon_event(env, event);

	/* As long as there are packets available, we want to be reading
	 * them. Once we get a "wait" event of some description, then we
	 * can fall out of this loop */
	} while (poll_again);
}


/* Callback function for a file descriptor event
 *
 * Parameters:
 * 	evcb - the event that was triggered
 * 	ev - the type of event that occured
 */
static void fd_read_event(struct wand_fdcb_t *evcb, enum wand_eventtype_t ev)
{

	mon_env_t *env = (mon_env_t *)evcb->data;
	wand_del_event(env->wand_ev_hdl, evcb);

	/* Not very nice if this fails but it really REALLY shouldn't fail */
	//assert(ev == EV_READ);
	mon_event(env);
}

/* Callback function for a timer event
 *
 * Paramters:
 * 	timer - the timer event that was triggered
 */
static void timer_event(struct wand_timer_t *timer)
{
	mon_event((mon_env_t *)timer->data);
}

/* Processes a libtrace event
 *
 * Parameters:
 * 	env - the maji environment variables
 * 	event - the libtrace event to be processed
 *
 * Returns:
 * 	1 if the caller should immediately check for another event, 0 otherwise
 */
static int process_mon_event(mon_env_t *env, libtrace_eventobj_t event) {
	wand_event_handler_t *ev_hdl = env->wand_ev_hdl;

	switch(event.type)
	{
		case TRACE_EVENT_IOWAIT:
			/* File descriptor has no data - wait until it does */
			env->fd_cb.fd = event.fd;
			env->fd_cb.flags = EV_READ;
			env->fd_cb.data = env;
			env->fd_cb.callback = fd_read_event;
			wand_add_event(ev_hdl, &env->fd_cb);
			return 0;

		case TRACE_EVENT_SLEEP:
			/* Next packet will be available in N seconds, sleep
			 * until then */
			int micros;
			micros = (int)((event.seconds - (int)event.seconds) * 1000000.0);
			env->timer.expire = wand_calc_expire(ev_hdl,
					(int)event.seconds, micros);
			env->timer.callback = timer_event;
			env->timer.data = env;
			env->timer.prev = env->timer.next = NULL;
			wand_add_timer(ev_hdl, &env->timer);
			return 0;

		case TRACE_EVENT_PACKET:
			/* A packet is available - pass it on to the meter */
			if (event.size == -1)
			{
				/* Error occured */
				/* We don't need wdcap's fancy
				 * error handling - just drop the
				 * trace */
				ev_hdl->running = false;
				return 0;
			}
			per_packet(env->packet);
			if (!env->packet)
				env->packet = trace_create_packet();
			return 1;

		case TRACE_EVENT_TERMINATE:
			/* The input trace has terminated */
			ev_hdl->running = false;
			return 0;

		default:
			logger(LOG_DAEMON | LOG_DEBUG,
					"Unknown libtrace event type: %d\n",
					event.type);

			return 0;
	}

}

int per_packet(libtrace_packet_t *packet)
{
        ospf_t *ospf_pkt;
        ospf_db_desc_t *ospf_db;
		libtrace_ip_t *ip;
		uint32_t *remaining = 0;ip = trace_get_ip(packet);
                        if( ip == NULL )
                        {
                            return -1;
                        }
			trace_get_layer3(packet, NULL, remaining);
			if(ip->ip_p == 89)
			{

				//ospf_pkt = (ospf_t *)

				u_char* the_packet = (u_char*)trace_get_payload_from_ip(ip, NULL, remaining); //get the packet after the IP header
				ospf_pkt = get_ospf(the_packet); //get the ospf header from the payload
                    if(ospf_pkt == NULL)
                                    return -1;
                    if(ospf_pkt->ospf_type == OSPF_DATADESC) {
                        ospf_db = get_ospf_db_desc_header(the_packet); //if this is a datadesc packet get the hedaer
                    }
                                        //do some debug information if asked for and then skip over the OSPF_HELLO, OSPF_LSUPDATE, OSPF_LSREQ packets
					if(verbose) printf("OSPF v%i Packet\n", ospf_pkt->ospf_v);
					if(ospf_pkt->ospf_type == OSPF_HELLO)
					{
						if(verbose) printf("\tType: HELLO\n");
                                                return -1;
					}
					if(ospf_pkt->ospf_type == OSPF_LSUPDATE)
					{
						if(verbose) printf("\tType: LINK STATE UPDATE\n");
					}
					if(ospf_pkt->ospf_type == OSPF_LSREQ)
					{
						if(verbose) printf("\tType: LINK STATE REQUEST\n");
                                                return -1;
					}
					if(ospf_pkt->ospf_type == OSPF_LSACK)
					{
						if(verbose) printf("\tType: LINK STATE ACKNOWLEDGE\n");
                                                return -1;
					}
					if(ospf_pkt->ospf_type == OSPF_DATADESC)
					{
						if(verbose) printf("\tTYPE: DATABASE DESCRIPTION\n");
					}
                                        //more debug stuff
					if(verbose) {
					    printf("\tPacket Length: %i\n", htons(ospf_pkt->ospf_len));
                        printf("\tRouter ID: %s\n", inet_ntoa(ospf_pkt->ospf_router));
                        printf("\tArea ID: %s\n",inet_ntoa(ospf_pkt->ospf_area));
                        printf("\tChecksum: 0x%X\n",htons(ospf_pkt->ospf_sum));
                        printf("\tAuthentication Type: 0x%X\n",ospf_pkt->ospf_au);
					}

                                        if(ospf_pkt->ospf_type == OSPF_DATADESC) {
                                            if(verbose) printf("\tDD Sequence: %i\n", htonl(ospf_db->ospf_db_desc_seq));
                                            //go through each lsa getting each lsa header from the database desc packet, and printing debug information
                                            uint32_t total = 0;
                                            ospf_lsa_t *lsa;
                                            for(;;)
                                            {
                                                lsa = get_ospf_next_lsa_from_packet(the_packet, &total);
                                                if(lsa == NULL)
                                                    break;
                                                if(verbose) {
                                                    printf("\tLink-State Header\n");
                                                    printf("\t\tLS Age: %i\n", htons(lsa->ospf_lsa_age));
                                                    printf("\t\tLink-State ID: %s\n", inet_ntoa(lsa->ospf_lsa_ls_id));
                                                    printf("\t\tAdvertising Router: %s\n", inet_ntoa(lsa->ospf_lsa_adv_router));
                                                    printf("\t\tSequence Number: %x\n", htonl(lsa->ospf_lsa_seq));
                                                    printf("\t\tLSA Type: %x\n", lsa->ospf_lsa_type);
                                                }
                                            }
                                        }
                                        if(ospf_pkt->ospf_type == OSPF_LSUPDATE)
                                        {
                                            //get the lsa_update packet
                                            ospf_ls_update_t *ls_upd = get_ospf_ls_update(the_packet);
                                            if(verbose) printf("\tNumber Advertisements: %i\n",htonl(ls_upd->ospf_ls_num_adv));
                                            //loop over the lsa's
                                            uint32_t total = 0;
                                            ospf_lsa_t *lsa;
                                            for(long i = 0; i < htonl(ls_upd->ospf_ls_num_adv); i++)
                                            {
                                            	list<in_addr> ext_routes;
                                                lsa = get_ospf_next_lsa_from_packet(the_packet, &total);
                                                if(lsa == NULL)
                                                    break;
                                                if(verbose)
                                                {
                                                    printf("\tLink-State Header\n");
                                                    printf("\t\tLS Age: %i\n", htons(lsa->ospf_lsa_age));
                                                    printf("\t\tLink-State ID: %s\n", inet_ntoa(lsa->ospf_lsa_ls_id));
                                                    printf("\t\tAdvertising Router: %s\n", inet_ntoa(lsa->ospf_lsa_adv_router));
                                                    printf("\t\tSequence Number: %x\n", htonl(lsa->ospf_lsa_seq));
                                                    printf("\t\tLSA Type: %x\n", lsa->ospf_lsa_type);
                                                }
                                                insert_router(db, ospf_pkt, lsa); //for every lsa insert the router information into the database
                                                if(lsa->ospf_lsa_type == OSPF_LS_ROUTER)
                                                {
                                                    //this code handles the links within the OSPF_LS_ROUTER lsa
                                                    ospf_link_t *link;
                                                    uint32_t link_index = 0;
                                                    list<in_addr> links;
                                                    //get the router_lsa, we need to skip over all the extra ospf headers before it first
                                                    ospf_router_lsa_t *router_lsa = (ospf_router_lsa_t *)(the_packet + sizeof(ospf_t) + sizeof(ospf_ls_update_t) + sizeof(ospf_lsa_t) + total - htons(lsa->ospf_lsa_length));
                                                    if(verbose) printf("%i links\n", htons(router_lsa->ospf_router_lsa_num));
                                                   //loop over each link in the lsa
                                                    for(uint16_t k = 0; k < htons(router_lsa->ospf_router_lsa_num); k++) {
                                                        link = get_ospf_link_from_lsa(the_packet, lsa, &total, &link_index);
                                                        if(link == NULL)
                                                            break;
                                                        //display some connection information and then insert the router interface into the database
                                                        if(verbose) printf("\t\t%s is connected to ", inet_ntoa(lsa->ospf_lsa_adv_router));
                                                        if(verbose) printf("%s\n", inet_ntoa(link->ospf_router_lsa_link_id));
                                                        insert_router_interface(db, lsa, link);
                                                        links.push_back(link->ospf_router_lsa_link_id);
                                                    }
                                                    check_router_interface(db, lsa->ospf_lsa_adv_router, links);
                                                }
                                                if(lsa->ospf_lsa_type == OSPF_LS_NETWORK)
                                                {
                                                    //this code parses the network lsa
                                                    ospf_network_lsa_t *network_lsa = (ospf_network_lsa_t *)(the_packet + sizeof(ospf_t) + sizeof(ospf_ls_update_t) + sizeof(ospf_lsa_t) + total - htons(lsa->ospf_lsa_length));
                                                    if(verbose) printf("\t\t%s/", inet_ntoa(lsa->ospf_lsa_ls_id));
                                                    if(verbose) printf("%s\n", inet_ntoa(network_lsa->ospf_network_lsa_network_mask));
                                                    uint32_t router_index = 0;
                                                    ospf_router_t *router;
                                                    //loop over the contents, printing out each router listed in the packet
                                                        while(htons(lsa->ospf_lsa_length)/4 >= sizeof(ospf_network_lsa_t) + router_index)
                                                        {
                                                            router = (ospf_router_t *)(the_packet + sizeof(ospf_t) + sizeof(ospf_ls_update_t) + sizeof(ospf_lsa_t) + total - htons(lsa->ospf_lsa_length) + sizeof(ospf_network_lsa_t) + router_index);
                                                            if(verbose) printf("\t\t\t router %s\n", inet_ntoa(router->ospf_router_id));
                                                            router_index += sizeof(ospf_router_t);
                                                        }
                                                }
                                                if(lsa->ospf_lsa_type == OSPF_LS_EXTERNAL)
                                                {
                                                    //get the external route information and insert it into the database
                                                    ospf_as_external_lsa_t *external_lsa = (ospf_as_external_lsa_t *)(the_packet + sizeof(ospf_t) + sizeof(ospf_ls_update_t) + sizeof(ospf_lsa_t) + total - htons(lsa->ospf_lsa_length));
                                                    if(verbose) printf("\t\t%s/", inet_ntoa(lsa->ospf_lsa_ls_id));
                                                    if(verbose) printf("%s\n", inet_ntoa(external_lsa->ospf_as_external_network));
                                                    //links.push_back(external_lsa->ospf_as_external_network);
                                                    insert_external_route(db, lsa, external_lsa);
                                                    update_external_route(db, lsa, external_lsa);
                                                }
                                            }

                                        }
                                        return 0;
			}
			return -1;
}
void cleanup(int sig)
{
	sqlite3_close(db);
	exit(0);
}
int main(int argc, char *argv[])
{
	signal(SIGABRT, &cleanup);
	signal(SIGKILL, &cleanup);
	signal(SIGINT, &cleanup);
	signal(SIGTERM, &cleanup);
    int runAsDaemon = 0;
    int useInterface = 0;
    libtrace_filter_t *filter;
    opterr = 0;

    struct arg_file *database_file = arg_file1(NULL, "database", NULL, "SQLite3 database");
    struct arg_lit *interface = arg_lit0("i", NULL, 		"interface");
    struct arg_lit *daemon = arg_lit0("d", NULL,		"run as daemon");
    struct arg_lit *debug = arg_lit0("v","verbose,debug",	"verbose messages");
    struct arg_lit *help = arg_lit0("?h","help",		"display help");
    struct arg_file *infiles = arg_file1(NULL,NULL,NULL,	"trace file or interface name");
    struct arg_end *end = arg_end(20);
    int nerrors;
    void *argtable[] = {interface, database_file, daemon, debug, help, infiles, end};


    if(arg_nullcheck(argtable) != 0)
    {
	printf("%s: insufficient memory\n", argv[0]);
	return -1;
    }
    nerrors = arg_parse(argc,argv,argtable);

    if(help->count > 0)
    {
	logger(LOG_DAEMON | LOG_DEBUG, "Usage: %s", argv[0]);
	arg_print_syntaxv(stdout, argtable, "\n");
	arg_print_glossary(stdout, argtable,"	%-25s %s\n");
	return 0;
    }

    if(nerrors > 0)
    {
	arg_print_errors(stdout,end,argv[0]);
	logger(LOG_DAEMON | LOG_DEBUG, "Try '%s --help' for more information.\n", argv[0]);
	return 0;
    }
    else
    {
	if(interface->count > 0) useInterface = 1;
	if(daemon->count > 0) runAsDaemon = 1;
	if(debug->count > 0) verbose = 1;
        printf("filename %s\n", infiles->filename[0]);
        /*for (arg = 1; arg < argc-1; arg++)
        {
            if (strcmp(argv[arg], "--debug") == 0)
            {
                verbose = 1;
            }
            if (strcmp(argv[arg], "-d") == 0)
            {
                runAsDaemon = 1;
            }
            if(strcmp(argv[arg], "-i") == 0)
            {
                useInterface = 1;
            }
        }*/
    }
    version();

    wand_event_handler_t *ev_hdl = NULL;

	mon_env_t env;

	env.packet = NULL;

    if (wand_event_init() == -1) {
		logger(LOG_DAEMON | LOG_DEBUG,
			"Error initialising libwandevent\n");
		return -1;
	}

    if(runAsDaemon)
    {
        prog = strdup(argv[0]);
        daemonise(prog);
        put_pid(pidfile);
        logger(LOG_DAEMON, "OSPF Monitor Daemon Started");
        //daemon(0,0);
    }

	/* Create the event handler */
	ev_hdl = wand_create_event_handler();
	if (ev_hdl == NULL) {
		logger(LOG_DAEMON | LOG_DEBUG, "Error creating event handler\n");
		return -1;
	}

	env.wand_ev_hdl = ev_hdl;



    int rc;
    rc = sqlite3_open_v2(database_file->filename[0], &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL); //open this database, this needs SQLite 3.5.0 or higher
    if ( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    char str[1024] = "";
    if(useInterface)
    {
    	strcat(str,"pcapint:");
    }
    else
    {
        strcat(str, "pcapfile:");
    }
    strcat(str, infiles->filename[0]);
	libtrace_t *trace = trace_create(str); //create our trace
		if(useInterface) {
			int one = 1;
			trace_config(trace, TRACE_OPTION_PROMISC, &one);
		}
        if(!useInterface) {
            //having issues with this on live capture so disable if using interface
            filter = trace_create_filter("ip proto 89");
            trace_config(trace, TRACE_OPTION_FILTER, filter);
            int one = 1;
            trace_config(trace, TRACE_OPTION_EVENT_REALTIME, &one);
        }
	if(trace_start(trace) == 0)
	{
	    env.trace = trace;
	    ev_hdl->running = true;

	    mon_event(&env);

	    wand_event_run(ev_hdl);

	    if(trace_is_err(trace))
	    {
	        trace_perror(trace, "Error reading packets");
            sqlite3_close(db);
            trace_destroy(trace);
            return -1;
	    }
		/*libtrace_packet_t *packet = trace_create_packet();
		libtrace_eventobj_t event = trace_event(trace, packet);
		//int pack = trace_read_packet(trace, packet);
		if(pack < 0)
		{
			trace_perror(trace, "Packet read error");
		}
		while(pack > 0)
		{
			if(pack < 0)
			{
				trace_perror(trace, "Packet read error");
			}

			per_packet(packet);

			pack = trace_read_packet(trace, packet);

		}*/
	trace_destroy_packet(env.packet);
	}
	else
	{
	    //do some cleanup
        if(!useInterface) {
            //trace_destroy_filter(filter);
        }
		//trace_perror(trace, "Error starting trace");

        trace_err_t prob = trace_get_err(trace);


        sqlite3_close(db);
		trace_destroy(trace);
        logger(LOG_DAEMON | LOG_DEBUG, "Unknown Error: %s", prob.problem);
		return -1;
	}
        //do some cleanup
        	if(!useInterface) {
               trace_destroy_filter(filter);
              }
	trace_destroy(trace);
        sqlite3_close(db);
    if(prog)
        free(prog);
	     logger(LOG_DAEMON, "Shutting Down");
     	arg_freetable(argtable, sizeof(argtable)/sizeof(argtable[0]));
	wand_destroy_event_handler(env.wand_ev_hdl);
	return 0;
}

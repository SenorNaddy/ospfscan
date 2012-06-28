#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

/* A structure to hold our "global" variables */
typedef struct mon_environment {
	/* The event handler environment for the entire metering process */
	wand_event_handler_t *wand_ev_hdl;

	/* A file descriptor event - to be used when we're waiting on input
	 * from a live interface */
	struct wand_fdcb_t fd_cb;
	/* A timer event - to be used when we're waiting for the next packet
	 * to occur in a trace file replay */
	struct wand_timer_t timer;

	/* The trace we are reading packets from */
	libtrace_t *trace;
	/* The most recent packet we have read using libtrace */
	libtrace_packet_t *packet;
} mon_env_t;
//static void fd_read_event(struct wand_fdcb_t *evcb, enum wand_eventtype_t ev);
//static void timer_event(struct wand_timer_t *timer);
//static void mon_event(mon_env_t *env);
//static int process_mon_event(mon_env_t *env, libtrace_eventobj_t event);

#endif // EVENT_H_INCLUDED

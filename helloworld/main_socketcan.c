#include "CANopen.h"
#include "app_socketcan.h"
#include <sys/poll.h>
#include <sys/poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <string.h>
#include <sched.h>
#include <stdarg.h>
#include <assert.h>
#include <linux/can.h>
#include <linux/can/raw.h>

typedef void (*sa_sigaction_t)(int, siginfo_t *, void *);

#define TIMER_MS 1

int debug = 0;
const char *interface = "vcan0";
const char *progname = "";
uint64_t tick = 0;
int sighdlr = 1;
int nfd = 3;

void /* interrupt */ CO_TimerInterruptHandler(void);

int get_timerfd(int milliseconds)
{
    int timerfd = timerfd_create(CLOCK_MONOTONIC,0);
    struct itimerspec timspec;
    struct sched_param schedparm;

    memset(&schedparm, 0, sizeof(schedparm));
    schedparm.sched_priority = 1; // lowest rt priority
    sched_setscheduler(0, SCHED_FIFO, &schedparm);

    bzero(&timspec, sizeof(timspec));
    timspec.it_interval.tv_sec = 0;
    timspec.it_interval.tv_nsec = milliseconds * 1000000;
    timspec.it_value.tv_sec = 0;
    timspec.it_value.tv_nsec = 1;

    int res = timerfd_settime(timerfd, 0, &timspec, 0);
    if(res < 0) {
       perror("timerfd_settime:");
       return res;
    }
    return timerfd;
}

static void sigaction_handler(int sig, siginfo_t *si, void *uctx)
{
    LOG("signal %d - '%s' received, dumping core (current dir=%s)",
	sig, strsignal(sig), get_current_dir_name());
    exit(1);
}

int setup_signals(const sa_sigaction_t handler, ...)
{
    sigset_t sigmask;
    sigfillset(&sigmask);

    // SIGSEGV,SIGBUS,SIGILL,SIGFPE delivered via sigaction if handler given
    if (handler != NULL) {
	struct sigaction sig_act;
	sigemptyset( &sig_act.sa_mask );
	sig_act.sa_sigaction = handler;
	sig_act.sa_flags   = SA_SIGINFO;
	sigaction(SIGSEGV, &sig_act, (struct sigaction *) NULL);
	sigaction(SIGBUS,  &sig_act, (struct sigaction *) NULL);
	sigaction(SIGILL,  &sig_act, (struct sigaction *) NULL);
	sigaction(SIGFPE,  &sig_act, (struct sigaction *) NULL);

	// if they go through sigaction, block delivery through normal handler
	sigdelset(&sigmask, SIGSEGV);
	sigdelset(&sigmask, SIGBUS);
	sigdelset(&sigmask, SIGILL);
	sigdelset(&sigmask, SIGFPE);
	if (sigprocmask(SIG_SETMASK, &sigmask, NULL) == -1)
	    perror("sigprocmask");
    }

    // now explicitly turn on the signals delivered via  signalfd()
    // sigset of all the signals that we're interested in
    // these we want delivered via signalfd()
    int retval;
    retval = sigemptyset(&sigmask);        assert(retval == 0);

    va_list ap;
    int signo;
    va_start(ap, handler);
    do {
	signo = va_arg(ap, int);
	if (signo < 0 ) break;
	retval = sigaddset(&sigmask, signo);  assert(retval == 0);
    } while (1);
    va_end(ap);
    return signalfd(-1, &sigmask, 0);
}

void  dumpframe(const char *tag, const CO_CANrxMsg_t *cf)
{
    int i;
    fprintf(stderr, "%s ident=%d dlc=%d, data = ", tag, cf->ident, cf->DLC);
    for (i = 0; i < cf->DLC; i++)
	fprintf(stderr, "%2.2x ", cf->data[i]);
    fprintf(stderr, "\n");
}

static const char *option_string = "dG";
static struct option long_options[] = {
    {"debug", no_argument, 0, 'd'},
    {"nosighdlr",   no_argument,    0, 'G'},
    {0,0,0,0}
};

static void usage(const char *name)
{
    printf("Usage:  %s [options]\n", name);
    printf("Options are:\n"
	   "-d or --debug\n"
	   "    log to stderr"
	   "-G or --nosighdlr\n"
	   "    do not set up signal handlers (for debugging under gdb)\n");
}

int main (const int argc, char **argv)
{
    struct pollfd pfd[3];
    int cansocket, retval, opt, signal_fd, timer_fd;
    progname = argv[0];
    while ((opt = getopt_long(argc, argv, option_string,
			      long_options, NULL)) != -1) {
	switch(opt) {
	case 'd':
	    debug++;
	    break;
	case 'G':
	    sighdlr = 0;
	    nfd = 2;
	    break;
	default:
	    usage(progname);
	    exit(0);
	}
    }

    CO_NMT_reset_cmd_t reset = CO_RESET_NOT;

    int milliseconds = TIMER_MS; // atoi(argv[1]);

    /* Configure microcontroller. */

    /* initialize EEPROM */

    /* Application interface */
    if ((cansocket = programStart(interface)) < 0)
	exit(1);

    timer_fd = get_timerfd(milliseconds);

    pfd[0].fd = cansocket;
    pfd[0].events = POLLIN;

    pfd[1].fd = timer_fd;
    pfd[1].events = POLLIN;

    if (sighdlr) {
	signal_fd = setup_signals(sigaction_handler, SIGINT, SIGQUIT, SIGKILL, SIGTERM, -1);
	pfd[2].fd = signal_fd;
	pfd[2].events = POLLIN;
    }

    /* increase variable each startup. Variable is stored in EEPROM. */
    OD_powerOnCounter++;

    while(reset != CO_RESET_APP){
	/* CANopen communication reset - initialize CANopen objects *******************/
        CO_ReturnError_t err;
        uint16_t timer1msPrevious;

        /* disable timer and CAN interrupts */

        /* initialize CANopen */
        err = CO_init();
        if(err != CO_ERROR_NO){
	    LOG("CANopen init failed");
	    exit(1);
            /* CO_errorReport(CO->em, CO_EM_MEMORY_ALLOCATION_ERROR, CO_EMC_SOFTWARE_INTERNAL, err); */
        }

        /* initialize variables */

        reset = CO_RESET_NOT;
        /* Configure Timer interrupt function for execution every 1 millisecond */
        /* Configure CAN transmit and receive interrupt */
        /* Application interface */
        communicationReset();
        /* start CAN and enable interrupts */
        CO_CANsetNormalMode(ADDR_CAN1);

        while (reset == CO_RESET_NOT) {

	    retval = poll(pfd, nfd, -1);
	    if (!retval)
		continue;

	    // LOG("poll: %d", retval);

	    if (pfd[1].revents & POLLIN) {
		// LOG("tick");
		// a timer event is pending
		uint64_t expirations = 0;
		int res = read(pfd[1].fd, &expirations, sizeof(expirations));
		if(res < 0) {
		    perror("read timerfd:");
		    break;
		}
		tick += expirations;
		uint16_t timer1msDiff = (uint16_t) expirations;

		/* Application interface */
		programAsync(timer1msDiff);
		/* CANopen process */
		reset = CO_process(CO, timer1msDiff);
		/* Process EEPROM */
	    }

	    if (sighdlr && (pfd[2].revents & POLLIN)) {
		// a signal is pending
		struct signalfd_siginfo fdsi;
		ssize_t s;

		s = read(pfd[2].fd, &fdsi, sizeof(struct signalfd_siginfo));
		if (s != sizeof(struct signalfd_siginfo)) {
		    perror("read signalfd");
		}
		switch (fdsi.ssi_signo) {
		default:
		    LOG("signal %d - '%s' received",
			fdsi.ssi_signo,strsignal(fdsi.ssi_signo));
		}
		reset = CO_RESET_APP;
		continue;
	    }

	    if (pfd[0].revents & POLLIN) {
		// a socket frame is pending
		CO_CANrxMsg_t inframe;
		size_t fs = read(pfd[0].fd, &inframe, sizeof(inframe));
		if(fs && debug)
		    dumpframe("received: ", &inframe);

		CO_CANProcessRxFrame(CO->CANmodule[0], &inframe);
	    }
	    // CO_TimerInterruptHandler();

	    /* loop for normal program execution */
            // uint16_t timer1msDiff = timestamp_ms() - timer1msPrevious;

            /* CO_DISABLE_INTERRUPTS(); */
            /* timer1msDiff = CO_timer1ms - timer1msPrevious; */
            /* timer1msPrevious = CO_timer1ms; */
            /* CO_ENABLE_INTERRUPTS(); */

            /* Application interface */
            /* programAsync(timer1msDiff); */
            /* /\* CANopen process *\/ */
            /* reset = CO_process(CO, timer1msDiff); */
            /* Process EEPROM */
        }
    }


    /* program exit ***************************************************************/
    CO_DISABLE_INTERRUPTS();
    /* Application interface */
    programEnd(cansocket);
    /* delete objects from memory */
    CO_delete();
    /* reset */
    return 0;
}


/* timer interrupt function executes every millisecond ************************/
void /* interrupt */ CO_TimerInterruptHandler(void){

    /* clear interrupt flag */
    //CO_timer1ms++;
    CO_process_RPDO(CO);
    /* Application interface */
    program1ms();
    CO_process_TPDO(CO);
    /* verify timer overflow (is flag set again?) */
    if(0){
        CO_errorReport(CO->em, CO_EM_ISR_TIMER_OVERFLOW, CO_EMC_SOFTWARE_INTERNAL, 0U);
    }
}

#if 0
/* CAN interrupt function *****************************************************/
void /* interrupt */ CO_CAN1InterruptHandler(void){
    CO_CANinterrupt(CO->CANmodule[0]);


    /* clear interrupt flag */
}
#endif

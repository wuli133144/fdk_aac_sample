#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <libobject/attrib_priority.h>
#include <libobject/core/utils/args/cmd_args.h>
#include <libobject/libobject.h>
#include <libobject/test.h>

#define LIBRARY_VERSION "libobject version: 2.0.0.0"

#ifndef MAKELIB

typedef struct config_list_s {
} config_list_t;

typedef struct base_s {
    config_list_t config;
    args_processor_t *p;
} base_t;


static int args_process_help(void *base, int argc, char **argv)
{
    args_print_help_info(args_get_processor_globle_addr());
    exit(1);
    return 0;
}

static int args_process_ipaddr(void *base, int argc, char **argv)
{
    dbg_str(DBG_DETAIL, "args_process_ipaddr:%s", argv[0]);
    return 1;
}

static int args_process_port(void *base, int argc, char **argv)
{
    dbg_str(DBG_DETAIL, "args_process_port:%s", argv[0]);
    return 1;
}

static int args_process_log_server(void *base, int argc, char **argv)
{
    log_server();
    return 0;
}

static int args_process_mockery(void *base, int argc, char **argv)
{
    return mockery(argc, argv);
}

static cmd_config_t cmds[] = {
    {"mockery", args_process_mockery, 0, "app", "N/A", "test framework"}, 
    {"log_server", args_process_log_server, 0, "app", "N/A", "log server"}, 
    {"port", args_process_port, 1, "config", "NN(number)", "udp port, using to send/rcv msg with neighbor"}, 
    {"ip", args_process_ipaddr, 1, "config", "xx.xx.xx.xx", "ip addr, using to send/rcv msg with neighbor"}, 
    {"help", args_process_help, 0, "help", "N/A", "help info"}, 
    {NULL, NULL, 0, NULL, NULL, NULL}, 
};

/*
 * The libs used modules has been registered before main func, 
 * and debugger has been construct before main too. so can use
 * it derectly.
 */
int main(int argc, char *argv[])
{
    int ret = 0;

    /*
     *libobject_set_run_path("./");
     */
    libobject_init();
    /*
     *INIT_LIBOBJECT();
     */

    dbg_str(DBG_DETAIL, "main func start");

    args_process(NULL, cmds, argc, argv);

    dbg_str(DBG_DETAIL, "main func end");

    libobject_exit();

    while (1) ;
    return ret;
}
#endif

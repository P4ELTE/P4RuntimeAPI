#include <stdio.h>
#include "PI/proto/pi_server.h"
#include "device_mgr.h"

extern device_mgr_t *dev_mgr_ptr;

void dummy_callback(struct p4_ctrl_msg* ctrl_m) {
        printf("Dummy callback - table_name::%s\n", ctrl_m->table_name);
}


int
main (int argc, char **argv)
{
    dev_mgr_init_with_t4p4s(dev_mgr_ptr, dummy_callback);

    PIGrpcServerRun();

    PIGrpcServerWait();

}


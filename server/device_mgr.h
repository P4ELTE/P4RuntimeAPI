#ifndef __DEVICE_MGR_H__
#define __DEVICE_MGR_H__

#include <stdint.h>
#include "google/rpc/code.grpc-c.h"
#include "p4/v1/p4runtime.grpc-c.h"
#include "p4/config/v1/p4info.grpc-c.h"
#include "utils/map.h"
#include "handlers.h" /* T4P4S*/
#include "messages.h" /* T4P4S*/


typedef struct device_mgr_t {
	uint64_t device_id;
	/*TODO: ADD P4Info and other meta information */
	map_t id_map;
	p4_msg_callback cb;
} device_mgr_t;

typedef enum {
	P4IDS_TABLE = 0,
} p4_ids;


grpc_c_status_t table_insert(device_mgr_t *dm, P4__V1__TableEntry* table_entry);

grpc_c_status_t table_modify(device_mgr_t *dm, P4__V1__TableEntry* table_entry);

grpc_c_status_t table_delete(device_mgr_t *dm, P4__V1__TableEntry* table_entry); 

grpc_c_status_t table_write(device_mgr_t *dm, P4__V1__Update__Type update, P4__V1__TableEntry* table_entry);

grpc_c_status_t dev_mgr_write(device_mgr_t *dm, P4__V1__WriteRequest *request);

grpc_c_status_t dev_mgr_read(device_mgr_t *dm, P4__V1__ReadRequest *request);

grpc_c_status_t dev_mgr_set_pipeline_config(device_mgr_t *dm, P4__V1__SetForwardingPipelineConfigRequest__Action action, P4__V1__ForwardingPipelineConfig *config);

void dev_mgr_init(device_mgr_t *dm);
void dev_mgr_init_with_t4p4s(device_mgr_t *dm, p4_msg_callback cb);

#endif /* __DEVICE_MGR_H__ */

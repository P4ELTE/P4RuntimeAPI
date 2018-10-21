#ifndef __DEVICE_MGR_H__
#define __DEVICE_MGR_H__

#include <stdint.h>
#include "google/rpc/code.grpc-c.h"
#include "p4/v1/p4runtime.grpc-c.h"
#include "p4/config/v1/p4info.grpc-c.h"


typedef struct device_mgr_t {
	uint64_t device_id;
	/*TODO: ADD P4Info and other meta information */

} device_mgr_t;

typedef enum {
	P4IDS_TABLE = 0,
} p4_ids;


grpc_c_status_t table_insert(P4__V1__TableEntry* table_entry);

grpc_c_status_t table_modify(P4__V1__TableEntry* table_entry);

grpc_c_status_t table_delete(P4__V1__TableEntry* table_entry); 

grpc_c_status_t table_write(P4__V1__Update__Type update, P4__V1__TableEntry* table_entry);

grpc_c_status_t dev_mgr_write(P4__V1__WriteRequest *request);

grpc_c_status_t dev_mgr_read(P4__V1__ReadRequest *request);

grpc_c_status_t dev_mgr_set_pipeline_config(P4__V1__SetForwardingPipelineConfigRequest__Action action, P4__V1__ForwardingPipelineConfig *config);

#endif /* __DEVICE_MGR_H__ */

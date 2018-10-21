#include "device_mgr.h"
#include <stdio.h>
#include <arpa/inet.h>

grpc_c_status_t table_insert(P4__V1__TableEntry* table_entry) {
	uint32_t table_id;
	uint32_t field_id;
	uint32_t action_id;
	size_t i;
	uint16_t value16 = 0;
	grpc_c_status_t status;
	P4__V1__FieldMatch *match;
	P4__V1__TableAction *action;
	P4__V1__FieldMatch__Exact *exact;
	P4__V1__Action *tmp_act;
	P4__V1__Action__Param *param;


	table_id = table_entry->table_id;

	for (i=0;i<table_entry->n_match;i++) {
		match = table_entry->match[i];
		
		switch(match->field_match_type_case) {
			case P4__V1__FIELD_MATCH__FIELD_MATCH_TYPE_EXACT:
				exact = match->exact;
				field_id = match->field_id;
				if (exact->value.len>=2) value16 = ntohs(*(uint16_t*)(exact->value.data));
				printf("EXACT MATCH TableID:%d FieldID:%d KEY_LENGTH:%d VALUE16: %d  -- ", table_id, field_id, exact->value.len, value16); /* len - length , data - uint8_t* */
				status.gcs_code = GOOGLE__RPC__CODE__OK;
				break;
			default:
				status.gcs_code = GOOGLE__RPC__CODE__UNIMPLEMENTED;
				break;
		}
	}

	action = table_entry->action;

	switch(action->type_case) {
		case P4__V1__TABLE_ACTION__TYPE_ACTION:
			tmp_act = action->action;
			action_id = tmp_act->action_id;
			printf("ActionID: %d PARAMS:\n", action_id); 
			for (i=0;i<tmp_act->n_params;i++) {
				param = tmp_act->params[i];
				if (param->value.len>=2) value16 = ntohs(*(uint16_t*)(param->value.data));
				else value16 = 0;
				printf(" ( id: %d, vlen: %d, value16: %d )", param->param_id, param->value.len, value16);  
			}
			printf(" -\n");
			status.gcs_code = GOOGLE__RPC__CODE__OK;
			break;
		default:
			status.gcs_code = GOOGLE__RPC__CODE__UNIMPLEMENTED;
			break;
	}

        return status;
}

grpc_c_status_t table_modify(P4__V1__TableEntry* table_entry) {
	grpc_c_status_t status;
	status.gcs_code = GOOGLE__RPC__CODE__UNIMPLEMENTED;
	return status;
}

grpc_c_status_t table_delete(P4__V1__TableEntry* table_entry) {
        grpc_c_status_t status;
        status.gcs_code = GOOGLE__RPC__CODE__UNIMPLEMENTED;
        return status;
}

bool check_p4_id(uint32_t id, int type) {
	return true;
}

grpc_c_status_t table_write(P4__V1__Update__Type update, P4__V1__TableEntry* table_entry) {
	grpc_c_status_t status;
	if (!check_p4_id(table_entry->table_id, P4IDS_TABLE)) {
		status.gcs_code = GOOGLE__RPC__CODE__UNKNOWN;
		return status; /*TODO: more informative error msg is needed!!!*/
	}

	switch (update) {
		case P4__V1__UPDATE__TYPE__UNSPECIFIED:
			status.gcs_code = GOOGLE__RPC__CODE__UNKNOWN; 
			/*TODO: more informative error msg is needed!!!*/
	        	break;
		case P4__V1__UPDATE__TYPE__INSERT:
			return table_insert(table_entry);
		case P4__V1__UPDATE__TYPE__MODIFY:
			return table_modify(table_entry);
		case P4__V1__UPDATE__TYPE__DELETE:
			return table_delete(table_entry);
		default:
			status.gcs_code = GOOGLE__RPC__CODE__UNKNOWN; 
			/*TODO: more informative error msg is needed!!!*/
			break;
	}
	return status;
}


grpc_c_status_t dev_mgr_write(P4__V1__WriteRequest *request) {
	grpc_c_status_t status;
	size_t i;
	P4__V1__Entity *entity;

	if (request->atomicity != P4__V1__WRITE_REQUEST__ATOMICITY__CONTINUE_ON_ERROR) {
		status.gcs_code = GOOGLE__RPC__CODE__UNIMPLEMENTED;
		return status;
	}

	status.gcs_code = GOOGLE__RPC__CODE__OK;
	
	for (i=0;i<request->n_updates;i++) {
		entity = request->updates[i]->entity;
		switch(entity->entity_case) {
			case P4__V1__ENTITY__ENTITY_TABLE_ENTRY:
				status = table_write(request->updates[i]->type, entity->table_entry);
				break;
			default:
				status.gcs_code = GOOGLE__RPC__CODE__UNKNOWN;
				break;
		}
		/* TODO:collect multiple status messages - now we assume a simple update */
	}
	
	return status;
}

grpc_c_status_t dev_mgr_read(P4__V1__ReadRequest *request) {
        grpc_c_status_t status;
	return status;
}

grpc_c_status_t dev_mgr_set_pipeline_config(P4__V1__SetForwardingPipelineConfigRequest__Action action, P4__V1__ForwardingPipelineConfig *config) {
	grpc_c_status_t status;
	size_t i;
	P4__Config__V1__Table *table;
	P4__Config__V1__Action *taction;
	P4__Config__V1__Preamble *preamble;

	P4__Config__V1__P4Info *info = config->p4info;

	for (i=0;i<info->n_tables;++i) {
		table = info->tables[i];
		preamble = table->preamble;
		printf("TABLE id: %d name:%s\n", preamble->id, preamble->name); 
	}

	for (i=0;i<info->n_actions;++i) {
		taction = info->actions[i];
		preamble = taction->preamble;
		printf("ACTION id: %d name:%s\n", preamble->id, preamble->name); 
	}


	status.gcs_code = GOOGLE__RPC__CODE__OK;

	return status;
}


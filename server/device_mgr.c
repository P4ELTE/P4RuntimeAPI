#include "device_mgr.h"
#include <stdio.h>
#include <arpa/inet.h>

struct p4_field_match_header* _gen_match_rule_exact(argument_t *arg, P4__V1__FieldMatch__Exact *exact) {
	struct p4_field_match_exact *result = malloc(sizeof(struct p4_field_match_exact));

	strcpy(result->header.name, arg->name);
	result->header.type = P4_FMT_EXACT;
	result->length = arg->bitwidth;
	memcpy(result->bitmap, exact->value.data, exact->value.len);

	return (struct p4_field_match_header*)result; /* TODO: NTOH !!! */
}

struct p4_field_match_header* _gen_match_rule_lpm(argument_t *arg, P4__V1__FieldMatch__LPM *lpm) {
        struct p4_field_match_lpm *result = malloc(sizeof(struct p4_field_match_lpm));

        strcpy(result->header.name, arg->name);
        result->header.type = P4_FMT_LPM;
        result->prefix_length = lpm->prefix_len;
        memcpy(result->bitmap, lpm->value.data, lpm->value.len);

        return (struct p4_field_match_header*)result; /* TODO: NTOH !!! */
}

struct p4_field_match_header* _gen_match_rule_ternary(argument_t *arg, P4__V1__FieldMatch__Ternary *ternary) {
        struct p4_field_match_ternary *result = malloc(sizeof(struct p4_field_match_ternary));

        strcpy(result->header.name, arg->name);
        result->header.type = P4_FMT_TERNARY;
        result->length = arg->bitwidth;
        memcpy(result->bitmap, ternary->value.data, ternary->value.len);
	memcpy(result->mask, ternary->mask.data, ternary->mask.len);

        return (struct p4_field_match_header*)result; /* TODO: NTOH !!! */
}

struct p4_action_parameter* _gen_action_param(argument_t *arg, P4__V1__Action__Param *param) {
	struct p4_action_parameter *result = malloc(sizeof(struct p4_action_parameter));

	strcpy(result->name, arg->name);
	result->length = arg->bitwidth;
	memcpy(result->bitmap, param->value.data, param->value.len);

	return result; /* TODO: NTOH  */
}

grpc_c_status_t table_insert(device_mgr_t *dm, P4__V1__TableEntry* table_entry) {
	uint32_t table_id;
	uint32_t field_id;
	uint32_t action_id;
	uint16_t value16; /* TODO: remove after testing */
	size_t i;
	int32_t prefix_len = 0; /* in bits */

	grpc_c_status_t status;
	P4__V1__FieldMatch *match;
	P4__V1__TableAction *action;
	P4__V1__FieldMatch__Exact *exact;
	P4__V1__FieldMatch__LPM *lpm;
	P4__V1__FieldMatch__Ternary *ternary;
	
	P4__V1__Action *tmp_act;
	P4__V1__Action__Param *param;
	
	table_id = table_entry->table_id;

	element_t *elem = get_element(&(dm->id_map), table_id);
	argument_t *arg = NULL;

	struct p4_ctrl_msg ctrl_m;
	ctrl_m.num_field_matches = 0;
	ctrl_m.num_action_params = 0;
	ctrl_m.type = P4T_ADD_TABLE_ENTRY;
	ctrl_m.table_name = strdup(elem->value);

	for (i=0;i<table_entry->n_match;i++) {
		match = table_entry->match[i];
		field_id = match->field_id;
		arg = get_argument(elem, field_id);
		if (arg==NULL) {
			printf("NULL ARGUMENT for FIELD_ID=%d\n", field_id);
		}
		switch(match->field_match_type_case) {
			case P4__V1__FIELD_MATCH__FIELD_MATCH_TYPE_EXACT:
				exact = match->exact;
				ctrl_m.field_matches[ctrl_m.num_field_matches] = _gen_match_rule_exact(arg, exact);
				ctrl_m.num_field_matches++;	
				status.gcs_code = GOOGLE__RPC__CODE__OK;
				break;
			case P4__V1__FIELD_MATCH__FIELD_MATCH_TYPE_LPM:
				lpm = match->lpm;
				prefix_len = lpm->prefix_len;
                                if (lpm->value.len>=2) value16 = ntohs(*(uint16_t*)(lpm->value.data));
				printf("LPM MATCH TableID:%:%d (%s) FieldID:%d (%s) KEY_LENGTH:%d VALUE16: %d PREFIX_LEN: %d  -- \n", table_id, elem->value, field_id, arg->name, lpm->value.len, value16, prefix_len);
				ctrl_m.field_matches[ctrl_m.num_field_matches] = _gen_match_rule_lpm(arg, lpm);
				ctrl_m.num_field_matches++;
				status.gcs_code = GOOGLE__RPC__CODE__OK;
				break;
			case P4__V1__FIELD_MATCH__FIELD_MATCH_TYPE_TERNARY:
				ternary = match->ternary;
				printf("TERNARY MATCH TableID:%d (%s) FieldID:%d (%s) KEY_LENGTH:%d VALUE16: %d M_LEN:%d MASK:%d  --\n", table_id, elem->value, field_id, arg->name, ternary->value.len, ternary->value.data[0], ternary->mask.len, ternary->mask.data[0]); /* len - length , data - uint8_t* */
				ctrl_m.field_matches[ctrl_m.num_field_matches] = _gen_match_rule_ternary(arg, ternary);
				ctrl_m.num_field_matches++;
                                status.gcs_code = GOOGLE__RPC__CODE__OK;
                                break;

			case P4__V1__FIELD_MATCH__FIELD_MATCH_TYPE_RANGE:	
			default:
				status.gcs_code = GOOGLE__RPC__CODE__UNIMPLEMENTED;
				break;
		}
	}

	if (table_entry->has_is_default_action && table_entry->is_default_action) { /* n_match is 0 in this case */
		ctrl_m.type = P4T_SET_DEFAULT_ACTION;
	}

	action = table_entry->action;

	switch(action->type_case) {
		case P4__V1__TABLE_ACTION__TYPE_ACTION:
			ctrl_m.action_type = P4_AT_ACTION; /* ACTION PROFILE IS NOT SUPPORTED */
			tmp_act = action->action;
			action_id = tmp_act->action_id;
			elem = get_element(&(dm->id_map), action_id);
			for (i=0;i<tmp_act->n_params;i++) {
				param = tmp_act->params[i];
				arg = get_argument(elem, param->param_id);
				ctrl_m.action_params[ctrl_m.num_action_params] = _gen_action_param(arg, param);
				ctrl_m.num_action_params++;
			}
			status.gcs_code = GOOGLE__RPC__CODE__OK;
			break;
		default:
			status.gcs_code = GOOGLE__RPC__CODE__UNIMPLEMENTED;
			break;
	}

	if (status.gcs_code == GOOGLE__RPC__CODE__OK) {
		dm->cb(&ctrl_m);
	}

        return status;
}

grpc_c_status_t table_modify(device_mgr_t *dm, P4__V1__TableEntry* table_entry) {
	grpc_c_status_t status;
	status.gcs_code = GOOGLE__RPC__CODE__UNIMPLEMENTED;
	return status;
}

grpc_c_status_t table_delete(device_mgr_t *dm, P4__V1__TableEntry* table_entry) {
        grpc_c_status_t status;
        status.gcs_code = GOOGLE__RPC__CODE__UNIMPLEMENTED;
        return status;
}

bool check_p4_id(uint32_t id, int type) {
	return true;
}

grpc_c_status_t table_write(device_mgr_t *dm, P4__V1__Update__Type update, P4__V1__TableEntry* table_entry) {
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
			return table_insert(dm, table_entry);
		case P4__V1__UPDATE__TYPE__MODIFY:
			return table_modify(dm, table_entry);
		case P4__V1__UPDATE__TYPE__DELETE:
			return table_delete(dm, table_entry);
		default:
			status.gcs_code = GOOGLE__RPC__CODE__UNKNOWN; 
			/*TODO: more informative error msg is needed!!!*/
			break;
	}
	return status;
}


grpc_c_status_t dev_mgr_write(device_mgr_t *dm, P4__V1__WriteRequest *request) {
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
				status = table_write(dm, request->updates[i]->type, entity->table_entry);
				break;
			default:
				status.gcs_code = GOOGLE__RPC__CODE__UNKNOWN;
				break;
		}
		/* TODO:collect multiple status messages - now we assume a simple update */
	}
	
	return status;
}

grpc_c_status_t dev_mgr_read(device_mgr_t *dm, P4__V1__ReadRequest *request) {
        grpc_c_status_t status;
	return status;
}

grpc_c_status_t dev_mgr_set_pipeline_config(device_mgr_t *dm, P4__V1__SetForwardingPipelineConfigRequest__Action action, P4__V1__ForwardingPipelineConfig *config) {
	grpc_c_status_t status;
	size_t i,j;
	P4__Config__V1__Table *table;
	P4__Config__V1__Action *taction;
	P4__Config__V1__Preamble *preamble;
	P4__Config__V1__MatchField *mf;
	P4__Config__V1__Action__Param *param;
	element_t *elem;

	P4__Config__V1__P4Info *info = config->p4info;

	printf("P4Info configuration received...\n");
	for (i=0;i<info->n_tables;++i) {
		table = info->tables[i];
		preamble = table->preamble;
		printf("  [+] TABLE id: %d; name: %s\n", preamble->id, preamble->name);
		elem = add_element(&(dm->id_map), preamble->id, preamble->name);
		if (elem == NULL) {
			printf("   +-----> ERROR\n");
			break;
		}
		for (j=0;j<table->n_match_fields;++j) {
			mf = table->match_fields[j];
			printf("   +-----> MATCH FIELD; name: %s; id: %d; bitwidth: %d\n", mf->name, mf->id, mf->bitwidth);
			strcpy(elem->args[elem->n_args].name, mf->name);
			elem->args[elem->n_args].id = mf->id;
			elem->args[elem->n_args].bitwidth = mf->bitwidth;
			elem->n_args++;
		}
	}

	for (i=0;i<info->n_actions;++i) {
		taction = info->actions[i];
		preamble = taction->preamble;
		printf("  [#] ACTION id: %d; name: %s\n", preamble->id, preamble->name); 
		elem = add_element(&(dm->id_map), preamble->id, preamble->name);
		for (j=0;j<taction->n_params;++j) {
			param = taction->params[j];
			printf("   #-----> ACTION PARAM; name: %s; id: %d; bitwidth: %d\n", param->name, param->id, param->bitwidth);
			strcpy(elem->args[elem->n_args].name, param->name);
                        elem->args[elem->n_args].id = param->id;
                        elem->args[elem->n_args].bitwidth = param->bitwidth;
                        elem->n_args++;
		}
	}

	status.gcs_code = GOOGLE__RPC__CODE__OK;

	return status;
}

void dev_mgr_init(device_mgr_t *dm) {
	init_map(&(dm->id_map));
}

void dev_mgr_init_with_t4p4s(device_mgr_t *dm, p4_msg_callback cb) {
	dev_mgr_init(dm);
	dm->cb = cb;
}

#!/usr/bin/env python2
import argparse
import os
from time import sleep

import p4runtime_lib.bmv2
import p4runtime_lib.helper


def dmac_add_entry(p4info_helper, sw, macAddr, output_port):
    print "- ADD PORT ENTRY TO TABLE DMAC -"
    table_entry = p4info_helper.buildTableEntry(
            table_name="dmac",
            match_fields={
                "ethernet.dstAddr": macAddr
            },
            action_name="forward",
            action_params={
                "port": output_port
            })
    sw.WriteTableEntry(table_entry)

def smac_add_entry(p4info_helper, sw, macAddr):
    print "- ADD PORT ENTRY TO TABLE SMAC -"
    table_entry = p4info_helper.buildTableEntry(
            table_name="smac",
            match_fields={
                "ethernet.srcAddr": macAddr
            },
            action_name="_nop",
            action_params={})
    sw.WriteTableEntry(table_entry)

def dmac_set_default_action(p4info_helper, sw):
    print "- SET DEFAULT ENTRY OF TABLE DMAC -"
    table_entry = p4info_helper.buildTableEntry(
            table_name="dmac",
            action_name="bcast")
    table_entry.is_default_action = True
    sw.WriteTableEntry(table_entry)

def smac_set_default_action(p4info_helper, sw):
    print "- SET DEFAULT ENTRY OF TABLE SMAC -"
    table_entry = p4info_helper.buildTableEntry(
            table_name="smac",
            action_name="mac_learn")
    table_entry.is_default_action = True
    sw.WriteTableEntry(table_entry)


def main(p4info_file_path, bmv2_file_path, ip, port):
        p4info_helper = p4runtime_lib.helper.P4InfoHelper(p4info_file_path)

        print '\n----- Connecting to switch at {}:{} -----'.format(ip, port)
        s = p4runtime_lib.bmv2.Bmv2SwitchConnection('s1', address='{}:{}'.format(ip,port), device_id=0)
        print '[DONE]\n'

        print '\n----- Installing P4 program----'
        s.SetForwardingPipelineConfig(p4info=p4info_helper.p4info, bmv2_json_file_path=bmv2_file_path)
        print '[DONE]\n'

        dmac_add_entry(p4info_helper, s, "aa:bb:cc:aa:bb:11", 2)
        dmac_add_entry(p4info_helper, s, "a2:1b:11:a2:b2:12", 42)

        smac_add_entry(p4info_helper, s, "a2:1b:11:a2:b2:12")
        smac_add_entry(p4info_helper, s, "aa:bb:cc:aa:bb:11")

        dmac_set_default_action(p4info_helper, s)
        smac_set_default_action(p4info_helper, s)

        print "\n"


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='P4Runtime Controller')
    parser.add_argument('--p4info', help='p4info proto in text format from p4c',
                        type=str, action="store", required=True)
    parser.add_argument('--bmv2-json', help='BMv2 JSON file from p4c',
                        type=str, action="store", required=True)
    parser.add_argument('--ip',help='Control IP address of the P4 switch (default: 127.0.0.1)',
                                        type=str, action="store",required=False,
                                        default='127.0.0.1')
    parser.add_argument('--port',help='Control port of the P4 switch (default : 50051)',
                                        type=str, action="store",required=False,
                                        default=50051)
    args = parser.parse_args()

    if not os.path.exists(args.p4info):
        parser.print_help()
        print "\np4info file not found: %s\nHave you run 'make'?" % args.p4info
        parser.exit(1)
    if not os.path.exists(args.bmv2_json):
        parser.print_help()
        print "\nBMv2 JSON file not found: %s\nHave you run 'make'?" % args.bmv2_json
        parser.exit(1)

    main(args.p4info, args.bmv2_json, args.ip, args.port)

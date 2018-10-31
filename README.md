# P4Runtime API for T4P4S switches

P4Runtime support od T4P4S is under development. This repo will contain the source files needed for the interaction with P4Runtime-capable control planes. Currently, the source codes in this repo are experimental!!!

## Dependencies

* grpc-c:  https://github.com/Juniper/grpc-c
* P4 PI: https://github.com/p4lang/PI
* T4P4S (only 2 header files are needed): https://github.com/P4ELTE/t4p4s

P4 PI was configured with the following arguments:
```
./configure --with-proto --without-internal-rpc --without-cli
```

## P4Runtime GRPC-C stubs
After you managed to compile P4 PI, all the proto files needed are generated.
Update the PLUGIN, OUTDIR and PSRC variables in  ./install.sh, and execute it. The P4Runtime stub code is then generated into the folder OUTDIR.

## Compilation of source files and an example server
Update the GRPCDIR and T4P4SDIR in ./compile.sh and execute it:
```
./compile.sh
```

For debugging (with gdb) you can use "-D" option: 
```
./compile.sh -D
```
## Run the example server
Currently pi_server is bound to 172.17.0.1:50051 (the docker if for testing purposes)
```
sudo ./pi_server
```

## L2FWD Controller example
First the p4runtime and IR json files should be generated from the P4 description:
```
p4c-bm2-ss --p4v 14 --p4runtime-file l2fwd14.p4runtime --p4runtime-format text --toJSON l2fwd14.json  t4p4s/examples/l2fwd.p4_14
```

After that we can execute the controller:
```
python l2.py --p4info l2fwd14.p4runtime --bmv2-json l2fwd14.json --ip 172.17.0.1
```

## L3FWD Controller example
First the p4runtime and IR json files should be generated from the P4 description:
```
p4c-bm2-ss --p4v 14 --p4runtime-file l3fwd14.p4runtime --p4runtime-format text --toJSON l3fwd14.json  t4p4s/examples/l3fwd-with-chksm.p4_14
```

After that we can execute the controller:
```
python l3.py --p4info l3fwd14.p4runtime --bmv2-json l3fwd14.json --ip 172.17.0.1
```

# P4Runtime API for T4P4S switches

P4Runtime support od T4P4S is under development. This repo will contain the source files needed for the interaction with P4Runtime-capable control planes. Currently, the source codes in this repo are experimental!!!

## Dependencies

* grpc-c:  https://github.com/Juniper/grpc-c
* P4 PI: https://github.com/p4lang/PI

P4 PI was configured with the following arguments:
./configure --with-proto --without-internal-rpc --without-cli

## P4Runtime GRPC-C stubs
After you managed to compile P4 PI, all the proto files needed are generated.
Update the PLUGIN, OUTDIR and PSRC variables in  ./install.sh, and execute it. The P4Runtime stub code is then generated into the folder OUTDIR.

## Compilation of source files and an example server
Update the GRPCDIR in ./compile.sh and execute it.

## Run the example server
Currently pi_server is bound to 172.17.0.1:50051 (the docker if for testing purposes)
sudo ./pi_server



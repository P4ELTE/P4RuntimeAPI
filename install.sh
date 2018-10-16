#!/bin/bash

OUTDIR=./grpc-c-out
PLUGIN=../grpc-c/build/compiler/protoc-gen-grpc-c
PSRC=../PI/proto

read -r -d '' PROTOS <<- EOM
${PSRC}/p4/v1/p4data.proto
${PSRC}/p4/v1/p4runtime.proto
${PSRC}/p4/config/v1/p4info.proto
${PSRC}/p4/config/v1/p4types.proto
${PSRC}/google/rpc/status.proto
${PSRC}/google/rpc/code.proto
${PSRC}/p4/tmp/p4config.proto
${PSRC}/gnmi/gnmi.proto
EOM

mkdir -p $OUTDIR

for protofile in $PROTOS
do
	echo Processing: ${protofile}
	protoc -I $PSRC --grpc-c_out=${OUTDIR} --plugin=protoc-gen-grpc-c=${PLUGIN} ${protofile}
done

protoc -I ../grpc-c/third_party/protobuf/src --grpc-c_out=${OUTDIR} --plugin=protoc-gen-grpc-c=${PLUGIN} ../grpc-c/third_party/protobuf/src/google/protobuf/any.proto

protoc -I ../grpc-c/third_party/protobuf/src --grpc-c_out=${OUTDIR} --plugin=protoc-gen-grpc-c=${PLUGIN} ../grpc-c/third_party/protobuf/src/google/protobuf/descriptor.proto

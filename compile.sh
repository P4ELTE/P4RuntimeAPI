#!/bin/bash

DEBUG=""

if [ "$1" = "-D" ];then
	DEBUG="-g"
fi

GRPCDIR="../grpc-c/build"

T4P4SDIR="../t4p4s"

#LFLAGS="${GRPCDIR}/lib/libgrpc-c.la -lgrpc -lgpr -lprotobuf-c -lpthread"
LFLAGS="-lgrpc -lgpr -lprotobuf-c -lpthread"



IFLAGS="-I. -I${GRPCDIR}/lib/h/ -I${GRPCDIR}/third_party/protobuf-c -I${GRPCDIR}/third_party/grpc/include -I./grpc-c-out/ -I./server/ -I../PI/proto/server -I${T4P4SDIR}/src/hardware_dep/shared/ctrl_plane"

PROTO_SOURCES="grpc-c-out/p4/tmp/p4config.grpc-c.service.c grpc-c-out/p4/tmp/p4config.grpc-c.c grpc-c-out/p4/v1/p4data.grpc-c.service.c grpc-c-out/p4/v1/p4runtime.grpc-c.service.c grpc-c-out/p4/v1/p4data.grpc-c.c grpc-c-out/p4/v1/p4runtime.grpc-c.c grpc-c-out/p4/config/v1/p4types.grpc-c.c grpc-c-out/p4/config/v1/p4types.grpc-c.service.c grpc-c-out/p4/config/v1/p4info.grpc-c.service.c grpc-c-out/p4/config/v1/p4info.grpc-c.c grpc-c-out/google/rpc/status.grpc-c.service.c grpc-c-out/google/rpc/status.grpc-c.c grpc-c-out/google/rpc/code.grpc-c.c grpc-c-out/google/rpc/code.grpc-c.service.c grpc-c-out/gnmi/gnmi.grpc-c.c grpc-c-out/gnmi/gnmi.grpc-c.service.c grpc-c-out/google/protobuf/any.grpc-c.c grpc-c-out/google/protobuf/descriptor.grpc-c.c"

mkdir -p obj

for src in $PROTO_SOURCES
do
	echo Compiling $src
	obj=`echo $src | sed 's/c$/o/' | sed 's/^.*\///g'`
	gcc ${DEBUG} ${IFLAGS} -c -o ./obj/${obj} ${src}
done

gcc ${DEBUG} ${IFLAGS} -c -o obj/pi_server.o server/pi_server.c

gcc ${DEBUG} ${IFLAGS} -c -o obj/device_mgr.o server/device_mgr.c

gcc ${DEBUG} ${IFLAGS} -c -o obj/map.o utils/map.c

gcc ${DEBUG} ${IFLAGS} -o pi_server server/test_server.c obj/*.o ${GRPCDIR}/lib/.libs/libgrpc-c.so ${LFLAGS}

mkdir -p static_lib
ar rcs static_lib/libp4rt.a obj/*.o

mkdir -p include
mkdir -p include/p4rt
mkdir -p include/utils
cp server/device_mgr.h include/p4rt/
cp server/config.h include/
cp utils/map.h include/utils/


echo "Static library is available in folder ./static_lib"
echo "Usage example:"
echo "gcc ${IFLAGS} server/test_server.c -L./static_lib -lp4rt -L${GRPCDIR}/lib/.libs -lgrpc-c ${LFLAGS} -o test_server"


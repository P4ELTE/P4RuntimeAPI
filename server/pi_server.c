#include "google/rpc/code.grpc-c.h"
#include "p4/v1/p4runtime.grpc-c.h"
#include "uint128.h"
#include <stdio.h>
#include <signal.h>
#include "PI/proto/pi_server.h"
#include "device_mgr.h"

void gnmi__g_nmi__capabilities_cb (grpc_c_context_t *context)
{
	 printf("gnmi__g_nmi__capabilities_cb\n");
}

void gnmi__g_nmi__get_cb (grpc_c_context_t *context)
{
	printf("gnmi__g_nmi__get_cb\n");
}

void gnmi__g_nmi__set_cb (grpc_c_context_t *context)
{
	printf("gnmi__g_nmi__set_cb");
}

void gnmi__g_nmi__subscribe_cb (grpc_c_context_t *context)
{
	printf("gnmi__g_nmi__set_cb");
}

void p4__v1__p4_runtime__write_cb (grpc_c_context_t *context)
{
    P4__V1__WriteRequest *request;

    printf("P4Runtime Write\n");

    /*
     * Read incoming message into request
     */
    if (context->gcc_stream->read(context, (void **)&request, 0, -1)) {
        printf("Failed to read data from client\n");
        exit(1);
    }

    if (request->has_device_id) {
        printf("device id: %d\n", request->device_id);
    }

    grpc_c_status_t status;
    status = dev_mgr_write( request );

    /*
     * Finish response for RPC
     */
    if (context->gcc_stream->finish(context, &status, 0)) {
        printf("Failed to write status\n");
        exit(1);
    }
}

void p4__v1__p4_runtime__read_cb (grpc_c_context_t *context)
{
    P4__V1__ReadRequest *request;

    printf("P4Runtime Read\n");

    /*
     * Read incoming message into request
     */
    if (context->gcc_stream->read(context, (void **)&request, 0, -1)) {
        printf("Failed to read data from client\n");
        exit(1);
    }

    if (request->has_device_id) {
        printf("device id: %d\n", request->device_id);
    }


    /*
     * Create a reply
     */
    P4__V1__ReadResponse r;
    p4__v1__ReadResponse__init(&r);

/*    char buf[1024];
    buf[0] = '\0';
    snprintf(buf, 1024, "hello, ");
    strcat(buf, h->name);
    r.message = buf;
*/


    /*
     * Write reply back to the client
     */
    if (!context->gcc_stream->write(context, &r, 0, -1)) {
        printf("Wrote ReadResponse to %s\n", grpc_c_get_client_id(context));
    } else {
        printf("Failed to write\n");
        exit(1);
    }

    grpc_c_status_t status;
    status.gcs_code = 0;

    /*
     * Finish response for RPC
     */
    if (context->gcc_stream->finish(context, &status, 0)) {
        printf("Failed to write status\n");
        exit(1);
    }

}


void p4__v1__p4_runtime__set_forwarding_pipeline_config_cb (grpc_c_context_t *context)
{
	printf("set_forwarding_pipeline_config\n");
}

void p4__v1__p4_runtime__get_forwarding_pipeline_config_cb (grpc_c_context_t *context)
{
	printf("get_forwarding_pipeline_config\n");
}

void p4__v1__p4_runtime__stream_channel_cb (grpc_c_context_t *context)
{
	printf("stream_channel\n");
}


static grpc_c_server_t *p4runtime_server;

static void sigint_handler (int x) {
    grpc_c_server_destroy(p4runtime_server);
    exit(0);
}


void PIGrpcServerRunAddrGnmi(const char *server_address, void *gnmi_service) 
{
    signal(SIGINT, sigint_handler);

    /*
     * Initialize grpc-c library to be used with vanilla gRPC
     */
    grpc_c_init(GRPC_THREADS, NULL);

    /*
     * Create server object
     */
    p4runtime_server = grpc_c_server_create_by_host(server_address, NULL, NULL);
    if (p4runtime_server == NULL) {
        printf("Failed to create server\n");
        exit(1);
    }

    /*grpc_c_server_add_insecure_http2_port(p4runtime_server, server_address);*/

    /*
     * Initialize greeter service
     */
    p4__v1__p4_runtime__service_init(p4runtime_server);

    /*
     * Start server
     */
    grpc_c_server_start(p4runtime_server);

}

void PIGrpcServerRunAddr(const char *server_address) {
  PIGrpcServerRunAddrGnmi(server_address, 0);
}

void PIGrpcServerRun() {
  PIGrpcServerRunAddrGnmi("172.17.0.1:50051", 0);
}

int PIGrpcServerGetPort() {
  return 50051;
}

uint64_t PIGrpcServerGetPacketInCount(uint64_t device_id) {
  return 0;
}

uint64_t PIGrpcServerGetPacketOutCount(uint64_t device_id) {
  return 0;
}

void PIGrpcServerWait() {
  grpc_c_server_wait(p4runtime_server);
}

void PIGrpcServerShutdown() {
  grpc_c_server_destroy(p4runtime_server);
}

void PIGrpcServerForceShutdown(int deadline_seconds) {
/*  using clock = std::chrono::system_clock;
  auto deadline = clock::now() + std::chrono::seconds(deadline_seconds);
  server_data->server->Shutdown(deadline);
*/
  grpc_c_server_destroy(p4runtime_server);
}

void PIGrpcServerCleanup() {
  /*delete server_data;*/
}

int
main (int argc, char **argv)
{

    PIGrpcServerRun();

    PIGrpcServerWait();

}


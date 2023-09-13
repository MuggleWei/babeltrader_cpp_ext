#include <memory>
#include "muggle/cpp/muggle_cpp.h"
#include "babeltrader/cpp/ext/babeltrader_cpp_ext.h"
#include "demo_msg.h"
#include "tcp_client_session.h"
#include "tcp_client_handle.h"

typedef struct sys_args {
	char host[64];
	char port[16];
	char ca[MUGGLE_MAX_PATH];
} sys_args_t;

char *get_certs_dir(char *buf, size_t bufsize)
{
	char buf1[MUGGLE_MAX_PATH] = {};
	char buf2[MUGGLE_MAX_PATH] = {};
	if (muggle_os_process_path(buf1, sizeof(buf1)) != 0) {
		LOG_ERROR("failed get process path");
		return nullptr;
	}
	if (muggle_path_dirname(buf1, buf2, sizeof(buf2)) != 0) {
		LOG_ERROR("failed get process dir");
		return nullptr;
	}
	if (muggle_path_dirname(buf2, buf1, sizeof(buf1)) != 0) {
		LOG_ERROR("failed get project root dir");
		return nullptr;
	}
	if (muggle_path_join(buf1, "certs", buf, bufsize) != 0) {
		LOG_ERROR("failed join path");
		return nullptr;
	}

	return buf;
}

bool parse_sys_args(int argc, char **argv, sys_args_t *args)
{
	char str_usage[1024];
	snprintf(str_usage, sizeof(str_usage),
			 "Usage: %s -H <host> -P <port>\n"
			 "\t-h, --help  print help information\n"
			 "\t-H, --host  listen/connect host\n"
			 "\t-P, --port  listen/connect port"
			 "\t-c, --CA    CA filepath\n"
			 "",
			 argv[0]);
	memset(args, 0, sizeof(*args));

	static struct option long_options[] = {
		{ "help", no_argument, NULL, 'h' },
		{ "host", required_argument, NULL, 'H' },
		{ "port", required_argument, NULL, 'P' },
		{ "CA", required_argument, NULL, 'c' },
		{ NULL, 0, NULL, 0 }
	};

	while (true) {
		int c = 0;
		int option_index = 0;
		c = getopt_long(argc, argv, "hH:P:c:", long_options, &option_index);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'h': {
			LOG_INFO("%s", str_usage);
			exit(EXIT_SUCCESS);
		} break;
		case 'H': {
			strncpy(args->host, optarg, sizeof(args->host) - 1);
		} break;
		case 'P': {
			strncpy(args->port, optarg, sizeof(args->port) - 1);
		} break;
		case 'c': {
			strncpy(args->ca, optarg, sizeof(args->ca) - 1);
		} break;
		}
	}

	if (args->host[0] == '\0') {
		LOG_WARNING("run without 'host', set 127.0.0.1 by default");
		strncpy(args->host, "127.0.0.1", sizeof(args->host) - 1);
	}
	if (args->port[0] == '\0') {
		LOG_WARNING("run without 'port', set 8080 by default");
		strncpy(args->port, "8080", sizeof(args->port) - 1);
	}
	if (args->ca[0] == '\0') {
		LOG_WARNING("run without 'ca', "
					"set '$ORIGIN/../certs/ca.crt' by default");
		char buf[MUGGLE_MAX_PATH] = {};
		if (!get_certs_dir(buf, sizeof(buf))) {
			LOG_ERROR("failed get 'certs' dir");
			return false;
		}
		if (muggle_path_join(buf, "ca.crt", args->ca, sizeof(args->ca)) != 0) {
			LOG_ERROR("failed join path: %s, %s", buf, "ca.crt");
			return false;
		}
	}

	LOG_INFO("\n"
			 "----- input args -----\n"
			 "host=%s\n"
			 "port=%s\n"
			 "ca=%s\n"
			 "----------------------",
			 args->host, args->port, args->ca);

	return true;
}

#define REGISTER_CALLBACK(msg_id, funcname) \
	dispatcher.RegisterCallback(msg_id, TcpClientSession::s_##funcname);
void register_callbacks(Dispatcher &dispatcher)
{
	REGISTER_CALLBACK(DEMO_MSG_ID_PONG, OnPong);
	REGISTER_CALLBACK(DEMO_MSG_ID_RSP_LOGIN, OnRspLogin);
	REGISTER_CALLBACK(DEMO_MSG_ID_RSP_SUM, OnRspSum);
}

void run_tcp_client(sys_args_t *args)
{
	// register callbacks
	Dispatcher dispatcher;
	dispatcher.SetTotalMessage(MAX_DEMO_MSG_ID);
	register_callbacks(dispatcher);

	// codec
	MsgToBytesEncoder encoder;
	BytesToMsgDecoder decoder;
	CodecChain codec_chain;
	codec_chain.Append(&encoder);
	codec_chain.Append(&decoder);

	// ssl
	LOG_INFO("setup ssl context");
	std::unique_ptr<SSLContextFactory> factory(NewSSLContextFactory());
	if (!factory) {
		LOG_ERROR("failed new ssl context factory");
		return;
	}
	std::shared_ptr<SSLContext> ssl_ctx(factory->NewClientContext(args->ca));
	if (!ssl_ctx) {
		LOG_ERROR("failed new ssl client context");
		return;
	}

	// evloop
	NetEventLoop evloop(8, 0);

	// client handle
	TcpClientHandle handle;
	handle.SetServerAddr(args->host, args->port);
	handle.SetEventLoop(&evloop);
	handle.SetCodecChain(&codec_chain);
	handle.SetDispatcher(&dispatcher);
	handle.SetSSLContext(ssl_ctx.get());

	evloop.SetHandle(&handle);
	evloop.SetTimerInterval(3000);

	handle.Connect();

	evloop.Run();
}

int main(int argc, char *argv[])
{
	if (!muggle::Log::ComplicatedInit(LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG, NULL)) {
		fprintf(stderr, "failed init log");
		exit(EXIT_FAILURE);
	}

	if (muggle_socket_lib_init() != 0) {
		LOG_ERROR("failed initalize socket library");
		exit(EXIT_FAILURE);
	}

	srand(time(NULL));

	sys_args_t args;
	if (!parse_sys_args(argc, argv, &args)) {
		LOG_ERROR("failed parse input arguments");
		exit(EXIT_FAILURE);
	}

	run_tcp_client(&args);

	return 0;
}

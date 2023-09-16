#include <memory>
#include "muggle/cpp/muggle_cpp.h"
#include "babeltrader/cpp/ext/babeltrader_cpp_ext.h"
#include "demo_msg.h"
#include "tcp_server_session.h"
#include "tcp_server_handle.h"

typedef struct sys_args {
	char host[64];
	char port[16];
	char key[MUGGLE_MAX_PATH];
	char crt[MUGGLE_MAX_PATH];
	char passphrase[64];
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
			 "\t-P, --port  listen/connect port\n"
			 "\t-c, --cert  ssl certificate filepath\n"
			 "\t-k, --key   ssl key filepath\n"
			 "\t-p, --pass  ssl key passphrase\n"
			 "",
			 argv[0]);
	memset(args, 0, sizeof(*args));

	static struct option long_options[] = {
		{ "help", no_argument, NULL, 'h' },
		{ "host", required_argument, NULL, 'H' },
		{ "port", required_argument, NULL, 'P' },
		{ "cert", required_argument, NULL, 'c' },
		{ "key", required_argument, NULL, 'k' },
		{ "pass", required_argument, NULL, 'p' },
		{ NULL, 0, NULL, 0 }
	};

	while (true) {
		int c = 0;
		int option_index = 0;
		c = getopt_long(argc, argv, "hH:P:c:k:p:", long_options, &option_index);
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
			strncpy(args->crt, optarg, sizeof(args->crt) - 1);
		} break;
		case 'k': {
			strncpy(args->key, optarg, sizeof(args->key) - 1);
		} break;
		case 'p': {
			strncpy(args->passphrase, optarg, sizeof(args->passphrase) - 1);
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
	if (args->key[0] == '\0') {
		LOG_WARNING("run without 'key', "
					"set '$ORIGIN/../certs/server.key' by default");
		char buf[MUGGLE_MAX_PATH] = {};
		if (!get_certs_dir(buf, sizeof(buf))) {
			LOG_ERROR("failed get 'certs' dir");
			return false;
		}
		if (muggle_path_join(buf, "server.key", args->key, sizeof(args->key)) !=
			0) {
			LOG_ERROR("failed join path: %s, %s", buf, "server.key");
			return false;
		}
	}
	if (args->crt[0] == '\0') {
		LOG_WARNING("run without 'cert', "
					"set '$ORIGIN/../certs/server.crt' by default");
		char buf[MUGGLE_MAX_PATH] = {};
		if (!get_certs_dir(buf, sizeof(buf))) {
			LOG_ERROR("failed get 'certs' dir");
			return false;
		}
		if (muggle_path_join(buf, "server.crt", args->crt, sizeof(args->crt)) !=
			0) {
			LOG_ERROR("failed join path: %s, %s", buf, "server.crt");
			return false;
		}
	}

	LOG_INFO("\n"
			 "----- input args -----\n"
			 "host=%s\n"
			 "port=%s\n"
			 "key=%s\n"
			 "cert=%s\n"
			 "pass=%s\n"
			 "----------------------",
			 args->host, args->port, args->key, args->crt);

	return true;
}

#define REGISTER_CALLBACK(msg_id, funcname) \
	dispatcher.RegisterCallback(msg_id, TcpServerSession::s_##funcname);
void register_callbacks(Dispatcher &dispatcher)
{
	REGISTER_CALLBACK(DEMO_MSG_ID_PING, OnPing);
	REGISTER_CALLBACK(DEMO_MSG_ID_REQ_LOGIN, OnLogin);
	REGISTER_CALLBACK(DEMO_MSG_ID_REQ_SUM, OnReqSum);
}

void run_tcp_server(sys_args_t *args)
{
	// register callbacks
	LOG_INFO("setup callbacks");
	Dispatcher dispatcher;
	dispatcher.SetTotalMessage(MAX_DEMO_MSG_ID);
	register_callbacks(dispatcher);

	// codec
	LOG_INFO("setup codec");
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
	std::shared_ptr<SSLContext> ssl_ctx;
	if (args->passphrase[0] != '\0') {
		ssl_ctx = std::shared_ptr<SSLContext>(
			factory->NewServerContext(args->crt, args->key, args->passphrase));
	} else {
		ssl_ctx = std::shared_ptr<SSLContext>(
			factory->NewServerContext(args->crt, args->key, nullptr));
	}
	if (!ssl_ctx) {
		LOG_ERROR("failed new ssl server context");
		return;
	}

	// server handle
	TcpServerHandle handle;
	handle.SetCodecChain(&codec_chain);
	handle.SetDispatcher(&dispatcher);
	handle.SetSSLContext(ssl_ctx.get());

	NetEventLoop evloop(128, 0);
	evloop.SetHandle(&handle);
	evloop.SetTimerInterval(3000);

	SocketContext *listen_ctx =
		SocketUtils::TCPListen(args->host, args->port, 512);
	if (listen_ctx == nullptr) {
		LOG_ERROR("failed listen: host=%s, port=%s", args->host, args->port);
		return;
	}
	LOG_INFO("TCP Server Listen: host=%s, port=%s", args->host, args->port);

	evloop.AddContext(listen_ctx);
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

	sys_args_t args;
	if (!parse_sys_args(argc, argv, &args)) {
		LOG_ERROR("failed parse input arguments");
		exit(EXIT_FAILURE);
	}

	run_tcp_server(&args);

	return 0;
}

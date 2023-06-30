#include <vector>
#include <stack>
#include <map>
#include <unordered_map>
#include <cmath>
#include <cstring>
#include <string>
#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <future>
#include <unordered_set>
#include <set>
#include <cstdio>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpc++/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#define BUFFER_SIZE 256

const char C_RED[16] = "\033[0;31m";
const char C_YELLOW[16] = "\033[0;33m";
const char C_GREEN[16] = "\033[0;34m";
const char C_NONE[16] = "\033[0m";


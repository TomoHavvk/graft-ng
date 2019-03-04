
#pragma once

#include "lib/graft/router.h"
#include "lib/graft/inout.h"
#include "lib/graft/jsonrpc.h"

#define tst false

namespace graft::supernode::request {

void registerRedirectRequests(graft::Router &router);

GRAFT_DEFINE_IO_STRUCT(RegisterSupernode,
                       (uint32_t, broadcast_hops),
                       (uint32_t, redirect_timeout_ms),
                       (std::string, supernode_id), //supernode public identification key
                       (std::string, supernode_url), //base URL for forwarding requests to supernode
                       (std::string, redirect_uri) //special uri for UDHT protocol redirection mechanism
                       );

GRAFT_DEFINE_JSON_RPC_REQUEST(RegisterSupernodeJsonRpcRequest, RegisterSupernode);

///////

GRAFT_DEFINE_IO_STRUCT_INITED(SupernodeRedirectIds,
                              (std::string, id, std::string())
                       );

GRAFT_DEFINE_IO_STRUCT_INITED(SupernodeRedirectIdsResponse,
                              (int, Status, 0)
                              );

GRAFT_DEFINE_JSON_RPC_REQUEST(SupernodeRedirectIdsJsonRpcRequest, SupernodeRedirectIds);
GRAFT_DEFINE_JSON_RPC_RESPONSE_RESULT(SupernodeRedirectIdsJsonRpcResponse, SupernodeRedirectIdsResponse);

graft::Status periodicRegisterSupernode(const graft::Router::vars_t& vars, const graft::Input& input, graft::Context& ctx,
        graft::Output& output);
graft::Status periodicUpdateRedirectIds(const graft::Router::vars_t& vars, const graft::Input& input, graft::Context& ctx,
        graft::Output& output);
#if tst
graft::Status test_startBroadcast(const graft::Router::vars_t& vars, const graft::Input& input, graft::Context& ctx, graft::Output& output);
#endif

} //namespace graft::supernode::request


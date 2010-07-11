/*
  Copyright (c) 2006-2009 Gluster, Inc. <http://www.gluster.com>
  This file is part of GlusterFS.

  GlusterFS is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published
  by the Free Software Foundation; either version 3 of the License,
  or (at your option) any later version.

  GlusterFS is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see
  <http://www.gnu.org/licenses/>.
*/

#ifndef _GLUSTERD_H_
#define _GLUSTERD_H_

#ifndef _CONFIG_H
#define _CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <uuid/uuid.h>

#include "rpc-clnt.h"
#include "glusterfs.h"
#include "xlator.h"
#include "logging.h"
#include "call-stub.h"
#include "authenticate.h"
#include "fd.h"
#include "byte-order.h"
//#include "protocol.h"
#include "glusterd-mem-types.h"
#include "rpcsvc.h"
#include "glusterd1.h"




/*struct _glusterd_connection {
	struct list_head    list;
	char               *id;
	int                 ref;
        int                 active_transports;
	pthread_mutex_t     lock;
	char                disconnected;
	xlator_t           *bound_xl;
};

typedef struct _glusterd_connection glusterd_connection_t;
*/


typedef enum glusterd_peer_state_ {
        GD_PEER_STATE_NONE = 0,
        GD_PEER_STATE_INBOUND,
        GD_PEER_STATE_OUTBOUND,
        GD_PEER_STATE_FRIEND
} glusterd_peer_state_t;


typedef struct glusterd_peer_state_info_ {
        glusterd_peer_state_t   state;
        struct timeval          transition_time;
}glusterd_peer_state_info_t;


struct glusterd_peerinfo_ {
        uuid_t                          uuid;
        glusterd_peer_state_info_t      state;
        char                            *hostname;
        int                             port;
        struct list_head                uuid_list;
        struct list_head                op_peers_list;
 //       struct list_head                pending_uuid;
        struct rpc_clnt                 *rpc;
};

typedef struct glusterd_peerinfo_ glusterd_peerinfo_t;

typedef struct {
        struct _volfile_ctx *volfile;
	pthread_mutex_t   mutex;
	struct list_head  peers;
//	struct list_head  pending_peers;
        gf_boolean_t      verify_volfile_checksum;
        gf_boolean_t      trace;
        uuid_t            uuid;
        char              workdir[PATH_MAX];
        rpcsvc_t          *rpc;
        rpc_clnt_prog_t   *mgmt;
        struct list_head  volumes;
} glusterd_conf_t;

struct glusterd_brickinfo {
        char    hostname[1024];
        char    path[PATH_MAX];
        struct list_head  brick_list;
};

typedef struct glusterd_brickinfo glusterd_brickinfo_t;

struct glusterd_volinfo_ {
        char    volname[1024];
        int     type;
        int     brick_count;
        struct list_head        vol_list;
        struct list_head        bricks;
};

typedef struct glusterd_volinfo_ glusterd_volinfo_t;


#define GLUSTERD_DEFAULT_WORKDIR "/etc/glusterd"
#define GLUSTERD_DEFAULT_PORT   4284

typedef ssize_t (*gd_serialize_t) (struct iovec outmsg, void *args);

//void glusterd_init (int);

int
glusterd_probe_begin (rpcsvc_request_t *req, const char *hoststr);

/*int
glusterd_interpret (xlator_t *this, transport_t *trans,
                    char *hdr_p, size_t hdrlen, struct iobuf *iobuf);


int
glusterd_friend_probe (const char *hoststr);
*/


int
glusterd_xfer_friend_add_resp (rpcsvc_request_t *req, char *hostname);

int
glusterd_friend_find (uuid_t uuid, char *hostname, 
                      glusterd_peerinfo_t **peerinfo);

int
glusterd_friend_add (const char *hoststr,
                     glusterd_peer_state_t state,
                     uuid_t *uuid, struct rpc_clnt    *rpc,
                     glusterd_peerinfo_t **friend);
/*
int
glusterd_xfer_friend_req_msg (glusterd_peerinfo_t *peerinfo, xlator_t *this);

int
glusterd_xfer_cluster_lock_req (xlator_t *this, int32_t *lock_count);
*/

/*int
glusterd_xfer_cluster_unlock_req (xlator_t *this, int32_t *unlock_count);
*/

int
glusterd_op_lock_send_resp (rpcsvc_request_t *req, int32_t status);

int
glusterd_op_unlock_send_resp (rpcsvc_request_t *req, int32_t status);

int
glusterd_op_stage_send_resp (rpcsvc_request_t *req, 
                             int32_t op, int32_t status);

int
glusterd_op_commmit_send_resp (rpcsvc_request_t *req, 
                               int32_t op, int32_t status);

int32_t
glusterd_create_volume (rpcsvc_request_t *req, dict_t *dict);

int
glusterd_rpc_notify (struct rpc_clnt *rpc, void *mydata, 
                     rpc_clnt_event_t event,
                     void *data);
int
glusterd_handle_incoming_friend_req (rpcsvc_request_t *req);

int
glusterd_handle_probe_query (rpcsvc_request_t *req);

int
glusterd_handle_cluster_lock (rpcsvc_request_t *req);

int
glusterd_handle_cluster_unlock (rpcsvc_request_t *req);

int
glusterd_handle_stage_op (rpcsvc_request_t *req);

int
glusterd_handle_commit_op (rpcsvc_request_t *req);

int
glusterd_handle_cli_probe (rpcsvc_request_t *req);

int
glusterd_handle_create_volume (rpcsvc_request_t *req);

int
glusterd_xfer_cli_probe_resp (rpcsvc_request_t *req, int32_t op_ret,
                              int32_t op_errno, char *hostname);


int
glusterd_op_commit_send_resp (rpcsvc_request_t *req, 
                               int32_t op, int32_t status);
#endif
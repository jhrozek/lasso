/* -*- Mode: c; c-basic-offset: 8 -*-
 *
 * $Id: Lasso-wsf.i,v 1.79 2006/03/06 14:01:29 Exp $
 *
 * SWIG bindings for Lasso Library
 *
 * Copyright (C) 2004, 2005 Entr'ouvert
 * http://lasso.entrouvert.org
 *
 * Authors: See AUTHORS file in top-level directory.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

%include Lasso-wsf2-disco.i
%include Lasso-wsf-soap.i

%{
#include <lasso/id-wsf-2.0/discovery.h>
%}


/***********************************************************************
 ***********************************************************************
 * ID-WSF
 ***********************************************************************
 ***********************************************************************/


/***********************************************************************
 * lasso:Discovery
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(IdWsf2Discovery) LassoIdWsf2Discovery;
#endif
typedef struct {
} LassoIdWsf2Discovery;
%extend LassoIdWsf2Discovery {

	/* Attributes inherited from Wsf2Profile */

	%immutable msgBody;
	char *msgBody;

	%immutable msgUrl;
	char *msgUrl;

	%newobject request_get;
	LassoNode *request;

	%newobject response_get;
	LassoNode *response;

	%newobject server_get;
	LassoServer *server;

	%newobject soapEnvelopeRequest_get;
	LassoSoapEnvelope *soapEnvelopeRequest;

	%newobject soapEnvelopeResponse_get;
	LassoSoapEnvelope *soapEnvelopeResponse;
	
	/* Attributes */

	%newobject metadata_get;
	LassoIdWsf2DiscoSvcMetadata *metadata;

	%immutable svcMDID;
	char *svcMDID;

	/* Constructor, Destructor & Static Methods */

	LassoIdWsf2Discovery(LassoServer *server);

	~LassoIdWsf2Discovery();

	/* Methods inherited from Wsf2Profile */

	THROW_ERROR()
	int buildRequestMsg();
	END_THROW_ERROR()

	THROW_ERROR()
	int buildResponseMsg();
	END_THROW_ERROR()

	/* Methods */

	THROW_ERROR()
	int initMetadataRegister(char *service_type, char *abstract, char *disco_provider_id);
	END_THROW_ERROR()

	THROW_ERROR()
	int processMetadataRegisterMsg(const gchar *message);
	END_THROW_ERROR()

	THROW_ERROR()
	int processMetadataRegisterResponseMsg(const gchar *message);
	END_THROW_ERROR()

	THROW_ERROR()
	int initQuery(const char *security_mech_id = NULL);
	END_THROW_ERROR()
}

%{

/* Attributes inherited from Wsf2Profile implementations */

/* msgBody */
#define LassoIdWsf2Discovery_get_msgBody(self) LASSO_WSF2_PROFILE(self)->msg_body
#define LassoIdWsf2Discovery_msgBody_get(self) LASSO_WSF2_PROFILE(self)->msg_body

/* msgUrl */
#define LassoIdWsf2Discovery_get_msgUrl(self) LASSO_WSF2_PROFILE(self)->msg_url
#define LassoIdWsf2Discovery_msgUrl_get(self) LASSO_WSF2_PROFILE(self)->msg_url

/* request */
#define LassoIdWsf2Discovery_get_request(self) get_node(LASSO_WSF2_PROFILE(self)->request)
#define LassoIdWsf2Discovery_request_get(self) get_node(LASSO_WSF2_PROFILE(self)->request)
#define LassoIdWsf2Discovery_set_request(self, value) set_node((gpointer *) &LASSO_WSF2_PROFILE(self)->request, (value))
#define LassoIdWsf2Discovery_request_set(self, value) set_node((gpointer *) &LASSO_WSF2_PROFILE(self)->request, (value))

/* response */
#define LassoIdWsf2Discovery_get_response(self) get_node(LASSO_WSF2_PROFILE(self)->response)
#define LassoIdWsf2Discovery_response_get(self) get_node(LASSO_WSF2_PROFILE(self)->response)
#define LassoIdWsf2Discovery_set_response(self, value) set_node((gpointer *) &LASSO_WSF2_PROFILE(self)->response, (value))
#define LassoIdWsf2Discovery_response_set(self, value) set_node((gpointer *) &LASSO_WSF2_PROFILE(self)->response, (value))

/* server */
#define LassoIdWsf2Discovery_get_server(self) get_node(LASSO_WSF2_PROFILE(self)->server)
#define LassoIdWsf2Discovery_server_get(self) get_node(LASSO_WSF2_PROFILE(self)->server)
#define LassoIdWsf2Discovery_set_server(self, value) set_node((gpointer *) &LASSO_WSF2_PROFILE(self)->server, (value))
#define LassoIdWsf2Discovery_server_set(self, value) set_node((gpointer *) &LASSO_WSF2_PROFILE(self)->server, (value))

/* soapEnvelopeRequest */
#define LassoIdWsf2Discovery_get_soapEnvelopeRequest(self) get_node(LASSO_WSF2_PROFILE(self)->soap_envelope_request)
#define LassoIdWsf2Discovery_soapEnvelopeRequest_get(self) get_node(LASSO_WSF2_PROFILE(self)->soap_envelope_request)
#define LassoIdWsf2Discovery_set_soapEnvelopeRequest(self, value) set_node((gpointer *) &LASSO_WSF2_PROFILE(self)->soap_envelope_request, (value))
#define LassoIdWsf2Discovery_soapEnvelopeRequest_set(self, value) set_node((gpointer *) &LASSO_WSF2_PROFILE(self)->soap_envelope_request, (value))

/* soapEnvelopeResponse */
#define LassoIdWsf2Discovery_get_soapEnvelopeResponse(self) get_node(LASSO_WSF2_PROFILE(self)->soap_envelope_response)
#define LassoIdWsf2Discovery_soapEnvelopeResponse_get(self) get_node(LASSO_WSF2_PROFILE(self)->soap_envelope_response)
#define LassoIdWsf2Discovery_set_soapEnvelopeResponse(self, value) set_node((gpointer *) &LASSO_WSF2_PROFILE(self)->soap_envelope_response, (value))
#define LassoIdWsf2Discovery_soapEnvelopeResponse_set(self, value) set_node((gpointer *) &LASSO_WSF2_PROFILE(self)->soap_envelope_response, (value))

/* Attributes */

/* metadata */
#define LassoIdWsf2Discovery_get_metadata(self) get_node(self->metadata)
#define LassoIdWsf2Discovery_metadata_get(self) get_node(self->metadata)
#define LassoIdWsf2Discovery_set_metadata(self, value) set_node((gpointer *) &self->metadata, value)
#define LassoIdWsf2Discovery_metadata_set(self, value) set_node((gpointer *) &self->metadata, value)

/* svcMDID */
#define LassoIdWsf2Discovery_get_svcMDID(self) self->svcMDID
#define LassoIdWsf2Discovery_svcMDID_get(self) self->svcMDID

/* Constructors, destructors & static methods implementations */

#define new_LassoIdWsf2Discovery lasso_idwsf2_discovery_new
#define delete_LassoIdWsf2Discovery(self) lasso_node_destroy(LASSO_NODE(self))

/* Implementations of methods inherited from Wsf2Profile */

#define LassoIdWsf2Discovery_buildRequestMsg(self) lasso_wsf2_profile_build_soap_request_msg(LASSO_WSF2_PROFILE(self))
#define LassoIdWsf2Discovery_buildResponseMsg(self) lasso_wsf2_profile_build_soap_response_msg(LASSO_WSF2_PROFILE(self))

/* Methods implementations */

#define LassoIdWsf2Discovery_initMetadataRegister lasso_idwsf2_discovery_init_metadata_register
#define LassoIdWsf2Discovery_processMetadataRegisterMsg lasso_idwsf2_discovery_process_metadata_register_msg
#define LassoIdWsf2Discovery_processMetadataRegisterResponseMsg lasso_idwsf2_discovery_process_metadata_register_response_msg
#define LassoIdWsf2Discovery_initQuery lasso_idwsf2_discovery_init_query

%}
/* $Id$
 *
 * Lasso - A free implementation of the Liberty Alliance specifications.
 *
 * Copyright (C) 2004 Entr'ouvert
 * http://lasso.entrouvert.org
 * 
 * Author: Valery Febvre <vfebvre@easter-eggs.com>
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

#include <lasso/environs/logout.h>

static GObjectClass *parent_class = NULL;

/*****************************************************************************/
/* public methods                                                            */
/*****************************************************************************/

gchar *
lasso_logout_dump(LassoLogout *logout)
{
  LassoProfileContext *profileContext;
  gchar *dump;

  g_return_val_if_fail(LASSO_IS_LOGOUT(logout), NULL);

  return(dump);
}

gint
lasso_logout_build_request_msg(LassoLogout *logout)
{
  LassoProfileContext *profileContext;
  LassoProvider *provider;
  xmlChar *protocolProfile, *singleLogoutServiceURL;

  g_return_val_if_fail(LASSO_IS_LOGOUT(logout), -1);
  
  profileContext = LASSO_PROFILE_CONTEXT(logout);

  provider = lasso_server_get_provider(profileContext->server, profileContext->remote_providerID);
  if(provider==NULL){
    debug(ERROR, "Provider %s not found\n", profileContext->remote_providerID);
    return(-2);
  }

  /* get the prototocol profile of the logout request */
  protocolProfile = lasso_provider_get_singleLogoutProtocolProfile(provider);

  if(protocolProfile==NULL){
    debug(ERROR, "Single Logout Protocol profile not found\n");
    return(-3);
  }

  if(xmlStrEqual(protocolProfile, lassoLibProtocolProfileSloSpSoap) || xmlStrEqual(protocolProfile, lassoLibProtocolProfileSloIdpSoap)){
    debug(DEBUG, "Building a soap request message\n");
    profileContext->request_type = lassoHttpMethodSoap;

    /* sign the request message */
    lasso_samlp_request_abstract_set_signature(LASSO_SAMLP_REQUEST_ABSTRACT(profileContext->request),
					       profileContext->server->signature_method,
					       profileContext->server->private_key,
					       profileContext->server->certificate);
    
    profileContext->msg_url  = lasso_provider_get_soapEndpoint(provider);
    profileContext->msg_body = lasso_node_export_to_soap(profileContext->request);
  }
  else if(xmlStrEqual(protocolProfile,lassoLibProtocolProfileSloSpHttp)||xmlStrEqual(protocolProfile,lassoLibProtocolProfileSloIdpHttp)){
    debug(DEBUG, "Building a http get request message\n");
    profileContext->request_type = lassoHttpMethodRedirect;
    profileContext->msg_url = lasso_provider_get_singleLogoutServiceURL(provider);
    profileContext->msg_url = lasso_node_export_to_query(profileContext->request,
							 profileContext->server->signature_method,
							 profileContext->server->private_key);
    profileContext->msg_body = NULL;
  }

  return(0);
}

gint
lasso_logout_build_response_msg(LassoLogout *logout)
{
  LassoProfileContext *profileContext;
  LassoProvider *provider;
  xmlChar *protocolProfile;
  int codeError = 0;
  
  if(!LASSO_IS_LOGOUT(logout)){
    debug(ERROR, "Not a Logout object\n");
    return(-1);
  }
  
  profileContext = LASSO_PROFILE_CONTEXT(logout);

  provider = lasso_server_get_provider(profileContext->server, profileContext->remote_providerID);
  if(provider==NULL){
    debug(ERROR, "Provider not found %s\n", profileContext->remote_providerID);
    return(-2);
  }

  protocolProfile = lasso_provider_get_singleLogoutProtocolProfile(provider);
  if(protocolProfile==NULL){
    debug(ERROR, "Single Logout Protocol profile not found\n");
    return(-3);
  }

  if(xmlStrEqual(protocolProfile, lassoLibProtocolProfileSloSpSoap) || xmlStrEqual(protocolProfile, lassoLibProtocolProfileSloIdpSoap)){
    debug(DEBUG, "Building a soap response message\n");
    profileContext->msg_url = NULL;
    profileContext->msg_body = lasso_node_export_to_soap(profileContext->response);
  }
  else if(xmlStrEqual(protocolProfile,lassoLibProtocolProfileSloSpHttp)||xmlStrEqual(protocolProfile,lassoLibProtocolProfileSloIdpHttp)){
    debug(DEBUG, "Building a http get response message\n");
    profileContext->response_type = lassoHttpMethodRedirect;
    profileContext->msg_url = lasso_node_export_to_query(profileContext->response,
							 profileContext->server->signature_method,
							 profileContext->server->private_key);
    profileContext->msg_body = NULL;
  }

  return(0);
}

void
lasso_logout_destroy(LassoLogout *logout)
{
  g_object_unref(G_OBJECT(logout));
}

gint
lasso_logout_init_request(LassoLogout *logout,
			  gchar       *remote_providerID)
{
  LassoProfileContext *profileContext;
  LassoNode           *nameIdentifier;
  LassoIdentity       *identity;
  LassoLogoutRequest  *request;

  xmlChar *content, *nameQualifier, *format;

  g_return_val_if_fail(LASSO_IS_LOGOUT(logout), -1);

  profileContext = LASSO_PROFILE_CONTEXT(logout);

  if(remote_providerID==NULL){
    debug(INFO, "No remote provider id, get the next assertion peer provider id\n");
    profileContext->remote_providerID = lasso_user_get_next_assertion_remote_providerID(profileContext->user);
  }
  else{
    debug(INFO, "A remote provider id for logout request : %s\n", remote_providerID);
    profileContext->remote_providerID = g_strdup(remote_providerID);
  }

  if(profileContext->remote_providerID==NULL){
    debug(ERROR, "No provider id for init request\n");
    return(-2);
  }

  /* get identity */
  identity = lasso_user_get_identity(profileContext->user, profileContext->remote_providerID);
  if(identity==NULL){
    debug(ERROR, "Identity not found\n");
    return(-3);
  }

  /* get the name identifier (!!! depend on the provider type : SP or IDP !!!)*/
  switch(profileContext->provider_type){
  case lassoProviderTypeSp:
    nameIdentifier = LASSO_NODE(lasso_identity_get_local_nameIdentifier(identity));
    if(!nameIdentifier)
      nameIdentifier = LASSO_NODE(lasso_identity_get_remote_nameIdentifier(identity));
    break;
  case lassoProviderTypeIdp:
    nameIdentifier = LASSO_NODE(lasso_identity_get_remote_nameIdentifier(identity));
    if(!nameIdentifier)
      nameIdentifier = LASSO_NODE(lasso_identity_get_local_nameIdentifier(identity));
    break;
  default:
    debug(ERROR, "Unknown provider type\n");
    return(-4);
  }
  
  if(!nameIdentifier){
    debug(ERROR, "Name identifier not found for %s\n", profileContext->remote_providerID);
    return(-5);
  }

  /* build the request */
  content = lasso_node_get_content(nameIdentifier);
  nameQualifier = lasso_node_get_attr_value(nameIdentifier, "NameQualifier");
  format = lasso_node_get_attr_value(nameIdentifier, "Format");
  profileContext->request = lasso_logout_request_new(lasso_provider_get_providerID(LASSO_PROVIDER(profileContext->server)),
						     content,
						     nameQualifier,
						     format);

  if(profileContext->request==NULL){
    debug(ERROR, "Error while creating the request\n");
    return(-6);
  }

  return(0);
}

gint
lasso_logout_process_request_msg(LassoLogout      *logout,
				 gchar            *request_msg,
				 lassoHttpMethods  request_method)
{
  LassoProfileContext *profileContext;
  LassoIdentity *identity;
  LassoNode *nameIdentifier, *assertion;
  LassoNode *statusCode;
  LassoNodeClass *statusCode_class;
  xmlChar *remote_providerID;

  g_return_val_if_fail(LASSO_IS_LOGOUT(logout), -1);
  g_return_val_if_fail(request_msg!=NULL, -2);

  profileContext = LASSO_PROFILE_CONTEXT(logout);

  switch(request_method){
  case lassoHttpMethodSoap:
    debug(DEBUG, "Build a logout request from soap msg\n");
    profileContext->request = lasso_logout_request_new_from_export(request_msg, lassoNodeExportTypeSoap);
    break;
  case lassoHttpMethodRedirect:
    debug(DEBUG, "Build a logout request from query msg\n");
    profileContext->request = lasso_logout_request_new_from_export(request_msg, lassoNodeExportTypeQuery);
    break;
  case lassoHttpMethodGet:
    debug(WARNING, "TODO, implement the get method\n");
    break;
  default:
    debug(ERROR, "Unknown request method\n");
    return(-3);
  }

  /* set the remote provider id from the request */
  remote_providerID = lasso_node_get_child_content(profileContext->request, "ProviderID", NULL);
  profileContext->remote_providerID = remote_providerID;

  /* set LogoutResponse */
  profileContext->response = lasso_logout_response_new(lasso_provider_get_providerID(LASSO_PROVIDER(profileContext->server)),
						       lassoSamlStatusCodeSuccess,
						       profileContext->request);

  if(profileContext->response==NULL){
    debug(ERROR, "Error while building response\n");
    return(-4);
  }

  statusCode = lasso_node_get_child(profileContext->response, "StatusCode", NULL);
  statusCode_class = LASSO_NODE_GET_CLASS(statusCode);

  nameIdentifier = lasso_node_get_child(profileContext->request, "NameIdentifier", NULL);
  if(nameIdentifier==NULL){
    debug(ERROR, "Name identifier not found in logout request\n");
    statusCode_class->set_prop(statusCode, "Value", lassoLibStatusCodeFederationDoesNotExist);
    return(-5);
  }

  remote_providerID = lasso_node_get_child_content(profileContext->request, "ProviderID", NULL);
  if(remote_providerID==NULL){
    debug(ERROR, "Provider id not found in logout request\n");
    return(-6);
  }

  /* verify authentication (if ok, delete assertion) */
  if(profileContext->user==NULL){
    debug(WARNING, "User environ not found\n");
    statusCode_class->set_prop(statusCode, "Value", lassoSamlStatusCodeRequestDenied);
  }

  assertion = lasso_user_get_assertion(profileContext->user, remote_providerID);
  if(assertion==NULL){
    debug(WARNING, "%s has no assertion\n", remote_providerID);
    statusCode_class->set_prop(statusCode, "Value", lassoSamlStatusCodeRequestDenied);
    return(-9);
  }

  /* Verify federation */
  identity = lasso_user_get_identity(profileContext->user, remote_providerID);
  if(identity==NULL){
    debug(WARNING, "No identity for %s\n", remote_providerID);
    statusCode_class->set_prop(statusCode, "Value", lassoLibStatusCodeFederationDoesNotExist);
    return(-7);
  }

  if(lasso_identity_verify_nameIdentifier(identity, nameIdentifier)==FALSE){
    debug(WARNING, "No name identifier for %s\n", remote_providerID);
    statusCode_class->set_prop(statusCode, "Value", lassoLibStatusCodeFederationDoesNotExist);
    return(-8);
  }

  return(0);
}

gint
lasso_logout_process_response_msg(LassoLogout      *logout,
				  gchar            *response_msg,
				  lassoHttpMethods  response_method)
{
  LassoProfileContext *profileContext;
  xmlChar   *statusCodeValue;
  LassoNode *statusCode;

  g_return_val_if_fail(LASSO_IS_LOGOUT(logout), -1);
  g_return_val_if_fail(response_msg!=NULL, -2);

  profileContext = LASSO_PROFILE_CONTEXT(logout);

  /* parse LogoutResponse */
  switch(response_method){
  case lassoHttpMethodSoap:
    profileContext->response = lasso_logout_response_new_from_export(response_msg, lassoNodeExportTypeSoap);
    break;
  case lassoHttpMethodRedirect:
    profileContext->response = lasso_logout_response_new_from_export(response_msg, lassoNodeExportTypeQuery);
    break;
  default:
    debug(ERROR, "Unknown response method\n");
    return(-3);
  }
 
  statusCode = lasso_node_get_child(profileContext->response, "StatusCode", NULL);
  statusCodeValue = lasso_node_get_attr_value(statusCode, "Value");
  if(!xmlStrEqual(statusCodeValue, lassoSamlStatusCodeSuccess)){
    return(-4);
  }

  return(0);
}

/*****************************************************************************/
/* overrided parent class methods                                            */
/*****************************************************************************/

static void
lasso_logout_finalize(LassoLogout *logout)
{  
  debug(INFO, "Logout object 0x%x finalized ...\n", logout);

  parent_class->finalize(LASSO_PROFILE_CONTEXT(logout));
}

/*****************************************************************************/
/* instance and class init functions                                         */
/*****************************************************************************/

static void
lasso_logout_instance_init(LassoLogout *logout)
{

}

static void
lasso_logout_class_init(LassoLogoutClass *class)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(class);
  
  parent_class = g_type_class_peek_parent(class);
  /* override parent class methods */
  gobject_class->finalize = (void *)lasso_logout_finalize;
}

GType lasso_logout_get_type() {
  static GType this_type = 0;

  if (!this_type) {
    static const GTypeInfo this_info = {
      sizeof (LassoLogoutClass),
      NULL,
      NULL,
      (GClassInitFunc) lasso_logout_class_init,
      NULL,
      NULL,
      sizeof(LassoLogout),
      0,
      (GInstanceInitFunc) lasso_logout_instance_init,
    };
    
    this_type = g_type_register_static(LASSO_TYPE_PROFILE_CONTEXT,
				       "LassoLogout",
				       &this_info, 0);
  }
  return this_type;
}

LassoLogout *
lasso_logout_new(LassoServer        *server,
		 LassoUser          *user,
		 lassoProviderTypes  provider_type)
{
  LassoLogout *logout;

  g_return_val_if_fail(LASSO_IS_SERVER(server), NULL);
  g_return_val_if_fail(LASSO_IS_USER(user), NULL);

  /* set the logout object */
  logout = g_object_new(LASSO_TYPE_LOGOUT,
			"server", server,
			"user", user,
			"provider_type", provider_type,
			NULL);

  return(logout);
}

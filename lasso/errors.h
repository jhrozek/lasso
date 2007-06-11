/* $Id$ 
 *
 * Lasso - A free implementation of the Liberty Alliance specifications.
 *
 * Copyright (C) 2004-2007 Entr'ouvert
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

/* Negative errors : programming or runtime recoverable errors */
/* Positive errors : Liberty Alliance recoverable errors */

/* undefined */
#define LASSO_ERROR_UNDEFINED                           -1
#define LASSO_ERROR_UNIMPLEMENTED                       -2

/* generic XML */
#define LASSO_XML_ERROR_NODE_NOT_FOUND                 -10
#define LASSO_XML_ERROR_NODE_CONTENT_NOT_FOUND         -11
#define LASSO_XML_ERROR_ATTR_NOT_FOUND                 -12
#define LASSO_XML_ERROR_ATTR_VALUE_NOT_FOUND           -13
#define LASSO_XML_ERROR_INVALID_FILE                   -14
#define LASSO_XML_ERROR_OBJECT_CONSTRUCTION_FAILED     -15
#define LASSO_XML_ERROR_MISSING_NAMESPACE              -16

/* XMLDSig */
#define LASSO_DS_ERROR_SIGNATURE_NOT_FOUND             101
#define LASSO_DS_ERROR_INVALID_SIGNATURE               102
#define LASSO_DS_ERROR_SIGNATURE_TMPL_CREATION_FAILED -103
#define LASSO_DS_ERROR_CONTEXT_CREATION_FAILED        -104
#define LASSO_DS_ERROR_PUBLIC_KEY_LOAD_FAILED         -105
#define LASSO_DS_ERROR_PRIVATE_KEY_LOAD_FAILED        -106
#define LASSO_DS_ERROR_CERTIFICATE_LOAD_FAILED        -107
#define LASSO_DS_ERROR_SIGNATURE_FAILED               -108
#define LASSO_DS_ERROR_KEYS_MNGR_CREATION_FAILED      -109
#define LASSO_DS_ERROR_KEYS_MNGR_INIT_FAILED          -110
#define LASSO_DS_ERROR_SIGNATURE_VERIFICATION_FAILED  -111
#define LASSO_DS_ERROR_CA_CERT_CHAIN_LOAD_FAILED      -112
#define LASSO_DS_ERROR_INVALID_SIGALG                 -113
#define LASSO_DS_ERROR_DIGEST_COMPUTE_FAILED          -114
#define LASSO_DS_ERROR_SIGNATURE_TEMPLATE_NOT_FOUND   -115

/* Server */
#define LASSO_SERVER_ERROR_PROVIDER_NOT_FOUND         -201
#define LASSO_SERVER_ERROR_ADD_PROVIDER_FAILED        -202
#define LASSO_SERVER_ERROR_ADD_PROVIDER_PROTOCOL_MISMATCH -203
#define LASSO_SERVER_ERROR_SET_ENCRYPTION_PRIVATE_KEY_FAILED 204
#define LASSO_SERVER_ERROR_INVALID_XML			-205

/* Single Logout */
#define LASSO_LOGOUT_ERROR_UNSUPPORTED_PROFILE        -301
#define LASSO_LOGOUT_ERROR_REQUEST_DENIED              302
#define LASSO_LOGOUT_ERROR_FEDERATION_NOT_FOUND        303
#define LASSO_LOGOUT_ERROR_UNKNOWN_PRINCIPAL           304

/* Profile */
#define LASSO_PROFILE_ERROR_INVALID_QUERY             -401
#define LASSO_PROFILE_ERROR_INVALID_POST_MSG          -402
#define LASSO_PROFILE_ERROR_INVALID_SOAP_MSG          -403
#define LASSO_PROFILE_ERROR_MISSING_REQUEST           -404
#define LASSO_PROFILE_ERROR_INVALID_HTTP_METHOD       -405
#define LASSO_PROFILE_ERROR_INVALID_PROTOCOLPROFILE   -406
#define LASSO_PROFILE_ERROR_INVALID_MSG               -407
#define LASSO_PROFILE_ERROR_MISSING_REMOTE_PROVIDERID -408
#define LASSO_PROFILE_ERROR_UNSUPPORTED_PROFILE       -409
#define LASSO_PROFILE_ERROR_UNKNOWN_PROFILE_URL       -410
#define LASSO_PROFILE_ERROR_IDENTITY_NOT_FOUND        -411
#define LASSO_PROFILE_ERROR_FEDERATION_NOT_FOUND      -412
#define LASSO_PROFILE_ERROR_NAME_IDENTIFIER_NOT_FOUND -413
#define LASSO_PROFILE_ERROR_BUILDING_QUERY_FAILED     -414
#define LASSO_PROFILE_ERROR_BUILDING_REQUEST_FAILED   -415
#define LASSO_PROFILE_ERROR_BUILDING_MESSAGE_FAILED   -416
#define LASSO_PROFILE_ERROR_BUILDING_RESPONSE_FAILED  -417
#define LASSO_PROFILE_ERROR_SESSION_NOT_FOUND         -418
#define LASSO_PROFILE_ERROR_BAD_IDENTITY_DUMP         -419
#define LASSO_PROFILE_ERROR_BAD_SESSION_DUMP          -420
#define LASSO_PROFILE_ERROR_MISSING_RESPONSE          -421
#define LASSO_PROFILE_ERROR_MISSING_STATUS_CODE       -422
#define LASSO_PROFILE_ERROR_MISSING_ARTIFACT          -423
#define LASSO_PROFILE_ERROR_MISSING_RESOURCE_OFFERING      424
#define LASSO_PROFILE_ERROR_MISSING_SERVICE_DESCRIPTION    425
#define LASSO_PROFILE_ERROR_MISSING_SERVICE_TYPE           426
#define LASSO_PROFILE_ERROR_MISSING_ASSERTION         -427
#define LASSO_PROFILE_ERROR_MISSING_SUBJECT           -428
#define LASSO_PROFILE_ERROR_MISSING_NAME_IDENTIFIER   -429
#define LASSO_PROFILE_ERROR_INVALID_ARTIFACT          -430
#define LASSO_PROFILE_ERROR_MISSING_ENCRYPTION_PRIVATE_KEY -431
#define LASSO_PROFILE_ERROR_STATUS_NOT_SUCCESS        -432
#define LASSO_PROFILE_ERROR_MISSING_ISSUER            -433
#define LASSO_PROFILE_ERROR_MISSING_SERVICE_INSTANCE  -434
#define LASSO_PROFILE_ERROR_MISSING_ENDPOINT_REFERENCE -435
#define LASSO_PROFILE_ERROR_MISSING_ENDPOINT_REFERENCE_ADDRESS -436

/* functions/methods parameters checking */
#define LASSO_PARAM_ERROR_BAD_TYPE_OR_NULL_OBJ        -501
#define LASSO_PARAM_ERROR_INVALID_VALUE               -502
#define LASSO_PARAM_ERROR_CHECK_FAILED                -503

/* Single Sign-On */
#define LASSO_LOGIN_ERROR_FEDERATION_NOT_FOUND		 601
#define LASSO_LOGIN_ERROR_CONSENT_NOT_OBTAINED		 602
#define LASSO_LOGIN_ERROR_INVALID_NAMEIDPOLICY		-603
#define LASSO_LOGIN_ERROR_REQUEST_DENIED		 604
#define LASSO_LOGIN_ERROR_INVALID_SIGNATURE		 605
#define LASSO_LOGIN_ERROR_UNSIGNED_AUTHN_REQUEST	 606
#define LASSO_LOGIN_ERROR_STATUS_NOT_SUCCESS             607
#define LASSO_LOGIN_ERROR_UNKNOWN_PRINCIPAL              608
#define LASSO_LOGIN_ERROR_NO_DEFAULT_ENDPOINT		 609
#define LASSO_LOGIN_ERROR_ASSERTION_REPLAY		 610

/* Federation Termination Notification */
#define LASSO_DEFEDERATION_ERROR_MISSING_NAME_IDENTIFIER  -700

/* Soap */
#define LASSO_SOAP_FAULT_REDIRECT_REQUEST                800
#define LASSO_SOAP_ERROR_MISSING_HEADER                 -801
#define LASSO_SOAP_ERROR_MISSING_BODY                   -802

/* Name Identifier Mapping */
#define LASSO_NAME_IDENTIFIER_MAPPING_ERROR_MISSING_TARGET_NAMESPACE -900
#define LASSO_NAME_IDENTIFIER_MAPPING_ERROR_FORBIDDEN_CALL_ON_THIS_SIDE -901
#define LASSO_NAME_IDENTIFIER_MAPPING_ERROR_MISSING_TARGET_IDENTIFIER -902

/* Data Service */
#define LASSO_DATA_SERVICE_ERROR_UNREGISTERED_DST       -1000

/* WSF Profile */
#define LASSO_WSF_PROFILE_ERROR_MISSING_CORRELATION     -1100
#define LASSO_WSF_PROFILE_ERROR_MISSING_SECURITY        -1101

/* ID-WSF 2 Discovery */
#define LASSO_IDWSF2_DISCOVERY_ERROR_SVC_METADATA_REGISTER_FAILED              -1200
#define LASSO_IDWSF2_DISCOVERY_ERROR_SVC_METADATA_ASSOCIATION_ADD_FAILED       -1201
#define LASSO_IDWSF2_DISCOVERY_ERROR_MISSING_REQUESTED_SERVICE                 -1202

/* ID-WSF 2 Data Service */
#define LASSO_IDWSF2_DST_QUERY_FAILED              -1300


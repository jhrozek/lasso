#! /usr/bin/env python
# -*- coding: utf-8 -*-
#
# $Id$
#
# PyLasso - High-level Python bindings for Lasso Library
#
# Copyright (C) 2004 Entr'ouvert
# http://lasso.entrouvert.org
#
# Author: Emmanuel Raviart <eraviart@entrouvert.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


__docformat__ = "plaintext en"


import lassomod


################################################################################
# Constants
################################################################################


# Copy constants from lassomod as is, except that the 'lasso' prefix is removed
# and the first letter is lowered.
_globals = globals()
for constantName, constantValue in lassomod.__dict__.iteritems():
    if constantName.startswith('lassoHttpMethod') \
           or constantName.startswith('lassoLibConsent') \
           or constantName.startswith('lassoLibNameIDPolicyType') \
           or constantName.startswith('lassoLibProtocolProfile') \
           or constantName.startswith('lassoLoginProtocolProfile') \
           or constantName.startswith('lassoMessageType') \
           or constantName.startswith('lassoProviderType') \
           or constantName.startswith('lassoRequestType') \
           or constantName.startswith('lassoSamlAuthenticationMethod') \
           or constantName.startswith('lassoSignatureMethod'):
        _globals[constantName[5].lower() + constantName[6:]] = constantValue


################################################################################
# Errors
################################################################################


class Error(Exception):
    code = None # Use positive error codes for binding specific errors.
    functionName = None

    def __init__(self, functionName):
        self.functionName = functionName

    def __str__(self):
        return repr(self.msg)


class ErrorUnknown(Error):
    def __init__(self, code, functionName):
        Error.__init__(self, functionName)
        self.code = code

    def __str__(self):
        return 'Unknown error number %d in Lasso function %s' % (self.code, self.functionName)


class ErrorLassoAlreadyInitialized(Error):
    code = 1
    msg = 'Lasso already initialized'


class ErrorLassoNotInitialized(Error):
    code = 2
    msg = 'Lasso not initialized or already shotdown'

    
class ErrorInstanceCreationFailed(Error):
    code = 3

    def __str__(self):
        return 'Instance creation failed in Lasso function %s()' % self.functionName

    
class ErrorUnknownRequestType(Error):
    code = 4
    requestType = None

    def __init__(self, functionName, requestType):
        super(ErrorUnknownRequestType, self).__init__(functionName)
        self.requestType = requestType

    def __str__(self):
        return 'Unknown request type (= %s) in Lasso function %s()' % (
            self.requestType, self.functionName)

    
class ErrorUnknownResponseType(Error):
    code = 5
    responseType = None

    def __init__(self, functionName, responseType):
        super(ErrorUnknownResponseType, self).__init__(functionName)
        self.responseType = responseType

    def __str__(self):
        return 'Unknown response type (= %s) in Lasso function %s()' % (
            self.responseType, self.functionName)


def newError(code, functionName):
    # FIXME: Use proper ErrorClass, when Lasso will have well defined error codes.
    return ErrorUnknown(code, functionName)


################################################################################
# Common
################################################################################


class _ObjectMixin(object):
    """Abstract mixin class"""

    def __init__(self, *arguments, **keywordArguments):
        # Don't execute SWIG __init__ method, because object has already be inited in constructor.
        pass

    def __repr__(self):
        return '<Lasso %s instance wrapping %s>' % (self.__class__.__name__, self.this)

    def swig_init(self):
        # Call SWIG __init__.
        super(_ObjectMixin, self).__init__()


################################################################################
# Protocols
################################################################################


class AuthnRequest(_ObjectMixin, lassomod.LassoAuthnRequest):
    # Attributes

    def set_affiliationID(self, affiliationID):
        lassomod.lasso_lib_authn_request_set_affiliationID(self.parent, affiliationID)
    affiliationID = property(None, set_affiliationID)

    def set_assertionConsumerServiceID(self, assertionConsumerServiceID):
        lassomod.lasso_lib_authn_request_set_assertionConsumerServiceID(
            self.parent, assertionConsumerServiceID)
    assertionConsumerServiceID = property(None, set_assertionConsumerServiceID)

    def set_consent(self, consent):
        lassomod.lasso_lib_authn_request_set_consent(self.parent, consent)
    consent = property(None, set_consent)

    def set_forceAuthn(self, forceAuthn):
        lassomod.lasso_lib_authn_request_set_forceAuthn(self.parent, forceAuthn)
    forceAuthn = property(None, set_forceAuthn)

    def set_isPassive(self, isPassive):
        lassomod.lasso_lib_authn_request_set_isPassive(self.parent, isPassive)
    isPassive = property(None, set_isPassive)

    def set_nameIDPolicy(self, nameIDPolicy):
        lassomod.lasso_lib_authn_request_set_nameIDPolicy(self.parent, nameIDPolicy)
    nameIDPolicy = property(None, set_nameIDPolicy)

    def set_protocolProfile(self, protocolProfile):
        lassomod.lasso_lib_authn_request_set_protocolProfile(self.parent, protocolProfile)
    protocolProfile = property(None, set_protocolProfile)

    def set_providerID(self, providerID):
        lassomod.lasso_lib_authn_request_set_providerID(self.parent, providerID)
    providerID = property(None, set_providerID)

    def set_relayState(self, relayState):
        lassomod.lasso_lib_authn_request_set_relayState(self.parent, relayState)
    relayState = property(None, set_relayState)


class Request(_ObjectMixin, lassomod.LassoRequestPtr):
    pass


################################################################################
# Profiles
################################################################################


get_request_type_from_soap_msg = lassomod.lasso_profile_get_request_type_from_soap_msg


class Server(_ObjectMixin, lassomod.LassoServer):
    # Constructors

    def __new__(cls, metadata = None, public_key = None, private_key = None, certificate = None,
                signature_method = signatureMethodRsaSha1):
        self = lassomod.lasso_server_new(
            metadata, public_key, private_key, certificate, signature_method)
        if self is None:
            raise ErrorInstanceCreationFailed('lasso_server_new')
        object.__setattr__(self, '__class__', cls)
        return self

    def new_from_dump(cls, dump):
        self = lassomod.lasso_server_new_from_dump(dump)
        if self is None:
            raise ErrorInstanceCreationFailed('lasso_server_new_from_dump')
        object.__setattr__(self, '__class__', cls)
        return self
    new_from_dump = classmethod(new_from_dump)

    # Methods

    def add_provider(self, metadata, public_key = None, certificate = None):
        errorCode = lassomod.lasso_server_add_provider(self, metadata, public_key, certificate)
        if errorCode:
            raise newError(errorCode, 'lasso_server_add_provider')

    def dump(self):
        return lassomod.lasso_server_dump(self)


class Identity(_ObjectMixin, lassomod.LassoIdentity):
    # Constructors

    def __new__(cls, metadata = None, public_key = None, private_key = None, certificate = None,
                signature_method = signatureMethodRsaSha1):
        self = lassomod.lasso_identity_new(
            metadata, public_key, private_key, certificate, signature_method)
        if self is None:
            raise ErrorInstanceCreationFailed('lasso_identity_new')
        object.__setattr__(self, '__class__', cls)
        return self

    def new_from_dump(cls, dump):
        self = lassomod.lasso_identity_new_from_dump(dump)
        if self is None:
            raise ErrorInstanceCreationFailed('lasso_identity_new_from_dump')
        object.__setattr__(self, '__class__', cls)
        return self
    new_from_dump = classmethod(new_from_dump)

    # Methods

    def dump(self):
        return lassomod.lasso_identity_dump(self)


class Session(_ObjectMixin, lassomod.LassoSession):
    # Constructors

    def __new__(cls, metadata = None, public_key = None, private_key = None, certificate = None,
                signature_method = signatureMethodRsaSha1):
        self = lassomod.lasso_session_new(
            metadata, public_key, private_key, certificate, signature_method)
        if self is None:
            raise ErrorInstanceCreationFailed('lasso_session_new')
        object.__setattr__(self, '__class__', cls)
        return self

    def new_from_dump(cls, dump):
        self = lassomod.lasso_session_new_from_dump(dump)
        if self is None:
            raise ErrorInstanceCreationFailed('lasso_session_new_from_dump')
        object.__setattr__(self, '__class__', cls)
        return self
    new_from_dump = classmethod(new_from_dump)

    # Attributes

    def get_authentication_method(self, remote_providerID = None):
        return lassomod.lasso_session_get_authentication_method(self, remote_providerID)
    authentication_method = property(get_authentication_method)

    # Methods

    def dump(self):
        return lassomod.lasso_session_dump(self)


class _ProfileChild(object):
    """Abstract class for all Lasso objects that inherit from LassoProfile"""

    # Attributes

    def get_identity(self):
        identity = lassomod.lasso_profile_get_identity(self.parent)
        if identity is not None:
            object.__setattr__(identity, '__class__', Identity)
        return identity
    def set_identity(self, identity):
        lassomod.lasso_profile_set_identity(self.parent, identity)
    identity = property(get_identity, set_identity)

    def get_msg_body(self):
        return self.parent.msg_body
    msg_body = property(get_msg_body)

    def get_msg_relayState(self):
        return self.parent.msg_relayState
    msg_relayState = property(get_msg_relayState)

    def get_msg_url(self):
        return self.parent.msg_url
    msg_url = property(get_msg_url)

    def get_nameIdentifier(self):
        return self.parent.nameIdentifier
    nameIdentifier = property(get_nameIdentifier)

    def get_provider_type(self):
        return self.parent.provider_type
    provider_type = property(get_provider_type)

    def set_remote_providerID(self, remote_providerID):
        lassomod.lasso_profile_set_remote_providerID(self.parent, remote_providerID)
    remote_providerID = property(None, set_remote_providerID)

    def get_request(self):
        request_type = self.request_type
        if request_type == messageTypeAuthnRequest:
            request = lassomod.lasso_profile_get_authn_request_ref(self.parent)
            object.__setattr__(request, '__class__', AuthnRequest)
        elif request_type == messageTypeRequest:
            request = lassomod.lasso_profile_get_request_ref(self.parent)
            object.__setattr__(request, '__class__', Request)
        else:
            raise ErrorUnknownRequestType('lasso_profile_get_???_request', request_type)
        return request
    request = property(get_request)

    def get_request_type(self):
        return self.parent.request_type
    request_type = property(get_request_type)

    def get_response(self):
        response_type = self.response_type
        if response_type == messageTypeAuthnResponse:
            response = lassomod.lasso_profile_get_authn_response_ref(self.parent)
            object.__setattr__(response, '__class__', AuthnResponse)
        elif response_type == messageTypeResponse:
            response = lassomod.lasso_profile_get_response_ref(self.parent)
            object.__setattr__(response, '__class__', Response)
        else:
            raise ErrorUnknownResponseType('lasso_profile_get_???_response', response_type)
        return response
    response = property(get_response)

    def set_response_status(self, response_status):
        lassomod.lasso_profile_set_response_status(self.parent, response_status)
    response_status = property(None, set_response_status)

    def get_response_type(self):
        return self.parent.response_type
    response_type = property(get_response_type)

    def get_server(self):
        server = self.parent.server
        if server is not None:
            object.__setattr__(server, '__class__', Server)
        return server
    server = property(get_server)

    def get_session(self):
        session = lassomod.lasso_profile_get_session(self.parent)
        if session is not None:
            object.__setattr__(session, '__class__', Session)
        return session
    def set_session(self, session):
        lassomod.lasso_profile_set_session(self.parent, session)
    session = property(get_session, set_session)

    def is_identity_dirty(self):
        return lassomod.lasso_profile_is_identity_dirty(self.parent)
    identity_dirty = property(is_identity_dirty)

    def is_session_dirty(self):
        return lassomod.lasso_profile_is_session_dirty(self.parent)
    session_dirty = property(is_session_dirty)

    # Methods

    def dump(self):
        return lassomod.lasso_profile_dump(self.parent)

    def set_identity_from_dump(self, dump):
        lassomod.lasso_profile_set_identity_from_dump(self.parent, dump)

    def set_session_from_dump(self, dump):
        lassomod.lasso_profile_set_session_from_dump(self.parent, dump)


class Login(_ObjectMixin, lassomod.LassoLogin, _ProfileChild):
    # Constructors

    def __new__(cls, server):
        self = lassomod.lasso_login_new(server)
        if self is None:
            raise ErrorInstanceCreationFailed('lasso_login_new')
        object.__setattr__(self, '__class__', cls)
        return self

    def new_from_dump(cls, server, dump):
        self = lassomod.lasso_login_new_from_dump(server, dump)
        if self is None:
            raise ErrorInstanceCreationFailed('lasso_login_new_from_dump')
        object.__setattr__(self, '__class__', cls)
        return self
    new_from_dump = classmethod(new_from_dump)

    # Methods

    def accept_sso(self):
        errorCode = lassomod.lasso_login_accept_sso(self)
        if errorCode:
            raise newError(errorCode, 'lasso_login_accept_sso')

    def build_artifact_msg(self, authentication_result, authenticationMethod,
                           reauthenticateOnOrAfter, method):
        errorCode = lassomod.lasso_login_build_artifact_msg(
            self, authentication_result, authenticationMethod, reauthenticateOnOrAfter, method)
        if errorCode:
            raise newError(errorCode, 'lasso_login_build_artifact_msg')

    def build_authn_request_msg(self, remote_providerID):
        errorCode = lassomod.lasso_login_build_authn_request_msg(self, remote_providerID)
        if errorCode:
            raise newError(errorCode, 'lasso_login_build_authn_request_msg')

    def build_authn_response_msg(self, authentication_result, authenticationMethod,
                                 reauthenticateOnOrAfter):
        errorCode = lassomod.lasso_login_build_authn_response_msg(
            self, authentication_result, authenticationMethod, reauthenticateOnOrAfter)
        if errorCode:
            raise newError(errorCode, 'lasso_login_build_authn_response_msg')

    def build_request_msg(self):
        errorCode = lassomod.lasso_login_build_request_msg(self)
        if errorCode:
            raise newError(errorCode, 'lasso_login_build_request_msg')

    def dump(self):
        return lassomod.lasso_login_dump(self)

    def init_authn_request(self):
        errorCode = lassomod.lasso_login_init_authn_request(self)
        if errorCode:
            raise newError(errorCode, 'lasso_login_init_authn_request')

    def init_from_authn_request_msg(self, authn_request_msg, authn_request_method):
        errorCode = lassomod.lasso_login_init_from_authn_request_msg(
            self, authn_request_msg, authn_request_method)
        if errorCode:
            raise newError(errorCode, 'lasso_login_init_from_authn_request_msg')

    def init_request(self, response_msg, response_method):
        errorCode = lassomod.lasso_login_init_request(self, response_msg, response_method)
        if errorCode:
            raise newError(errorCode, 'lasso_login_init_request')

    def must_authenticate(self):
        return lassomod.lasso_login_must_authenticate(self)

    def process_authn_response_msg(self, authn_response_msg):
        errorCode = lassomod.lasso_login_process_authn_response_msg(self, authn_response_msg)
        if errorCode:
            raise newError(errorCode, 'lasso_login_process_authn_response_msg')

    def process_request_msg(self, request_msg):
        errorCode = lassomod.lasso_login_process_request_msg(self, request_msg)
        if errorCode:
            raise newError(errorCode, 'lasso_login_process_request_msg')

    def process_response_msg(self, response_msg):
        errorCode = lassomod.lasso_login_process_response_msg(self, response_msg)
        if errorCode:
            raise newError(errorCode, 'lasso_login_process_response_msg')


class Logout(_ObjectMixin, lassomod.LassoLogout, _ProfileChild):
    # Constructors

    def __new__(cls, server, provider_type):
        self = lassomod.lasso_logout_new(server, provider_type)
        if self is None:
            raise ErrorInstanceCreationFailed('lasso_logout_new')
        object.__setattr__(self, '__class__', cls)
        return self
    new = classmethod(__new__)

    # Methods

    def build_request_msg(self):
        errorCode = lassomod.lasso_logout_build_request_msg(self)
        if errorCode:
            raise newError(errorCode, 'lasso_logout_build_request_msg')

    def build_response_msg(self):
        errorCode = lassomod.lasso_logout_build_response_msg(self)
        if errorCode:
            raise newError(errorCode, 'lasso_logout_build_response_msg')

    def get_next_providerID(self):
        return lassomod.lasso_logout_get_next_providerID(self)

    def init_request(self, remote_providerID = None):
        errorCode = lassomod.lasso_logout_init_request(self, remote_providerID)
        if errorCode:
            raise newError(errorCode, 'lasso_logout_init_request')

    def process_request_msg(self, request_msg, request_method):
        errorCode = lassomod.lasso_logout_process_request_msg(self, request_msg, request_method)
        if errorCode:
            raise newError(errorCode, 'lasso_logout_process_request_msg')

    def process_response_msg(self, response_msg, response_method):
        errorCode = lassomod.lasso_logout_process_response_msg(
            self, response_msg, response_method)
        if errorCode:
            raise newError(errorCode, 'lasso_logout_process_response_msg')

    def validate_request(self):
        errorCode = lassomod.lasso_logout_validate_request(self)
        if errorCode:
            raise newError(errorCode, 'lasso_logout_validate_request')


class Lecp(_ObjectMixin, lassomod.LassoLecp):
    # Attributes

    def get_msg_body(self):
        return self.parent.parent.msg_body
    msg_body = property(get_msg_body)

    def get_msg_url(self):
        return self.parent.parent.msg_url
    msg_url = property(get_msg_url)

    def get_request(self):
        request_type = self.request_type
        if request_type == messageTypeAuthnRequest:
            request = lassomod.lasso_profile_get_authn_request_ref(self.parent.parent)
            object.__setattr__(request, '__class__', AuthnRequest)
        elif request_type == messageTypeRequest:
            request = lassomod.lasso_profile_get_request_ref(self.parent.parent)
            object.__setattr__(request, '__class__', Request)
        else:
            raise ErrorUnknownRequestType('lasso_profile_get_???_request', request_type)
        return request
    request = property(get_request)

    def get_request_type(self):
        return self.parent.parent.request_type
    request_type = property(get_request_type)

    # Constructors

    def __new__(cls, server):
        self = lassomod.lasso_lecp_new(server)
        if self is None:
            raise ErrorInstanceCreationFailed('lasso_lecp_new')
        object.__setattr__(self, '__class__', cls)
        return self

    def new_from_dump(cls, server, dump):
        self = lassomod.lasso_lecp_new_from_dump(server, dump)
        if self is None:
            raise ErrorInstanceCreationFailed('lasso_lecp_new_from_dump')
        object.__setattr__(self, '__class__', cls)
        return self
    new_from_dump = classmethod(new_from_dump)

    # Methods

    def build_authn_request_envelope_msg(self):
        errorCode = lassomod.lasso_lecp_build_authn_request_envelope_msg(self)
        if errorCode:
            raise newError(errorCode, 'lasso_lecp_build_authn_request_envelope_msg')

    def build_authn_request_msg(self, remote_providerID):
        errorCode = lassomod.lasso_lecp_build_authn_request_msg(self, remote_providerID)
        if errorCode:
            raise newError(errorCode, 'lasso_lecp_build_authn_request_msg')

    def build_authn_response_envelope_msg(self, authentication_result, authenticationMethod,
                                          reauthenticateOnOrAfter):
        errorCode = lassomod.lasso_lecp_build_authn_response_envelope_msg(
            self, authentication_result, authenticationMethod, reauthenticateOnOrAfter)
        if errorCode:
            raise newError(errorCode, 'lasso_lecp_build_authn_response_envelope_msg')

    def build_authn_response_msg(self):
        errorCode = lassomod.lasso_lecp_build_authn_response_msg(self)
        if errorCode:
            raise newError(errorCode, 'lasso_lecp_build_authn_response_msg')

    def init_authn_request(self):
        errorCode = lassomod.lasso_lecp_init_authn_request(self)
        if errorCode:
            raise newError(errorCode, 'lasso_lecp_init_authn_request')

    def init_from_authn_request_msg(self, authn_request_msg, authn_request_method):
        errorCode = lassomod.lasso_lecp_init_from_authn_request_msg(
            self, authn_request_msg, authn_request_method)
        if errorCode:
            raise newError(errorCode, 'lasso_lecp_init_from_authn_request_msg')

    def process_authn_request_envelope_msg(self, request_msg):
        errorCode = lassomod.lasso_lecp_process_authn_request_envelope_msg(self, request_msg)
        if errorCode:
            raise newError(errorCode, 'lasso_lecp_process_authn_request_envelope_msg')

    def process_authn_response_envelope_msg(self, response_msg):
        errorCode = lassomod.lasso_lecp_process_authn_response_envelope_msg(self, response_msg)
        if errorCode:
            raise newError(errorCode, 'lasso_lecp_process_authn_response_envelope_msg')

    def set_identity_from_dump(self, dump):
        lassomod.lasso_profile_set_identity_from_dump(self.parent.parent, dump)

    def set_session_from_dump(self, dump):
        lassomod.lasso_profile_set_session_from_dump(self.parent.parent, dump)


################################################################################
# Module Initialization
################################################################################


_initialized = False

def init():
    """Initialize Lasso library."""
    global _initialized
    if _initialized:
        raise ErrorLassoAlreadyInitialized()
    _initialized = True
    lassomod.lasso_init()


def shutdown():
    """Shutdown Lasso Library."""
    global _initialized
    if not _initialized:
        raise ErrorLassoNotInitialized()
    _initialized = False
    lassomod.lasso_shutdown()


################################################################################
# Module Test
################################################################################


if __name__ == '__main__':
    import os

    init()
    dataDirectoryPath = '../tests/data'
    
    server = Server(
        os.path.join(dataDirectoryPath, 'sp1-la/metadata.xml'),
        os.path.join(dataDirectoryPath, 'sp1-la/public-key.pem'),
        os.path.join(dataDirectoryPath, 'sp1-la/private-key-raw.pem'),
        os.path.join(dataDirectoryPath, 'sp1-la/certificate.pem'),
        signatureMethodRsaSha1)
    server.add_provider(
        os.path.join(dataDirectoryPath, 'idp1-la/metadata.xml'),
        os.path.join(dataDirectoryPath, 'idp1-la/public-key.pem'),
        os.path.join(dataDirectoryPath, 'idp1-la/certificate.pem'))
    login = Login(server)
    login.init_authn_request()
    login.request.set_isPassive(False)
    login.request.set_nameIDPolicy(libNameIDPolicyTypeFederated)
    login.request.set_consent(libConsentObtained)
    login.build_authn_request_msg('https://idp1/metadata')
    print login.msg_url
    shutdown()
else:
    if not _initialized:
        init()

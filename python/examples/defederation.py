#! /usr/bin/env python

import sys
sys.path.insert(0, '../')
import lasso

lasso.init()

# servers :
spserver = lasso.Server.new("../../examples/sp.xml",
			    "../../examples/rsapub.pem", "../../examples/rsakey.pem", "../../examples/rsacert.pem",
			    lasso.signatureMethodRsaSha1)
spserver.add_provider("../../examples/idp.xml", None, None)

idpserver = lasso.Server.new("../../examples/idp.xml",
			    "../../examples/rsapub.pem", "../../examples/rsakey.pem", "../../examples/rsacert.pem",
			    lasso.signatureMethodRsaSha1)
spserver.add_provider("../../examples/sp.xml", None, None)


# users :
spuser_dump = "<LassoUser><LassoIdentities><LassoIdentity RemoteProviderID=\"https://identity-provider:2003/liberty-alliance/metadata\"><LassoRemoteNameIdentifier><NameIdentifier NameQualifier=\"qualifier.com\" Format=\"federated\">1111111111111111111111111</NameIdentifier></LassoRemoteNameIdentifier></LassoIdentity></LassoIdentities></LassoUser>"
spuser = lasso.User.new_from_dump(spuser_dump)

idpuser_dump = "<LassoUser><LassoIdentities><LassoIdentity RemoteProviderID=\"https://service-provider:2003/liberty-alliance/metadata\"><LassoLocalNameIdentifier><NameIdentifier NameQualifier=\"qualifier.com\" Format=\"federated\">1111111111111111111111111</NameIdentifier></LassoLocalNameIdentifier></LassoIdentity></LassoIdentities></LassoUser>"
idpuser = lasso.User.new_from_dump(idpuser_dump)


# sp federation termination :
spdefederation = lasso.FederationTermination.new(spserver, spuser, lasso.providerTypeSp)
spdefederation.init_notification("https://identity-provider:2003/liberty-alliance/metadata")
spdefederation.build_notification_msg()
print 'url : ', spdefederation.msg_url
print 'body : ', spdefederation.msg_body

sys.exit(1)

# idp federation termination :
print "---------------------------------------------------------"
print " At identity provider "
idpdefederation = lasso.FederationTermination.new(idpserver, idpuser, lasso.providerTypeIdp)
idpdefederation.process_notification_msg(spdefederation.msg_body, lasso.httpMethodSoap)
idpdefederation.build_response_msg()
print 'url : ', idpdefederation.msg_url
print 'body : ', idpdefederation.msg_body


print 'End of federation termination'

lasso.shutdown()

# Example SP Python code to get attributes from an assertion

from six import print_

for attribute in assertion.attributeStatement[0].attribute:
    if attribute.name == lasso.SAML2_ATTRIBUTE_NAME_EPR:
        continue
    print_('attribute : ' + attribute.name)
    for value in attribute.attributeValue:
        print_('  value : ' + value.any[0].content)

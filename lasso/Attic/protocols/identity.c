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

#include <lasso/protocols/identity.h>

/*****************************************************************************/
/* public methods                                                            */
/*****************************************************************************/

xmlChar *
lasso_identity_dump(LassoIdentity *identity)
{
  LassoNode *identity_node, *nameIdentifier;
  LassoNodeClass *identity_class, *class;

  identity_node = lasso_node_new();
  identity_class = LASSO_NODE_GET_CLASS(identity_node);
  identity_class->set_name(identity_node, "Identity");

  /* set the remote providerID */
  identity_class->set_prop(identity_node, "RemoteProviderID", identity->remote_providerID);

  /* add the remote name identifier */
  if(identity->remote_nameIdentifier){
    nameIdentifier = lasso_node_new();
    class = LASSO_NODE_GET_CLASS(nameIdentifier);
    class->set_name(nameIdentifier, "RemoteNameIdentifier");
    class->add_child(nameIdentifier, identity->remote_nameIdentifier, FALSE);
    identity_class->add_child(identity_node, nameIdentifier, FALSE);
  }

  /* add the local name identifier */
  if(identity->local_nameIdentifier){
    nameIdentifier = lasso_node_new();
    class = LASSO_NODE_GET_CLASS(nameIdentifier);
    class->set_name(nameIdentifier, "LocalNameIdentifier");
    class->add_child(nameIdentifier, identity->local_nameIdentifier, FALSE);
    identity_class->add_child(identity_node, nameIdentifier, FALSE);
  }

  return(lasso_node_export(identity_node));
}

void
lasso_identity_set_local_nameIdentifier(LassoIdentity *identity,
					LassoNode     *nameIdentifier)
{
  identity->local_nameIdentifier = nameIdentifier;
}

void
lasso_identity_set_remote_nameIdentifier(LassoIdentity *identity,
					 LassoNode     *nameIdentifier)
{
  identity->remote_nameIdentifier = nameIdentifier;
}

LassoNode *
lasso_identity_get_local_nameIdentifier(LassoIdentity *identity)
{
  return(identity->local_nameIdentifier);
}

LassoNode *
lasso_identity_get_remote_nameIdentifier(LassoIdentity *identity)
{
  return(identity->remote_nameIdentifier);
}


/*****************************************************************************/
/* instance and class init functions                                         */
/*****************************************************************************/

static void
lasso_identity_instance_init(LassoIdentity *identity)
{
  identity->local_nameIdentifier  = NULL;
  identity->remote_nameIdentifier = NULL;
}

static void
lasso_identity_class_init(LassoIdentityClass *klass)
{

}

GType lasso_identity_get_type() {
  static GType this_type = 0;

  if (!this_type) {
    static const GTypeInfo this_info = {
      sizeof (LassoIdentityClass),
      NULL,
      NULL,
      (GClassInitFunc) lasso_identity_class_init,
      NULL,
      NULL,
      sizeof(LassoIdentity),
      0,
      (GInstanceInitFunc) lasso_identity_instance_init,
    };
    
    this_type = g_type_register_static(G_TYPE_OBJECT,
				       "LassoIdentity",
				       &this_info, 0);
  }
  return this_type;
}

LassoIdentity*
lasso_identity_new(gchar *remote_providerID)
{
  LassoIdentity *identity;

  identity = LASSO_IDENTITY(g_object_new(LASSO_TYPE_IDENTITY, NULL));

  identity->remote_providerID = g_strdup(remote_providerID);

  return(identity);
}

LassoIdentity*
lasso_identity_new_from_dump(xmlChar *dump)
{
  LassoIdentity *identity;

  identity = LASSO_IDENTITY(g_object_new(LASSO_TYPE_IDENTITY, NULL));

  return(identity);
}

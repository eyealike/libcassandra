/*
 * LibCassandra
 * Copyright (C) 2010 Padraig O'Sullivan
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license. See
 * the COPYING file in the parent directory for full text.
 */

#ifndef __LIBCASSANDRA_KEYSPACE_FACTORY_H
#define __LIBCASSANDRA_KEYSPACE_FACTORY_H

#include <string>
#include <map>
#include <tr1/memory>

namespace libcassandra
{

class Keyspace;
class Cassandra;

class KeyspaceFactory
{

public:

  KeyspaceFactory();
  ~KeyspaceFactory();

  /**
   * @return a shared ptr which points to a Keyspace class
   */
  std::tr1::shared_ptr<Keyspace> create(Cassandra *client,
                                        const std::string &name,
                                        const std::map< std::string, std::map<std::string, std::string> > &descrip,
                                        org::apache::cassandra::ConsistencyLevel readLevel,
                                        org::apache::cassandra::ConsistencyLevel writeLevel );

};

} /* end namespace libcassandra */

#endif /* __LIBCASSANDRA_KEYSPACE_FACTORY_H */

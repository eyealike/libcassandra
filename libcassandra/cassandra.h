/*
 * LibCassandra
 * Copyright (C) 2010 Padraig O'Sullivan
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license. See
 * the COPYING file in the parent directory for full text.
 */

#ifndef __LIBCASSANDRA_CASSANDRA_H
#define __LIBCASSANDRA_CASSANDRA_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <tr1/memory>

#include "../libgenthrift/cassandra_types.h"

namespace org
{
namespace apache
{
namespace cassandra
{
class CassandraClient;
}
}
}

namespace libcassandra
{

class Keyspace;

class Cassandra
{

public:

  Cassandra(org::apache::cassandra::CassandraClient *in_thrift_client,
            const std::string &in_host,
            int in_port);
  ~Cassandra();

  enum FailoverPolicy
  {
    FAIL_FAST= 0,
    ON_FAIL_TRY_ONE_NEXT_AVAILABLE,
    ON_FAIL_TRY_ALL_AVAILABLE
  };

  /**
   * @return the underlying cassandra thrift client.
   */
  org::apache::cassandra::CassandraClient *getCassandra();

  /**
   * @return all the keyspace names.
   */
  std::map<std::string, org::apache::cassandra::KsDef> getKeyspaces();

  /**
   * @return the keyspace with the given name.
   */
  std::tr1::shared_ptr<Keyspace> getKeyspace(const std::string &name);

  /**
   * @return the keyspace with the given name at the given consistency level.
   */
  std::tr1::shared_ptr<Keyspace> getKeyspace(
	  const std::string &name,
	  const org::apache::cassandra::ConsistencyLevel::type readLevel,
	  const org::apache::cassandra::ConsistencyLevel::type writeLevel);

  /**
   * Remove the given keyspace.
   */
  void removeKeyspace(std::tr1::shared_ptr<Keyspace> k);

  /**
   * @return the target server cluster name.
   */
  std::string getClusterName();

  /**
   * @return the server version.
   */
  std::string getServerVersion();

  /**
   * @param[in] fresh whether to refresh the token map or not
   * @return a map of the tokens in this cluster
   */
  std::map<std::string, std::string> getTokenMap(const std::string & keyspaceName, bool fresh);

  /**
   * @return hostname
   */
  std::string getHost();

  /**
   * @return port number
   */
  int getPort() const;

private:

  /**
   * Creates a unique map name for the keyspace and its consistency levels
   */
  std::string buildKeyspaceMapName(std::string keyspace, int readLevel, int writeLevel );

  org::apache::cassandra::CassandraClient *thrift_client;
  std::string host;
  int port;
  std::string cluster_name;
  std::string server_version;
  std::string config_file;
  std::map<std::string, org::apache::cassandra::KsDef> key_spaces;
  std::map<std::string, std::string> token_map;
  std::map<std::string, std::tr1::shared_ptr<Keyspace> > keyspace_map;

  Cassandra(const Cassandra&);
  Cassandra &operator=(const Cassandra&);

};

} /* end namespace libcassandra */

#endif /* __LIBCASSANDRA_CASSANDRA_H */

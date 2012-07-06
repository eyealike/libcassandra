/*
 * LibCassandra
 * Copyright (C) 2010 Padraig O'Sullivan
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license. See
 * the COPYING file in the parent directory for full text.
 */

#include <string>
#include <set>
#include <sstream>

#include "../libgenthrift/Cassandra.h"

#include "cassandra.h"
#include "keyspace.h"
#include "exception.h"

using namespace std;
using namespace org::apache::cassandra;
using namespace libcassandra;

/* utility functions */

template<class T>
inline string toString(const T &tt)
{
  stringstream ss;
  ss << tt;
  return ss.str();
}

Cassandra::Cassandra(CassandraClient *in_thrift_client,
                     const string &in_host,
                     int in_port)
  :
    thrift_client(in_thrift_client),
    host(in_host),
    port(in_port),
    cluster_name(),
    server_version(),
    config_file(),
    key_spaces(),
    token_map(),
    keyspace_map(),
    active_keyspace(0)
{}


Cassandra::~Cassandra()
{
  delete thrift_client;
}


CassandraClient *Cassandra::getCassandra( const Keyspace * keyspace )
{
  if( keyspace != active_keyspace ) {
	  thrift_client->set_keyspace( keyspace->getName() );
	  active_keyspace = keyspace;
  }
  return thrift_client;
}


map<string, org::apache::cassandra::KsDef> Cassandra::getKeyspaces()
{
  if (key_spaces.empty())
  {
	vector<org::apache::cassandra::KsDef> ksDefs;
    thrift_client->describe_keyspaces(ksDefs);
    for( vector<org::apache::cassandra::KsDef>::iterator ksDef = ksDefs.begin(); ksDef < ksDefs.end() ; ++ksDef ) {
       key_spaces[ ksDef->name ] = *ksDef;
    }
  }
  return key_spaces;
}


tr1::shared_ptr<Keyspace> Cassandra::getKeyspace(const string &name)
{
  return getKeyspace(name, ConsistencyLevel::LOCAL_QUORUM, ConsistencyLevel::LOCAL_QUORUM );
}


tr1::shared_ptr<Keyspace> Cassandra::getKeyspace(const string &name,
                                                 const ConsistencyLevel::type readLevel,
                                                 const ConsistencyLevel::type writeLevel )
{
  string keymap_name= buildKeyspaceMapName(name, readLevel, writeLevel );
  map<string, tr1::shared_ptr<Keyspace> >::iterator key_it= keyspace_map.find(keymap_name);
  if (key_it == keyspace_map.end())
  {
    map<string, org::apache::cassandra::KsDef>::iterator it= getKeyspaces().find(name);
    if (it != key_spaces.end())
    {
      tr1::shared_ptr<Keyspace> ret(new Keyspace(this, name, readLevel, writeLevel));
      keyspace_map[keymap_name]= ret;
    }
    else
    {
      /* throw an exception */
      throw(NotFoundException());
    }
  }
  return keyspace_map[keymap_name];
}


void Cassandra::removeKeyspace(tr1::shared_ptr<Keyspace> k)
{
  string keymap_name= buildKeyspaceMapName(k->getName(), k->getReadConsistencyLevel(), k->getWriteConsistencyLevel());
  keyspace_map.erase(keymap_name);
}


string Cassandra::getClusterName()
{
  if (cluster_name.empty())
  {
    thrift_client->describe_cluster_name(cluster_name);
  }
  return cluster_name;
}


string Cassandra::getServerVersion()
{
  if (server_version.empty())
  {
    thrift_client->describe_version(server_version);
  }
  return server_version;
}


map<string, string> Cassandra::getTokenMap(const string & keyspaceName, bool fresh)
{
  if (token_map.empty() || fresh)
  {
	vector<TokenRange> ranges;
	thrift_client->describe_ring( ranges, keyspaceName);
    token_map.clear();
    for( vector<TokenRange>::iterator range = ranges.begin() ; range < ranges.end() ; ++range ) {
    	token_map[ range->end_token ] = *(range->endpoints.begin());
    }

  }
  return token_map;
}


string Cassandra::getHost()
{
  return host;
}


int Cassandra::getPort() const
{
  return port;
}


string Cassandra::buildKeyspaceMapName(string keyspace, int readLevel, int writeLevel )
{
  keyspace.append("[");
  keyspace.append(toString(readLevel));
  keyspace.append(",");
  keyspace.append(toString(writeLevel));
  keyspace.append("]");
  return keyspace;
}

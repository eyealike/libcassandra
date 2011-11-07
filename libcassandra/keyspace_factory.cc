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

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../libgenthrift/Cassandra.h"

#include "cassandra.h"
#include "keyspace.h"
#include "keyspace_factory.h"

using namespace libcassandra;
using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace org::apache::cassandra;
using namespace boost;


KeyspaceFactory::KeyspaceFactory() {}

KeyspaceFactory::~KeyspaceFactory() {}


tr1::shared_ptr<Keyspace> KeyspaceFactory::create(Cassandra *client,
                                                  const string &name,
                                                  ConsistencyLevel::type readLevel,
                                                  ConsistencyLevel::type writeLevel)
{
  tr1::shared_ptr<Keyspace> ret(new Keyspace(client, name, readLevel, writeLevel));
  return ret;
}


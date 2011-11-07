/*
 * LibCassandra
 * Copyright (C) 2010 Padraig O'Sullivan
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license. See
 * the COPYING file in the parent directory for full text.
 */

#include <time.h>

#include <string>
#include <map>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../libgenthrift/Cassandra.h"

#include "cassandra.h"
#include "keyspace.h"
#include "exception.h"

using namespace libcassandra;
using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace org::apache::cassandra;
using namespace boost;


Keyspace::Keyspace(Cassandra *in_client,
                   const string &in_name,
                   const ConsistencyLevel::type in_readLevel,
                   const ConsistencyLevel::type in_writeLevel)
  :
    client(in_client),
    name(in_name),
    readLevel(in_readLevel),
    writeLevel(in_writeLevel)
{
	client->getCassandra()->set_keyspace( in_name );
}


void Keyspace::insertColumn(const string &key,
                            const string &column_family,
                            const string &super_column_name,
                            const string &column_name,
                            const string &value)
{
  ColumnParent parent;
  parent.column_family.assign( column_family );
  if (! super_column_name.empty())
  {
    parent.super_column.assign(super_column_name);
    parent.__isset.super_column= true;
  }

  Column column;
  column.name.assign( column_name );
  column.value.assign( value );
  column.timestamp = createTimestamp();
  column.__isset.timestamp = true;
  column.__isset.value = true;

  client->getCassandra()->insert( key, parent, column, writeLevel);
}


void Keyspace::insertColumn(const string &key,
                            const string &column_family,
                            const string &column_name,
                            const string &value)
{
  insertColumn(key, column_family, "", column_name, value);
}


void Keyspace::remove(const string &key,
                      const ColumnPath &col_path)
{
  client->getCassandra()->remove(key, col_path, createTimestamp(), writeLevel);
}


void Keyspace::remove(const string &key,
                      const string &column_family,
                      const string &super_column_name,
                      const string &column_name)
{
  ColumnPath col_path;
  col_path.column_family.assign(column_family);
  if (! super_column_name.empty()) 
  {
    col_path.super_column.assign(super_column_name);
    col_path.__isset.super_column= true;
  }
  if (! column_name.empty()) 
  {
    col_path.column.assign(column_name);
    col_path.__isset.column= true;
  }
  remove(key, col_path);
}


void Keyspace::removeColumn(const string &key,
                            const string &column_family,
                            const string &super_column_name,
                            const string &column_name)
{
  remove(key, column_family, super_column_name, column_name);
}


void Keyspace::removeSuperColumn(const string &key,
                                 const string &column_family,
                                 const string &super_column_name)
{
  remove(key, column_family, super_column_name, "");
}


Column Keyspace::getColumn(const string &key,
                           const string &column_family,
                           const string &super_column_name,
                           const string &column_name)
{
  ColumnPath col_path;
  col_path.column_family.assign(column_family);
  if (! super_column_name.empty()) 
  {
    col_path.super_column.assign(super_column_name);
    col_path.__isset.super_column= true;
  }
  col_path.column.assign(column_name);
  col_path.__isset.column= true;
  ColumnOrSuperColumn cosc;
  client->getCassandra()->get(cosc, key, col_path, readLevel);
  if (cosc.column.name.empty())
  {
    /* throw an exception */
    throw(InvalidRequestException());
  }
  return cosc.column;
}

Column Keyspace::getColumn(const string &key,
                           const string &column_family,
                           const string &column_name)
{
  return getColumn(key, column_family, "", column_name);
}


string Keyspace::getColumnValue(const string &key,
                                const string &column_family,
                                const string &super_column_name,
                                const string &column_name)
{
  return getColumn(key, column_family, super_column_name, column_name).value;
}


string Keyspace::getColumnValue(const string &key,
                                const string &column_family,
                                const string &column_name)
{
	return getColumn(key, column_family, column_name).value;
}


SuperColumn Keyspace::getSuperColumn(const string &key,
                                     const string &column_family,
                                     const string &super_column_name)
{
  ColumnPath col_path;
  col_path.column_family.assign(column_family);
  col_path.super_column.assign(super_column_name);
  /* this is ugly but thanks to thrift is needed */
  col_path.__isset.super_column= true;
  ColumnOrSuperColumn cosc;
  client->getCassandra()->get(cosc, key, col_path, readLevel);
  if (cosc.super_column.name.empty())
  {
    /* throw an exception */
    throw(InvalidRequestException());
  }
  return cosc.super_column;
}


vector<Column> Keyspace::getSliceNames(const string &key,
                                       const ColumnParent &col_parent,
                                       SlicePredicate &pred)
{
  vector<ColumnOrSuperColumn> ret_cosc;
  vector<Column> result;
  /* damn you thrift! */
  pred.__isset.column_names= true;
  client->getCassandra()->get_slice(ret_cosc, key, col_parent, pred, readLevel);
  for (vector<ColumnOrSuperColumn>::iterator it= ret_cosc.begin();
       it != ret_cosc.end();
       ++it)
  {
    if (! (*it).column.name.empty())
    {
      result.push_back((*it).column);
    }
  }
  return result;
}


vector<Column> Keyspace::getSliceRange(const string &key,
                                       const ColumnParent &col_parent,
                                       SlicePredicate &pred)
{
  vector<ColumnOrSuperColumn> ret_cosc;
  vector<Column> result;
  /* damn you thrift! */
  pred.__isset.slice_range= true;
  client->getCassandra()->get_slice(ret_cosc, key, col_parent, pred, readLevel);
  for (vector<ColumnOrSuperColumn>::iterator it= ret_cosc.begin();
       it != ret_cosc.end();
       ++it)
  {
    if (! (*it).column.name.empty())
    {
      result.push_back((*it).column);
    }
  }
  return result;
}


void Keyspace::getRangeSlicesRaw(vector<KeySlice> &key_slices,
								const ColumnParent &col_parent,
								const SlicePredicate &pred,
								const KeyRange &range)
{
	client->getCassandra()->get_range_slices(key_slices,
	                                          col_parent,
	                                          pred,
	                                          range,
	                                          readLevel);
}

void Keyspace::multigetSliceRaw(std::map<std::string, std::vector<ColumnOrSuperColumn> > & _return,
								const ColumnParent &col_parent,
								const SlicePredicate &pred,
								const std::vector<std::string> &keys)
{
	client->getCassandra()->multiget_slice(_return,
											keys,
											col_parent,
											pred,
											readLevel);
}

map<string, vector<Column> > Keyspace::getRangeSlices(const ColumnParent &col_parent,
                                                     const SlicePredicate &pred,
                                                     const KeyRange &range)
{
  map<string, vector<Column> > ret;
  vector<KeySlice> key_slices;
  client->getCassandra()->get_range_slices(key_slices,
                                          col_parent,
                                          pred,
                                          range,
                                          readLevel);
  if (! key_slices.empty())
  {
    for (vector<KeySlice>::iterator it= key_slices.begin();
         it != key_slices.end();
         ++it)
    {
      ret.insert(make_pair((*it).key, getColumnList((*it).columns)));
    }
  }
  return ret;
}


map<string, vector<SuperColumn> > Keyspace::getSuperRangeSlices(const ColumnParent &col_parent,
                                                               const SlicePredicate &pred,
                                                               const KeyRange &range)
{
  map<string, vector<SuperColumn> > ret;
  vector<KeySlice> key_slices;
  client->getCassandra()->get_range_slices(key_slices,
                                          col_parent,
                                          pred,
                                          range,
                                          readLevel);
  if (! key_slices.empty())
  {
    for (vector<KeySlice>::iterator it= key_slices.begin();
         it != key_slices.end();
         ++it)
    {
      ret.insert(make_pair((*it).key, getSuperColumnList((*it).columns)));
    }
  }
  return ret;
}


vector<Column> Keyspace::getColumnList(vector<ColumnOrSuperColumn> &cols)
{
  vector<Column> ret(cols.size());
  for (vector<ColumnOrSuperColumn>::iterator it= cols.begin();
       it != cols.end();
       ++it)
  {
    ret.push_back((*it).column);
  }
  return ret;
}


vector<SuperColumn> Keyspace::getSuperColumnList(vector<ColumnOrSuperColumn> &cols)
{
  vector<SuperColumn> ret(cols.size());
  for (vector<ColumnOrSuperColumn>::iterator it= cols.begin();
       it != cols.end();
       ++it)
  {
    ret.push_back((*it).super_column);
  }
  return ret;
}


int32_t Keyspace::getCount(const string &key, const ColumnParent &col_parent, const SlicePredicate &predicate )
{
  return (client->getCassandra()->get_count(key, col_parent, predicate, readLevel));
}


string Keyspace::getName()
{
  return name;
}


ConsistencyLevel::type Keyspace::getReadConsistencyLevel() const
{
  return readLevel;
}


ConsistencyLevel::type Keyspace::getWriteConsistencyLevel() const
{
  return writeLevel;
}

int64_t Keyspace::createTimestamp()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (int64_t) tv.tv_sec * 1000000 + (int64_t) tv.tv_usec;
}

/* 
 * File:   sql.h
 * Author: Simon
 *
 * Created on 13 June 2011, 1:20 PM
 */
#include <list>
#ifndef SQL_H
#define	SQL_H

int invalidateRoutes(sqlite3 *db, ospf_lsa_t *lsa, list<in_addr> routes);
int insert_router(sqlite3 *db, ospf_t *ospf, ospf_lsa_t *lsa);
int insert_external_route(sqlite3 *db, ospf_lsa_t *lsa, ospf_as_external_lsa_t *external);
int insert_router_interface(sqlite3 *db, ospf_lsa_t *lsa, ospf_link_t *link);
int check_router_interface(sqlite3 *db, in_addr id, std::list<in_addr> links);
int update_external_route(sqlite3 *db, ospf_lsa_t *lsa, ospf_as_external_lsa_t *external);
#endif	/* SQL_H */


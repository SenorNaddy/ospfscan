#include "591.h"

int insert_router(sqlite3 *db, ospf_t *ospf, ospf_lsa_t *lsa)
{
    char sql[1024];
    snprintf(sql, sizeof(sql), "INSERT OR REPLACE INTO routers (router_id, area) VALUES ('%s',%i)", inet_ntoa(lsa->ospf_lsa_adv_router),ospf->ospf_area.s_addr);
    char *error;
    int res = sqlite3_exec(db, sql, NULL, NULL, &error);
    if(error != NULL) {
        printf("ERROR: %s\n", error);
    }
    sqlite3_free(error);
    return res;
}
int check_router_interface(sqlite3 *db, in_addr id, list<in_addr> links)
{
	char query[1024] = "(";
	list<in_addr>::iterator it;
    char adv_router[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &id, adv_router, INET_ADDRSTRLEN);
    char lsa_ls_id[INET_ADDRSTRLEN];
    for(it=links.begin(); it != links.end(); it++)
	{
		if(it != links.begin())
		{
			strcat(query, ",");
		}
		inet_ntop(AF_INET, &(*it), lsa_ls_id, INET_ADDRSTRLEN);
		strcat(query, "'");
		strcat(query, lsa_ls_id);
		strcat(query, "'");
	}
	strcat(query, ")");
	char sql[2048] = "";
	snprintf(sql, sizeof(sql), "UPDATE interfaces SET valid = '0' WHERE router_id='%s' AND interface_id NOT IN %s",
	            adv_router, query);
	printf("SQL: %s\n", sql);
	char *error;
	int res = sqlite3_exec(db, sql, NULL, NULL, &error);
	if(error != NULL) {
		printf("ERROR: %s\n", error);
	}
	sqlite3_free(error);
	return res;

}
int insert_external_route(sqlite3 *db, ospf_lsa_t *lsa, ospf_as_external_lsa_t *external)
{
    char sql[1024] = "";
    char adv_router[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(lsa->ospf_lsa_adv_router), adv_router, INET_ADDRSTRLEN);
    char lsa_ls_id[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(lsa->ospf_lsa_ls_id), lsa_ls_id, INET_ADDRSTRLEN);
    char network[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(external->ospf_as_external_network), network, INET_ADDRSTRLEN);

    snprintf(sql, sizeof(sql), "SELECT count(*) FROM external WHERE router_id='%s' AND prefix='%s' AND prefix_mask='%s'", adv_router, lsa_ls_id, network);
	printf("%s\n", sql);
    char **results;
	int rows;
	int cols;
	char *err;
	//printf("%s\n", sql);
	int res = sqlite3_get_table(db, sql, &results, &rows, &cols, &err);

	if(atoi(results[1]) == 0)
	{

		snprintf(sql, sizeof(sql), "INSERT OR REPLACE INTO external (router_id, prefix, prefix_mask) VALUES ('%s','%s','%s')",
				adv_router, lsa_ls_id, network);
		char *error;
		res = sqlite3_exec(db, sql, NULL, NULL, &error);
		if(error != NULL) {
			printf("ERROR: %s\n", error);
		}
		sqlite3_free(error);
    }
    sqlite3_free_table(results);
    return res;
}
int update_external_route(sqlite3 *db, ospf_lsa_t *lsa, ospf_as_external_lsa_t *external)
{
	char sql[1024] = "";
	    char adv_router[INET_ADDRSTRLEN];
	    inet_ntop(AF_INET, &(lsa->ospf_lsa_adv_router), adv_router, INET_ADDRSTRLEN);
	    char lsa_ls_id[INET_ADDRSTRLEN];
	    inet_ntop(AF_INET, &(lsa->ospf_lsa_ls_id), lsa_ls_id, INET_ADDRSTRLEN);
	    char network[INET_ADDRSTRLEN];
	    inet_ntop(AF_INET, &(external->ospf_as_external_network), network, INET_ADDRSTRLEN);

	    snprintf(sql, sizeof(sql), "UPDATE external SET expiry = '%i' WHERE router_id = '%s' AND prefix = '%s' AND prefix_mask = '%s'",
	            (int)(time(NULL) + htons(lsa->ospf_lsa_age)), adv_router, lsa_ls_id, network);
	    printf("SQL: %s\n", sql);
	    printf("Current Time: %i \t Expiry Time: %i\n", (int)time(NULL), (int)(time(NULL) + htons(lsa->ospf_lsa_age)));
	    char *error;
	    int res = sqlite3_exec(db, sql, NULL, NULL, &error);

	    if(error != NULL) {
	        printf("ERROR: %s\n", error);
	    }
	    sqlite3_free(error);
	    return res;
}
int invalidateRoutes(sqlite3 *db, ospf_lsa_t *lsa, list<in_addr> routes)
{
	return 0;
}
}
int numberOfResults(sqlite3 *db, char *zSQL)
{
    int rows;
    sqlite3_get_table(db, zSQL, NULL, &rows, NULL, NULL);
    return rows;
}
int insert_router_interface(sqlite3 *db, ospf_lsa_t *lsa, ospf_link_t *link)
{
    char sql[1024] = "";
    char adv_router[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(lsa->ospf_lsa_adv_router), adv_router, INET_ADDRSTRLEN);
    char lsa_link_id[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(link->ospf_router_lsa_link_id), lsa_link_id, INET_ADDRSTRLEN);

    snprintf(sql, sizeof(sql), "SELECT count(*) FROM interfaces WHERE router_id='%s' AND interface_id='%s'", adv_router, lsa_link_id);
    char **results;
    int rows;
    int cols;
    char *err;
    //printf("%s\n", sql);
    int res = sqlite3_get_table(db, sql, &results, &rows, &cols, &err);

    if(atoi(results[1]) == 0)
    {

        snprintf(sql, sizeof(sql), "INSERT OR REPLACE INTO interfaces (router_id, interface_id, valid) VALUES ('%s','%s',1)", adv_router, lsa_link_id);
        char *error;
        res = sqlite3_exec(db, sql, NULL, NULL, &error);
        if(error != NULL) {
            printf("ERROR: %s\n", error);
        }
        printf("%s\n", sql);
        sqlite3_free(error);
    }
    sqlite3_free_table(results);
    return res;
}

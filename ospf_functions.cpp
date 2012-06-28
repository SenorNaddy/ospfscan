#include "591.h"

ospf_t *get_ospf(u_char *ip)
{
    /*libtrace has returned us the ip payload so can just return this*/
    void* ret = ip;
    return (ospf_t*)ret;
}

ospf_hello_t *get_ospf_hello(u_char *ip)
{
    void *ret;
    ret = (ospf_t*)ip;

    if(((ospf_t*)ret)->ospf_type != OSPF_HELLO)
        return NULL;   
    //if we are a OSPF_HELLO packet then return it
    ret = (ip+sizeof(ospf_t));
    return (ospf_hello_t*)ret;
}

ospf_db_desc_t *get_ospf_db_desc_header(u_char *ip)
{
    void *ret;
    ret = (ospf_t*)ip;
    if(((ospf_t*)ret)->ospf_type != OSPF_DATADESC)
        return NULL;
    //if this is the ospf_db_desc packet then return it
    ret = (ip+sizeof(ospf_t));
    return (ospf_db_desc_t*)ret;
}

ospf_ls_req_t *get_ospf_ls_req(u_char *ip, uint32_t *index)
{
    void *ret;
    ret = (ospf_t*)ip;
    int ospf_length = htons(((ospf_t*)ret)->ospf_len);
    
    if(((ospf_t*)ret)->ospf_type != OSPF_LSREQ)
        return NULL;
    //here we unfortunately can have an issue where multiple requests can be made in the one packet, so must be able to index
    if(ospf_length - sizeof(ospf_t) - (sizeof(ospf_ls_req_t) * (*index)) <= 0)
        return NULL;
    
    //return our packet
    ret = (ip+sizeof(ospf_t)+(sizeof(ospf_ls_req_t)*(*index)));
    *index += 1; //for convenience increment the index by 1
    return (ospf_ls_req_t*)ret;
}

ospf_ls_update_t *get_ospf_ls_update(u_char *ip)
{
    void *ret;
    ret = (ospf_t*)ip;
    
    if(((ospf_t*)ret)->ospf_type != OSPF_LSUPDATE)
        return NULL;
    //return the ls_update header
    ret = (ip + sizeof(ospf_t));
    return (ospf_ls_update_t*)ret;
}

ospf_lsa_t *get_ospf_next_lsa_from_packet(u_char *ip, uint32_t *index)
{
    void *ret;
    ret = (ospf_t*)ip;
    int ospf_length = htons(((ospf_t*)ret)->ospf_len);
    
    if(((ospf_t*)ret)->ospf_type == OSPF_LSUPDATE)
    {
        ret = (ip + sizeof(ospf_t) + sizeof(ospf_ls_update_t) + (*index));
        if(ospf_length - sizeof(ospf_t) - sizeof(ospf_ls_update_t) - (*index) <= 0)
            return NULL;
        *index += htons(((ospf_lsa_t*)ret)->ospf_lsa_length);
    }
    else if(((ospf_t*)ret)->ospf_type == OSPF_DATADESC)
    {
        ret = (ip + sizeof(ospf_t) + (sizeof(ospf_db_desc_t) - sizeof(ospf_lsa_t)) + (*index));
        if(ospf_length - sizeof(ospf_t) - (sizeof(ospf_db_desc_t) - sizeof(ospf_lsa_t)) - (*index) <= 0)
            return NULL;
        *index += sizeof(ospf_lsa_t);
    }
    
    return (ospf_lsa_t*) ret;
}

ospf_link_t *get_ospf_link_from_lsa(u_char *ip, ospf_lsa_t *lsa, uint32_t *lsa_index, uint32_t *link_index)
{
        void *ret;
    ret = (ospf_t*)ip;
    int ospf_length = htons(((ospf_t*)ret)->ospf_len);
    
    if(((ospf_t*)ret)->ospf_type == OSPF_LSUPDATE)
    {
        ret = (ip + sizeof(ospf_t) + sizeof(ospf_ls_update_t) + ((*lsa_index) - htons(lsa->ospf_lsa_length)) + sizeof(ospf_lsa_t) + sizeof(ospf_router_lsa_t) + (*link_index));
        if(ospf_length - sizeof(ospf_t) - sizeof(ospf_ls_update_t) - ((*lsa_index) - htons(lsa->ospf_lsa_length)) - sizeof(ospf_lsa_t) - sizeof(ospf_router_lsa_t) - (*link_index) <= 0)
            return NULL;
        *link_index += sizeof(ospf_link_t);
    }
    return (ospf_link_t*) ret;
}

void *get_ospf_lsa_from_header(ospf_lsa_t *lsa, uint32_t *index)
{
    void *ret;
    if(lsa->ospf_lsa_type == OSPF_LS_ROUTER)
    {
        if(lsa->ospf_lsa_length - sizeof(ospf_lsa_t) - *index <= 0)
            return NULL;
        ret = (ospf_router_lsa_t *)(lsa + sizeof(ospf_lsa_t) + *index);
        *index += sizeof(ospf_router_lsa_t *);
        return (ospf_router_lsa_t *)ret;
    }
    return NULL;
}

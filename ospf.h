/*
 * ospf.h
 *
 *  Created on: 15/03/2011
 *      Author: Simon Wadsworth
 */


#ifndef ospf_H_
#define ospf_H_

#ifndef LIBTRACE_H
	#if __GNUC__ >= 3
	#  define DEPRECATED __attribute__((deprecated))
	#  define SIMPLE_FUNCTION __attribute__((pure))
	#  define UNUSED __attribute__((unused))
	#  define PACKED __attribute__((packed))
	#  define PRINTF(formatpos,argpos) __attribute__((format(printf,formatpos,argpos)))
	#else
	#  define DEPRECATED
	#  define SIMPLE_FUNCTION
	#  define UNUSED
	#  define PACKED
	#  define PRINTF(formatpos,argpos)
	#endif
#endif

typedef struct ospf
{
          uint8_t ospf_v;
          uint8_t ospf_type;
          uint16_t ospf_len;
          struct in_addr ospf_router;
          struct in_addr ospf_area;
          uint16_t ospf_sum;
          uint16_t ospf_au;
          uint32_t ospf_au1;
          uint32_t ospf_au2;
} PACKED ospf_t;

typedef struct ospf_lsa
{
    uint16_t ospf_lsa_age;
    uint8_t ospf_lsa_options;
    uint8_t ospf_lsa_type;
    in_addr ospf_lsa_ls_id;
    in_addr ospf_lsa_adv_router;
    uint32_t ospf_lsa_seq;
    uint16_t ospf_lsa_checksum;
    uint16_t ospf_lsa_length;
} PACKED ospf_lsa_t;

typedef struct ospf_hello
{
          struct in_addr ospf_hello_mask;
          uint16_t ospf_hello_int;
#if BYTE_ORDER == LITTLE_ENDIAN
	  LT_BITFIELD8 ospf_hello_t:1;
	  LT_BITFIELD8 ospf_hello_e:1;
	  LT_BITFIELD8 ospf_hello_zero2:6;
#elif BYTE_ORDER == BIG_ENDIAN
          LT_BITFIELD8 ospf_hello_zero:6;
          LT_BITFIELD8 ospf_hello_e:1;
          LT_BITFIELD8 ospf_hello_t:1;
#endif
          uint8_t ospf_hello_priority;
          uint32_t ospf_hello_deadint;
          struct in_addr ospf_hello_designated;
          struct in_addr ospf_hello_backup;
          struct in_addr ospf_hello_neighbor;
} PACKED ospf_hello_t;

typedef struct ospf_db_desc
{
          uint16_t ospf_db_desc_mtu;
          uint8_t ospf_db_desc_options;
#if BYTE_ORDER == LITTLE_ENDIAN
          LT_BITFIELD8 ospf_db_desc_ms:1;
          LT_BITFIELD8 ospf_db_desc_m:1;
          LT_BITFIELD8 ospf_db_desc_i:1;
          LT_BITFIELD8 ospf_db_desc_zero:5;
#elif BYTE_ORDER == BIG_ENDIAN
          LT_BITFIELD8 ospf_db_desc_zero:5;
          LT_BITFIELD8 ospf_db_desc_i:1;
          LT_BITFIELD8 ospf_db_desc_m:1;
          LT_BITFIELD8 ospf_db_desc_ms:1;
#endif
          uint32_t ospf_db_desc_seq;
          ospf_lsa_t ospf_db_desc_header;
} PACKED ospf_db_desc_t;

typedef struct ospf_ls_req
{
		uint32_t ospf_ls_type;
		in_addr ospf_ls_id;
		in_addr ospf_advertising_router;
} PACKED ospf_ls_req_t;

typedef struct ospf_ls_update
{
    uint32_t ospf_ls_num_adv;
} PACKED ospf_ls_update_t;

typedef struct ospf_ls_ack
{
    ospf_lsa_t ospf_ls_ack_lsa_header;
} PACKED ospf_ls_ack_t;

typedef struct ospf_router
{
    in_addr ospf_router_id;
} PACKED ospf_router_t;
typedef struct ospf_link
{
    in_addr ospf_router_lsa_link_id;
    uint32_t ospf_router_lsa_link_data;
    uint8_t ospf_router_lsa_type;
    uint8_t ospf_router_lsa_num_tos;
    uint16_t ospf_router_lsa_tos_metric;
} PACKED ospf_link_t;
typedef struct ospf_router_lsa
{
    LT_BITFIELD8 ospf_router_lsa_zero:5;
    LT_BITFIELD8 ospf_router_lsa_v:1;
    LT_BITFIELD8 ospf_router_lsa_e:1;
    LT_BITFIELD8 ospf_router_lsa_b:1;
    LT_BITFIELD8 ospf_router_lsa_z:8;
    uint16_t ospf_router_lsa_num;
} PACKED ospf_router_lsa_t;

typedef struct ospf_as_external_lsa
{
    in_addr ospf_as_external_network;
    LT_BITFIELD8 ospf_as_external_e:2;
    LT_BITFIELD8 ospf_as_external_tos:6;
    uint8_t ospf_as_external_metric;
    uint16_t ospf_as_external_metric2;
    in_addr ospf_as_external_forwarding;
    uint32_t ospf_as_external_tag;
} PACKED ospf_as_external_lsa_t;

typedef struct ospf_network_lsa
{
    in_addr ospf_network_lsa_network_mask;
} PACKED ospf_network_lsa_t;

typedef struct ospf_summary_lsa
{
    in_addr ospf_summary_lsa_network_mask;
} PACKED ospf_summary_lsa_t;
typedef enum {
	OSPF_HELLO	= 1,
	OSPF_DATADESC = 2,
	OSPF_LSREQ	= 3,
	OSPF_LSUPDATE = 4,
	OSPF_LSACK	= 5
} ospf_types_t;

typedef enum {
	OSPF_LS_ROUTER = 1,
	OSPF_LS_NETWORK = 2,
	OSPF_LS_SUMMARY = 3,
	OSPF_LS_ASBR_SUMMARY = 4,
	OSPF_LS_EXTERNAL = 5
} ospf_ls_types_t;

ospf_t *get_ospf(u_char *ip);
ospf_hello_t *get_ospf_hello(u_char *ip);
ospf_db_desc_t *get_ospf_db_desc_header(u_char *ip);
ospf_lsa_t *get_ospf_next_lsa_from_packet(u_char *ip, uint32_t *index);
ospf_ls_update_t *get_ospf_ls_update(u_char *ip);
ospf_link_t *get_ospf_link_from_lsa(u_char *ip, ospf_lsa_t *lsa, uint32_t *lsa_index, uint32_t *link_index);
#endif /* ospf_H_ */

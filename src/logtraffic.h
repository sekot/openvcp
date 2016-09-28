#include <netinet/in.h>
#include <pcap.h>
#include <netinet/ether.h> 


#ifndef LOGTRAFFIC_H
	#define LOGTRAFFIC_H
	
	#define HDR_IP_LEN_MIN	5
	
	#ifndef ETHERTYPE_IPV6
		#define ETHERTYPE_IPV6          0x86dd
	#endif
	
	#ifdef IPV6_SUPPORT
	void ipv4toipv6(uint32_t v4ip, struct in6_addr *nip);
	typedef struct in6_addr ovcp_ip;
	#else
	typedef uint32_t ovcp_ip;
	#endif
	
	typedef struct _iptraffic_node
	{
		ovcp_ip ip;
		
		long long int in;
		long long int out;
		long long int total;	
		
		
		enum {red, black} color;
		struct _iptraffic_node *left;
		struct _iptraffic_node *right;
		struct _iptraffic_node *parent;
		
	} node;
	
	
	struct _packetlogger
	{
		pthread_t thread;
		pthread_mutex_t mutex;
	
		node *root_node;
		pcap_t *descr;	
		u_int8_t ether_local[ETH_ALEN];	
	};

	struct _packetlogger *logtraffic_attach_iface(char *iface);
	char *ip2str(int ip);
	uint32_t str2ip(char *ipstr);
	void free_tree(node *root_node);

#endif

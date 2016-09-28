#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h> 
#include <net/ethernet.h>
#include <netinet/ether.h> 
#include <netinet/ip.h> 
#include <netinet/ip6.h> 
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "logtraffic.h"





static void tree_add_packet(node **root_node, ovcp_ip ip, long long int in, long long int out);
static void rebalance_rbtree(node **root_node, node *n);



static int iface_get_hwaddr(char *dev, u_int8_t *etheraddr)
{
	int sock;
	struct ifreq iface;
  

	if(dev == NULL)
		return -1;
  
	strcpy(iface.ifr_name, dev);
  

	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return -1;
	
	if((ioctl(sock, SIOCGIFHWADDR, &iface)) < 0)
	{
		close(sock);
		return -1;
	}

	memcpy(etheraddr, iface.ifr_hwaddr.sa_data, ETH_ALEN);

	close(sock);

	return 0;
}


void handle_packets(u_char *arg,const struct pcap_pkthdr* pkthdr,const u_char *packet)
{
	struct ether_header *eptr;  
	u_short ether_type;
	u_int length = pkthdr->len;
	struct ip *ip_hdr;
	
	#ifdef IPV6_SUPPORT
	struct ip6_hdr *ipv6_hdr;
	ovcp_ip hostip;
	#endif

	
	struct _packetlogger *packetlogdata = (struct _packetlogger *) arg;

	
	if(!ETHER_IS_VALID_LEN(pkthdr->caplen))
			return;
    
	eptr = (struct ether_header *) packet;
	ether_type = ntohs(eptr->ether_type);

        

	if(ether_type == ETHERTYPE_IP)
	{
	
		ip_hdr = (struct ip *)(packet + sizeof(struct ether_header));
		length -= sizeof(struct ether_header); 

		
		if (length < sizeof(struct ip) || ip_hdr->ip_v != 4 || ip_hdr->ip_hl < HDR_IP_LEN_MIN)
			return;
		


		pthread_mutex_lock (&packetlogdata->mutex);
		
		if(memcmp(eptr->ether_dhost, packetlogdata->ether_local, ETH_ALEN) == 0)
		{
			
			#ifdef IPV6_SUPPORT
			ipv4toipv6(ip_hdr->ip_dst.s_addr, &hostip);
			tree_add_packet(&packetlogdata->root_node, hostip, pkthdr->len, 0);
			#else
			tree_add_packet(&packetlogdata->root_node, ip_hdr->ip_dst.s_addr, pkthdr->len, 0);
			#endif
			
		}
		else
		if(memcmp(eptr->ether_shost, packetlogdata->ether_local, ETH_ALEN) == 0)
		{
			
			#ifdef IPV6_SUPPORT
			ipv4toipv6(ip_hdr->ip_src.s_addr, &hostip);
			tree_add_packet(&packetlogdata->root_node, hostip, 0, pkthdr->len);
			#else
			tree_add_packet(&packetlogdata->root_node, ip_hdr->ip_src.s_addr, 0, pkthdr->len);
			#endif
			
		}

		pthread_mutex_unlock (&packetlogdata->mutex);
	}
	#ifdef IPV6_SUPPORT
	else if(ether_type == ETHERTYPE_IPV6)
	{
			
		ipv6_hdr = (struct ip6_hdr *)(packet + sizeof(struct ether_header));
		length -= sizeof(struct ether_header); 

		
		if (length < sizeof(struct ip6_hdr))
			return;
		

		pthread_mutex_lock (&packetlogdata->mutex);
		
		if(memcmp(eptr->ether_dhost, packetlogdata->ether_local, ETH_ALEN) == 0)
		{
			hostip = ipv6_hdr->ip6_dst;			
			tree_add_packet(&packetlogdata->root_node, hostip, pkthdr->len, 0);
		}
		else
		if(memcmp(eptr->ether_shost, packetlogdata->ether_local, ETH_ALEN) == 0)
		{
			hostip = ipv6_hdr->ip6_src;
			tree_add_packet(&packetlogdata->root_node, hostip, 0, pkthdr->len);
		}

		pthread_mutex_unlock (&packetlogdata->mutex);
	}
	#endif
}

static void tree_add_packet(node **root_node, ovcp_ip ip, long long int in, long long int out)
{
	node *new_node;

	node *current_node = NULL;
	node *previous_node = NULL;
	


	if(*root_node == NULL)
	{
		new_node = (node *) malloc(sizeof(node));
		*root_node = new_node;
	
		new_node->parent = NULL;
		new_node->left = NULL;
		new_node->right = NULL;
		new_node->color = red;
	
		new_node->ip = ip;
		new_node->in = in;
		new_node->out = out;
		new_node->total = in+out;
		
		rebalance_rbtree(root_node, new_node);
	
	}
	else
	{	

		current_node = *root_node;
	
		while(current_node != NULL)
		{
			previous_node = current_node;
		
			if(memcmp(&ip, &current_node->ip, sizeof(ovcp_ip)) == 0)
				break;
			
			if(memcmp(&ip, &current_node->ip, sizeof(ovcp_ip)) < 0)
			{			
				current_node = current_node->left;
			}
			else
			{
				current_node = current_node->right;
			}
		}


		if(current_node != NULL)
		{
			current_node->in += in;
			current_node->out += out;
			current_node->total += in+out;
		}
		else
		{
			new_node = (node *) malloc(sizeof(node));
			

			if(memcmp(&ip, &previous_node->ip, sizeof(ovcp_ip)) < 0)
			{
				previous_node->left = new_node;
			}
			else
			{
				previous_node->right = new_node;
			}
			new_node->parent = previous_node;
			new_node->color = red;
						
			new_node->right = NULL;
			new_node->left = NULL;
			
			new_node->ip = ip;
			new_node->in = in;
			new_node->out = out;
			new_node->total = in+out;
			
			rebalance_rbtree(root_node, new_node);
	
		}
		
		
	}
		


}

char *ip2str(int ip)
{
	char *ipstr = (char *) malloc(17);
	

	snprintf(ipstr, 17, "%d.%d.%d.%d", (ip&0xff000000)>>24, (ip&0x00ff0000)>>16, (ip&0x0000ff00)>>8, (ip&0x000000ff));

	return ipstr;
}

#ifdef IPV6_SUPPORT

void ipv4toipv6(uint32_t v4ip, struct in6_addr *nip)
{
	
		
	nip->in6_u.u6_addr16[0] = 0;
	nip->in6_u.u6_addr16[1] = 0;	
	nip->in6_u.u6_addr16[2] = 0;
	nip->in6_u.u6_addr16[3] = 0;	
	nip->in6_u.u6_addr16[4] = 0;
	nip->in6_u.u6_addr16[5] = 0xFFFF;
	
	memcpy(&nip->in6_u.u6_addr16[6], &v4ip, 4);
}

#endif

uint32_t str2ip(char *ipstr)
{
	char *octet;
	uint32_t ip = 0;
	
	
	octet = strtok(ipstr, ".");
	if(octet != NULL)
		ip = ip | (atoi(octet)<<24);
	
	octet = strtok(NULL, ".");
	if(octet != NULL)
		ip = ip | (atoi(octet)<<16);
	
	octet = strtok(NULL, ".");
	if(octet != NULL)
		ip = ip | (atoi(octet)<<8);
	
	octet = strtok(NULL, ".");
	if(octet != NULL)
		ip = ip | atoi(octet);
	
	
	return ip;
}


void free_tree(node *root_node)
{

	if(root_node == NULL)
		return;

	if(root_node->right != NULL)
	{
		free_tree(root_node->right);
	}
	if(root_node->left != NULL)
	{
		free_tree(root_node->left);
	}

	free(root_node);
}


void free_packetlogger(struct _packetlogger *loggerdata)
{

	pcap_close(loggerdata->descr);
	free_tree(loggerdata->root_node);
	free(loggerdata);
}


void *logpackets(void *data)
{
	struct _packetlogger *packetlogdata = (struct _packetlogger *) data;
	
	pcap_loop(packetlogdata->descr, -1, handle_packets, (u_char *)packetlogdata);
	
	pthread_exit(NULL);
}

struct _packetlogger *logtraffic_attach_iface(char *iface)
{


	char errbuf[PCAP_ERRBUF_SIZE];
	struct _packetlogger *packetlogdata;
	pthread_attr_t thread_attr;
	
	packetlogdata = (struct _packetlogger *) malloc(sizeof(struct _packetlogger)); 

	memset(packetlogdata, 0, sizeof(struct _packetlogger));

	iface_get_hwaddr(iface, packetlogdata->ether_local);

	packetlogdata->root_node = NULL;
	packetlogdata->descr = pcap_open_live(iface, BUFSIZ, 1, 100, errbuf);
	    
	if(packetlogdata->descr == NULL)
	{ 
	    	printf("pcap_open_live(): %s\n",errbuf); 
	    	exit(1); 
	}

	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	
	pthread_create (&packetlogdata->thread, &thread_attr, logpackets, (void *)packetlogdata);

	pthread_attr_destroy(&thread_attr);
	
	pthread_mutex_init (&packetlogdata->mutex, NULL);
	
	
	return packetlogdata;
}






static inline node *uncle(node *n)
{
	if(n->parent == NULL)
		return NULL;
	if(n->parent->parent == NULL)
		return NULL;
	
	if(n->parent == n->parent->parent->left)
		return n->parent->parent->right;
	else return n->parent->parent->left;
}

static void rotate_left(node **root_node, node *n)
{
	node *rightchild = n->right;
	node *leftchild = n->left;
	
	node *leftchild_of_rightchild = rightchild->left;
	node *rightchild_of_rightchild = rightchild->right;
	
	node *parentofn = n->parent;
	
	
	if(parentofn != NULL)
	{
		if(n == parentofn->left)
			parentofn->left = rightchild;
		else
			parentofn->right = rightchild;
	}
	else
	{
		*root_node = rightchild;
	}
	
	rightchild->parent = parentofn;
	rightchild->right = rightchild_of_rightchild;
	rightchild->left = n;
	
	n->parent = rightchild;
	
	n->left = leftchild;
	n->right = leftchild_of_rightchild;
	

	if(leftchild != NULL)
		leftchild->parent=n;

	if(leftchild_of_rightchild != NULL)
		leftchild_of_rightchild->parent=n;

	if(rightchild_of_rightchild != NULL)
		rightchild_of_rightchild->parent=rightchild;

}

static void rotate_right(node **root_node, node *n)
{
	node *leftchild = n->left;
	node *rightchild = n->right;
	
	node *leftchild_of_leftchild = leftchild->left;
	node *rightchild_of_leftchild = leftchild->right;

	
	node *parentofn = n->parent;
	
	if(parentofn != NULL)
	{
		if(n == parentofn->left)
			parentofn->left = leftchild;
		else
			parentofn->right = leftchild;
	}
	else
	{
		*root_node = leftchild;
	}
	
	leftchild->parent = parentofn;
	leftchild->left = leftchild_of_leftchild;
	leftchild->right = n;
	
	
	n->parent = leftchild;
	n->left = rightchild_of_leftchild;
	n->right = rightchild;

	
		
	if(leftchild_of_leftchild != NULL)
		leftchild_of_leftchild->parent = leftchild;
	
	if(rightchild_of_leftchild != NULL)
		rightchild_of_leftchild->parent = n;
		
	if(rightchild != NULL)
		rightchild->parent = n;
	
}
		


static void rebalance_rbtree(node **root_node, node *n)
{

	node *uncleofn = uncle(n);
	node *grandparentofn = (n->parent != NULL) ? n->parent->parent : NULL;
	node *parentofn = n->parent; 
	

	/* Case 1 */
	if(parentofn == NULL)
	{
		n->color = black;
		return;
	}


	/* Case 2 */
	if(parentofn->color == black)
		return;


	/* Case 3 */
	if((uncleofn != NULL) && (uncleofn->color == red))
	{
		parentofn->color = black;
		uncleofn->color = black;
		grandparentofn->color = red;
				
		rebalance_rbtree(root_node, grandparentofn);
	}
	else
	{
		/* Case 4 */
		if((n == parentofn->right) && (parentofn == grandparentofn->left))
		{
			rotate_left(root_node, parentofn);
			n = n->left;
			
			uncleofn = uncle(n);
			grandparentofn = n->parent->parent;
			parentofn = n->parent;
		}
		else
		if((n == parentofn->left) && (parentofn == grandparentofn->right))
		{
			rotate_right(root_node, parentofn);
			
			n = n->right;
			
			uncleofn = uncle(n);
			grandparentofn = n->parent->parent;
			parentofn = n->parent;
		}
				
		/* Case 5 */
		parentofn->color = black;
		grandparentofn->color = red;
				
		if((n == parentofn->left) && (parentofn == grandparentofn->left))
			rotate_right(root_node, grandparentofn);
		else
			rotate_left(root_node, grandparentofn);
					
					
	}

}




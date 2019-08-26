
#include <netinet/ip_var.h>
#include <netmpls/mpls.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#define DUMMY_PORT 10010

#define MAX_LSRR		((MAX_IPOPTLEN - 4) / 4)

#define MPLS_LABEL(m)		((m & MPLS_LABEL_MASK) >> MPLS_LABEL_OFFSET)
#define MPLS_EXP(m)		((m & MPLS_EXP_MASK) >> MPLS_EXP_OFFSET)

/*
 * Format of the data in a (udp) probe packet.
 */
struct packetdata {
	u_char seq;		/* sequence number of this packet */
	u_int8_t ttl;		/* ttl packet left with */
	u_char pad[2];
	u_int32_t sec;		/* time packet left */
	u_int32_t usec;
} __packed;

struct tr_conf {
	int		 incflag;	/* Do not inc the dest. port num */
	int		 first_ttl;	/* Set the first TTL or hop limit */
	u_char		 proto;		/* IP payload protocol to use */
	u_int8_t	 max_ttl;	/* Set the maximum TTL / hop limit */
	int		 nprobes;
	u_int16_t 	 port;		/* start udp dest port */
	int		 waittime;	/* time to wait for a response */
	int		 Aflag;		/* lookup ASN */
	int		 dflag;		/* set SO_DEBUG */
	int		 dump;
	int		 lsrr;		/* Loose Source Record Route */
	struct in_addr	 gateway[MAX_LSRR + 1];
	int		 lsrrlen;
	int		 protoset;
	int		 ttl_flag;	/* display ttl on returned packet */
	int		 nflag;		/* print addresses numerically */
	char		*source;
	int		 sump;
	int		 tos;
	int		 tflag;		/* tos value was set */
	int		 verbose;
	u_int		 rtableid;	/* Set the routing table */
	u_short		 ident;
};

extern int32_t		 sec_perturb;
extern int32_t		 usec_perturb;

extern u_char		 packet[512];
extern u_char		*outpacket;	/* last inbound (icmp) packet */

int		 wait_for_reply(int, struct msghdr *, int);
void		 dump_packet(void);
void		 build_probe4(struct tr_conf *, int, u_int8_t, int);
void		 build_probe6(struct tr_conf *, int, u_int8_t, int,
			struct sockaddr *);
void		 send_probe(struct tr_conf *, int, u_int8_t, int,
			struct sockaddr *);
struct udphdr	*get_udphdr(struct tr_conf *, struct ip6_hdr *, u_char *);
int		 packet_ok(struct tr_conf *, int, struct msghdr *, int, int,
			int);
int		 packet_ok4(struct tr_conf *, struct msghdr *, int, int, int);
int		 packet_ok6(struct tr_conf *, struct msghdr *, int, int, int);
void		 icmp_code(int, int, int *, int *);
void		 icmp4_code(int, int *, int *);
void		 icmp6_code(int, int *, int *);
void		 dump_packet(void);
void		 print_exthdr(u_char *, int);
void		 check_tos(struct ip*, int *);
void		 print(struct tr_conf *, struct sockaddr *, int, const char *);
const char	*inetname(struct sockaddr*);
void		 print_asn(struct sockaddr_storage *);
u_short		 in_cksum(u_short *, int);
char		*pr_type(u_int8_t);
int		 map_tos(char *, int *);
double		 deltaT(struct timeval *, struct timeval *);

void		 gettime(struct timeval *);

extern int		 rcvsock;  /* receive (icmp) socket file descriptor */
extern int		 sndsock;  /* send (udp) socket file descriptor */

extern int		 rcvhlim;
extern struct in6_pktinfo *rcvpktinfo;

extern int		 datalen;  /* How much data */

extern char		*hostname;

extern u_int16_t	 srcport;

#define ICMP_CODE 0;

extern int verbose;
extern int dump;

extern char *__progname;
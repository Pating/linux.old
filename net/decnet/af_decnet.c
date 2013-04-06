
/*
 * DECnet       An implementation of the DECnet protocol suite for the LINUX
 *              operating system.  DECnet is implemented using the  BSD Socket
 *              interface as the means of communication with the user level.
 *
 *              DECnet Socket Layer Interface
 *
 * Authors:     Eduardo Marcelo Serrat <emserrat@geocities.com>
 *              Patrick Caulfield <patrick@pandh.demon.co.uk>
 *
 * Changes:
 *        Steve Whitehouse: Copied from Eduardo Serrat and Patrick Caulfield's
 *                          version of the code. Original copyright preserved
 *                          below.
 *        Steve Whitehouse: Some bug fixes, cleaning up some code to make it
 *                          compatible with my routing layer.
 *        Steve Whitehouse: Merging changes from Eduardo Serrat and Patrick
 *                          Caulfield.
 *        Steve Whitehouse: Further bug fixes, checking module code still works
 *                          with new routing layer.
 *        Steve Whitehouse: Additional set/get_sockopt() calls.
 *        Steve Whitehouse: Fixed TIOCINQ ioctl to be same as Eduardo's new
 *                          code.
 *        Steve Whitehouse: recvmsg() changed to try and behave in a POSIX like
 *                          way. Didn't manage it entirely, but its better.
 *        Steve Whitehouse: ditto for sendmsg().
 *        Steve Whitehouse: A selection of bug fixes to various things.
 *        Steve Whitehouse: Added TIOCOUTQ ioctl.
 *        Steve Whitehouse: Fixes to username2sockaddr & sockaddr2username.
 *        Steve Whitehouse: Fixes to connect() error returns.
 *       Patrick Caulfield: Fixes to delayed acceptance logic.
 */


/******************************************************************************
    (c) 1995-1998 E.M. Serrat		emserrat@geocities.com
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

HISTORY:

Version           Kernel     Date       Author/Comments
-------           ------     ----       ---------------
Version 0.0.1     2.0.30    01-dic-97	Eduardo Marcelo Serrat
					(emserrat@geocities.com)

                                        First Development of DECnet Socket La-
					yer for Linux. Only supports outgoing
					connections.

Version 0.0.2	  2.1.105   20-jun-98   Patrick J. Caulfield
					(patrick@pandh.demon.co.uk)

					Port to new kernel development version.

Version 0.0.3     2.1.106   25-jun-98   Eduardo Marcelo Serrat
					(emserrat@geocities.com)
					_
                                        Added support for incoming connections
                                        so we can start developing server apps
                                        on Linux.
					-
					Module Support
Version 0.0.4     2.1.109   21-jul-98   Eduardo Marcelo Serrat
                                       (emserrat@geocities.com)
                                       _
                                        Added support for X11R6.4. Now we can 
                                        use DECnet transport for X on Linux!!!
                                       -
Version 0.0.5    2.1.110   01-aug-98   Eduardo Marcelo Serrat
                                       (emserrat@geocities.com)
                                       Removed bugs on flow control
                                       Removed bugs on incoming accessdata
                                       order
                                       -
Version 0.0.6    2.1.110   07-aug-98   Eduardo Marcelo Serrat
                                       dn_recvmsg fixes

                                        Patrick J. Caulfield
                                       dn_bind fixes
*******************************************************************************/

#include <linux/config.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/string.h>
#include <linux/sockios.h>
#include <linux/net.h>
#include <linux/netdevice.h>
#include <linux/inet.h>
#include <linux/route.h>
#include <net/sock.h>
#include <asm/segment.h>
#include <asm/system.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <net/neighbour.h>
#include <net/dst.h>
#include <net/dn.h>
#include <net/dn_nsp.h>
#include <net/dn_dev.h>
#include <net/dn_route.h>
#include <net/dn_fib.h>
#include <net/dn_raw.h>
#include <net/dn_neigh.h>

#define MAX(a,b) ((a)>(b)?(a):(b))

static void dn_keepalive(struct sock *sk);

/*
 * decnet_address is kept in network order, decnet_ether_address is kept
 * as a string of bytes.
 */
dn_address decnet_address = 0;
unsigned char decnet_ether_address[ETH_ALEN] = { 0xAA, 0x00, 0x04, 0x00, 0x00, 0x00 };
int decnet_node_type = DN_RT_INFO_ENDN;

static struct proto_ops dn_proto_ops;
static struct sock *dn_sklist = NULL;
static struct sock *dn_wild_sk = NULL;

static int _dn_setsockopt(struct socket *sock, int level, int optname, char *optval, int optlen, int flags);
static int _dn_getsockopt(struct socket *sock, int level, int optname, char *optval, int *optlen, int flags);

int dn_sockaddr2username(struct sockaddr_dn *sdn, unsigned char *buf, unsigned char type)
{
	int len = 2;

	*buf++ = type;

	switch(type) {
		case 0:
			*buf++ = sdn->sdn_objnum;
			break;
		case 1:
			*buf++ = 0;
			*buf++ = sdn->sdn_objnamel;
			memcpy(buf, sdn->sdn_objname, sdn->sdn_objnamel);
			len = 3 + sdn->sdn_objnamel;
			break;
		case 2:
			memset(buf, 0, 5);
			buf += 5;
			*buf++ = sdn->sdn_objnamel;
			memcpy(buf, sdn->sdn_objname, sdn->sdn_objnamel);
			len = 7 + sdn->sdn_objnamel;
			break;
	}

	return len;
}

/*
 * On reception of usernames, we handle types 1 and 0 for destination
 * addresses only. Types 2 and 4 are used for source addresses, but the
 * UIC, GIC are ignored and they are both treated the same way. Type 3
 * is never used as I've no idea what its purpose might be or what its
 * format is.
 */
int dn_username2sockaddr(unsigned char *data, int len, struct sockaddr_dn *sdn, unsigned char *fmt)
{
	unsigned char type;
	int size = len;
	int namel = 12;

	sdn->sdn_objnum = 0;
	sdn->sdn_objnamel = 0;
	memset(sdn->sdn_objname, 0, DN_MAXOBJL);

	if (len < 2)
		return -1;

	len -= 2;
	*fmt = *data++;
	type = *data++;

	switch(*fmt) {
		case 0:
			sdn->sdn_objnum = type;
			return 2;
		case 1:
			namel = 16;
			break;
		case 2:
			len  -= 4;
			data += 4;
			break;
		case 4:
			len  -= 8;
			data += 8;
			break;
		default:
			return -1;
	}

	len -= 1;

	if (len < 0)
		return -1;

	sdn->sdn_objnamel = *data++;
	len -= sdn->sdn_objnamel;

	if ((len < 0) || (sdn->sdn_objnamel > namel))
		return -1;

	memcpy(sdn->sdn_objname, data, sdn->sdn_objnamel);

	return size - len;
}

struct sock *dn_sklist_find_listener(struct sockaddr_dn *addr)
{
	struct sock *sk;

	for(sk = dn_sklist; sk != NULL; sk = sk->next) {
		struct dn_scp *scp = &sk->protinfo.dn;
		if (sk->state != TCP_LISTEN)
			continue;
		if (scp->addr.sdn_objnum) {
			if (scp->addr.sdn_objnum != addr->sdn_objnum)
				continue;
		} else {
			if (addr->sdn_objnum)
				continue;
			if (scp->addr.sdn_objnamel != addr->sdn_objnamel)
				continue;
			if (memcmp(scp->addr.sdn_objname, addr->sdn_objname, addr->sdn_objnamel) != 0)
				continue;
		}
		return sk;
	}

	return (dn_wild_sk && (dn_wild_sk->state == TCP_LISTEN)) ? dn_wild_sk : NULL;
}

struct sock *dn_sklist_find(unsigned short port)
{
        struct sock *s;

        for (s = dn_sklist; s != NULL; s = s->next) {
                if (s->protinfo.dn.addrloc == port) {
                        return s;
                }
        }

        return NULL;
}

static struct sock *dn_sklist_find_by_objnum(unsigned char objnum)
{
        struct sock *s;

        for (s = dn_sklist; s != NULL; s = s->next) {
                if ((s->protinfo.dn.addr.sdn_objnum == objnum) &&
                     (s->state == TCP_LISTEN)) {
                        return s;
                }
        }
        return NULL;
}

static struct sock *dn_sklist_find_by_name(char *name)
{
       struct sock *s;

       for (s = dn_sklist; s != NULL; s = s->next) {
		if (s->protinfo.dn.addr.sdn_objnum)
			continue;
               if ((memcmp(s->protinfo.dn.addr.sdn_objname,name,
                           s->protinfo.dn.addr.sdn_objnamel) == 0) 
                     && (s->state == TCP_LISTEN)) {
                       return s;
               }
       }
       return NULL;
}


struct sock *dn_find_by_skb(struct sk_buff *skb)
{
	struct dn_skb_cb *cb = (struct dn_skb_cb *)skb->cb;
	struct sock *sk;
	struct dn_scp *scp;

	for(sk = dn_sklist; sk != NULL; sk = sk->next) {
		scp = &sk->protinfo.dn;
		if (cb->src != dn_saddr2dn(&scp->peer))
			continue;
		if (cb->dst_port != scp->addrloc)
			continue;
		if (scp->addrrem && (cb->src_port != scp->addrrem))
			continue;
		break;
	}

	return sk;
}


unsigned short dn_alloc_port(void)
{
	struct sock *sk;
	static unsigned short dn_port = 0x2000;
	short port;

	start_bh_atomic();

	do {
		port = dn_port++;
		sk = dn_sklist_find(port);
	} while((sk != NULL) || (port == 0));

	end_bh_atomic();

        return dn_htons(port);
};


static void dn_destruct(struct sock *sk)
{
	struct dn_scp *scp = &sk->protinfo.dn;

	skb_queue_purge(&scp->data_xmit_queue);
	skb_queue_purge(&scp->other_xmit_queue);
	skb_queue_purge(&scp->other_receive_queue);

	dst_release(xchg(&sk->dst_cache, NULL));

	MOD_DEC_USE_COUNT;
}

struct sock *dn_alloc_sock(struct socket *sock, int flags)
{
	struct sock *sk;
	struct dn_scp *scp;

	if  ((sk = sk_alloc(PF_DECnet, flags, 1)) == NULL) 
		goto no_sock;

	if (sock) {
#ifdef CONFIG_DECNET_RAW
		if (sock->type == SOCK_RAW)
			sock->ops = &dn_raw_proto_ops;
		else
#endif /* CONFIG_DECNET_RAW */
			sock->ops = &dn_proto_ops;
	}
	sock_init_data(sock,sk);
	scp = &sk->protinfo.dn;

	sk->backlog_rcv = dn_nsp_backlog_rcv;
	sk->destruct    = dn_destruct;
	sk->no_check    = 1;
	sk->family      = PF_DECnet;
	sk->protocol    = 0;

	/* Initialization of DECnet Session Control Port		*/
	scp->state	= DN_O;		/* Open			*/
	scp->numdat	= 1;		/* Next data seg to tx	*/
	scp->numoth	= 1;		/* Next oth data to tx  */
	scp->ackxmt_dat = 0;		/* Last data seg ack'ed */
	scp->ackxmt_oth = 0;		/* Last oth data ack'ed */
	scp->ackrcv_dat = 0;		/* Highest data ack recv*/
	scp->ackrcv_oth = 0;		/* Last oth data ack rec*/
        scp->flowrem_sw = DN_SEND;
	scp->flowloc_sw = DN_SEND;
	scp->accept_mode = ACC_IMMED;
	scp->addr.sdn_family    = AF_DECnet;
	scp->peer.sdn_family    = AF_DECnet;
	scp->accessdata.acc_accl = 5;
	memcpy(scp->accessdata.acc_acc, "LINUX", 5);
	scp->mss = 1460;

	scp->snd_window   = NSP_MIN_WINDOW;
	scp->nsp_srtt     = NSP_INITIAL_SRTT;
	scp->nsp_rttvar   = NSP_INITIAL_RTTVAR;
	scp->nsp_rxtshift = 0;

	skb_queue_head_init(&scp->data_xmit_queue);
	skb_queue_head_init(&scp->other_xmit_queue);
	skb_queue_head_init(&scp->other_receive_queue);

	scp->persist = 0;
	scp->persist_fxn = NULL;
	scp->keepalive = 10 * HZ;
	scp->keepalive_fxn = dn_keepalive;

	init_timer(&scp->delack_timer);
	scp->delack_pending = 0;
	scp->delack_fxn = dn_nsp_delayed_ack;

	dn_start_slow_timer(sk);

	MOD_INC_USE_COUNT;

	return sk;
no_sock:
	return NULL;
}

/*
 * Keepalive timer.
 * FIXME: Should respond to SO_KEEPALIVE etc.
 */
static void dn_keepalive(struct sock *sk)
{
	struct dn_scp *scp = &sk->protinfo.dn;

	/*
	 * By checking the other_data transmit queue is empty
	 * we are double checking that we are not sending too
	 * many of these keepalive frames.
	 */
	if (skb_queue_len(&scp->other_xmit_queue) == 0)
		dn_nsp_send_lnk(sk, DN_NOCHANGE);
}


/*
 * Timer for shutdown/destroyed sockets.
 * When socket is dead & no packets have been sent for a
 * certain amount of time, they are removed by this
 * routine. Also takes care of sending out DI & DC
 * frames at correct times. This is called by both
 * socket level and interrupt driven code.
 */
static int dn_destroy_timer(struct sock *sk)
{
	struct dn_scp *scp = &sk->protinfo.dn;

	scp->persist = dn_nsp_persist(sk);

	switch(scp->state) {
		case DN_DI:
			/* printk(KERN_DEBUG "dn_destroy_timer: DI\n"); */
			dn_send_disc(sk, NSP_DISCINIT, 0);
			if (scp->nsp_rxtshift >= decnet_di_count)
				scp->state = DN_CN;
			return 0;

		case DN_DR:
			/* printk(KERN_DEBUG "dn_destroy_timer: DR\n"); */
			dn_send_disc(sk, NSP_DISCINIT, 0);
			if (scp->nsp_rxtshift >= decnet_dr_count)
				scp->state = DN_DRC;
			return 0;

		case DN_DN:
			if (scp->nsp_rxtshift < decnet_dn_count) {
				/* printk(KERN_DEBUG "dn_destroy_timer: DN\n"); */
				dn_send_disc(sk, NSP_DISCCONF, NSP_REASON_DC);
				return 0;
			}
	}

	scp->persist = (HZ * decnet_time_wait);

/*	printk(KERN_DEBUG "dn_destroy_timer: testing dead\n"); */
	
	if (sk->socket)
		return 0;

	dn_stop_fast_timer(sk); /* unlikely, but possible that this is runninng */
	if ((jiffies - scp->stamp) >= (HZ * decnet_time_wait)) {
		sklist_destroy_socket(&dn_sklist, sk);
		return 1;
	}

	/*printk(KERN_DEBUG "dn_destroy_timer: dead 'n' waiting...\n"); */

	return 0;
}

void dn_destroy_sock(struct sock *sk)
{
	struct dn_scp *scp = &sk->protinfo.dn;

	if (sk->dead)
		return;

	sk->dead = 1;
	scp->nsp_rxtshift = 0; /* reset back off */

	if (sk->socket) {
		if (sk->socket->state != SS_UNCONNECTED)
			sk->socket->state = SS_DISCONNECTING;
	}

	sk->state = TCP_CLOSE;

	switch(scp->state) {
		case DN_DN:
			dn_send_disc(sk, NSP_DISCCONF, NSP_REASON_DC);
			scp->persist_fxn = dn_destroy_timer;
			scp->persist = dn_nsp_persist(sk);
			break;
		case DN_CD:
		case DN_CR:
			scp->state = DN_DR;
			goto disc_reject;
		case DN_RUN:
			scp->state = DN_DI;
		case DN_DI:
		case DN_DR:
disc_reject:
			dn_send_disc(sk, NSP_DISCINIT, 0);
		case DN_NC:
		case DN_NR:
		case DN_RJ:
		case DN_DIC:
		case DN_CN:
		case DN_DRC:
		case DN_CI:
			scp->persist_fxn = dn_destroy_timer;
			scp->persist = dn_nsp_persist(sk);
			break;
		default:
			printk(KERN_DEBUG "DECnet: dn_destroy_sock passed socket in invalid state\n");
		case DN_O:
			start_bh_atomic();
			dn_stop_fast_timer(sk);
			dn_stop_slow_timer(sk);

	                if (sk == dn_wild_sk) {
        	                dn_wild_sk = NULL;
                	        sklist_destroy_socket(NULL, sk);
			} else {
                        	sklist_destroy_socket(&dn_sklist, sk);
                	}

			end_bh_atomic();
			break;
	}
}

char *dn_addr2asc(dn_address addr, char *buf)
{
	unsigned short node, area;

	node = addr & 0x03ff;
	area = addr >> 10;
	sprintf(buf, "%hd.%hd", area, node);

	return buf;
}


static char *dn_state2asc(unsigned char state)
{
	switch(state) {
		case DN_O:
			return "OPEN";
		case DN_CR:
			return "  CR";
		case DN_DR:
			return "  DR";
		case DN_DRC:
			return " DRC";
		case DN_CC:
			return "  CC";
		case DN_CI:
			return "  CI";
		case DN_NR:
			return "  NR";
		case DN_NC:
			return "  NC";
		case DN_CD:
			return "  CD";
		case DN_RJ:
			return "  RJ";
		case DN_RUN:
			return " RUN";
		case DN_DI:
			return "  DI";
		case DN_DIC:
			return " DIC";
		case DN_DN:
			return "  DN";
		case DN_CL:
			return "  CL";
		case DN_CN:
			return "  CN";
	}

	return "????";
}

static int dn_get_info(char *buffer, char **start, off_t offset,
			    int length, int dummy)
{
	struct	sock	*sk;
	int		len = 0;
	off_t		pos = 0;
	off_t		begin = 0;
	char buf[DN_ASCBUF_LEN];

	len += sprintf(buffer+len,"%-8s%-7s%-7s%-7s%-5s%-13s%-13s\n",
			"Remote","Source","Remote","Object","Link ",
			"   Data  Packets ","Link  Packets");
	len += sprintf(buffer+len,"%-8s%-7s%-7s%-7s%-5s%-13s%-13s\n\n",
			"Node  ","Port  ","Port  ","Number","State",
			"   Out     In ","   Out     In");
	start_bh_atomic();
	for (sk = dn_sklist; sk != NULL; sk = sk->next) {
		len += sprintf(buffer+len,
		      "%6s  %04X   %04X   %6d %4s %6d %6d %6d %6d\n",
					   
		       dn_addr2asc(dn_ntohs(dn_saddr2dn(&sk->protinfo.dn.peer)), buf),
		       sk->protinfo.dn.addrloc,sk->protinfo.dn.addrrem,
		       sk->protinfo.dn.addr.sdn_objnum,
		       dn_state2asc(sk->protinfo.dn.state),
		       sk->protinfo.dn.numdat, sk->protinfo.dn.numdat_rcv,
		       sk->protinfo.dn.numoth, sk->protinfo.dn.numoth_rcv);

		pos = begin + len;
		if (pos < offset) {
                        len = 0;
			begin = pos;
		}
		if (pos > offset+length) 
			break;
	}
	end_bh_atomic();

	*start = buffer + (offset - begin);
	len -= (offset - begin);

	if (len > length)
		len = length;

	return len;
}

static int dn_create(struct socket *sock, int protocol)
{
	struct	sock	*sk;

	switch(sock->type) {
		case SOCK_SEQPACKET:
			if (protocol != DNPROTO_NSP)
				return -EPROTONOSUPPORT;
			break;
		case SOCK_STREAM:
			break;
#ifdef CONFIG_DECNET_RAW
		case SOCK_RAW:
			if ((protocol != DNPROTO_NSP) &&
#ifdef CONFIG_DECNET_MOP
					(protocol != DNPROTO_MOP) &&
#endif /* CONFIG_DECNET_MOP */
					(protocol != DNPROTO_ROU))
				return -EPROTONOSUPPORT;
			break;
#endif /* CONFIG_DECNET_RAW */
		default:
			return -ESOCKTNOSUPPORT;
	}


	if ((sk = dn_alloc_sock(sock, GFP_KERNEL)) == NULL) 
		return -ENOBUFS;

	sk->protocol = protocol;

	return 0;
}


static int
dn_release(struct socket *sock, struct socket *peer)
{
	struct sock *sk = sock->sk;

	if (sk) {
		lock_sock(sk);
		sock->sk = NULL;
		sk->socket = NULL;
		dn_destroy_sock(sk);
		release_sock(sk);
	}

        return 0;
}

static int dn_bind(struct socket *sock, struct sockaddr *uaddr, int addr_len)
{
	struct	sock	*sk = sock->sk;
	struct sockaddr_dn *saddr = (struct sockaddr_dn *)uaddr;

	if (sk->zapped == 0)
		return -EINVAL;

	if (addr_len != sizeof(struct sockaddr_dn))
		return -EINVAL;

	if (saddr->sdn_family != AF_DECnet)
		return -EINVAL;

	if (saddr->sdn_objnum && !suser())
		return -EPERM;

	if (!saddr->sdn_objname && (saddr->sdn_objnamel > DN_MAXOBJL))
		return -EINVAL;

	if (saddr->sdn_flags & ~SDF_WILD)
		return -EINVAL;

	if ((saddr->sdn_flags & SDF_WILD) && !suser())
		return -EPERM;

	start_bh_atomic();

	if (saddr->sdn_flags & SDF_WILD) {
		if (dn_wild_sk) {
			end_bh_atomic();
			return -EADDRINUSE;
		}
		dn_wild_sk = sk;
		sk->zapped = 0;
		memcpy(&sk->protinfo.dn.addr, saddr, addr_len);
		end_bh_atomic();
		return 0;
	}

	if (saddr->sdn_objnum && dn_sklist_find_by_objnum(saddr->sdn_objnum)) {
		end_bh_atomic();
		return -EADDRINUSE;
	}

	if (!saddr->sdn_objnum) {
		if (dn_sklist_find_by_name(saddr->sdn_objname)) {
			end_bh_atomic();
			return -EADDRINUSE;
		}
	}

	memcpy(&sk->protinfo.dn.addr, saddr, addr_len);
	sk->zapped = 0;
	sklist_insert_socket(&dn_sklist, sk);
	end_bh_atomic();

        return 0;
}


static int dn_auto_bind(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct dn_scp *scp = &sk->protinfo.dn;

	sk->zapped = 0;

	scp->addr.sdn_flags  = 0;
	scp->addr.sdn_objnum = 0;

	/*
	 * This stuff is to keep compatibility with Eduardo's
	 * patch. I hope I can dispense with it shortly...
	 */
	if ((scp->accessdata.acc_accl != 0) &&
		(scp->accessdata.acc_accl <= 12)) {
	
		scp->addr.sdn_objnamel = scp->accessdata.acc_accl;
		memcpy(scp->addr.sdn_objname, scp->accessdata.acc_acc, scp->addr.sdn_objnamel);

		scp->accessdata.acc_accl = 0;
		memset(scp->accessdata.acc_acc, 0, 40);
	}

	scp->addr.sdn_add.a_len = 2;
	*(dn_address *)scp->addr.sdn_add.a_addr = decnet_address;

	sklist_insert_socket(&dn_sklist, sk);

	return 0;
}


static int dn_connect(struct socket *sock, struct sockaddr *uaddr, int addr_len, int flags)
{
	struct sockaddr_dn *addr = (struct sockaddr_dn *)uaddr;
	struct sock *sk = sock->sk;
	int err = -EISCONN;

	lock_sock(sk);

	if (sock->state == SS_CONNECTED) 
		goto out;

	if (sock->state == SS_CONNECTING) {
		err = 0;
		if (sk->state == TCP_ESTABLISHED)
			goto out;

		err = -ECONNREFUSED;
		if (sk->state == TCP_CLOSE)
			goto out;
	}

	err = -EINVAL;
	if (sk->protinfo.dn.state != DN_O)
		goto out;

	if (addr_len != sizeof(struct sockaddr_dn))
		goto out;

	if (addr->sdn_family != AF_DECnet)
		goto out;

	if (addr->sdn_flags & SDF_WILD)
		goto out;

	err = -EADDRNOTAVAIL;
	if (sk->zapped && (err = dn_auto_bind(sock)))
		goto out;

	memcpy(&sk->protinfo.dn.peer, addr, addr_len);

	err = -EHOSTUNREACH;
	if (dn_route_output(sk) < 0)
		goto out;

	sk->state   = TCP_SYN_SENT;
	sock->state = SS_CONNECTING;
	sk->protinfo.dn.state = DN_CI;

	dn_nsp_send_conninit(sk, NSP_CI);

	err = -EINPROGRESS;
	if ((sk->state == TCP_SYN_SENT) && (flags & O_NONBLOCK))
		goto out;

	while(sk->state == TCP_SYN_SENT) {

		err = -ERESTARTSYS;
		if (signal_pending(current))
			goto out;

		if ((err = sock_error(sk)) != 0) {
			sock->state = SS_UNCONNECTED;
			goto out;
		}

		SOCK_SLEEP_PRE(sk);

		if (sk->state == TCP_SYN_SENT)
			schedule();

		SOCK_SLEEP_POST(sk);
	}

	if (sk->state != TCP_ESTABLISHED) {
		sock->state = SS_UNCONNECTED;
		err = sock_error(sk);
		goto out;
	}

	err = 0;
	sock->state = SS_CONNECTED;
out:
	release_sock(sk);

        return err;
}

static int dn_access_copy(struct sk_buff *skb, struct accessdata_dn *acc)
{
        unsigned char *ptr = skb->data;

        acc->acc_userl = *ptr++;
        memcpy(&acc->acc_user, ptr, acc->acc_userl);
        ptr += acc->acc_userl;

        acc->acc_passl = *ptr++;
        memcpy(&acc->acc_pass, ptr, acc->acc_passl);
        ptr += acc->acc_passl;

        acc->acc_accl = *ptr++;
        memcpy(&acc->acc_acc, ptr, acc->acc_accl);

        skb_pull(skb, acc->acc_accl + acc->acc_passl + acc->acc_userl + 3);

	return 0;
}

static int dn_user_copy(struct sk_buff *skb, struct optdata_dn *opt)
{
        unsigned char *ptr = skb->data;
        
        opt->opt_optl   = *ptr++;
        opt->opt_status = 0;
        memcpy(opt->opt_data, ptr, opt->opt_optl);
        skb_pull(skb, opt->opt_optl + 1);

	return 0;
}


/*
 * This is here for use in the sockopt() call as well as
 * in accept(). Must be called with a locked socket.
 */
static int dn_wait_accept(struct socket *sock, int flags)
{
        struct sock *sk = sock->sk;

	/* printk(KERN_DEBUG "dn_wait_accept: in\n"); */

        while(sk->state == TCP_LISTEN) {
                if (flags & O_NONBLOCK) {
                        return -EAGAIN;
                }

		SOCK_SLEEP_PRE(sk)

		if (sk->state == TCP_LISTEN)
			schedule();

		SOCK_SLEEP_POST(sk)

                if (signal_pending(current)) {
			/* printk(KERN_DEBUG "dn_wait_accept: signal\n"); */
                        return -ERESTARTSYS; /* But of course you don't! */
                }
        }

        if ((sk->protinfo.dn.state != DN_RUN) && (sk->protinfo.dn.state != DN_DRC)) {
                sock->state = SS_UNCONNECTED;
                return sock_error(sk);
        }

	sock->state = SS_CONNECTED;
	/* printk(KERN_DEBUG "dn_wait_accept: out\n"); */

        return 0;
}


static int dn_accept(struct socket *sock, struct socket *newsock,int flags)
{
	struct	sock	*sk=sock->sk, *newsk;
	struct	sk_buff	*skb = NULL;
	struct dn_skb_cb *cb;
	unsigned char menuver;
	int err = 0;
	unsigned char type;

	lock_sock(sk);

        if (sk->state != TCP_LISTEN) {
		release_sock(sk);
		return -EINVAL;
	}

	if (sk->protinfo.dn.state != DN_O) {
		release_sock(sk);
		return -EINVAL;
	}

	if (newsock->sk != NULL) {
		newsock->sk->socket = NULL;
		dn_destroy_sock(newsock->sk);
		newsock->sk = NULL;
	}

        do
        {
		/* printk(KERN_DEBUG "dn_accept: loop top\n"); */
                if ((skb = skb_dequeue(&sk->receive_queue)) == NULL)
                {
                        if (flags & O_NONBLOCK)
                        {
                                release_sock(sk);
                                return -EAGAIN;
                        }

			SOCK_SLEEP_PRE(sk);

			if (!skb_peek(&sk->receive_queue))
				schedule();

			SOCK_SLEEP_POST(sk);

                        if (signal_pending(current))
                        {
				release_sock(sk);
                                return -ERESTARTSYS;
                        }
                }
        } while (skb == NULL);

	cb = (struct dn_skb_cb *)skb->cb;

	if ((newsk = dn_alloc_sock(newsock, GFP_KERNEL)) == NULL) {
		release_sock(sk);
		kfree_skb(skb);
		return -ENOBUFS;
	}
	sk->ack_backlog--;
	release_sock(sk);

	dst_release(xchg(&newsk->dst_cache, skb->dst));
	skb->dst = NULL;


        newsk->protinfo.dn.state      = DN_CR;
        newsk->protinfo.dn.addrloc    = dn_alloc_port();
	newsk->protinfo.dn.addrrem    = cb->src_port;
	newsk->protinfo.dn.mss        = cb->segsize;
	newsk->protinfo.dn.accept_mode = sk->protinfo.dn.accept_mode;
	
	if (newsk->protinfo.dn.mss < 230)
		newsk->protinfo.dn.mss = 230;

	newsk->state  = TCP_LISTEN;
	newsk->zapped = 0;

	memcpy(&newsk->protinfo.dn.addr, &sk->protinfo.dn.addr, sizeof(struct sockaddr_dn));

	skb_pull(skb, dn_username2sockaddr(skb->data, skb->len, &newsk->protinfo.dn.addr, &type));
	skb_pull(skb, dn_username2sockaddr(skb->data, skb->len, &newsk->protinfo.dn.peer, &type));
	*(dn_address *)newsk->protinfo.dn.peer.sdn_add.a_addr = cb->src;

	menuver = *skb->data;
	skb_pull(skb, 1);

	if (menuver & DN_MENUVER_ACC)
		dn_access_copy(skb, &newsk->protinfo.dn.accessdata);

	if (menuver & DN_MENUVER_USR)
		dn_user_copy(skb, &newsk->protinfo.dn.conndata_in);

	if (menuver & DN_MENUVER_PRX)
		newsk->protinfo.dn.peer.sdn_flags |= SDF_PROXY;

	if (menuver & DN_MENUVER_UIC)
		newsk->protinfo.dn.peer.sdn_flags |= SDF_UICPROXY;

	kfree_skb(skb);

	memcpy(&newsk->protinfo.dn.conndata_out, &sk->protinfo.dn.conndata_out,
		sizeof(struct optdata_dn));
	memcpy(&newsk->protinfo.dn.discdata_out, &sk->protinfo.dn.discdata_out,
		sizeof(struct optdata_dn));

	lock_sock(newsk);
        sklist_insert_socket(&dn_sklist, newsk);

	dn_send_conn_ack(newsk);

	if (newsk->protinfo.dn.accept_mode == ACC_IMMED) {
		newsk->protinfo.dn.state = DN_CC;
        	dn_send_conn_conf(newsk);
		err = dn_wait_accept(newsock, flags);
	}

	release_sock(newsk);
        return err;
}


static int dn_getname(struct socket *sock, struct sockaddr *uaddr,int *uaddr_len,int peer)
{
	struct sockaddr_dn *sa = (struct sockaddr_dn *)uaddr;
	struct sock *sk = sock->sk;
	struct dn_scp *scp = &sk->protinfo.dn;

	*uaddr_len = sizeof(struct sockaddr_dn);

	lock_sock(sk);

	if (peer) {
		if (sock->state != SS_CONNECTED && sk->protinfo.dn.accept_mode == ACC_IMMED)
			return -ENOTCONN;

		memcpy(sa, &scp->peer, sizeof(struct sockaddr_dn));
	} else {
		memcpy(sa, &scp->addr, sizeof(struct sockaddr_dn));
	}

	release_sock(sk);

        return 0;
}


static unsigned int dn_poll(struct file *file, struct socket *sock, poll_table  *wait)
{
	struct sock *sk = sock->sk;
	struct dn_scp *scp = &sk->protinfo.dn;
	int mask = datagram_poll(file, sock, wait);

	if (skb_queue_len(&scp->other_receive_queue))
		mask |= POLLRDBAND;

	return mask;
}

static int dn_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	struct sock *sk = sock->sk;
	int err = -EOPNOTSUPP;
	unsigned long amount = 0;
	struct sk_buff *skb;

#if 0
	struct dn_naddr dnaddr;
#endif
	switch(cmd)
	{
	case SIOCGIFADDR:
	case SIOCSIFADDR:
		return dn_dev_ioctl(cmd, (void *)arg);

#ifdef CONFIG_DECNET_ROUTER
	case SIOCADDRT:
	case SIOCDELRT:
		return dn_fib_ioctl(sock, cmd, arg);
#endif /* CONFIG_DECNET_ROUTER */

#if 0
	case SIOCSIFADDR:
		if (!suser())    return -EPERM;

		if ((err = copy_from_user(devname, ioarg->devname, 5)) != 0)
			break;
		if ((err = copy_from_user(addr, ioarg->exec_addr, 6)) != 0)
			break;
		if ((dev = dev_get(devname)) == NULL) {
			err = -ENODEV;
			break;
		}
		if (dev->dn_ptr == NULL) {
			err = -ENODEV;
			break;
		}

		dn_dev_devices_off();

		decnet_default_device = dev;
		memcpy(decnet_ether_address, addr, ETH_ALEN);
		decnet_address = dn_htons(dn_eth2dn(decnet_ether_address));

		dn_dev_devices_on();

		break;

	case SIOCGIFADDR:
		if (decnet_default_device)
			strcpy(devname, decnet_default_device->name);
		else
			memset(devname, 0, 6);

		if ((err = copy_to_user(ioarg->devname, devname, 5)) != 0)
			break;

		if ((err = copy_to_user(ioarg->exec_addr, decnet_ether_address, 6)) != 0)
			break;

		break;
#endif

#if 0
	case SIOCSNETADDR:
		if (!suser()) {
			err = -EPERM;
			break;
		}

		if ((err = copy_from_user(&dnaddr, (void *)arg, sizeof(struct dn_naddr))) != 0)
			break;

		if (dnaddr.a_len != ETH_ALEN) {
			err = -EINVAL;
			break;
		}

		dn_dev_devices_off();

		memcpy(decnet_ether_address, dnaddr.a_addr, ETH_ALEN);
		decnet_address = dn_htons(dn_eth2dn(decnet_ether_address));

		dn_dev_devices_on();
		break;

	case SIOCGNETADDR:
		dnaddr.a_len = ETH_ALEN;
		memcpy(dnaddr.a_addr, decnet_ether_address, ETH_ALEN);

		if ((err = copy_to_user((void *)arg, &dnaddr, sizeof(struct dn_naddr))) != 0)
			break;

		break;
#endif
	case OSIOCSNETADDR:
		if (!suser()) {
			err = -EPERM;
			break;
		}

		dn_dev_devices_off();

		decnet_address = (unsigned short)arg;
		dn_dn2eth(decnet_ether_address, dn_ntohs(decnet_address));

		dn_dev_devices_on();
		err = 0;
		break;

	case OSIOCGNETADDR:
		if ((err = put_user(decnet_address, (unsigned short *)arg)) != 0)
			break;
		break;
        case SIOCGIFCONF:
        case SIOCGIFFLAGS:
        case SIOCGIFBRDADDR:
                return dev_ioctl(cmd,(void *)arg);

	case TIOCOUTQ:
		amount = sk->sndbuf - atomic_read(&sk->wmem_alloc);
		if (amount < 0)
			amount = 0;
		err = put_user(amount, (int *)arg);
		break;

	case TIOCINQ:
		lock_sock(sk);
		if ((skb = skb_peek(&sk->receive_queue)) != NULL)
			amount = skb->len;
		release_sock(sk);
		err = put_user(amount, (int *)arg);
		break;
	}

	return err;
}

static int dn_listen(struct socket *sock, int backlog)
{
	struct sock *sk = sock->sk;

	if (sk->zapped)
		return -EINVAL;

	if ((sk->protinfo.dn.state != DN_O) || (sk->state == TCP_LISTEN))
		return -EINVAL;

	if (backlog > SOMAXCONN)
		backlog = SOMAXCONN;

	sk->max_ack_backlog = backlog;
	sk->ack_backlog     = 0;
	sk->state           = TCP_LISTEN;

        return 0;
}


static int dn_shutdown(struct socket *sock, int how)
{
	struct sock *sk = sock->sk;
	struct dn_scp *scp = &sk->protinfo.dn;
	int err = -ENOTCONN;

	lock_sock(sk);

	if (sock->state == SS_UNCONNECTED)
		goto out;

	err = 0;
	if (sock->state == SS_DISCONNECTING)
		goto out;

	err = -EINVAL;
	if (scp->state == DN_O)
		goto out;

	if (how != SHUTDOWN_MASK)
		goto out;


	sk->shutdown = how;
	dn_destroy_sock(sk);
	err = 0;

out:
	release_sock(sk);

	return err;
}

static int dn_setsockopt(struct socket *sock, int level, int optname, char *optval, int optlen)
{
	return _dn_setsockopt(sock, level, optname, optval, optlen, 0);
}

static int _dn_setsockopt(struct socket *sock, int level,int optname, char *optval, int optlen, int flags) 
{
	struct	sock *sk = sock->sk;
	struct dn_scp *scp = &sk->protinfo.dn;
	struct optdata_dn opt;
	struct accessdata_dn acc;
#ifdef CONFIG_DECNET_FW
	char tmp_fw[MAX(sizeof(struct dn_fwtest),sizeof(struct dn_fwnew))];
#endif
	int err;

	if (optlen && !optval)
		return -EINVAL;

	switch(optname) {
		case DSO_CONDATA:
			if (sock->state == SS_CONNECTED) 
				return -EISCONN;
			if ((scp->state != DN_O) && (scp->state != DN_CR))
				return -EINVAL;

			if (optlen != sizeof(struct optdata_dn))
				return -EINVAL;

			if (copy_from_user(&opt, optval, optlen))
				return -EFAULT;

			if (opt.opt_optl > 16)
				return -EINVAL;

			memcpy(&scp->conndata_out, &opt, sizeof(struct optdata_dn));
			break;

		case DSO_DISDATA:
	   	        if (sock->state != SS_CONNECTED && sk->protinfo.dn.accept_mode == ACC_IMMED)
				return -ENOTCONN;

			if (optlen != sizeof(struct optdata_dn))
				return -EINVAL;

			if (copy_from_user(&opt, optval, sizeof(struct optdata_dn)))
				return -EFAULT;

			if (opt.opt_optl > 16)
				return -EINVAL;

			memcpy(&scp->discdata_out, &opt, sizeof(struct optdata_dn));
			break;

		case DSO_CONACCESS:
			if (sock->state == SS_CONNECTED) 
				return -EISCONN;
			if (scp->state != DN_O)
				return -EINVAL;

			if (optlen != sizeof(struct accessdata_dn))
				return -EINVAL;

			if (copy_from_user(&acc, optval, sizeof(struct accessdata_dn)))
				return -EFAULT;

			if ((acc.acc_accl > DN_MAXACCL) ||
					(acc.acc_passl > DN_MAXACCL) ||
					(acc.acc_userl > DN_MAXACCL))
				return -EINVAL;

			memcpy(&scp->accessdata, &acc, sizeof(struct accessdata_dn));
			break;

		case DSO_ACCEPTMODE:
			if (sock->state == SS_CONNECTED)
				return -EISCONN;
			if (scp->state != DN_O)
				return -EINVAL;

			if (optlen != sizeof(int))
				return -EINVAL;

			{
				int mode;

				if (get_user(mode, optval))
					return -EFAULT;
				if ((mode != ACC_IMMED) && (mode != ACC_DEFER))
					return -EINVAL;

				scp->accept_mode = (unsigned char)mode;
			}
			break;

		case DSO_CONACCEPT:
			lock_sock(sk);

			if (scp->state != DN_CR)
				return -EINVAL;

			scp->state = DN_CC;
			dn_send_conn_conf(sk);
			err = dn_wait_accept(sock, sock->file->f_flags);
			release_sock(sk);
			return err;

		case DSO_CONREJECT:
			lock_sock(sk);

			if (scp->state != DN_CR)
				return -EINVAL;

			scp->state = DN_DR;
			sk->shutdown = SHUTDOWN_MASK;
			dn_send_disc(sk, 0x38, 0);
			release_sock(sk);
			break;

#ifdef CONFIG_DECNET_FW
		case DN_FW_MASQ_TIMEOUTS:
                case DN_FW_APPEND:
                case DN_FW_REPLACE:
                case DN_FW_DELETE:
                case DN_FW_DELETE_NUM:
                case DN_FW_INSERT:
                case DN_FW_FLUSH:
                case DN_FW_ZERO:
                case DN_FW_CHECK:
                case DN_FW_CREATECHAIN:
                case DN_FW_DELETECHAIN:
                case DN_FW_POLICY:

                        if (!capable(CAP_NET_ADMIN))
                                return -EACCES;
                        if ((optlen > sizeof(tmp_fw)) || (optlen < 1))
                                return -EINVAL;
                        if (copy_from_user(&tmp_fw, optval, optlen))
                                return -EFAULT;
                        err = dn_fw_ctl(optname, &tmp_fw, optlen);
                        return -err;    /* -0 is 0 after all */
#endif
		default:
		case DSO_LINKINFO:
		case DSO_STREAM:
		case DSO_SEQPACKET:

			return -EOPNOTSUPP;
	}

	return 0;
}

static int dn_getsockopt(struct socket *sock, int level, int optname, char *optval, int *optlen)
{
	return _dn_getsockopt(sock, level, optname, optval, optlen, 0);
}

static int
_dn_getsockopt(struct socket *sock, int level,int optname, char *optval,int *optlen, int flags)
{
	struct	sock *sk = sock->sk;
	struct dn_scp *scp = &sk->protinfo.dn;
	struct linkinfo_dn link;
	int mode = scp->accept_mode;

	switch(optname) {
		case DSO_CONDATA:
			if (*optlen != sizeof(struct optdata_dn))
				return -EINVAL;

			if (copy_to_user(optval, &scp->conndata_in, sizeof(struct optdata_dn)))
				return -EFAULT;
			break;

		case DSO_DISDATA:
			if (*optlen != sizeof(struct optdata_dn))
				return -EINVAL;

			if (copy_to_user(optval, &scp->discdata_in, sizeof(struct optdata_dn)))
				return -EFAULT;

			break;

		case DSO_CONACCESS:
			if (*optlen != sizeof(struct accessdata_dn))
				return -EINVAL;

			if (copy_to_user(optval, &scp->accessdata, sizeof(struct accessdata_dn)))
				return -EFAULT;
			break;

		case DSO_ACCEPTMODE:
			if (put_user(mode, optval))
				return -EFAULT;
			break;

		case DSO_LINKINFO:
			if (*optlen != sizeof(struct linkinfo_dn))
				return -EINVAL;

			switch(sock->state) {
				case SS_CONNECTING:
					link.idn_linkstate = LL_CONNECTING;
					break;
				case SS_DISCONNECTING:
					link.idn_linkstate = LL_DISCONNECTING;
					break;
				case SS_CONNECTED:
					link.idn_linkstate = LL_RUNNING;
					break;
				default:
					link.idn_linkstate = LL_INACTIVE;
			}

			link.idn_segsize = scp->mss;

			if (copy_to_user(optval, &link, sizeof(struct linkinfo_dn)))
				return -EFAULT;
			break;

		case DSO_STREAM:
		case DSO_SEQPACKET:
		case DSO_CONACCEPT:
		case DSO_CONREJECT:
		default:
        		return -EOPNOTSUPP;
	}

	return 0;
}


/*
 * Used by send/recvmsg to wait until the socket is connected
 * before passing data.
 */
static int dn_wait_run(struct sock *sk, int flags)
{
	struct dn_scp *scp = &sk->protinfo.dn;
	int err = 0;

	/* printk(KERN_DEBUG "dn_wait_run %d\n", scp->state); */

	switch(scp->state) {
		case DN_RUN:
			return 0;

		case DN_CR:
			scp->state = DN_CC;
			dn_send_conn_conf(sk);
			return dn_wait_accept(sk->socket, (flags & MSG_DONTWAIT) ? O_NONBLOCK : 0);
		case DN_CI:
		case DN_CC:
			break;
		default:
			return -ENOTCONN;
			goto out;
	}

	if (flags & MSG_DONTWAIT)
		return -EWOULDBLOCK;

	do {
		if ((err = sock_error(sk)) != 0)
			goto out;

		if (signal_pending(current)) {
			err = -ERESTARTSYS;
			goto out;
		}

		SOCK_SLEEP_PRE(sk)

		if (scp->state != DN_RUN)
			schedule();

		SOCK_SLEEP_POST(sk)

	} while(scp->state != DN_RUN);

out:

	return 0;
}


static int dn_data_ready(struct sock *sk, struct sk_buff_head *q, int flags, int target)
{
	struct sk_buff *skb = q->next;
	int len = 0;

	if (flags & MSG_OOB)
		return skb_queue_len(q) ? 1 : 0;

	while(skb != (struct sk_buff *)q) {
		struct dn_skb_cb *cb = (struct dn_skb_cb *)skb->cb;
		len += skb->len;

		if (cb->nsp_flags & 0x40) {
			/* SOCK_SEQPACKET reads to EOM */
			if (sk->type == SOCK_SEQPACKET)
				return 1;
			/* so does SOCK_STREAM unless WAITALL is specified */
			if (!(flags & MSG_WAITALL))
				return 1;
		}

		/* minimum data length for read exceeded */
		if (len >= target)
			return 1;
	}

	return 0;
}


static int dn_recvmsg(struct socket *sock, struct msghdr *msg, int size,
	int flags, struct scm_cookie *scm)
{
	struct sock *sk = sock->sk;
	struct dn_scp *scp = &sk->protinfo.dn;
	struct sk_buff_head *queue = &sk->receive_queue;
	int target = size > 1 ? 1 : 0;
	int copied = 0;
	int rv = 0;
	struct sk_buff *skb = NULL, **pskb;
	struct dn_skb_cb *cb = NULL;

	lock_sock(sk);

	if (sk->zapped) {
		rv = -EADDRNOTAVAIL;
		goto out;
	}

	if ((rv = dn_wait_run(sk, flags)) != 0)
		goto out;

	if (sk->shutdown & RCV_SHUTDOWN) {
		send_sig(SIGPIPE, current, 0);
		rv = -EPIPE;
		goto out;
	}

	if (flags & ~(MSG_PEEK|MSG_OOB|MSG_WAITALL|MSG_DONTWAIT)) {
		rv = -EOPNOTSUPP;
		goto out;
	}

	if (flags & MSG_OOB)
		queue = &scp->other_receive_queue;

	if (flags & MSG_WAITALL)
		target = size;


	/*
	 * See if there is data ready to read, sleep if there isn't
	 */
	for(;;) {
		if (sk->err)
			goto out;

		if (skb_queue_len(&scp->other_receive_queue)) {
			if (!(flags & MSG_OOB)) {
				msg->msg_flags |= MSG_OOB;
				if (!scp->other_report) {
					scp->other_report = 1;
					goto out;
				}
			}
		}
		
		if (scp->state != DN_RUN)
			goto out;

		if (signal_pending(current)) {
			rv = -ERESTARTSYS;
			goto out;
		}

		if (dn_data_ready(sk, queue, flags, target))
			break;

		if (flags & MSG_DONTWAIT) {
			rv = -EWOULDBLOCK;
			goto out;
		}

		sock->flags |= SO_WAITDATA;
		SOCK_SLEEP_PRE(sk)

		if (!dn_data_ready(sk, queue, flags, target))
			schedule();

		SOCK_SLEEP_POST(sk)
		sock->flags &= ~SO_WAITDATA;
	}

	pskb = &((struct sk_buff *)queue)->next;
	while((skb = *pskb) != (struct sk_buff *)queue) {
		int chunk = skb->len;
		cb = (struct dn_skb_cb *)skb->cb;

		if ((chunk + copied) > size)
			chunk = size - copied;

		if (memcpy_toiovec(msg->msg_iov, skb->data, chunk)) {
			rv = -EFAULT;
			break;
		}
		copied += chunk;

		if (!(flags & MSG_PEEK))
			skb->len -= chunk;

		if (skb->len == 0) {
			skb_unlink(skb);
			kfree_skb(skb);
			if ((scp->flowloc_sw == DN_DONTSEND) && !dn_congested(sk)) {
				scp->flowloc_sw = DN_SEND;
				dn_nsp_send_lnk(sk, DN_SEND);
			}
		}

		pskb = &skb->next;

		if (cb->nsp_flags & 0x40) {
                        if (sk->type == SOCK_SEQPACKET) {
                                msg->msg_flags |= MSG_EOR;
				break;
			}
		}

		if (!(flags & MSG_WAITALL))
			break;

		if (flags & MSG_OOB)
			break;

		if (copied >= target)
			break;
	}

	rv = copied;
out:
	if (rv == 0)
		rv = (flags & MSG_PEEK) ? -sk->err : sock_error(sk);

	release_sock(sk);

	return rv;
}


static int dn_sendmsg(struct socket *sock, struct msghdr *msg, int size, 
	   struct scm_cookie *scm)
{
	struct sock *sk = sock->sk;
	struct dn_scp *scp = &sk->protinfo.dn;
	int mss = scp->mss;
	int mtu = 230 - 11; /* maximum value thats always safe */
	struct sk_buff_head *queue = &scp->data_xmit_queue;
	int flags = msg->msg_flags;
	unsigned short numseg = 0;
	int err = 0;
	int sent = 0;
	int addr_len = msg->msg_namelen;
	struct sockaddr_dn *addr = (struct sockaddr_dn *)msg->msg_name;
	struct sk_buff *skb = NULL;
	struct dn_skb_cb *cb;
	unsigned char msgflg;
	unsigned char *ptr;
	unsigned short ack;
	int len;

	if (flags & ~(MSG_TRYHARD|MSG_OOB|MSG_DONTWAIT))
		return -EOPNOTSUPP;

	if (addr_len && (addr_len != sizeof(struct sockaddr_dn)))
		return -EINVAL;

	if (sk->zapped && dn_auto_bind(sock))  {
		err = -EADDRNOTAVAIL;
		goto out;
	}

	if (scp->state == DN_O) {
		if (!addr_len || !addr) {
			err = -ENOTCONN;
			goto out;
		}

		if ((err = dn_connect(sock, (struct sockaddr *)addr, addr_len, (flags & MSG_DONTWAIT) ? O_NONBLOCK : 0)) < 0)
			goto out;
	}

	lock_sock(sk);

	if ((err = dn_wait_run(sk, flags)) < 0)
		goto out;

	if (sk->shutdown & SEND_SHUTDOWN) {
		send_sig(SIGPIPE, current, 0);
		err = -EPIPE;
		goto out;
	}

	if ((flags & MSG_TRYHARD) && sk->dst_cache)
		dst_negative_advice(&sk->dst_cache);

	if (sk->dst_cache && sk->dst_cache->neighbour) {
		struct dn_neigh *dn = (struct dn_neigh *)sk->dst_cache->neighbour;
		if (dn->blksize > 230)
			mtu = dn->blksize - 11;
	}

	/*
	 * The only difference between SEQPACKET & STREAM sockets under DECnet
	 * AFAIK is that SEQPACKET sockets set the MSG_EOR flag for the last
	 * session control message segment.
	 */

	if (flags & MSG_OOB) {
		mss = 16;
		queue = &scp->other_xmit_queue;
		if (size > mss) {
			err = -EMSGSIZE;
			goto out;
		}
	}

	if (mss < mtu)
		mtu = mss;

	scp->persist_fxn = dn_nsp_xmit_timeout;

	while(sent < size) {
		if ((err = sock_error(sk) != 0))
			goto out;

		if (signal_pending(current)) {
			err = -ERESTARTSYS;
			goto out;
		}

		/*
		 * Calculate size that we wish to send.
		 */
		len = size - sent;

		if (len > mtu)
			len = mtu;

		/*
		 * Wait for queue size to go down below the window
		 * size.
		 */
		if (skb_queue_len(queue) >= scp->snd_window) {
			if (flags & MSG_DONTWAIT) {
				err = -EWOULDBLOCK;
				goto out;
			}

			SOCK_SLEEP_PRE(sk)

			if (skb_queue_len(queue) >= scp->snd_window)
				schedule();

			SOCK_SLEEP_POST(sk)

			continue;
		}

		/*
		 * Get a suitably sized skb.
		 */
		skb = dn_alloc_send_skb(sk, &len, flags & MSG_DONTWAIT, &err);

		if (err)
			break;

		if (!skb)
			continue;

		cb = (struct dn_skb_cb *)skb->cb;

		ptr = skb_put(skb, 9);

		if (memcpy_fromiovec(skb_put(skb, len), msg->msg_iov, len)) {
			err = -EFAULT;
			goto out;
		}

		if (flags & MSG_OOB) {
			cb->segnum = scp->numoth++;
			scp->numoth &= 0x0fff;
			msgflg = 0x30;
			ack = scp->ackxmt_oth | 0x8000;
		} else {
			cb->segnum = scp->numdat++;
			scp->numdat &= 0x0fff;
			if (sock->type == SOCK_STREAM)
				msgflg = 0x60;
			else
				msgflg = 0x00;
			if (numseg == 0)
				msgflg |= 0x20;
			if ((sent + len) == size)
				msgflg |= 0x40;
			ack = scp->ackxmt_dat | 0x8000;
		}

		*ptr++ = msgflg;
		*(__u16 *)ptr = scp->addrrem;
		ptr += 2;
		*(__u16 *)ptr = scp->addrloc;
		ptr += 2;
		*(__u16 *)ptr = dn_htons(ack);
		ptr += 2;
		*(__u16 *)ptr = dn_htons(cb->segnum);

		sent += len;
		dn_nsp_queue_xmit(sk, skb, flags & MSG_OOB);
		numseg++;
		skb = NULL;

		scp->persist = dn_nsp_persist(sk);

	}
out:

	if (skb)
		kfree_skb(skb);

	release_sock(sk);

	return sent ? sent : err;
}

static int dn_device_event(struct notifier_block *this, unsigned long event,
			void *ptr)
{
	struct device *dev = (struct device *)ptr;

	switch(event) {
		case NETDEV_UP:
			dn_dev_up(dev);
			break;
		case NETDEV_DOWN:
			dn_dev_down(dev);
			break;
		default:
			break;
	}

	return NOTIFY_DONE;
}

static struct notifier_block dn_dev_notifier = {
	dn_device_event,
	0
};

extern int dn_route_rcv(struct sk_buff *, struct device *, struct packet_type *);

static struct packet_type dn_dix_packet_type = 
{
	__constant_htons(ETH_P_DNA_RT),
	NULL,		/* All devices */
	dn_route_rcv,
	NULL,
	NULL,
};

#ifdef CONFIG_PROC_FS
struct proc_dir_entry decnet_linkinfo = {
	PROC_NET_DN_SKT, 6, "decnet", S_IFREG | S_IRUGO,
	1, 0, 0, 0, &proc_net_inode_operations, dn_get_info
};

#ifdef CONFIG_DECNET_RAW

extern int dn_raw_get_info(char *, char **, off_t, int, int);

struct proc_dir_entry decnet_rawinfo = {
	PROC_NET_DN_RAW, 10, "decnet_raw", S_IFREG | S_IRUGO,
	1, 0, 0, 0, &proc_net_inode_operations, dn_raw_get_info
};

#endif /* CONFIG_DECNET_RAW */
#endif /* CONFIG_PROC_FS */
static struct net_proto_family	dn_family_ops = {
	AF_DECnet,
	dn_create
};

static struct proto_ops dn_proto_ops = {
        AF_DECnet,

	sock_no_dup,
	dn_release,
	dn_bind,
	dn_connect,
	sock_no_socketpair,
	dn_accept,
	dn_getname,
	dn_poll,
	dn_ioctl,
	dn_listen,
	dn_shutdown,
	dn_setsockopt,
	dn_getsockopt,
	sock_no_fcntl,
        dn_sendmsg,
	dn_recvmsg
};

#ifdef CONFIG_SYSCTL
void dn_register_sysctl(void);
void dn_unregister_sysctl(void);
#endif

void __init decnet_proto_init(struct net_proto *pro)
{
	sock_register(&dn_family_ops);
	dev_add_pack(&dn_dix_packet_type);
	register_netdevice_notifier(&dn_dev_notifier);

#ifdef CONFIG_PROC_FS
	proc_net_register(&decnet_linkinfo);
#ifdef CONFIG_DECNET_RAW
	proc_net_register(&decnet_rawinfo);
#endif
#endif
	dn_dev_init();
	dn_neigh_init();
	dn_route_init();

#ifdef CONFIG_DECNET_FW
	dn_fw_init();
#endif /* CONFIG_DECNET_FW */

#ifdef CONFIG_DECNET_ROUTER
	dn_fib_init();
#endif /* CONFIG_DECNET_ROUTER */

#ifdef CONFIG_SYSCTL
	dn_register_sysctl();
#endif /* CONFIG_SYSCTL */
        printk(KERN_INFO "DECnet for Linux: V.2.2.5s (C) 1995-1999 Linux DECnet Project Team\n");

}

void __init decnet_setup(char *str, int *ints)
{

	if ((ints[0] == 2) || (ints[0] == 3)) {

		if (ints[1] < 0)
			ints[1] = 0;
		if (ints[1] > 63)
			ints[1] = 63;

		if (ints[2] < 0)
			ints[2] = 0;
		if (ints[2] > 1023)
			ints[2] = 1023;

		decnet_address = dn_htons(ints[1] << 10 | ints[2]);
		dn_dn2eth(decnet_ether_address, dn_ntohs(decnet_address));

		if (ints[0] == 3) {
			switch(ints[3]) {
#ifdef CONFIG_DECNET_ROUTER
				case 1:
					decnet_node_type = DN_RT_INFO_L1RT;
					break;
				case 2:
					decnet_node_type = DN_RT_INFO_L2RT;
					break;
#endif /* CONFIG_DECNET_ROUTER */
				default:
					decnet_node_type = DN_RT_INFO_ENDN;
			}
		}
	} else {
		printk(KERN_ERR "DECnet: Invalid command line options\n");
	}
}

#ifdef MODULE
EXPORT_NO_SYMBOLS;
MODULE_DESCRIPTION("The Linux DECnet Network Protocol");
MODULE_AUTHOR("Linux DECnet Project Team");

static int addr[2] = {0, 0};
#ifdef CONFIG_DECNET_ROUTER
static int type = 0;
#endif

MODULE_PARM(addr, "2i");
MODULE_PARM_DESC(addr, "The DECnet address of this machine: area,node");
#ifdef CONFIG_DECNET_ROUTER
MODULE_PARM(type, "i");
MODULE_PARM_DESC(type, "The type of this DECnet node: 0=EndNode, 1,2=Router");
#endif

int init_module(void)
{
	if (addr[0] > 63 || addr[0] < 0) {
		printk(KERN_ERR "DECnet: Area must be between 0 and 63");
		return 1;
	}

	if (addr[1] > 1023 || addr[1] < 0) {
		printk(KERN_ERR "DECnet: Node must be between 0 and 1023");
		return 1;
	}

	decnet_address = dn_htons((addr[0] << 10) | addr[1]);
	dn_dn2eth(decnet_ether_address, dn_ntohs(decnet_address));

#ifdef CONFIG_DECNET_ROUTER
	switch(type) {
		case 0:
			decnet_node_type = DN_RT_INFO_ENDN;
			break;
		case 1:
			decnet_node_type = DN_RT_INFO_L1RT;
			break;
		case 2:
			decnet_node_type = DN_RT_INFO_L2RT;
			break;
		default:
			printk(KERN_ERR "DECnet: Node type must be between 0 and 2 inclusive\n");
			return 1;
	}
#else
	decnet_node_type = DN_RT_INFO_ENDN;
#endif

	decnet_proto_init(NULL);

	return 0;
}

void cleanup_module(void)
{
#ifdef CONFIG_SYSCTL
	dn_unregister_sysctl();
#endif /* CONFIG_SYSCTL */

	unregister_netdevice_notifier(&dn_dev_notifier);

	dn_route_cleanup();
	dn_neigh_cleanup();
	dn_dev_cleanup();

#ifdef CONFIG_DECNET_FW
	/* dn_fw_cleanup(); */
#endif /* CONFIG_DECNET_FW */

#ifdef CONFIG_DECNET_ROUTER
	dn_fib_cleanup();
#endif /* CONFIG_DECNET_ROUTER */

#ifdef CONFIG_PROC_FS
	proc_net_unregister(PROC_NET_DN_SKT);
#ifdef CONFIG_DECNET_RAW
	proc_net_unregister(PROC_NET_DN_RAW);
#endif
#endif

	dev_remove_pack(&dn_dix_packet_type);
	sock_unregister(AF_DECnet);
}

#endif
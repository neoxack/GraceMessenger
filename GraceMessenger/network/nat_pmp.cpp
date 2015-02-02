#include "nat_pmp.h"

#include <libnatpmp/natpmp.h>


void GraceMessenger::Network::nat_pmp::forward_port(unsigned short localPort, unsigned short publicPort)
{
	int r;
	natpmp_t natpmp;
	natpmpresp_t response;
	initnatpmp(&natpmp,0,0);
	sendnewportmappingrequest(&natpmp, NATPMP_PROTOCOL_UDP, localPort, publicPort, 43200);
	do {
		fd_set fds;
		struct timeval timeout;
		FD_ZERO(&fds);
		FD_SET(natpmp.s, &fds);
		getnatpmprequesttimeout(&natpmp, &timeout);
		select(FD_SETSIZE, &fds, nullptr, nullptr, &timeout);
		r = readnatpmpresponseorretry(&natpmp, &response);
	} while (r == NATPMP_TRYAGAIN);
	closenatpmp(&natpmp);
}


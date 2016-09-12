/**
 * \file
 *
 * \brief Ethernet management for LwIP (threaded mode).
 *
 * Copyright (c) 2013-2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include <string.h>
#include "board.h"
#include "netif/ethernet.h"
# include "ksz8851snl.h"
# include "ksz8851snl_reg.h"
# include "netif/sam_spi_ksz8851snl.h"
#include "sysclk.h"
/* lwIP includes */
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/tcp.h"
#include "lwip/tcpip.h"
#include "lwip/memp.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "lwip/stats.h"
#include "lwip/init.h"
#include "lwip/ip_frag.h"
#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
#include "netif/loopif.h"
#else
#include "lwip/inet.h"
#include "lwip/tcp_impl.h"
#endif
#include "netif/etharp.h"
#include "misc/debug.h"
//#include "misc/param_store.h"

/* Global variable containing MAC Config (hw addr, IP, GW, ...) */
struct netif gs_net_if;

//xQueueHandle dhcp_done = NULL;

/**
 * \brief Configure ethernet interface.
 */
static void ethernet_configure_interface(void)
{
	struct ip_addr x_ip_addr, x_net_mask, x_gateway;
	extern err_t ethernetif_init(struct netif *netif);

#if defined(DHCP_USED)
		/* DHCP mode. */
		x_ip_addr.addr = 0;
		x_net_mask.addr = 0;
#else
		/* Fixed IP mode. */
		/* Default IP addr */
		IP4_ADDR(&x_ip_addr, ETHERNET_CONF_IPADDR0, ETHERNET_CONF_IPADDR1,
				ETHERNET_CONF_IPADDR2, ETHERNET_CONF_IPADDR3);

		/* Default subnet mask. */
		IP4_ADDR(&x_net_mask, ETHERNET_CONF_NET_MASK0, ETHERNET_CONF_NET_MASK1,
				ETHERNET_CONF_NET_MASK2, ETHERNET_CONF_NET_MASK3);

		/* Default gateway addr. */
		IP4_ADDR(&x_gateway, ETHERNET_CONF_GATEWAY_ADDR0,
				ETHERNET_CONF_GATEWAY_ADDR1,
				ETHERNET_CONF_GATEWAY_ADDR2,
				ETHERNET_CONF_GATEWAY_ADDR3);
#endif

	/* Use ethernet_input as input method for standalone lwIP mode. */
	/* Use tcpip_input as input method for threaded lwIP mode. */
	if (NULL == netif_add(&gs_net_if, &x_ip_addr, &x_net_mask, &x_gateway, NULL,
			ethernetif_init, tcpip_input)) {
		LWIP_ASSERT("NULL == netif_add", 0);
	}

	/* Make it the default interface. */
	netif_set_default(&gs_net_if);

	/* Setup callback function for netif status change. */
	netif_set_status_callback(&gs_net_if, status_callback);

#if defined(DHCP_USED)
		/* DHCP mode. */
		if (ERR_OK != dhcp_start(&gs_net_if)) {
			LWIP_ASSERT("ERR_OK != dhcp_start", 0);
		}
#else
		/* Static mode. */
		netif_set_up(&gs_net_if);
#endif

}

/**
 * \brief Initialize ethernet stask.
 */
void init_ethernet(void)
{
	/* Initialize lwIP. */
	/* Call tcpip_init for threaded lwIP mode. */
	tcpip_init(NULL, NULL);

	/* Set hw and IP parameters, initialize MAC too */
	ethernet_configure_interface();
}

/**
 * \brief Status callback used to print assigned IP address.
 *
 * \param netif Instance to network interface.
 */
void status_callback(struct netif *netif)
{
	uint8_t ready;
	if (netif_is_up(netif)) {
		
		//param_write(LOCAL_SEV_IPADDR_ID, &(netif->ip_addr));
		//printf("IP Address: %s\r\n", inet_ntoa(*(struct in_addr *)&(netif->ip_addr)));
		//IoT_DEBUG(ETHERNET_DBG | IoT_DBG_INFO, 
		//		 ("IP Address: %s\r\n", inet_ntoa(netif->ip_addr)));
		//IoT_xQueueSend(dhcp_done, &ready, 0);
	}
}

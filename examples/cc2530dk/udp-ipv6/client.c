/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include <string.h>
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "debug.h"

#define DEBUG DEBUG_NONE
#include "net/ip/uip-debug.h"

#define MAX_PAYLOAD_LEN		5

static char buf[MAX_PAYLOAD_LEN];

/* Our destinations and udp conns. One link-local and one global */
#define LOCAL_CONN_PORT 3001
static struct uip_udp_conn *l_conn;

PROCESS(udp_client_process, "UDP client process");
AUTOSTART_PROCESSES(&udp_client_process);
extern process_event_t serial_line_event_message;

static void
udp_send_serial(const char *data)
{
  int left, len;
  leds_on(LEDS_RED);
  left = strlen(data);
  while (left > 0)
  {
    len = MIN(left, MAX_PAYLOAD_LEN);
    uip_udp_packet_send(l_conn, data, len);
    data += len;
    left -= len;
  }
  leds_off(LEDS_RED);
}

static void
on_packet_received(void)
{
  PRINTF("ignore incoming packet\n");
  uip_clear_buf();
}

RIME_SNIFFER(packet_remover, on_packet_received, NULL);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  uip_ipaddr_t ipaddr;

  PROCESS_BEGIN();

  uip_create_linklocal_allrouters_mcast(&ipaddr);
  /* new connection with remote host */
  l_conn = udp_new(&ipaddr, UIP_HTONS(3000), NULL);
  if(!l_conn) {
    PRINTF("udp_new l_conn error.\n");
  }
  udp_bind(l_conn, UIP_HTONS(LOCAL_CONN_PORT));

  PRINTF("Link-Local connection with ");
  PRINT6ADDR(&l_conn->ripaddr);
  PRINTF(" local/remote port %u/%u\n",
         UIP_HTONS(l_conn->lport), UIP_HTONS(l_conn->rport));

  rime_sniffer_add(&packet_remover);

  while(1) {
    PROCESS_WAIT_EVENT();
    if (ev == serial_line_event_message) {
      udp_send_serial(data);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

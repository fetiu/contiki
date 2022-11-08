/*
 * Copyright (c) 2022 Jake Fetiu Kim . All rights reserved.
 * 
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
 */

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include <string.h>

#define DEBUG DEBUG_PRINT
#include "dev/leds.h"
#include "debug.h"
#include "uart0.h"
#include "serial-line.h"

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[uip_l2_l3_hdr_len])

static struct uip_udp_conn *rx_conn, *tx_conn;

PROCESS(lowusb2serial_process, "LoWUSB2Serial");
AUTOSTART_PROCESSES(&lowusb2serial_process);

static void
print_new_src(uip_ip6addr_t *new)
{
  static uip_ip6addr_t old;
  if (!uip_ipaddr_cmp(&old, new)) {
    putchar('\n');
    putchar('}');
    puthex(new->u8[14]);
    puthex(new->u8[15]);
    putchar('{');
    putchar('\n');
    memcpy(&old, new, sizeof old);
  }
}

static void
udp_recv_packet(void)
{
  if(uip_newdata()) {
    leds_on(LEDS_GREEN);
    print_new_src(&UIP_IP_BUF->srcipaddr);
    putstring(uip_appdata);
  }
  leds_off(LEDS_GREEN);
  return;
}

static void
udp_send_serial(const char *data)
{
  int len = strlen(data) + 1; // for '\0'
  uip_udp_packet_send(tx_conn, data, len);
}

PROCESS_THREAD(lowusb2serial_process, ev, data)
{
  uip_ipaddr_t ripaddr;
  PROCESS_BEGIN();

  uip_create_linklocal_allrouters_mcast(&ripaddr);
  tx_conn = udp_new(&ripaddr, UIP_HTONS(3000), NULL);
  rx_conn = udp_new(NULL, UIP_HTONS(0), NULL);

  udp_bind(tx_conn, UIP_HTONS(3001));
  udp_bind(rx_conn, UIP_HTONS(3000));

  uart0_set_input(serial_line_input_byte);

  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      udp_recv_packet();
    } else if (ev == serial_line_event_message) {
      udp_send_serial(data);
    }
  }

  PROCESS_END();
}

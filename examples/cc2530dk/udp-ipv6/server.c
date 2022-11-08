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

#define DEBUG DEBUG_PRINT
#include "dev/leds.h"
#include "debug.h"

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[uip_l2_l3_hdr_len])

#define MAX_PAYLOAD_LEN 120

static struct uip_udp_conn *server_conn;
static char buf[MAX_PAYLOAD_LEN];

PROCESS(udp_server_process, "UDP server process");
AUTOSTART_PROCESSES(&udp_server_process);

static void
print_new_src(uip_ip6addr_t *new)
{
  static uip_ip6addr_t now;
  if (!uip_ipaddr_cmp(&now, &UIP_IP_BUF->srcipaddr)) {
    memcpy(&now, new, sizeof now);
    putchar('\n');
    putchar('}');
    puthex(now.u8[14]);
    puthex(now.u8[15]);
    putchar('{');
    putchar('\n');
  }
}

static void
tcpip_handler(void)
{
  if(uip_newdata()) {
    leds_on(LEDS_GREEN);
    print_new_src(&UIP_IP_BUF->srcipaddr);
    putstring(uip_appdata);
  }
  leds_off(LEDS_GREEN);
  return;
}

PROCESS_THREAD(udp_server_process, ev, data)
{
  PROCESS_BEGIN();

  // Disable transmission to nowent crash
  tcpip_set_outputfunc(NULL);
  server_conn = udp_new(NULL, UIP_HTONS(0), NULL);
  udp_bind(server_conn, UIP_HTONS(3000));

  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      tcpip_handler();
    }
  }

  PROCESS_END();
}

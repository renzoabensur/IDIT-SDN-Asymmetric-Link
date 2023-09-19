/*
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain this list of conditions
 *    and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE SOFTWARE PROVIDER OR DEVELOPERS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "dummy-metric.h"
#include "sdn-metric.h"

#define MAX_BATTERY_LEVEL 255UL

#define MAX_WEIGHT 100

//Defines minimum battery value for routing in node.
#define THRESHOLD_BATTERY_ROUTE 5 // in percent

#define MAX_ETX_METRIC 100 //1024 // 65535

static int get_weight(unsigned char energy, int etx);

static int get_weight(unsigned char energy, int etx) {

  float energy_n;
  float etx_n;
  int weight;

  energy_n = (float) energy;
  etx_n = (float) etx;

  if(energy < MAX_BATTERY_LEVEL * THRESHOLD_BATTERY_ROUTE / 100) {

    weight = MAX_WEIGHT;

  } else {

    //Scale ETX [0,1]
    if (etx >= MAX_ETX_METRIC) {
      etx_n = 1.0;
    } else {
      etx_n = (etx_n / MAX_ETX_METRIC);
    }

    //Scale Energy [0,1] and invert
    energy_n = 1.0 - (energy_n / (MAX_BATTERY_LEVEL * 1.0));

    etx_n = etx_n * WEIGHT_ETX;

    energy_n = energy_n * WEIGHT_ENERGY;

    weight = (int) ((etx_n + energy_n) * MAX_WEIGHT);
    //weight = (int) ((etx_n * energy_n) * MAX_WEIGHT);

    // If the weight is zero then it sets weight to one to calculate routes through shortest path.
    if(weight == 0) {

      weight = 1;
    }

  }

  return weight;
}

const struct sdn_metric dummy_metric = {
  get_weight
};
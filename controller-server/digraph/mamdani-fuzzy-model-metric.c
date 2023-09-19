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

#include "mamdani-fuzzy-model-metric.h"
#include "sdn-metric.h"

#define MAX_BATTERY_LEVEL 255UL

#define MAX_WEIGHT 100

//Defines minimum battery value for routing in node.
#define THRESHOLD_BATTERY_ROUTE 5 // in percent

#define MAX_ETX_METRIC 100 //1024 // 65535

// #define DEBUG_MAMDANI

#ifdef DEBUG_MAMDANI
	#define PRINTF_MAMDANI(...) printf(__VA_ARGS__);
#else // DEBUG_MAMDANI
  #define PRINTF_MAMDANI(...)
#endif // DEBUG_MAMDANI

enum{
	ETX_SMALL,
	ETX_AVERAGE,
	ETX_LONG,
	ETX_MAX_INDEX
};

enum{
	ENERGY_LOW,
	ENERGY_AVERAGE,
	ENERGY_HIGH,
	ENERGY_MAX_INDEX
};

enum{
	METRIC_VERY_BAD,
	METRIC_BAD,
	METRIC_AVERAGE,
	METRIC_GOOD,
	METRIC_VERY_GOOD,
	METRIC_MAX_INDEX
};

typedef struct triangle {
	float x[3];
	float y[3];
	float mass;
	float centroid;
} triangle; 

typedef struct rectangle {
	float x[4];
	float y[4];
	float mass;
	float centroid;
} rectangle; 

float etx_fuzz[ETX_MAX_INDEX];

float energy_fuzz[ENERGY_MAX_INDEX];

float metric_fuzz[METRIC_MAX_INDEX];

rectangle rect_very_good;
triangle right_very_good;
triangle left_good;
rectangle rect_good;
triangle right_good;
triangle left_average;
rectangle rect_average;
triangle right_average;
triangle left_bad;
rectangle rect_bad;
triangle right_bad;
triangle left_very_bad;
rectangle rect_very_bad;

void etx_fuzzification(float etx);

void energy_fuzzification(float energy);

void eval_rules();

float get_mean_of_maximum();

float get_centroid();

float verybad_defuzzification(float fuzzy);

float bad_defuzzification(float fuzzy);

float average_defuzzification(float fuzzy);

float good_defuzzification(float fuzzy);

float good_defuzzification(float fuzzy);

float verygood_defuzzification(float fuzzy);

static float get_max(float val1, float val2);

static float get_min(float val1, float val2);

static int get_weight(unsigned char energy, int etx);

static float get_max(float val1, float val2) {

	if(val1 > val2) {

		return val1;

	}

	return val2;
}

static float get_min(float val1, float val2) {

	if(val1 < val2) {

		return val1;

	}

	return val2;
}

static int get_weight(unsigned char energy, int etx) {

  float energy_n;
  float etx_n;
  float weight;

  energy_n = (float) energy;
  etx_n = (float) etx;

  if(energy < MAX_BATTERY_LEVEL * THRESHOLD_BATTERY_ROUTE / 100) {

    weight = MAX_WEIGHT;

  } else {

    //Scale ETX [0,1]
    if (etx >= MAX_ETX_METRIC) {
      etx_n = 1.0 * MAX_WEIGHT;
    } else {
      etx_n = (etx_n / MAX_ETX_METRIC) * MAX_WEIGHT;
    }

    //Scale Energy [0,1]
    energy_n = (energy_n / (MAX_BATTERY_LEVEL * 1.0)) * MAX_WEIGHT;

    etx_fuzzification(etx_n);

    energy_fuzzification(energy_n);

    eval_rules();

    // weight = get_centroid();

    weight = get_mean_of_maximum();

    // If the weight is less than zero then it sets weight to one to calculate routes through shortest path.
    if(weight < 1) {

      weight = 1;
    }

  }

  return (int) weight;
}

void eval_rules() {

	metric_fuzz[METRIC_VERY_BAD] = get_max(get_min(etx_fuzz[ETX_LONG], energy_fuzz[ENERGY_LOW]), 0);

	metric_fuzz[METRIC_BAD] = get_max(get_min(etx_fuzz[ETX_AVERAGE], energy_fuzz[ENERGY_LOW]), get_min(etx_fuzz[ETX_LONG], energy_fuzz[ENERGY_AVERAGE]));

	metric_fuzz[METRIC_AVERAGE] = get_max(get_min(etx_fuzz[ETX_SMALL], energy_fuzz[ENERGY_LOW]), get_min(etx_fuzz[ETX_AVERAGE], energy_fuzz[ENERGY_AVERAGE]));

	metric_fuzz[METRIC_AVERAGE] = get_max(metric_fuzz[METRIC_AVERAGE], get_min(etx_fuzz[ETX_LONG], energy_fuzz[ENERGY_HIGH]));

	metric_fuzz[METRIC_GOOD] = get_max(get_min(etx_fuzz[ETX_SMALL], energy_fuzz[ENERGY_AVERAGE]), get_min(etx_fuzz[ETX_AVERAGE], energy_fuzz[ENERGY_HIGH]));

	metric_fuzz[METRIC_VERY_GOOD] = get_max(get_min(etx_fuzz[ETX_SMALL], energy_fuzz[ENERGY_HIGH]), 0);

	PRINTF_MAMDANI("very_bad %f bad %f average %f good %f very_good %f\n", metric_fuzz[METRIC_VERY_BAD], metric_fuzz[METRIC_BAD], metric_fuzz[METRIC_AVERAGE], metric_fuzz[METRIC_GOOD], metric_fuzz[METRIC_VERY_GOOD]);

}

void zerofill_triangle(triangle *tri) {

	tri->x[0] = 0;
	tri->y[0] = 0;
	tri->x[1] = 0;
	tri->y[1] = 0;
	tri->x[2] = 0;
	tri->y[2] = 0;
	tri->mass = 0;
	tri->centroid = 0;
}

void zerofill_rect(rectangle *rect) {

	rect->x[0] = 0;
	rect->y[0] = 0;
	rect->x[1] = 0;
	rect->y[1] = 0;
	rect->x[2] = 0;
	rect->y[2] = 0;
	rect->x[3] = 0;
	rect->y[3] = 0;
	rect->mass = 0;
	rect->centroid = 0;
}

void calculate_rect(rectangle *rect) {

	rect->mass = (rect->y[0] - rect->y[3]) * (rect->x[2] - rect->x[3]);
	rect->centroid = rect->x[3] + (rect->x[2] - rect->x[3]) * 0.5;
}

void calculate_right_tri(triangle *tri) {

	tri->mass = (tri->y[0] - tri->y[2]) * (tri->x[1] - tri->x[2]) * 0.5;
	tri->centroid = tri->x[2] + (tri->x[1] - tri->x[2]) / 3;
}

void calculate_left_tri(triangle *tri) {

	tri->mass = (tri->y[0] - tri->y[1]) * (tri->x[1] - tri->x[2]) * 0.5;
	tri->centroid = tri->x[1] - (tri->x[1] - tri->x[2]) / 3;
}

// Defuzzification by Mean of Maximum (MoM)
float get_mean_of_maximum() {

	float mom = 0;
	int mom_qty = 0;

	float max_value = 0;

	int index_metric;

	for (index_metric = 0; index_metric < METRIC_MAX_INDEX; index_metric++) {

		if (metric_fuzz[index_metric] > max_value) {
			max_value = metric_fuzz[index_metric];
		}
	}

	if (metric_fuzz[METRIC_VERY_BAD] == max_value) {

		mom += verybad_defuzzification(metric_fuzz[METRIC_VERY_BAD]);
		mom_qty++;
	} 

	if (metric_fuzz[METRIC_BAD] == max_value) {

		mom += bad_defuzzification(metric_fuzz[METRIC_BAD]);
		mom_qty++;
	} 

	if (metric_fuzz[METRIC_AVERAGE] == max_value) {

		mom += average_defuzzification(metric_fuzz[METRIC_AVERAGE]);
		mom_qty++;
	} 

	if (metric_fuzz[METRIC_GOOD] == max_value) {

		mom += good_defuzzification(metric_fuzz[METRIC_GOOD]);
		mom_qty++;
	} 

	if (metric_fuzz[METRIC_VERY_GOOD] == max_value) {

		mom += verygood_defuzzification(metric_fuzz[METRIC_VERY_GOOD]);
		mom_qty++;
	}

	return mom / mom_qty;
}

// Defuzzification by centroid (aggregate_and_defuzzify)
float get_centroid() {

	float mass = 0;
	float mass_sum = 0;
	float mass_x_centroid = 0;
	float centroid = 0; 
	float z0 = 0;
	float z1 = 0;

	zerofill_rect(&rect_very_good);
	zerofill_triangle(&right_very_good);
	zerofill_triangle(&left_good);
	zerofill_rect(&rect_good);
	zerofill_triangle(&right_good);
	zerofill_triangle(&left_average);
	zerofill_rect(&rect_average);
	zerofill_triangle(&right_average);
	zerofill_triangle(&left_bad);
	zerofill_rect(&rect_bad);
	zerofill_triangle(&right_bad);
	zerofill_triangle(&left_very_bad);
	zerofill_rect(&rect_very_bad);

//#############################################################################################################

	if (metric_fuzz[METRIC_VERY_GOOD] > 0) {

		//Calculate rectangle METRIC_VERY_GOOD
		rect_very_good.x[0] = 0;
		rect_very_good.y[0] = metric_fuzz[METRIC_VERY_GOOD];
		rect_very_good.x[1] = 25 - (metric_fuzz[METRIC_VERY_GOOD] * 25);
		rect_very_good.y[1] = rect_very_good.y[0];
		rect_very_good.x[2] = rect_very_good.x[1];
		rect_very_good.y[2] = 0;
		rect_very_good.x[3] = rect_very_good.x[0];
		rect_very_good.y[3] = 0;

		calculate_rect(&rect_very_good);

PRINTF_MAMDANI("retangulo METRIC_VERY_GOOD: mass %f centroid %f\n", rect_very_good.mass, rect_very_good.centroid);

		//Calculate full right triangle METRIC_VERY_GOOD
		if (metric_fuzz[METRIC_GOOD] == 0) {

			right_very_good.x[0] = rect_very_good.x[1];
			right_very_good.y[0] = rect_very_good.y[1];
			right_very_good.x[1] = 25;
			right_very_good.y[1] = 0;
			right_very_good.x[2] = rect_very_good.x[1];
			right_very_good.y[2] = rect_very_good.y[3];

			calculate_right_tri(&right_very_good);

			mass = ((25 - z1) * metric_fuzz[METRIC_VERY_GOOD]) / 2.0;
			centroid = (25 - z1) / 3.0 + z0;

			mass_x_centroid += mass * centroid;
			mass_sum += mass;

PRINTF_MAMDANI("tri full r METRIC_VERY_GOOD: mass %f centroid %f\n", mass, centroid);
PRINTF_MAMDANI("tri full r METRIC_VERY_GOOD: mass %f centroid %f\n", right_very_good.mass, right_very_good.centroid);

		//Calculate parcial right triangle METRIC_VERY_GOOD		
		} else if (metric_fuzz[METRIC_VERY_GOOD] > metric_fuzz[METRIC_GOOD]) {

			right_very_good.x[0] = rect_very_good.x[1];
			right_very_good.y[0] = rect_very_good.y[1];
			right_very_good.x[1] = 25 - (metric_fuzz[METRIC_GOOD] * 25);
			right_very_good.y[1] = metric_fuzz[METRIC_GOOD];
			right_very_good.x[2] = right_very_good.x[0];
			right_very_good.y[2] = right_very_good.y[1];

			calculate_right_tri(&right_very_good);

PRINTF_MAMDANI("tri p r METRIC_VERY_GOOD: mass %f centroid %f\n", right_very_good.mass, right_very_good.centroid);

		//Calculate parcial left triangle METRIC_GOOD		
		} else {

			left_good.x[0] = metric_fuzz[METRIC_GOOD] * 25;
			left_good.y[0] = metric_fuzz[METRIC_GOOD];
			left_good.x[1] = left_good.x[0];
			left_good.y[1] = metric_fuzz[METRIC_VERY_GOOD];
			left_good.x[2] = (metric_fuzz[METRIC_VERY_GOOD] * 25);
			left_good.y[2] = left_good.y[1];

			calculate_left_tri(&left_good);

PRINTF_MAMDANI("tri p l METRIC_GOOD: mass %f centroid %f\n", left_good.mass, left_good.centroid);
		}	

	}

//#############################################################################################################

	if (metric_fuzz[METRIC_GOOD] > 0) {

		//Calculate rectangle
		rect_good.x[0] = metric_fuzz[METRIC_GOOD] * 25;
		rect_good.y[0] = metric_fuzz[METRIC_GOOD];
		rect_good.x[1] = 50 - rect_good.x[0];
		rect_good.y[1] = rect_good.y[0];
		rect_good.x[2] = rect_good.x[1];
		rect_good.y[2] = 0;
		rect_good.x[3] = rect_good.x[0];
		rect_good.y[3] = 0;

		calculate_rect(&rect_good);

PRINTF_MAMDANI("ret METRIC_GOOD: mass %f centroid %f\n", rect_good.mass, rect_good.centroid);

		//Calculate full left triangle
		if (metric_fuzz[METRIC_VERY_GOOD] == 0) {

			left_good.x[0] = rect_good.x[0];
			left_good.y[0] = rect_good.y[0];
			left_good.x[1] = left_good.x[0];
			left_good.y[1] = 0;
			left_good.x[2] = 0;
			left_good.y[2] = 0;

			calculate_left_tri(&left_good);

PRINTF_MAMDANI("tri full l METRIC_GOOD: mass %f centroid %f\n", left_good.mass, left_good.centroid);
		}	

		//Calculate full right triangle
		if (metric_fuzz[METRIC_AVERAGE] == 0) {

			right_good.x[0] = rect_good.x[1];
			right_good.y[0] = rect_good.y[1];
			right_good.x[1] = 50;
			right_good.y[1] = 0;
			right_good.x[2] = right_good.x[0];
			right_good.y[2] = 0;

			calculate_right_tri(&right_good);

PRINTF_MAMDANI("tri full r METRIC_GOOD: mass %f centroid %f\n", right_good.mass, right_good.centroid);

		//Calculate parcial right triangle METRIC_GOOD			
		} else if (metric_fuzz[METRIC_GOOD] > metric_fuzz[METRIC_AVERAGE]) {	

			right_good.x[0] = rect_good.x[1];
			right_good.y[0] = rect_good.y[1];
			right_good.x[1] = 50 - (metric_fuzz[METRIC_AVERAGE] * 25);
			right_good.y[1] = metric_fuzz[METRIC_AVERAGE];
			right_good.x[2] = right_good.x[0];
			right_good.y[2] = right_good.y[1];

			calculate_right_tri(&right_good);

			z0 = 50 - (metric_fuzz[METRIC_GOOD] * 25);;
			z1 = 50 - (metric_fuzz[METRIC_AVERAGE] * 25);

			mass = ((z1 - z0) * (metric_fuzz[METRIC_GOOD] - metric_fuzz[METRIC_AVERAGE])) / 2.0;
			centroid = (z1 - z0) / 3.0 + z0;

			mass_x_centroid += mass * centroid;
			mass_sum += mass;	

PRINTF_MAMDANI("tri p r METRIC_GOOD: mass %f centroid %f\n", mass, centroid);
PRINTF_MAMDANI("tri p r METRIC_GOOD: mass %f centroid %f\n", right_good.mass, right_good.centroid);

		//Calculate parcial left triangle METRIC_AVERAGE
		} else {

			left_average.x[0] = metric_fuzz[METRIC_AVERAGE] * 25 + 25;
			left_average.y[0] = metric_fuzz[METRIC_AVERAGE];
			left_average.x[1] = left_average.x[0];
			left_average.y[1] = metric_fuzz[METRIC_GOOD];
			left_average.x[2] = (metric_fuzz[METRIC_GOOD] * 25) + 25;
			left_average.y[2] = left_average.y[1];

			calculate_left_tri(&left_average);

PRINTF_MAMDANI("tri p l METRIC_AVERAGE: mass %f centroid %f\n", left_average.mass, left_average.centroid);
		}
	}

//#############################################################################################################

	if (metric_fuzz[METRIC_AVERAGE] > 0) {

		//Calculate rectangle
		rect_average.x[0] = metric_fuzz[METRIC_AVERAGE] * 25 + 25;
		rect_average.y[0] = metric_fuzz[METRIC_AVERAGE];
		rect_average.x[1] = 75 - (metric_fuzz[METRIC_AVERAGE] * 25);
		rect_average.y[1] = rect_average.y[0];
		rect_average.x[2] = rect_average.x[1];
		rect_average.y[2] = 0;
		rect_average.x[3] = rect_average.x[0];
		rect_average.y[3] = 0;

		calculate_rect(&rect_average);

PRINTF_MAMDANI("ret METRIC_AVERAGE: mass %f centroid %f\n", rect_average.mass, rect_average.centroid);

		//Calculate full left triangle
		if (metric_fuzz[METRIC_GOOD] == 0) {

			left_average.x[0] = rect_average.x[0];
			left_average.y[0] = rect_average.y[0];
			left_average.x[1] = left_average.x[0];
			left_average.y[1] = 0;
			left_average.x[2] = 25;
			left_average.y[2] = 0;

			calculate_left_tri(&left_average);

PRINTF_MAMDANI("(ok) tri full l METRIC_AVERAGE: mass %f centroid %f\n", left_average.mass, left_average.centroid);
		}	

		//Calculate full right triangle
		if (metric_fuzz[METRIC_BAD] == 0) {

			right_average.x[0] = rect_average.x[1];
			right_average.y[0] = rect_average.y[1];
			right_average.x[1] = 75;
			right_average.y[1] = 0;
			right_average.x[2] = right_average.x[0];
			right_average.y[2] = 0;

			calculate_right_tri(&right_average);

PRINTF_MAMDANI("(ok) tri full r METRIC_AVERAGE: mass %f centroid %f\n", right_average.mass, right_average.centroid);

		//Calculate parcial right triangle METRIC_AVERAGE			
		} else if (metric_fuzz[METRIC_AVERAGE] > metric_fuzz[METRIC_BAD]) {	

			right_average.x[0] = rect_average.x[1];
			right_average.y[0] = rect_average.y[1];
			right_average.x[1] = 75 - (metric_fuzz[METRIC_BAD] * 25);
			right_average.y[1] = metric_fuzz[METRIC_BAD];
			right_average.x[2] = right_average.x[0];
			right_average.y[2] = right_average.y[1];

			calculate_right_tri(&right_average);

PRINTF_MAMDANI("tri p r METRIC_AVERAGE: mass %f centroid %f\n", right_average.mass, right_average.centroid);

		//Calculate parcial left triangle METRIC_BAD
		} else {

			left_bad.x[0] = metric_fuzz[METRIC_BAD] * 25 + 50;
			left_bad.y[0] = metric_fuzz[METRIC_BAD];
			left_bad.x[1] = left_bad.x[0];
			left_bad.y[1] = metric_fuzz[METRIC_AVERAGE];
			left_bad.x[2] = (metric_fuzz[METRIC_AVERAGE] * 25) + 50;
			left_bad.y[2] = left_bad.y[1];

			calculate_left_tri(&left_bad);

PRINTF_MAMDANI("tri p l METRIC_BAD: mass %f centroid %f\n", left_bad.mass, left_bad.centroid);
		}
	}
//#############################################################################################################

	if (metric_fuzz[METRIC_BAD] > 0) {

		//Calculate rectangle
		rect_bad.x[0] = metric_fuzz[METRIC_BAD] * 25 + 50;
		rect_bad.y[0] = metric_fuzz[METRIC_BAD];
		rect_bad.x[1] = 100 - (metric_fuzz[METRIC_BAD] * 25);
		rect_bad.y[1] = rect_bad.y[0];
		rect_bad.x[2] = rect_bad.x[1];
		rect_bad.y[2] = 0;
		rect_bad.x[3] = rect_bad.x[0];
		rect_bad.y[3] = 0;

		calculate_rect(&rect_bad);

PRINTF_MAMDANI("ret METRIC_BAD: mass %f centroid %f\n", rect_bad.mass, rect_bad.centroid);

		//Calculate full left triangle
		if (metric_fuzz[METRIC_AVERAGE] == 0) {

			left_bad.x[0] = rect_bad.x[0];
			left_bad.y[0] = rect_bad.y[0];
			left_bad.x[1] = left_bad.x[0];
			left_bad.y[1] = 0;
			left_bad.x[2] = 50;
			left_bad.y[2] = 0;

			calculate_left_tri(&left_bad);

PRINTF_MAMDANI("tri full l METRIC_BAD: mass %f centroid %f\n", left_bad.mass, left_bad.centroid);
		}	

		//Calculate full right triangle
		if (metric_fuzz[METRIC_VERY_BAD] == 0) {

			right_bad.x[0] = rect_bad.x[1];
			right_bad.y[0] = rect_bad.y[1];
			right_bad.x[1] = 100;
			right_bad.y[1] = 0;
			right_bad.x[2] = right_bad.x[0];
			right_bad.y[2] = 0;

			calculate_right_tri(&right_bad);

PRINTF_MAMDANI("tri full r METRIC_BAD: mass %f centroid %f\n", right_bad.mass, right_bad.centroid);

		//Calculate parcial right triangle METRIC_BAD			
		} else if (metric_fuzz[METRIC_BAD] > metric_fuzz[METRIC_VERY_BAD]) {	

			right_bad.x[0] = rect_bad.x[1];
			right_bad.y[0] = rect_bad.y[1];
			right_bad.x[1] = 100 - (metric_fuzz[METRIC_VERY_BAD] * 25);
			right_bad.y[1] = metric_fuzz[METRIC_VERY_BAD];
			right_bad.x[2] = right_bad.x[0];
			right_bad.y[2] = right_bad.y[1];

			calculate_right_tri(&right_bad);

PRINTF_MAMDANI("tri p r METRIC_BAD: mass %f centroid %f\n", right_bad.mass, right_bad.centroid);

		//Calculate parcial left triangle METRIC_VERY_BAD
		} else {

			left_very_bad.x[0] = metric_fuzz[METRIC_VERY_BAD] * 25 + 75;
			left_very_bad.y[0] = metric_fuzz[METRIC_VERY_BAD];
			left_very_bad.x[1] = left_very_bad.x[0];
			left_very_bad.y[1] = metric_fuzz[METRIC_BAD];
			left_very_bad.x[2] = (metric_fuzz[METRIC_BAD] * 25) + 75;
			left_very_bad.y[2] = left_very_bad.y[1];

			calculate_left_tri(&left_very_bad);

PRINTF_MAMDANI("tri p l METRIC_VERY_BAD: mass %f centroid %f\n", left_very_bad.mass, left_very_bad.centroid);
		}
	}
//#############################################################################################################

	if (metric_fuzz[METRIC_VERY_BAD] > 0) {

		//Calculate rectangle
		rect_very_bad.x[0] = metric_fuzz[METRIC_VERY_BAD] * 25 + 75;
		rect_very_bad.y[0] = metric_fuzz[METRIC_VERY_BAD];
		rect_very_bad.x[1] = 100;
		rect_very_bad.y[1] = rect_very_bad.y[0];
		rect_very_bad.x[2] = rect_very_bad.x[1];
		rect_very_bad.y[2] = 0;
		rect_very_bad.x[3] = rect_very_bad.x[0];
		rect_very_bad.y[3] = 0;

		calculate_rect(&rect_very_bad);

PRINTF_MAMDANI("ret METRIC_VERY_BAD: mass %f centroid %f\n", rect_very_bad.mass, rect_very_bad.centroid);

		//Calculate full left triangle
		if (metric_fuzz[METRIC_BAD] == 0) {

			left_very_bad.x[0] = rect_very_bad.x[0];
			left_very_bad.y[0] = rect_very_bad.y[0];
			left_very_bad.x[1] = left_very_bad.x[0];
			left_very_bad.y[1] = 0;
			left_very_bad.x[2] = 75;
			left_very_bad.y[2] = 0;

			calculate_left_tri(&left_very_bad);

// PRINTF_MAMDANI("tri full l METRIC_AVERAGE: mass %f centroid %f\n", mass, centroid);
PRINTF_MAMDANI("tri full l METRIC_VERY_BAD: mass %f centroid %f\n", left_very_bad.mass, left_very_bad.centroid);
		}	
	}
//#############################################################################################################
	mass_sum = rect_very_good.mass + right_very_good.mass + left_good.mass + rect_good.mass + right_good.mass + left_average.mass + rect_average.mass + right_average.mass + left_bad.mass + rect_bad.mass + right_bad.mass + left_very_bad.mass + rect_very_bad.mass;
	mass_x_centroid = rect_very_good.mass * rect_very_good.centroid + right_very_good.mass * right_very_good.centroid + left_good.mass * left_good.centroid + rect_good.mass * rect_good.centroid + right_good.mass * right_good.centroid + left_average.mass * left_average.centroid + rect_average.mass * rect_average.centroid + right_average.mass * right_average.centroid + left_bad.mass * left_bad.centroid + rect_bad.mass * rect_bad.centroid + right_bad.mass * right_bad.centroid + left_very_bad.mass * left_very_bad.centroid + rect_very_bad.mass * rect_very_bad.centroid;

PRINTF_MAMDANI("metric x %f\n", mass_x_centroid / mass_sum);
	return mass_x_centroid / mass_sum;
}

void etx_fuzzification(float etx) {

	etx_fuzz[ETX_SMALL] = 0;
	etx_fuzz[ETX_AVERAGE] = 0;
	etx_fuzz[ETX_LONG] = 0;

    //μ_small(etx)
    if (etx >= 0 && etx <= 20) {

    	etx_fuzz[ETX_SMALL] = 1;

    } else if (etx > 20 && etx <= 40) {

    	etx_fuzz[ETX_SMALL] = (40 - etx) / 20;
    }

    //μ_average(etx)
    if (etx >= 20 && etx <= 40) {

    	etx_fuzz[ETX_AVERAGE] = (etx - 20) / 20;

    } else if (etx > 40 && etx <= 60) {

    	etx_fuzz[ETX_AVERAGE] = 1;

    } else if (etx > 60 && etx <= 80) {

    	etx_fuzz[ETX_AVERAGE] = (80 - etx) / 20;
    }

    //μ_long(etx)
    if (etx >= 60 && etx <= 80) {

    	etx_fuzz[ETX_LONG] = (etx - 60) / 20;

    } else if (etx > 80) {

    	etx_fuzz[ETX_LONG] = 1;
	}
}

void energy_fuzzification(float energy) {

	energy_fuzz[ENERGY_LOW] = 0;
	energy_fuzz[ENERGY_AVERAGE] = 0;
	energy_fuzz[ENERGY_HIGH] = 0;
PRINTF_MAMDANI("%f ENERGY_LOW %f\n", energy, energy_fuzz[ENERGY_LOW]);

	//μ_low(energy)
	if (energy >= 0 && energy <= 20) {

		energy_fuzz[ENERGY_LOW] = 1;

	} else if (energy > 20 && energy <= 40) {

		energy_fuzz[ENERGY_LOW] = (40 - energy) / 20;
	}

	//μ_average(energy)
	if (energy >= 20 && energy <= 40) {

		energy_fuzz[ENERGY_AVERAGE] = (energy - 20) / 20;

	} else if (energy > 40 && energy <= 60) {

		energy_fuzz[ENERGY_AVERAGE] = 1;

	} else if (energy > 60 && energy <= 80) {

		energy_fuzz[ENERGY_AVERAGE] = (80 - energy) / 20;
	}

	//μ_high(energy)
	if (energy >= 60 && energy <= 80) {

		energy_fuzz[ENERGY_HIGH] = (energy - 60) / 20;

	} else if (energy > 80) {

		energy_fuzz[ENERGY_HIGH] = 1;
	}
}

float verybad_defuzzification(float fuzzy) {

	float val1;

	val1 = (fuzzy * 25) + 75;

	return ((val1 + 100) / 2);
}

float bad_defuzzification(float fuzzy) {

	float val1;
	float val2;

	val1 = (fuzzy * 25) + 50;

	val2 = 100 - (fuzzy * 25);

	return ((val1 + val2) / 2);	
}

float average_defuzzification(float fuzzy) {

	float val1;
	float val2;

	val1 = (fuzzy * 25) + 25;

	val2 = 75 - (fuzzy * 25);

	return ((val1 + val2) / 2);	
}

float good_defuzzification(float fuzzy) {

	float val1;
	float val2;

	val1 = fuzzy * 25;

	val2 = 50 - (fuzzy * 25);

	return ((val1 + val2) / 2);
}

float verygood_defuzzification(float fuzzy) {

	float val1;

	val1 = 25 - (fuzzy * 25);

	return (val1 / 2);
}

const struct sdn_metric mamdani_fuzzy_model_metric = {
  get_weight
};
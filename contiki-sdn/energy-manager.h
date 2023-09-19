#ifndef ENERGY_MANAGER_H
#define ENERGY_MANAGER_H

#define MAX_BATTERY_LEVEL 255UL

void em_init();

uint8_t em_get_battery();

uint8_t em_has_battery();

#endif //ENERGY_MANAGER_H
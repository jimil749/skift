#pragma once

#include <libsystem/Common.h>

void lapic_found(uintptr_t address);

void lapic_initialize();

void lapic_ack();

void read(uint32_t reg);

void write(uint32_t reg, uint32_t data);

void lapic_initialize();

uint32_t lapic_getid();

void lapic_send_init(uint32_t apic_id);

void lapic_send_startup(uint32_t, uint32_t vector);

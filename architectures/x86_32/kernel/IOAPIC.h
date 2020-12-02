#pragma once

#include <libsystem/Common.h>

void ioapic_found(uintptr_t address);

// uint32_t ioapic_read(void *ioapicaddr, uint32_t reg);

// void ioapic_write(void *ioapicaddr, uint32_t reg, uint32_t value);

void ioapic_set_entry(uint8_t index, uint64_t data);

void ioapic_initialize();

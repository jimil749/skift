#pragma once

#include "kernel/handover/Handover.h"

#define MAX_CPU_COUNT 4

extern uint32_t acpi_cpu_count;
extern uint8_t acpi_cpu_id[MAX_CPU_COUNT];

void acpi_initialize(Handover *handover);

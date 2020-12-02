// smp initialization on bootup using APIC

#include "ACPI.h"
#include "LAPIC.h"

#include <libsystem/Logger.h>

void SmpInit()
{
    logger_info("\nWaking up all CPUs ...");

    uint32_t l_id = lapic_getid();

    // now sending INIT IPI to all other cpus except the BSP (ie self)
    for (uint i = 0; i < acpi_cpu_count; ++i)
    {
        uint apic_id = acpi_cpu_id[i];
        if (apic_id != l_id)
        {
            lapic_send_init(apic_id);
        }
    }

    // wait for 10 ms (yet to implement programmable Interrupt Timer)
    // pit_wait(10);

    // now sending STARTUP IPI to all cpus except the BSP
    for (uint i = 0; i < acpi_cpu_count; ++i)
    {
        uint apic_id = acpi_cpu_id[i];
        if (apic_id != l_id)
        {
            lapic_send_startup(apic_id, 0x8);
        }
    }

    // waiting untill cpu becomes active.... (not sure about this)
    // pit_wait(1);
    // while ()
}
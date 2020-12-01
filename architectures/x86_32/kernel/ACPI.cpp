#include <libsystem/Logger.h>

#include "architectures/x86_32/kernel/ACPI.h"
#include "architectures/x86_32/kernel/IOAPIC.h"
#include "architectures/x86_32/kernel/LAPIC.h"

#include "kernel/firmware/ACPI.h"

void acpi_madt_initialize(MADT *madt)
{
    logger_info("MADT found, size is %d", madt->record_count());

    // get the lapic address. (madt->local_apic stores the lapic address)
    lapic_found(madt->local_apic);

    madt->foreach_record([](auto record) {
        switch (record->type)
        {
        case MADTRecordType::LAPIC:
        {
            auto local_apic = reinterpret_cast<MADTLocalApicRecord *>(record);
            logger_info("Local APIC (cpu_id=%d, apic_id=%d, flags=%08x)", local_apic->processor_id, local_apic->apic_id, local_apic->flags);
        }
        break;

        case MADTRecordType::IOAPIC:
        {
            auto ioapic = reinterpret_cast<MADTIOApicRecord *>(record);
            logger_info("I/O APIC (id=%d, address=%08x)", ioapic->id, ioapic->address);
            ioapic_found(ioapic->address);
        }
        break;

        case MADTRecordType::ISO:
            logger_info("Interrupt Source Override");
            break;

        case MADTRecordType::NMI:
            logger_info("Non-maskable interrupts");
            break;

        case MADTRecordType::LAPIC_OVERRIDE:
            logger_info("Local APIC Address Override");
            break;

        default:
            break;
        }

        return Iteration::CONTINUE;
    });
}

void acpi_initialize(Handover *handover)
{
    if (!handover->acpi_rsdp_address)
    {
        logger_warn("No acpi rsdp found!");
        return;
    }

    RSDP *rsdp = (RSDP *)(handover->acpi_rsdp_address);

    RSDT *rsdt = (RSDT *)(rsdp->rsdt_address);

    MADT *madt = (MADT *)rsdt->child("APIC");

    acpi_madt_initialize(madt);
}

#include <libsystem/Logger.h>

#include "architectures/x86_32/kernel/IOAPIC.h"

// MM Registers for Accessing IOAPIC Registers
#define IO_REGSEL 0x00 // selects the IOAPIC Register to be read/written
#define IO_WIN 0x10    // used to read/write data

// IOAPIC Registers
#define IOAPIC_ID 0x00  //ID
#define IOAPIC_VER 0x01 // Version
#define IOAPIC_ARB 0x02
#define IO_REDTBL 0x10 // Redirection Table

static volatile uint32_t *ioapic = nullptr;

void ioapic_found(uintptr_t address)
{
    ioapic = reinterpret_cast<uint32_t *>(address);
    logger_info("IOAPIC found at %08x", ioapic);
}

// base address is given by ioapic

uint32_t ioapic_read(uint32_t reg)
{
    *((volatile uint32_t *)(ioapic + IO_REGSEL)) = reg;
    return *((volatile uint32_t *)(ioapic + IO_WIN));

    // ioapic[0] = (reg & 0xff);
    // return ioapic[4];
}

void ioapic_write(uint32_t reg, uint32_t value)
{
    *((volatile uint32_t *)(ioapic + IO_REGSEL)) = reg;
    *((volatile uint32_t *)(ioapic + IO_WIN)) = value;

    // ioapic[0] = (reg & 0xff);
    // ioapic[4] = value;
}

// we use 2 writes because that's the way a 64 bit register reads/writes!
// The redirection entry register for IRQ n is 0x10 + n*2(+1) (Each IRQ has 2 32 bit regs)
void ioapic_set_entry(uint8_t index, uint64_t data)
{
    ioapic_write(IO_REDTBL + index * 2, (uint32_t)data);
    ioapic_write(IO_REDTBL + index * 2 + 1, (uint32_t)(data >> 32));
}

void ioapic_initialize()
{
    // bits 16-23 of contain the max redirection entry i.e no of IRQs this IO APIC can handle
    uint32_t entry_count = ioapic_read(IOAPIC_VER);
    uint count = ((entry_count >> 16) & 0xff) + 1; // max redirection entry

    logger_info("Total I/O APIC pins = %d\n", count);

    for (uint i = 0; i < count; ++i)
    {
        ioapic_set_entry(i, 1 << 16);
    }
}

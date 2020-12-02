#include <libsystem/Logger.h>

#include "architectures/x86/kernel/PIC.h"
#include "architectures/x86_32/kernel/LAPIC.h"

/*
    Local APIC status and location:
    The address of the Local APIC is given by the MADT table
    that BIOS loads for us and is parsed in ACPI.cpp
*/

/* LAPIC Registers */

#define ID 0x0020      // Local APIC ID Register (Avoid writing to this)
#define VERSION 0x0030 // Local APIC Version Register
#define TPR 0x0080     // Task Priority Reg
#define APR 0x0090     // Arbitration PR
#define PPR 0x00a0     // Processor PR
#define EOI 0x00b0     // End of Interrupt
#define RRD 0x00c0     // Remote read
#define LDR 0x00d0     // Logical Destination
#define DFR 0x00e0     // Destination Format
#define SVR 0x00f0     // Spuriious Interr Vector
#define ISR 0x0100     // In service
#define TMR 0x0180     // Trigger mode
#define IRR 0x0200     // Interrupt Register
#define ESR 0x0280     // Error Status
#define ICRLO 0x0300   // Interrupt Command
#define ICRHI 0x0310   // Interrupt Command
#define TIMER 0x0320   // LVT Timer
#define THERMAL 0x0330 // LVT Thermal Sensor
#define PERF 0x0340    // LVT Performance
#define LINT0 0x0350   // LVT LINT0
#define LINT1 0x0360   // LVT LINT0
#define ERROR 0x0370   // LVT Error
#define TICR 0x0380    // Initial Count (Timer)
#define TCCR 0x0390    // Current Count (Timer)
#define TDCR 0x03e0    // Divide Config (Timer)

// ------------------------------------------
// Primary LAPIC IPIs is the ICR(Interrupt Command Register)
// Delivery Mode

#define ICR_FIXED 0x00000000 // Fixed mode, delivers` intr specified in vector field
#define ICR_LP 0x00000100    // Deliver the lowest priority intr
#define ICR_SMI 0x00000200   // Deliver an SMI intr
#define ICR_NMI 0x00000300   // Deliver an NMI intr
#define ICR_INIT 0x00000500  // Deliver INIT intr (causes other processors to perform INIT)
#define ICR_START 0x00000600 // Deliver Startup IPI

// Destination Mode (Physical or logical)
#define ICR_PHY 0x00000000
#define ICR_LOG 0x00000800

// Delivery Status
#define ICR_IDLE 0x00000000
#define ICR_PENDING 0x00001000

// Level
#define ICR_DEASSERT 0x00000000
#define ICR_ASSERT 0x00004000

// Trigger
#define ICR_EDGE 0x00000000
#define ICR_LEVEL 0x00008000

// Destination
#define ICR_NO_SHORTHAND 0x00000000
#define ICR_SELF 0x00040000
#define ICR_ALL 0x00080000
#define ICR_NOT_SELF 0x000c0000

// Destination (used when destination shorthand is 0B00)
#define ICR_DESTINATION 24

// constexpr int LAPIC_EOI = 0x00B0;

static volatile uint32_t *lapic = nullptr;

void lapic_found(uintptr_t address)
{
    lapic = reinterpret_cast<uint32_t *>(address);
    logger_info("LAPIC found at %08x", lapic);
}

uint32_t lapic_read(uint32_t reg)
{
    return *((volatile uint32_t *)(lapic + reg));
}

void lapic_write(uint32_t reg, uint32_t data)
{
    *((volatile uint32_t *)(lapic + reg)) = data;
}

void lapic_ack()
{
    lapic_write(EOI, 0);
}

void lapic_initialize()
{
    pic_disable();

    // clearing task priority to enable all interrupts
    lapic_write(TPR, 0);

    // Logical Destination Mode
    lapic_write(DFR, 0xffffffff); //flat
    lapic_write(LDR, 0x01000000); // logical id is 1

    lapic_write(SVR, 0x100 | 0xff); // initialize spurious interrupt vector register
}

// returns the id from LAPIC_ID Reg
uint32_t lapic_getid()
{
    return lapic_read(ID) >> 24;
}

// procedure for sending INIT IPI
void lapic_send_init(uint32_t apic_id)
{
    lapic_write(ICRHI, apic_id << ICR_DESTINATION);
    lapic_write(ICRLO, ICR_INIT | ICR_PHY | ICR_ASSERT | ICR_EDGE | ICR_NO_SHORTHAND);

    // wait until the interrupt is sent (essentially waiting till pending)
    while (lapic_read(ICRLO) & ICR_SEND_PENDING)
        ;
}

// procedure for sending STARTUP
void lapic_send_startup(uint32_t apic_id, uint32_t vector)
{
    lapic_write(LAPIC_ICRHI, apic_id << ICR_DESTINATION);
    lapic_write(ICRLO, vector | ICR_START | ICR_PHY | ICR_ASSERT | ICR_EDGE | ICR_NO_SHORTHAND);

    // wait until the interrupt is sent (essentially waiting till pending)
    while (lapic_read(ICRLO) & ICR_SEND_PENDING)
        ;
}
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>

#include "kernel/handover/Handover.h"
#include "kernel/system/System.h"

static Handover _handover;

const char *entry_type_to_string[] = {
    "AVAILABLE",
    "RESERVED",
    "ACPI",
    "NVS",
    "BADRAM",
    "KERNEL",
};

Handover *handover()
{
    return &_handover;
}

void handover_assert(uint32_t magic)
{
    if (!(is_multiboot2(magic) ||
          is_stivale2(magic)))
    {
        system_panic("Wrong bootloader please use any multiboot/stival bootloader\n\tMagic number: 0x%08x!", magic);
    }
}

void handover_dump()
{
    logger_info("Bootloader: '%s'", _handover.bootloader);
    logger_info("Command lines: '%s'", _handover.command_line);

    logger_info("Memory map:");
    for (size_t i = 0; i < _handover.memory_map_size; i++)
    {
        MemoryMapEntry *entry = &_handover.memory_map[i];

        logger_info("\t%d: %08p-%08p: %s",
                    i,
                    entry->range.base(),
                    entry->range.base() + entry->range.size() - 1,
                    entry_type_to_string[entry->type]);
    }
    logger_info("\t -> Usable memory: %dKio", _handover.memory_usable / 1024);

    logger_info("Modules:");
    for (size_t i = 0; i < _handover.modules_size; i++)
    {
        Module *module = &_handover.modules[i];
        logger_info("\t%d: %08p-%08p: %s",
                    i,
                    module->range.base(),
                    module->range.base() + module->range.size() - 1,
                    module->command_line);
    }
    logger_info("\t-> %d module found", _handover.modules_size);

    logger_info("SMP tag: ");
    logger_info("\tFlags:  %x", _handover.flag);
    logger_info("\tBSP LAPIC ID:  %d", _handover.bsp_lapic_id);
    for (size_t i = 0; i < _handover.cpu_count; i++)
    {
        SMPInfo *smp = &_handover.smp[i];
        logger_info("\tProcessor %d ID: %d", i, smp->acpi_processor_uid);
        logger_info("\tLAPIC ID:        %d", smp->lapic_id);
        logger_info("\tTarget Stack:    %d", smp->target_stack);
        logger_info("\tGOTO Address:    %d", smp->goto_address);
        logger_info("\tExtra Argument:  %d", smp->extra_argument);
    }
    logger_info("\t-> %d cpu found", _handover.cpu_count);
}

Handover *handover_initialize(void *header, uint32_t magic)
{
    handover_assert(magic);

    logger_info("Parsing handover informations...");
    logger_info("Header=%08x, Magic=%08x", header, magic);

    if (is_multiboot2(magic))
    {
        multiboot2_parse_header(&_handover, header);
    }
    else if (is_stivale2(magic))
    {
        stivale2_parse_header(&_handover, header);
    }
    else
    {
        ASSERT_NOT_REACHED();
    }

    handover_dump();

    return &_handover;
}

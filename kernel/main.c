/*                       _   _     _ _____ ____ _____                         */
/*                      | | | |   | | ____|  _ \_   _|                        */
/*                      | |_| |_  | |  _| | |_) || |                          */
/*                      |  _  | |_| | |___|  _ < | |                          */
/*                      |_| |_|\___/|_____|_| \_\|_|                          */
/*                                                                            */

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* main.c : the entry point of the kernel.                                    */

#include <libmath/math.h>
#include <libsystem/Result.h>
#include <libsystem/__plugs__.h>
#include <libsystem/atomic.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libsystem/process/Launchpad.h>

#include "kernel/clock.h"
#include "kernel/device/Device.h"
#include "kernel/filesystem/Filesystem.h"
#include "kernel/memory/Memory.h"
#include "kernel/modules/Modules.h"
#include "kernel/multiboot/Multiboot.h"
#include "kernel/platform.h"
#include "kernel/serial.h"
#include "kernel/system.h"
#include "kernel/tasking.h"
#include "kernel/x86/Interrupts.h"
#include "kernel/x86/gdt.h"

static TimeStamp boot_timestamp = 0;

ElapsedTime system_get_uptime(void)
{
    return clock_now() - boot_timestamp;
}

void userspace_initialize(void)
{
    logger_info("Starting the userspace...");

    Launchpad *init_lauchpad = launchpad_create("init", "/bin/init");

    Stream *keyboard_device = stream_open("/dev/keyboard", OPEN_READ);
    Stream *serial_device = stream_open("/dev/serial", OPEN_WRITE);

    launchpad_handle(init_lauchpad, HANDLE(keyboard_device), 0);
    launchpad_handle(init_lauchpad, HANDLE(serial_device), 1);
    launchpad_handle(init_lauchpad, HANDLE(serial_device), 2);
    launchpad_handle(init_lauchpad, HANDLE(serial_device), 3);

    int init_process = -1;
    Result result = launchpad_launch(init_lauchpad, &init_process);

    stream_close(keyboard_device);
    stream_close(serial_device);

    if (result != SUCCESS)
    {
        PANIC("Failled to start init : %s", result_to_string(result));
    }

    int init_exitvalue = 0;
    task_wait(init_process, &init_exitvalue);

    PANIC("Init has return with code %d!", init_exitvalue);
}

#ifdef __cplusplus
extern "C" void kmain(void *info, uint magic)
#else
void kmain(void *info, uint magic)
#endif
{
    __plug_init();
    boot_timestamp = clock_now();
    logger_level(LOGGER_TRACE);
    logger_info(KERNEL_UNAME);

    Multiboot *multiboot = multiboot_initialize(info, magic);

    if (multiboot->memory_usable < 255 * 1024)
    {
        PANIC("No enought memory (%uKio)!", multiboot->memory_usable / 1024);
    }

    logger_info("Initializing system...");
    setup(gdt);
    setup(platform);
    memory_initialize(multiboot);
    tasking_initialize();
    interrupts_initialize();

    /* --- Devices ---------------------------------------------------------- */

    filesystem_initialize();
    modules_initialize(multiboot);

    logger_info("Mounting devices...");
    null_initialize();
    zero_initialize();
    random_initialize();
    serial_initialize();
    mouse_initialize();
    keyboard_initialize();

    if (!framebuffer_initialize())
    {
        textmode_initialize();
    }

    userspace_initialize();
}

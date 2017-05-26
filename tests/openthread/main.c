#include <stdio.h>

#include "net/ipv6/addr.h"
#include "openthread/ip6.h"
#include "openthread/thread.h"
#include "openthread/udp.h"
#include "ot.h"
#include "shell.h"
#include "shell_commands.h"

int main(void)
{
    openthread_uart_run();
    return 0;
}

#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/sched.h>
#include <stdarg.h>

void tty_console(char *fmt, ...)
{
    int retval = 0;
    struct tty_struct *tty;
    char msg[256] = {0};
    va_list ap;

    tty = current->signal->tty;
    if (!tty) {
        printk(KERN_ERR "no tty\n");
        return;
    }
    va_start(ap, fmt);
    retval = vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);
    if (retval < 0) {
        printk(KERN_ERR "vsnprintf\n");
        return;
    }
	((tty->driver)->ops->write)(tty, msg, strlen(msg));
}

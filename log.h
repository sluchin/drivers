#ifndef LOG_H
#define LOG_H

#include <linux/kernel.h> /* printk */
#include <linux/string.h> /* kbasename */

#define kdebug(fmt, ...)                  \
    printk(KERN_DEBUG "%s[%d]: %s: "fmt, \
           kbasename(__FILE__), __LINE__, __func__, ## __VA_ARGS__);

#define kinfo(fmt, ...)                  \
    printk(KERN_INFO "%s[%d]: %s: "fmt, \
           kbasename(__FILE__), __LINE__, __func__, ## __VA_ARGS__);

#define kerror(fmt, ...)                  \
    printk(KERN_ERR "%s[%d]: %s: "fmt, \
           kbasename(__FILE__), __LINE__, __func__, ## __VA_ARGS__);

void tty_console(char *fmt, ...);

#endif /* LOG_H */

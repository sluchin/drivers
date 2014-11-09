#include <linux/kernel.h>   /* printk */

#include "log.h"
#include "ring.h"

#define MAXSIZE 4096

static unsigned char rbuff[MINOR_COUNT][MAXSIZE];
static int rp[MINOR_COUNT];
static int wp[MINOR_COUNT];

int put(int minor, unsigned char ch)
{
    int p = wp[minor];
    int next = (p + 1) % sizeof(int);

    if (next == p) /* MAXSIZEを超えた場合破棄 */
        return 0;
    rbuff[minor][p] = ch;
    kdebug("%04d[0x%02hhx]\n", p, ch);

    wp[minor] = next; /* 次を保存 */

    return next;
}

int get(int minor, unsigned char *ch)
{
    int p = rp[minor];
    int next = (p + 1) % sizeof(int);

    if (p == wp[minor]) /* バッファが空の場合 */
        return 0;
    *ch = rbuff[minor][p];
    kdebug("%04d[0x%02hhx]\n", p, *ch);

    rbuff[minor][p] = 0x00; /* クリア */
    rp[minor] = next;       /* 次を保存 */

    return next;
}

int is_empty(void)
{
    return (rp != wp) ? 1 : 0;
}

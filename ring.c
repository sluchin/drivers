#include "log.h"
#include "ring.h"

#define MAXSIZE 0xFFFF

static unsigned char rbuff[MINOR_COUNT][MAXSIZE];
static int rp[MINOR_COUNT];
static int wp[MINOR_COUNT];

int put(int minor, unsigned char ch)
{
    int p = wp[minor];
    int next = (p + 1) % MAXSIZE;

    if (next == p) /* MAXSIZEを超えた場合古いほう破棄 */
        kinfo("over buffer size.");
    rbuff[minor][p] = ch;
    kdebug("%04d[0x%02x]\n", p, ch);

    wp[minor] = next; /* 次を保存 */

    return next;
}

int get(int minor, unsigned char *ch)
{
    int p = rp[minor];
    int next = (p + 1) % MAXSIZE;

    if (p == wp[minor]) /* バッファが空の場合 */
        return 0;
    *ch = rbuff[minor][p];
    kdebug("%04d[0x%02x]\n", p, *ch);

    rbuff[minor][p] = 0x00; /* クリア */
    rp[minor] = next;       /* 次を保存 */

    return next;
}

int is_empty(void)
{
    return (rp != wp) ? 1 : 0;
}

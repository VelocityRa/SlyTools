#pragma once

#include "types.hpp"

#define btoi(b) ((b) / 16 * 10 + (b) % 16) /* BCD to u_char */
#define itob(i) ((i) / 10 * 16 + (i) % 10) /* u_char to BCD */

static s32 msf_to_lsn(u8* msf) {
    u32 lsn;

    lsn = msf[2];
    lsn += (msf[1] - 2) * 75;
    lsn += msf[0] * 75 * 60;
    return lsn;
}

static s32 msf_to_lba(u8 m, u8 s, u8 f) {
    u32 lsn;
    lsn = f;
    lsn += (s - 2) * 75;
    lsn += m * 75 * 60;
    return lsn;
}

static void lsn_to_msf(u8* msf, s32 lsn) {
    u8 m, s, f;

    lsn += 150;
    m = lsn / 4500;        // minuten
    lsn = lsn - m * 4500;  // minuten rest
    s = lsn / 75;          // sekunden
    f = lsn - (s * 75);    // sekunden rest
    msf[0] = itob(m);
    msf[1] = itob(s);
    msf[2] = itob(f);
}

static void lba_to_msf(s32 lba, u8* m, u8* s, u8* f) {
    lba += 150;
    *m = lba / (60 * 75);
    *s = (lba / 75) % 60;
    *f = lba % 75;
}

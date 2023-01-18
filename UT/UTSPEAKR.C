/*************************************************************************
    utspeakr - Turn the speaker on or off.

void utspeakr (unsigned freq);

unsigned freq   The frequency of the sound wave to
                produce, or 0 to turn speaker off.

Description     If freq is nonzero, this function turns the speaker on
                so that it produces a (square) sound wave with a specified
                frequency.  The resulting sound is produced until UTSPKR
                is invoked again. Freq must be at least 19.

                If freq is zero, the speaker is turned off.

Method          The 8253 Programmable Interrupt Timer expects a "latch
                register value" which when multiplied by the wave
                frequency is the clock frequency, 1,193,180 Hertz.  The
                latch value is computed and the 8253 is programmed
                accordingly.
****************************************************************************/

#include <dos.h>
#include <conio.h>
#include <slutil.h>

#define PB8255	  0x61		      /* Port B of the 8255 chip.   */
#define LTC8253   0x42		      /* Latch register 8253 (timer)*/
#define CMD8253   0x43		      /* Command register of 8253.  */
#define CLKFREQ   1193180L	      /* System clock frequency.    */

void utspeakr (unsigned freq) {
    unsigned latch;

    if (freq != 0) {
        outp (PB8255, inp (PB8255) | 3);
        outp (CMD8253, 0x0b6);
        latch = (unsigned int) (CLKFREQ / freq);
        outp (LTC8253, utlobyte (latch));
        outp (LTC8253, uthibyte (latch));
    }
    else outp (PB8255, inp (PB8255) & ~3);
}


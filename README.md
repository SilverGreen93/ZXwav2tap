The signal on the ZX-Spectrum tape is a frequency-modulated signal.
Zero is encoded with the highest frequency. The clock/sync signal is slightly higher frequency.
One is encoded with double the frequency of zero.
Start tone has the frequency 2.5 times lower than the zero frequency.
Encoded start tone sounds pi-i-i-i-i at the beginning of the tape.

All signals are rectangular:

_    __
 |  |  |               - clock/sync signal.
 |__|  |_
_     ___
 |   |   |             - zero or sync.
 |___|   |_

_        ______
 |      |      |       - one.
 |______|      |_

_          ________
 |        |        |   - start tone.
 |________|        |_


In general, the ZX-Spectrum signal looks like this:

_          ________          ________    __     ___        ______
 |        |        |        |        |  |  |   |   |      |      |
 |________|        |________|        |__|  |___|   |______|      |

                 start tone     sync signal   zero       one


The program analyzes only the upper parts of the signals, respectively, the program specifies the duration of these parts of the signals in the samples of the sound file multiplied by 5. That is, if a high level of the signal in the sound file takes 30 counts, then in the duration we write 150.
Only files with parameters 44100Hz, 8 bits, mono or stereo, not compressed, are decrypted.
Such files can be easily obtained from recording programs.

The signal level needs to be known in order to decide whether to receive a signal for a part of the high level or for a part of a low level. For this, two methods are used:
 - Automatic. The computer analyzes the signal for N samples of audio data forward, averages signals and learns the average level (because the signals have the upper and them, then the middle will be average).
 - Manual. The user himself sets the value of the level. For ideal files, this value is 127 - the middle of the range 0-255.

When recording from a tape recorder, it is recommended that the signal be all within the range 0-255, and there was no clipping for too strong a signal level. That is, the volume is needed to be 
small. Moreover, the noise at high volume also increases.

Successful decoding! :)

(c) Дмитрий Трунов (Dmitry Trunov) (da-nie), August 21, 2009, http://www.animekazan.net/forum2

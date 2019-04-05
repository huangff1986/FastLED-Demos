#ifndef SOUNDMEMS_H
#define SOUNDMEMS_H

void soundmems() {                                                              // Here's where we capture sound. It provides an average, a current sample as well as a peak trigger.
                                                                                // I tried some fancier math, but never came up with anything that really worked all that well. Must . . work. . harder.
// Local definitions
  #define NSAMPLES 32                                                           // Creating an array of lots of samples for decent averaging.
  #define DC_OFFSET  509                                                        // DC offset in mic signal. I subtract this value from the raw sample of a 'quiet room' test.
  #define MAXVOL 30                                                             // Let's not make our samplepeak too easy to get.

// Persistent local variables
  static int16_t samplearray[NSAMPLES];                                         // Array of samples for general sampling average.
  
  static uint16_t samplesum = 0;                                                // Sum of the last 64 samples. This had better be positive.
  static uint8_t samplecount = 0;                                               // A rollover counter to cycle through the array.
  static long peaktime;                                                         // Time of last peak, so that they're not too close together.

// Temporary local variables
  int16_t    micIn = 0;                                                         // Current sample starts with negative values and large values, which is why it's 16 bit signed.


  while(!(ADCSRA & 0x10));                                                      // wait for adc to be ready
  ADCSRA = 0xf5;                                                                // restart adc
  
  micIn = ADC - DC_OFFSET;                                                      // Get the data from the ADC and subtract the DC Offset.
  micIn = abs(micIn);                                                           // Get the absolute value of that.
  micIn   = (micIn <= squelch) ? 0 : (micIn - squelch);                         // Remove noise/hum.

//  sample = ((sample * 7) + micIn) >> 3;                                       // Very dampened reading.
  sample = ((sample * 3) + micIn) >> 2;                                         // Somewhat dampened reading, which is good enough for us.

//  if (sample < sampleavg+MAXVOL) samplepeak = 0;                              // Reset the global sample peak only if we're below MAXVOL. Actually, display routines need to reset this.


  
// Let's populate an array (circular buffer) in order to calculate an average value across NSAMPLES.
  samplesum += sample - samplearray[samplecount];                               // Add the new sample and remove the oldest sample in the array. No 'for' loops required here for extra speed.
  sampleavg = samplesum / NSAMPLES;                                             // Get an average.
  samplearray[samplecount] = sample;                                            // Update oldest sample in the array with new sample. By Andrew Tuline.
  samplecount = (samplecount + 1) % NSAMPLES;                                   // Update the counter for the array and rollover if we hit the max.


  if (  sample > (sampleavg+MAXVOL)                                                     &&      // Keep above a floor value. 
        sample < samplearray[(samplecount+30)%NSAMPLES]                                 &&      // Is it < previous sample.
        samplearray[(samplecount+30)%NSAMPLES] > samplearray[(samplecount+29)%NSAMPLES] &&      // Is previous sample > sample before that.
        millis() > (peaktime + 50)                                                      &&      // Wait at least 200ms for another peak.
        samplepeak == 0                                                                         // and there wasn't a recent peak.
        ) {samplepeak = 1;peaktime=millis();} else {/*samplepeak = 0;*/}   // Then we got a peak, else we don't. Err, let the display routines reset the samplepeak value.

/* 
  Serial.print(micIn);
  Serial.print(" ");
  Serial.print(sample);
  Serial.print(" ");
  Serial.print(sampleavg+MAXVOL);
  Serial.print(" ");
  Serial.print(samplepeak*255);
  Serial.print(" ");
  Serial.print(squelch);
  Serial.print(" ");
  Serial.print(0);
  Serial.print(" ");
  Serial.print(255);
  Serial.println(" ");
*/

} // soundmems()

#endif


Processing source

This displays an oscilloscope like view of the input. The ardumodem-RX sketch needs to be compiled with "#define TESTRXLEVEL". The input should have no offset and be in between the grey lines. Offset will absolutely destroy performance, and too small signal level will deteriorate performance.

Expect 10db SNR to work reliably.
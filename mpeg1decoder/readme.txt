MPEG-1 Decoder Educational Testbed 
 by Darren R. Starr <submux at gmail>

The MPEG-1 decoder presented in this directory is a nearly direct port of the 
Nokia MPEG-1 decoder project as presented at :
  http://www.developer.nokia.com/Community/Wiki/MPEG_decoder_in_Java_ME

This set of code is to be the foundation of a series of educational articles regarding 
the optimization and parallelization of video decoding on the Internet. It is not 
intended for production use and in its current form is highly unsuitable for such.

The license of this code is unknown as Nokia has not placed a license text anywhere I 
can find regarding this code. I would like to say however that I chose this particular 
codebase to start with as it was extremely well ... or poorly written depending on your 
perspective. From an optimization point of view, the code was awful as the only actual 
optimization I can find in the code was the use of fixed point math which in fact will 
be the first thing I revert. On the other hand, it was extremely easy to read, relatively 
compact and compared to most codecs, really well organized.

As far as I'm concerned, until such time as Nokia complains about the use of their code 
in such a way, I'll treat the code as if it were BSD and do my best to give credit where
credit is due. If they contact me, we'll see what needs to be done to satisfy both parties 
with regards to the licensing.

The code as it stands at the point of this first release is that it compiles, runs and
I have using debug messages seen that it decodes all the frames in the one mpeg movie
I've tested. The numbers on the output appear sane to me, however without making use of
a conversion class similar to the Bitmap class in the Java code, the numbers will be
useless.

If you have questions about this code... don't hesitate to contact me, I'm rubbish at
responding to large amounts of mail, but will do my best to get back to you as quickly 
as I can.

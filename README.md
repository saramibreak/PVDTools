# PVDTools

This is a collection of tools to decode VideoNow PVD discs.

Original binary and source code is [here](https://sourceforge.net/projects/pvdtools/)  
Technical documentation about black & white disc is [here](https://web.archive.org/web/20161026023116/http://pvdtools.sourceforge.net:80/format.txt)  
Technical documentation about color disc is [here](https://forum.videohelp.com/threads/123262-converting-video-formats-%28For-Hasbro-s-VideoNow%29-I-know-the/page17#post1149694)

## Usage
1. Execute command-line and type below  
   PVDTools.exe bw [cue file]  
   => You can get 1 [wav file](https://en.wikipedia.org/wiki/WAV) and multiple [pgm files](https://en.wikipedia.org/wiki/Netpbm_format) in the current directory.

   PVDTools.exe color [cue file]  
   => You can get 1 wav file and multiple ppm files in the current directory.

   PVDTools.exe xp [cue file]  
   => You can get 1 wav file and multiple ppm files in the current directory.

2. Get [irfanview](https://www.irfanview.com/) and [VirtualDub](http://www.virtualdub.org/) if you don't have it yet
3. Edit !PpmToBmpToAvi.bat in accordance with your environment
4. Execute !PpmToBmpToAvi.bat

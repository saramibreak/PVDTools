# PVDTools

This is a collection of tools to decode VideoNow PVD discs.

Original binary and source code is [here](https://sourceforge.net/projects/pvdtools/)  
Technical documentation about black & white disc is [here](https://web.archive.org/web/20161026023116/http://pvdtools.sourceforge.net:80/format.txt)  
Technical documentation about color disc is [here](https://www.videohelp.com/forum/viewtopic.php?p=1241647#1241647)

## Usage
1. Execute command-line and type below  
   PVDTools.exe bw [binary file]  
   => You can get 1 [wav file](https://en.wikipedia.org/wiki/WAV) and multiple [pgm files](https://en.wikipedia.org/wiki/Netpbm_format) in the current directory.
   
   PVDTools.exe color [binary file]  
   => You can get 1 wav file and multiple ppm files in the current directory.
   
   PVDTools.exe xp [binary file]  
   => You can get 1 wav file and multiple ppm files in the current directory.

   ** binary file is 2352 bytes per sector. You can generate it by [DiscImageCreator](https://github.com/saramibreak/DiscImageCreator)  
   
2. Get [irfanview](https://www.irfanview.com/) and [VirtualDub](http://www.virtualdub.org/) if you don't have it yet
3. Edit !PpmToBmpToAvi.bat in accordance with your environment
4. Execute !PpmToBmpToAvi.bat

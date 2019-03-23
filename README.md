# PVDTools

This is a collection of tools to decode VideoNow PVD discs.  Below are instructions.

Original binary and source code is [here](https://sourceforge.net/projects/pvdtools/)  
Technical documentation is [here](https://web.archive.org/web/20161026023116/http://pvdtools.sourceforge.net:80/format.txt)

## Usage
1. Execute command-line and type below  
   PVDTools.exe [binary file]  
   ** binary file is 2352 bytes per sector. You can generate it by [DiscImageCreator](https://github.com/saramibreak/DiscImageCreator)  
   => You can get 4 raw files and 1 [wav file](https://en.wikipedia.org/wiki/WAV) and multiple [pgm files](https://en.wikipedia.org/wiki/Netpbm_format) in the current directory.
2. Get [irfanview](https://www.irfanview.com/) if you don't have it already
3. Select Batch Convert/Rename from the file menu.
4. Select all of the extracted frames(hint:  hold down the shift key and select the first and last displayed)
5. Click add
6. Set "Work as:" to Batch conversion
7. Set Output format to "BMP"
8. Check(tick) "Use advanced options"
9. Check(tick) "Resize"
10. Uncheck(untick) "Preserve aspect ratio"
11. Be sure that "Use Resample function" is ticked
12. Type in 80 for Height and Width
13. Check(tick) "Change color depth"
14. Select "16.7 Million colors(24BPP)"
15. Check(tick) "Delete original files after conversion"
16. Select an output directory
17. Click Start
18. When IrfanView is done click exit and close IrfanVeiw
19. Open [VirtualDub](http://www.virtualdub.org/)
20. Select File>Open and open 1.bmp
21. Select Video>Frame Rate and select "Change framerate to" and enter 15
22. Click OK
23. Select Audio>WAV Audio and open input-right2.wav in the same directory as the original wav file.
24. Whatever you want to do.
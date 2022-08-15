# UOMap
Interatiive/Command file way to edit or get information for an Ultima Online map

UOMap allows one to read/edit/create a Ultima Online map (terrain and artwork) via a seriest of commands (either interatively or command files)

It can read/write both mul/uop for terrain files (map#.mul/map#LegacyMUL.uop) and satidx#.mul/statics#.mul.
One can issue a series of command to generate, and populate a map.  Or read/edit a current map.
Users can generate front ends if desired (graphical, or in any lanaguage) to generate the command scripts if desired.

UOMapExtract (another program available here), will go through the UO Client directory and extract into command scripts for each map
all the instructions to build the maps (warning, for map1/0 that can be 1gb in size given it is all text).

The program is started by either executing ./UOMap   (which puts in into interactive mode), or ./UOMap filepath  (where filepath is
command script file you wish to execute).

 UOMap Commands are:
 
  //  --- a comment indicator, everthing after this on a line is ignored.
  
 init $mapnum,$width,$height	                               // width or height == 0, default mapsize will be 
                                                               // used for the mapnum
 
 load terrain,uop or mul,$filepath                             // Load the terrain file at filepath
 
 load art,$idxpath,$mulpath                                    // Load the art specified by the paths
 
 diff terrain,$diflpath,$difpath                               // Apply the diff
 
 diff art,$diflpath,$difipath,$difpath                         // Apply the diff
 
 save terrain,uop or mul,$filepath                             // save the terrain in the specified format
 
 save art,$idxpath,$mulpath                                    // save the art at the secified paths
 
 query terrain,$x,$y,$path                                     // Provide the terrain info to the file, if no path, 
                                                               // it is provided to the std::cout
								
 query art,$x,$y,$path					       // Provide the art info to the file, if no path, it is
                                                               // provided to the std::cout
							       
 query terrain,$x,$y,$path				       // Provide the terrain info to the file, if no path,
                                                               // it is provided to the std::cout
							       
 query art,$x,$y,$z,$path				       // Provide the art info to the file, if no path, 
                                                               // it is provided to the std::cout
 
 remove $x,$y                                                  // remove all art at the x,y
 
 remove $x,$y,$z                                               // remove all art at the x,y,z
 
 add terrain,$x,$y,$tileid,$altitude                           // add terrain to the x,y
 
 add art,$x,$y,$tileid,&altitude,$hue                          // add art to the x,y
 
 add $x,$y,$path                                               // add all art in the file specified
 
 extract art,$startx,$starty,$endx,$endy,$filepath             // Provide the art info to the file
 
 extract terrain,$startx,$starty,$endx,$endy,$filepath         // Provide the terrain info to the file
 
 extract all,$startx,$starty,$endx,$endy,$filepath             // Provide terrain and art info to the file

 process $path                                                 // process the commands at the path specified
 
 msg $message                                                  // output the text string to std::cout 
                                                               // (usful for markers in large scripts)
							       
 quit                                                          // quit the processing stream
 

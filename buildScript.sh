#!/bin/bash
#this script is not yet tested, just a brainstorming yet...

sudo aptitude install build-essential cmake libboost-dev libfreeimage-dev libtinyxml-dev libxrandr-dev

#install boost
#sudo aptitude install libboost-all-dev

#install checkinstall for easy package creation:
#sudo aptitude install checkinstall



#create a thirdParty directory if it doesn't exist
cd ..
if [ ! -d thirdParty ] 
then
	mkdir thirdParty
fi
cd thirdParty


#have to get glfw manually, as the git repo has no tags identifying the 2.7 release, and the direct download via wget both produces a strange filename and th 
#download location does't seem quit permanent
if [ ! -d glfw-2.7 ] 
then
	echo you have to download GLFW 2.7 and unpack it into thirdparty/glfw-2.7; press enter when you\'re done
	read
else
	cd glfw-2.7
	sudo make x11-install
	cd ..
fi


#get glm math library from git
if [ ! -d ogl-math ] 
then
	git clone git://ogl-math.git.sourceforge.net/gitroot/ogl-math/ogl-math
fi


#get assimp asset loader library from git
if [ ! -d assimp ] 
then
	git clone git://github.com/assimp/assimp.git
	cd assimp
	cmake .
	cd ..
fi
	
cd assimp
if [ "`cat code/SplitByBoneCountProcess.cpp | grep "HAXX"`" == ""  ]
then	
	#hack not inserted yet
	echo insert the following code into assimp/code/SplitByBoneCountProcess.cpp to work around a compiler error and press enter:
	cat ../Flewnit/config/assimpHack.txt
	#wait for user	
	read
fi
	
make -j9
sudo make install
cd ..

#return to original Flewnit dir:
cd ../Flewnit
make clean
rm CMakeCache.txt
rm -r CMakeFiles
cmake .
make -j9










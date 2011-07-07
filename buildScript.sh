#!/bin/bash
#this script is not yet tested, just a brainstorming yet...


sudo aptitude install build-essential cmake git libboost-dev libfreeimage-dev libtinyxml-dev libxrandr-dev libgrantlee-dev

#install boost
#sudo aptitude install libboost-all-dev

#install checkinstall for easy package creation:
#sudo aptitude install checkinstall



#create a thirdParty directory if it doesn't exist
if [ $# -eq 0 ]
then
	echo "no param for third party library directory provided, checking for default location (../thirdparty)..."
	thirdPartyLibDir="../thirdParty"
else
	echo "using $1 as third party library directory"
	thirdPartyLibDir=$1
fi


if [ -d  $thirdPartyLibDir ] 
then
	echo "third party library  directory exists ($thirdPartyLibDir), using it"
else
	echo "third party library  directory ($thirdPartyLibDir) doesn't exist, creating it"
	mkdir $thirdPartyLibDir
fi


#save working dir to return to it after completing dependency download
flewnitDir=`pwd`
echo "directory of Flewnit library: $flewnitDir;"

#setup the dependencies ----------------------------------------------------------------------------------------

cd $thirdPartyLibDir

#have to get glfw manually, as the git repo has no tags identifying the 2.7 release, 
#and the direct download via wget both produces a strange filename and the 
#download location does't seem quite permanent
if [ ! -d glfw-2.7 ] 
then
	echo "you have to download GLFW 2.7 manually and unpack it into $thirdPartyLibDir/glfw-2.7\; press enter when you\'re done"
	read
	#compile and install
	cd glfw-2.7
	sudo make x11-install
	cd ..
fi


#get glm math library from git
#if [ ! -d glm ] 
if [ ! -d  ogl-math]
then
	git clone git://ogl-math.git.sourceforge.net/gitroot/ogl-math/ogl-math
	#rename folder
	#mv ogl-math glm
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
#compile and install assimp
make -j9
sudo make install
cd ..


#----------------------------------------------------------------------------------------

#all dependencies should have been setup, so let's build Flewnit, finally
cd $flewnitDir
	
make clean

echo "do you want to delete the cmake cache? y/n"
read answer
if [ "$answer" == "y" ]
then
	echo "removing cache"
	rm CMakeCache.txt
	rm -r CMakeFiles
fi


cmake-gui .
make -j9










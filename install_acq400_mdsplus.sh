#!/bin/bash


# This is an MDSplus devices install script.
# You need to install MDSplus from the YUM repo.
# Install instructions can be found on the MDSplus website.


# Make a PROJECTS directory if it doesn't exist.
mkdir -p ~/PROJECTS/


# Check if MDSplus is installed.
if [ -d /usr/local/mdsplus/ ] 
then
    echo "Directory /usr/local/mdsplus/ exists." 
else
    echo "Error: Directory /usr/local/mdsplus/ does not exist."
    echo "Please install MDSplus using YUM."
    exit 1
fi


# Check if HAPI has been installed.
if [ -d ~/PROJECTS/acq400_hapi/ ]
then
    echo "HAPI has already been cloned."
else
    echo "Cloning HAPI now."
    git clone https://github.com/D-TACQ/acq400_hapi.git ~/PROJECTS/acq400_hapi/
fi


# Check if MDSplus D-TACQ patch has been cloned.
if [ -d ~/PROJECTS/ACQ400_MDSPLUS/ ]
then
    echo "ACQ400_MDSPLUS already cloned"
else
    echo "Cloning ACQ400_MDSPLUS from github now."
    git clone https://github.com/D-TACQ/ACQ400_MDSPLUS.git ~/PROJECTS/ACQ400_MDSPLUS/
fi


# Check if the patch has been applied to the repo.
if [ -d /usr/local/mdsplus/pydevices/acq400Devices/ ] 
then 
    echo "/usr/local/mdsplus/pydevices/acq400Devices/ already exists."
    echo "If you want to reinstall the patch then please delete this directory and run this script again."
else
    echo "Patching in the acq400 devices into /usr/local/mdsplus/pydevices/"
    sudo cp -r ~/PROJECTS/ACQ400_MDSPLUS/pydevices/acq400Devices/ /usr/local/mdsplus/pydevices/
fi


# Check if HAPI is copied to MDSplus.
if [ -d /usr/local/mdsplus/pydevices/acq400Devices/acq400_hapi ]
then
    echo "HAPI already copied to mdsplus."
else
    echo "Copying HAPI into /usr/local/mdsplus/pydevices/acq400Devices/"
    sudo cp -r ~/PROJECTS/acq400_hapi/acq400_hapi/ /usr/local/mdsplus/pydevices/acq400Devices/
fi


echo ""
echo "--------------------"
echo "Install has finished."
echo "You can now create an MDSplus device using the D-TACQ device code."


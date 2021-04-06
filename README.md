# BRB-partially-connected-networks

This repository contains the Omnet++ code for the ICDCS'21 paper on Byzantine reliable broadcast on partially connected networks.
The code used to generate K-pasted and K-diamond graphs come from https://github.com/giovannifarina/BFT-ReliableCommunication

You can cite this work using the following bibtex entry. 

@inproceedings{bonomi2021practical,
  title={Byzantine Reliable Broadcast on Partially Connected Networks},
  author={Bonomi, Silva and Decouchant, J{\'e}r{\'e}mie and Farina, Giovanni and Rahli, Vincent and Tixeuil, S\'{e}bastien},
  booktitle={2021 IEEE 41st International Conference on Distributed Computing Systems (ICDCS)},
  pages={},
  year={2021},
  organization={IEEE}
}

The following installation guide has been tested on Ubuntu 20.04.

## Installation summary

I install omnet in a ~/sim directory., but it can be installed elsewhere (if you choose something else, you need to adapt the commands below). 
	
	cd ~ 
	mkdir -p sim  
	cd sim

Download and extract omnet version 5.5.1 using

	wget https://github.com/omnetpp/omnetpp/releases/download/omnetpp-5.5.1/omnetpp-5.5.1-src-linux.tgz
	tar xzvf omnetpp-5.5.1-src-linux.tgz
	
You can then follow the installation guide ~/sim/omnetpp-5.5.1/doc/InstallGuide.pdf
For the prerequisite packages, see section 5.3.
For Linux, see chapter 4.

### Install the prerequisite packages

I installed all prerequisite packages (you can probably do without most of the optional packages).

	sudo apt-get update

	sudo apt-get install build-essential gcc g++ bison flex perl \
	python python3 qt5-default libqt5opengl5-dev tcl-dev tk-dev \
	libxml2-dev zlib1g-dev default-jre doxygen graphviz libwebkitgtk-1.0
	
	sudo add-apt-repository ppa:ubuntugis/ppa
	sudo apt-get update
	sudo apt-get install openscenegraph-plugin-osgearth libosgearth-dev
	sudo apt-get install openmpi-bin libopenmpi-dev
	sudo apt-get install libpcap-dev
	
### Install omnetpp 

	cd omnetpp-5.5.1
	. setenv
	
Modify the path variable in the ~/.bashrc file.

	gedit ~/.bashrc
	export PATH=$HOME/omnetpp-5.5.1/bin:$PATH

Exit and update the path variable

	source ~/.bashrc
	
Compile

	./configure
	make
	
You can verify the installation
	
	cd samples/dyna
	./dyna
	
To start the IDE, in a terminal enter:

	omnetpp

### Download and compile inet

	wget https://github.com/inet-framework/inet/releases/download/v4.1.2/inet-4.1.2-src.tgz
	tar xzvf inet-4.1.2-src.tgz
	cd inet4
	
	make makefiles
	make

### Import the projects in Omnet

To import a project in the Omnet IDE use: File | Import | General | Existing projects into Workspace.
Then select the workspace dir as the root directory, and be sure NOT to check the
"Copy projects into workspace" box. Click Finish.

Import the BroadcastSign and the inet4 directories. 

Right click on BRB, then go to Project References and check the box associated to inet4 (or verify that it is checked).

Right click on BRB | OMNeT++ | Makemake | Select BRB: makemake... | Options (right of the screen) | Custom

Add the following line

	MSGC:=$(MSGC) --msg6

Right click on BRB and select build (might take a while if Inet has to be refreshed).


### Run an example

Right click on random_10_1_3.ini in BRB/simulations and Run as | Omnet++ simulations
Select BRB as executable (it should be the first option).
Confirm, and then select Run. 




	
	

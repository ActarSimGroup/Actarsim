#ACTARSim

##ACTARSIM, a simulation package developed to determine the response of the ACTAR-TPC Active Target and other similar Active Target projects, as well as their ancillary detectors.
ACTARSim is a simulation package developed to determine the response of the ACTAR-TPC Active Target and other similar Active Target projects, as well as their ancillary detectors. It uses Geant4 to track the beam and the reaction particles, and a ROOT-based code for the description of the electrons in the Active Target gas and the response of the detector after the electrons drift and diffusion. This software combination allows a versatile and modular configuration of different detector geometries, description of gas mixtures and pad plane detection methods of different shapes and sizes.

## Documentation
Present documentation is really old and it does not describe the present status of the code.
We are working on up-to-date documentation which is going to be announced and included in the
ActarSim-Manual folder.

### System Requirements
We have tested ActarSim on a very few sets of hardware-software combinations. Dev and master versions
are compiled and tested in OS X and Ubuntu Linux LTS, but they will probably work nicely on many Linux
systems. In both cases, only a few combinations of
Geant4 (v9) and ROOT5 have been tested. Therefore, we cannot guarantee the compilation on a particular system.
Please, consult us if you experience problems during the compilation or using ActarSim.
For compilation you would need a working CMake installation.

### Getting Sources and Contributing
If you wish to simply obtain the source for your own studies of ACTAR-like chambers,
you could simply:
     git clone https://github.com/ActarSimGroup/Actarsim

which will create the Actarsim folder un your computer and copy the code inside.
For brave people who want to contribute to the Actarsim development, we recommend to
fork the official branches from the https://github.com/ActarSimGroup/Actarsim webpage,
and make local tracking branches. We follow a common GitHub workflow. For more information,
please contact us (hector.alvarez@usc.es).

##Getting started
1. Create a new directory (ActarSim_build) where the program will be compiled and installed and change to it

$mkdir ../ActarSim-build
$cd ../ActarSim-build

2. Call cmake to create the makefile adapted to your system

$cmake -DGeant4_DIR=/pathToGeant4Instalation/lib/Geant<version> /pathToActarSimSource

3. Compile

$make

4. Install

$make install

You can find these instructions separately in the file INSTALL.

## License
ACTARSim is distributed under the terms of the GNU Lesser General Public Licence version 3 (LGPL) version 3,
available in the file lgpl-3.0.txt.

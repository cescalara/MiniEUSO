# Mini-EUSO CPU Software

The CPU software controls the startup, data acquisition and housekeeping of the Mini-EUSO instrument. The data acquisition chain of the main instrument is controlled by the Zynq board of the PDM-DP system. The setup of the CPU environment needed to run the CPU software is automated by running the scripts in the ```CPUsetup/``` directory as detailed below.

# Install

1. Set up the CPU hardware with a keyboard, screen, USB connector and working ethernet connection

2. Download and install the Debian i386 Standard Desktop from a bootable USB. Follow the default options and install onto the 32 GB flash storage of the CPU.
 * user: minieusouser
 * password: in documentation
 * connect to the network mirror to allow updates
 * install GRUB onto the 32 GB flash drive 

3. Restart the system and boot into the OS, login as root

4. Download the software from the repository
```
git clone https://github.com/cescalara/MiniEUSO /home/minieusouser
```

5. Run the setup script
```
cd /home/minieusouser/CPU/CPUsetup/
./runsetup.sh 
```
 * downloads the necessary packages
 * sets up the FTP server
 * sets up the directory structure
 * configures the network for use with the Zynq board
 * sets up autlogin to the root user on boot
 * restarts the shell 

# Run system tests
1. Use the following command to test the simultaneous aquisition from the PDM (via the Zynq board), NIR and visible cameras (via USB) and the photodiode sensors (via the analog board). The argument controls the length of the acquisition. For example, an argument of 10 gives 10x5s automated data collecting from the PDM, 10 photos from each camera and 10 output files from the photodiodes.
```
test_systems 10
```
* the data acquisition 
  * data from the PDM is collected in a non-triggered way, packets are sent from the Zynq every 5.24s with 3 levels of data and information on timestamping and the HV status
  * data from the cameras is collected by acquiring with one camera at a time,  waiting 5s between acquisitions
  * data from the photodiodes is read out into a FIFO and collected every 5s
* the output data is in /home/minieusouser/DATA/
  * frm_000000.dat for the PDM frames
  * output0.dat for the photodiode reading
  * .png for the photos from the cameras
* log files are in /home/minieusouser/log/


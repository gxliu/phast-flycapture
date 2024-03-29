/* This file runs a series of tests on pgr cameras

Note: To test USB3 camera, type sudo before running program
*/

// FlyCapture specific header, must be included in the same directory
#include "FlyCapture2.h"

// Standard include headers
#include "stdafx.h" // .h file must be included in same directory
#include <string>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <algorithm>

using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::ofstream;
using std::remove_if;

using namespace FlyCapture2;


/*******************************************************************************
PrintBuildInfo: Outputs version of software and timestamp for reference
	Arguments: none
	Returns: none
********************************************************************************/
void PrintBuildInfo()
{
    FC2Version fc2Version;
    Utilities::GetLibraryVersion( &fc2Version );
    char version[128];
    sprintf( 
        version, 
        "FlyCapture2 library version: %d.%d.%d.%d\n", 
        fc2Version.major, fc2Version.minor, fc2Version.type, fc2Version.build );

    cout << "\n" << version;

    char timeStamp[512];
    sprintf( timeStamp, "Application build date: %s %s\n\n", __DATE__, __TIME__ );

    cout << timeStamp;
}

/*********************************************************************************
PrintError: prints a formatted log trace to stderr
	Arguments:
		error - source of error
	Returns: none
**********************************************************************************/
void CheckError(Error error)
{
	if (error != PGRERROR_OK) {
		error.PrintErrorTrace();
	}	
}



/********************************************************************************* 
PrintCameraInfo: writes camera hardware info to file and saves file within camera
directory. This procedure is different for the GIGE and USB cameras so an if
structure determines which interface type to write for.
	Arguments: 
		pCamInfo - pointer to array of CameraInfo structures
		dir - main directory name as string
		interface - is camera USB or GIGE?
	Returns: none
**********************************************************************************/
void PrintCameraInfo(CameraInfo* pCamInfo, string dir, InterfaceType interface ) {
    	ofstream info_file;

	// Allocate char array for filename, not to exceed 512 characters
    	char filename[512];
    	const char * c = dir.c_str(); // turn dir from string to char* 
   	sprintf( filename, "%s/%s.txt", c, c); // save path for info_file
	info_file.open(filename);

	char info[2048];

	if (interface == INTERFACE_GIGE) { // if camera type is GIGE
		char macAddress[64];
    		sprintf( 
       	 		macAddress, 
        		"%02X:%02X:%02X:%02X:%02X:%02X", 
        		pCamInfo->macAddress.octets[0],
        		pCamInfo->macAddress.octets[1],
        		pCamInfo->macAddress.octets[2],
        		pCamInfo->macAddress.octets[3],
        		pCamInfo->macAddress.octets[4],
        		pCamInfo->macAddress.octets[5]
		);

    		char ipAddress[32];
    		sprintf( 
        		ipAddress, 
        		"%u.%u.%u.%u", 
       		 	pCamInfo->ipAddress.octets[0],
        		pCamInfo->ipAddress.octets[1],
        		pCamInfo->ipAddress.octets[2],
        		pCamInfo->ipAddress.octets[3]
		);

    		char subnetMask[32];
    		sprintf( 
        		subnetMask, 
        		"%u.%u.%u.%u", 
       	 		pCamInfo->subnetMask.octets[0],
        		pCamInfo->subnetMask.octets[1],
        		pCamInfo->subnetMask.octets[2],
        		pCamInfo->subnetMask.octets[3]
		);

    		char defaultGateway[32];
    		sprintf( 
        		defaultGateway, 
        		"%u.%u.%u.%u", 
        		pCamInfo->defaultGateway.octets[0],
        		pCamInfo->defaultGateway.octets[1],
        		pCamInfo->defaultGateway.octets[2],
        		pCamInfo->defaultGateway.octets[3]
		);

    		sprintf(
        		info, 
        		"\n*** CAMERA INFORMATION ***\n"
        		"Serial number - %u\n"
        		"Camera model - %s\n"
        		"Camera vendor - %s\n"
        		"Sensor - %s\n"
        		"Resolution - %s\n"
        		"Firmware version - %s\n"
        		"Firmware build time - %s\n"
        		"GigE version - %u.%u\n"
        		"XML URL 1 - %s\n"
        		"XML URL 2 - %s\n"
        		"MAC address - %s\n"
        		"IP address - %s\n"
        		"Subnet mask - %s\n"
        		"Default gateway - %s\n\n",
        		pCamInfo->serialNumber,
        		pCamInfo->modelName,
        		pCamInfo->vendorName,
        		pCamInfo->sensorInfo,
        		pCamInfo->sensorResolution,
        		pCamInfo->firmwareVersion,
        		pCamInfo->firmwareBuildTime,
        		pCamInfo->gigEMajorVersion,
        		pCamInfo->gigEMinorVersion,
        		pCamInfo->xmlURL1,
        		pCamInfo->xmlURL2,
        		macAddress,
        		ipAddress,
        		subnetMask,
        		defaultGateway 
		);
	}
	else { // else the camera type is USB3
    		sprintf(
        		info,
        		"\n*** CAMERA INFORMATION ***\n"
        		"Serial number - %u\n"
        		"Camera model - %s\n"
        		"Camera vendor - %s\n"
        		"Sensor - %s\n"
        		"Resolution - %s\n"
        		"Firmware version - %s\n"
        		"Firmware build time - %s\n\n",
        		pCamInfo->serialNumber,
       	 		pCamInfo->modelName,
        		pCamInfo->vendorName,
        		pCamInfo->sensorInfo,
        		pCamInfo->sensorResolution,
        		pCamInfo->firmwareVersion,
        		pCamInfo->firmwareBuildTime 
		);
	}
    
    	cout << info;
    	info_file << info;
    	info_file.close();
}

/************************************************************************************************
Init
*************************************************************************************************/
int InitializeConnection(PGRGuid guid, CameraBase& cam, InterfaceType interface) {
	Error error;	
	CameraInfo camInfo;
	
	// Connect to camera
	error = cam.Connect(&guid); 
	CheckError(error);

	// Retrieve camera info 
	error = cam.GetCameraInfo(&camInfo);
	CheckError(error);

	string directory = getDir(&camInfo);
	getPath(directory);
	PrintCameraInfo(&camInfo, directory, interface);
	return 0;
}

/*************************************************************************************************
getDir: Uses camera model name to create main directory for images
	Arguments:
		pCamInfo - pointer to array of CameraInfo structures
	Returns:
		dir - string version of directory for use in other methods
**************************************************************************************************/
string getDir(CameraInfo* pCamInfo) {

// MAKE SURE TO UNCOMMENT THIS LINE FOR THE REAL TRIALS
    	//string dir = pCamInfo->modelName;

	string dir = "Practice";
    	// get rid of spaces
    	dir.erase(remove_if(dir.begin(), dir.end(), isspace), dir.end());

    	const char* d = dir.c_str();

    	if(mkdir(d,0777)==-1) {
        	printf("Either there was an error creating the directory or you have already created"
			" the directory for that camera. Ctrl C if this is not the case. \n");
    	} 

    	return dir;
}


/***************************************************************************************************
getPath: Creates a path to save all image files for current trial
	Arguments:
		dir - main directory indicating camera working with
	Return: 
		p - full path to save image files 
****************************************************************************************************/
string getPath(string dir) {
    string trial;
    char directory[100];
    cout << "Enter a unique trial name: ";
    std::getline(cin, trial);
    
    const char * d = dir.c_str();
    const char * t = trial.c_str();
    // Concatenates main directory string and sub-directory lens
    // Lens directory is nested inside the camera model directory
    // This allows for better organization of data
    strcpy(directory, d);
    strcat(directory,"/");
    strcat(directory, t);
    
    const char* path = directory;


    if(mkdir(path,0777)==-1) {
        printf("Either there was an error creating the directory or you have already used that trial name. \n");
    }

    // puts(path);
    string p = string(path);
    return p;
}

/*
int gigESetup (GigECamera& cam) {
	Error error;
	GigEStreamChannel streamChannel;
	error = cam.GetGigEStreamChannelInfo(0, &streamChannel);
	CheckError(error);
	unsigned int packetSize;
	error = cam.DiscoverGigEPacketSize(&packetSize);
}
*/

/***********************************************************************************************************
************************************************************************************************************
Main Method
************************************************************************************************************/
int main (int /*argc*/, char** /*argv*/) {
	Error error;
	PrintBuildInfo();

	BusManager busMgr;
	PGRGuid guid;
	CameraBase cam;
	
	// Force IP Address for all GIGE cameras
	error = busMgr.ForceAllIPAddressesAutomatically(); 
	CheckError(error);

	// Retrieve unique ID for camera
	error = busMgr.GetCameraFromIndex (0, &guid); 
	CheckError(error);

	// Retrieve interface type
	error = busMgr.GetInterfaceTypeFromGuid(&guid, &interface);
	CheckError(error);
	
	// Configure Camera
	if (interface == INTERFACE_GIGE) {
		GigECamera cam;

		gigESetup(dynamic_cast<GigECamera&>(cam))

		cout << packetSize << "\n" ;
	}

	else {
		Camera cam;
		
	}
	InitializeConnection(PGRGuid guid, CameraBase& cam, InterfaceType interface);
    

    return 0;
}

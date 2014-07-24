//=============================================================================
// Copyright © 2008 Point Grey Research, Inc. All Rights Reserved.
//
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with PGR.
//
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: FlyCapture2Test.cpp,v 1.19 2010-03-11 22:58:37 soowei Exp $
//=============================================================================

#include "stdafx.h"

#include "FlyCapture2.h"

#include <string>
#include <iostream>
#include <fstream>
using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::ofstream;

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>


using namespace FlyCapture2;

void PrintBuildInfo()
{
    FC2Version fc2Version;
    Utilities::GetLibraryVersion( &fc2Version );
    char version[128];
    sprintf( 
        version, 
        "FlyCapture2 library version: %d.%d.%d.%d\n", 
        fc2Version.major, fc2Version.minor, fc2Version.type, fc2Version.build );

    cout << version ;

    char timeStamp[512];
    sprintf( timeStamp, "Application build date: %s %s\n\n", __DATE__, __TIME__ );

    cout << timeStamp ;
}

void PrintCameraInfo( CameraInfo* pCamInfo, string dir )
{
    ofstream myfile;

    // Create a unique filename
    char filename[512];
    // turn dir from string to char*
    const char * c = dir.c_str();
    sprintf( filename, "%s/camera.txt", c);

    myfile.open(filename);


    char info[2048];

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
        pCamInfo->firmwareBuildTime );
	
	cout << info;
    myfile << info;
    myfile.close();
}

void PrintError( Error error )
{
    error.PrintErrorTrace();
}

string getDir() {

    string dir;
    cout << "Enter your file: ";
    std::getline(cin, dir);


    const char * c = dir.c_str();

    if(mkdir(c,0777)==-1) {
        printf("error in creating directory \n");
    } 

    return dir;
}

int runShutter(Camera& cam, string dir, int ms) {
    

    Error error;

    PropertyType propTypes[7];
    propTypes[0] = BRIGHTNESS;
    propTypes[1] = AUTO_EXPOSURE;
    propTypes[2] = SHARPNESS;
    propTypes[3] = GAMMA;
    propTypes[4] = SHUTTER;
    propTypes[5] = GAIN;
    propTypes[6] = FRAME_RATE;

    PropertyType propType;
    Property prop;
    for(int i=0; i < 7; i++) {
        
        propType = propTypes[i];
        prop.type = propType;
        error = cam.GetProperty( &prop );

        if (error != PGRERROR_OK)
        {
            PrintError( error );
            return -1;
        }

        prop.autoManualMode = false;
        prop.onOff = false;

        error = cam.SetProperty( &prop );

        if (error != PGRERROR_OK)
        {
            PrintError( error );
            return -1;
        }

        
    }
    // Retrieve shutter property
    Property shutter;
    shutter.type = SHUTTER;
    error = cam.GetProperty( &shutter );

    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    shutter.absValue = ms;
    error = cam.SetProperty( &shutter );

    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }
    
    // Let it update...it takes a while...
    // Otherwise the first pictures comes out 
        //with the same shutter value as the old value
    sleep(5);

    

    //make directory for this number of milliseconds
    char msdir[512];
    const char * d = dir.c_str();
    sprintf( msdir, "%s/%d-ms", d, ms );

    if(mkdir(msdir,0777)==-1) {
        printf("error in creating directory \n");
    } 

    const int k_numImages = 5;
    Image rawImage; 
    int imageCnt=0;   
    while(imageCnt < k_numImages)
    {                
        // Retrieve an image
        error = cam.RetrieveBuffer( &rawImage );
        if (error != PGRERROR_OK)
        {
            PrintError( error );
            continue;
        }

        printf( "Grabbed image %d with a %d ms shutter.  \n", imageCnt, ms );

        // Create a converted image
        Image convertedImage;

        // Convert the raw image
        error = rawImage.Convert( PIXEL_FORMAT_MONO8, &convertedImage );
        if (error != PGRERROR_OK)
        {
            PrintError( error );
            return -1;
        }  

        // Create a unique filename
        char filename[512];
        // turn dir from string to char*
        const char * c = dir.c_str();
        sprintf( filename, "%s/%d-ms/img-%d.png", c, ms, imageCnt );
        puts(filename);
        // Save the image. If a file format is not passed in, then the file
        // extension is parsed to attempt to determine the file format.
        error = convertedImage.Save( filename );
        if (error != PGRERROR_OK)
        {
            PrintError( error );
            return -1;
        }
        imageCnt++;
    }            


    return 0;
}

int RunSingleCamera( PGRGuid guid )
{

    string dir = getDir();
    

    Error error;
    Camera cam;

    // Connect to a camera
    error = cam.Connect(&guid);
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    // Get the camera information
    CameraInfo camInfo;
    error = cam.GetCameraInfo(&camInfo);



    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    PrintCameraInfo(&camInfo, dir);   

    // Start capturing images
    error = cam.StartCapture();
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    //collect ms shutter values
    int trials = 4;
    int shuttervals[trials];

    for(int n=0; n<trials; n++) {
        cout << "Enter the number of ms for a shutter value: \n";
        cin >> shuttervals[n];
    }

    // run 5 pictures for each of those shutter values
    for(int n=0; n<trials; n++) {
        runShutter(cam, dir, shuttervals[n]);
    }
    

    

    // Stop capturing images
    error = cam.StopCapture();
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }      

    
    // Disconnect the camera
    error = cam.Disconnect();
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    return 0;
}

int main(int /*argc*/, char** /*argv*/)
{   
    

    PrintBuildInfo();

    Error error;

    // Since this application saves images in the current folder
    // we must ensure that we have permission to write to this folder.
    // If we do not have permission, fail right away.
    FILE* tempFile = fopen("test.txt", "w+");
    if (tempFile == NULL)
    {
        printf("Failed to create file in current folder.  Please check permissions.\n");
        return -1;
    }
    fclose(tempFile);
    remove("test.txt");

    BusManager busMgr;
    unsigned int numCameras;
    error = busMgr.GetNumOfCameras(&numCameras);
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    printf( "Number of cameras detected: %u\n", numCameras );

    for (unsigned int i=0; i < numCameras; i++)
    {
        PGRGuid guid;
        error = busMgr.GetCameraFromIndex(i, &guid);
        if (error != PGRERROR_OK)
        {
            PrintError( error );
            return -1;
        }

        RunSingleCamera( guid );
    }

    printf( "Done! Press Enter to exit...\n" );
    getchar();

    return 0;
}

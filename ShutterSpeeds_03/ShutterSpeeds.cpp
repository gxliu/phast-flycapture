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
#include <stdio.h>
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
    sprintf( filename, "%s/%s.txt", c, c);

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

string getDir(CameraInfo* pCamInfo) {
    string dir = pCamInfo->modelName;

    // get rid of spaces
    // dir.erase(remove_if(dir.begin(), dir.end(), isspace), dir.end());

    const char * d = dir.c_str();
    // puts(d);

    if(mkdir(d,0777)==-1) {
        printf("Either there was an error creating the directory or you have already created the directory for that camera. Ctrl C if this is not the case. \n");
    } 

    return dir;
}

string getPath(string dir) {
    string lens;
    char directory[100];
    cout << "Enter the Lens You Are Testing: ";
    std::getline(cin, lens);
    
    const char * d = dir.c_str();
    const char * l = lens.c_str();
    // Concatenates main directory string and sub-directory lens
    // Lens directory is nested inside the camera model directory
    // This allows for better organization of data
    strcpy(directory, d);
    strcat(directory,"/");
    strcat(directory, l);
    // puts(directory);
    // printf("%s", directory);
    
    const char* path = directory;


    if(mkdir(path,0777)==-1) {
        printf("Either there was an error creating the directory or you have already tested that lens. \n");
    }

    // puts(path);
    string p = string(path);
    return p;
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

    const int k_numImages = 3;
    Image rawImage; 
    int imageCnt=1;   
    while(imageCnt <= k_numImages)
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
        // puts(filename);
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


    // Get paths

    string dir = getDir(&camInfo);
    string path = getPath(dir);

    //Print cam info

    PrintCameraInfo(&camInfo, dir);   


    // Start capturing images
    error = cam.StartCapture();
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    //collect ms shutter values

    int start;
    int end;
    int inc;

    cout << "Enter the number of ms to start at (must be integer): \n";
    cin >> start;

    cout << "Enter the number of ms to end at (must be integer): \n";
    cin >> end;

    cout << "Enter the number of ms to increment by (must be integer): \n";
    cin >> inc;


    int shuttervals[1000]; // 1000 is overkill
    int count = 0;

    int cur = start;
    while(cur <= end) {
        shuttervals[count] = cur;

        cur = cur + inc;
        count++;
    }


    // run pictures for each of those shutter values
    for(int n=0; n<count; n++) {
        runShutter(cam, path, shuttervals[n]);
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

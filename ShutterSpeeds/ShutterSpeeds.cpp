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

#include <iostream>
using std::cout;
using std::endl;

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

void PrintCameraInfo( CameraInfo* pCamInfo )
{
    printf(
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
}

void PrintError( Error error )
{
    error.PrintErrorTrace();
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

    PrintCameraInfo(&camInfo);      

    // Get the property information
    // Aka shutter speed
    // PropertyInfo propinfo;
    // error = cam.GetPropertyInfo(&propInfo);

    // if (error != PGRERROR_OK)
    // {
    //     PrintError( error );
    //     return -1;
    // }

    // PrintPropertyInfo(&propInfo);   

    // PropertyType ptype = SHUTTER;
    // Property p = new Property(ptype);
    // error = cam.GetProperty(&p);

    // if (error != PGRERROR_OK)
    // {
    //     PrintError( error );
    //     return -1;
    // }

    // printf(
    //     "\n*** PROPERTY INFORMATION ***\n"
    //     "Value A - %u\n",
    //     p->valueA );
      
    // Retrieve properties
    // Property shutter;
    // shutter.type = SHUTTER;
    // error = cam.GetProperty( &shutter );

    // if (error != PGRERROR_OK)
    // {
    //     PrintError( error );
    //     return -1;
    // }

    // // number absValue holds number of ms
    // printf(
    // "\n*** PROPERTY INFORMATION ***\n"
    // "Abs Value - %f\n"
    // "\n",
    // shutter.absValue );

    //set shutter to new value
    Property shutter;
    shutter.type = SHUTTER;
    shutter.absValue = 5.0;
    shutter.autoManualMode = false;
    error = cam.SetProperty( &shutter );

    Property brightness;
    brightness.type = BRIGHTNESS;
    brightness.autoManualMode = false;
    error = cam.SetProperty( &brightness );

    Property autoexposure;
    autoexposure.type = AUTO_EXPOSURE;
    autoexposure.autoManualMode = false;
    error = cam.SetProperty( &autoexposure );

    Property whitebalance;
    whitebalance.type = WHITE_BALANCE;
    whitebalance.autoManualMode = false;
    error = cam.SetProperty( &whitebalance );

    Property hue;
    hue.type = HUE;
    hue.autoManualMode = false;
    error = cam.SetProperty( &hue );

    Property saturation;
    saturation.type = SATURATION;
    saturation.autoManualMode = false;
    error = cam.SetProperty( &saturation );

    Property gamma;
    gamma.type = GAMMA;
    gamma.autoManualMode = false;
    error = cam.SetProperty( &gamma );

    Property iris;
    iris.type = IRIS;
    iris.autoManualMode = false;
    error = cam.SetProperty( &iris );

    Property focus;
    focus.type = FOCUS;
    focus.autoManualMode = false;
    error = cam.SetProperty( &focus );

    Property zoom;
    zoom.type = ZOOM;
    zoom.autoManualMode = false;
    error = cam.SetProperty( &zoom );

    Property pan;
    pan.type = PAN;
    pan.autoManualMode = false;
    error = cam.SetProperty( &pan );

    Property tilt;
    tilt.type = TILT;
    tilt.autoManualMode = false;
    error = cam.SetProperty( &tilt );

    Property gain;
    gain.type = GAIN;
    gain.autoManualMode = false;
    error = cam.SetProperty( &gain );

    Property framerate;
    framerate.type = FRAME_RATE;
    framerate.autoManualMode = false;
    error = cam.SetProperty( &framerate );



    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    // number absValue holds number of ms
    for(int counter=0; counter < 5; counter++) {
        error = cam.GetProperty( &shutter );

        printf(
        "\n*** PROPERTY INFORMATION ***\n"
        "Abs Value - %f\n"
        "\n",
        shutter.absValue );

        sleep(1);
    }
    





    // Start capturing images
    error = cam.StartCapture();
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    const int k_numImages = 3;
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

        printf( "Grabbed image %d\n", imageCnt );

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
        sprintf( filename, "dirname/FlyCapture2Test-%u-%d.pgm", camInfo.serialNumber, imageCnt );

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

    if(mkdir("dirname",0777)==-1) {
        printf("error");
    }
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

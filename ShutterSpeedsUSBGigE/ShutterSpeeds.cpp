

/* 

NOTE: If more than one camera is plugged in...
To test the USB cam, type "sudo ./ShutterSpeeds" (USB cams are recognized first by the Bus Manager?)
To test the Ethernet cam, type "./ShutterSpeeds" (The USB cam won't be recognized if "sudo" isn't used)

*/

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

    cout << version;

    char timeStamp[512];
    sprintf( timeStamp, "Application build date: %s %s\n\n", __DATE__, __TIME__ );

    cout << timeStamp;
}

void PrintUSBCameraInfo( CameraInfo* pCamInfo, string dir )
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

void PrintGigECameraInfo( CameraInfo* pCamInfo, string dir )
{

    ofstream myfile;

    // Create a unique filename
    char filename[512];
    // turn dir from string to char*
    const char * c = dir.c_str();
    sprintf( filename, "%s/%s.txt", c, c);

    myfile.open(filename);


    char info[2048];

    

    char macAddress[64];
    sprintf( 
        macAddress, 
        "%02X:%02X:%02X:%02X:%02X:%02X", 
        pCamInfo->macAddress.octets[0],
        pCamInfo->macAddress.octets[1],
        pCamInfo->macAddress.octets[2],
        pCamInfo->macAddress.octets[3],
        pCamInfo->macAddress.octets[4],
        pCamInfo->macAddress.octets[5]);

    char ipAddress[32];
    sprintf( 
        ipAddress, 
        "%u.%u.%u.%u", 
        pCamInfo->ipAddress.octets[0],
        pCamInfo->ipAddress.octets[1],
        pCamInfo->ipAddress.octets[2],
        pCamInfo->ipAddress.octets[3]);

    char subnetMask[32];
    sprintf( 
        subnetMask, 
        "%u.%u.%u.%u", 
        pCamInfo->subnetMask.octets[0],
        pCamInfo->subnetMask.octets[1],
        pCamInfo->subnetMask.octets[2],
        pCamInfo->subnetMask.octets[3]);

    char defaultGateway[32];
    sprintf( 
        defaultGateway, 
        "%u.%u.%u.%u", 
        pCamInfo->defaultGateway.octets[0],
        pCamInfo->defaultGateway.octets[1],
        pCamInfo->defaultGateway.octets[2],
        pCamInfo->defaultGateway.octets[3]);

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
        "User defined name - %s\n"
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
        pCamInfo->userDefinedName,
        pCamInfo->xmlURL1,
        pCamInfo->xmlURL2,
        macAddress,
        ipAddress,
        subnetMask,
        defaultGateway );

    cout << info;
    myfile << info;
    myfile.close();
}

void PrintStreamChannelInfo( GigEStreamChannel* pStreamChannel )
{
    char ipAddress[32];
    sprintf( 
        ipAddress, 
        "%u.%u.%u.%u", 
        pStreamChannel->destinationIpAddress.octets[0],
        pStreamChannel->destinationIpAddress.octets[1],
        pStreamChannel->destinationIpAddress.octets[2],
        pStreamChannel->destinationIpAddress.octets[3]);

    printf(
        "Network interface: %u\n"
        "Host post: %u\n"
        "Do not fragment bit: %s\n"
        "Packet size: %u\n"
        "Inter packet delay: %u\n"
        "Destination IP address: %s\n"
        "Source port (on camera): %u\n\n",
        pStreamChannel->networkInterfaceIndex,
        pStreamChannel->hostPost,
        pStreamChannel->doNotFragment == true ? "Enabled" : "Disabled",
        pStreamChannel->packetSize,
        pStreamChannel->interPacketDelay,
        ipAddress,
        pStreamChannel->sourcePort );
}

void PrintError( Error error )
{
    error.PrintErrorTrace();
}


string getDir(CameraInfo* pCamInfo) {
    string dir = pCamInfo->modelName;

    // get rid of spaces
    dir.erase(remove_if(dir.begin(), dir.end(), isspace), dir.end());

    const char * d = dir.c_str();

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
    
    const char* path = directory;


    if(mkdir(path,0777)==-1) {
        printf("Either there was an error creating the directory or you have already tested that lens. \n");
    }

    // puts(path);
    string p = string(path);
    return p;
}


int runShutter(CameraBase& cam, string dir, int ms) {

    //shouldn't be running this property change every time
    //refactor

    Error error;

    PropertyType propTypes[15];
    propTypes[0] = BRIGHTNESS;
    propTypes[1] = AUTO_EXPOSURE;
    propTypes[2] = SHARPNESS;
    propTypes[3] = WHITE_BALANCE;
    propTypes[4] = HUE;
    propTypes[5] = SATURATION;
    propTypes[6] = GAMMA;
    propTypes[7] = IRIS;
    propTypes[8] = FOCUS;
    propTypes[9] = ZOOM;
    propTypes[10] = PAN;
    propTypes[11] = TILT;
    propTypes[12] = SHUTTER;
    propTypes[13] = GAIN;
    propTypes[14] = FRAME_RATE;

    int propPresent[15];
    float propAbsValues[15];

    PropertyType propType;
    Property prop;
    for(int i=0; i < 15; i++) {
        
        propType = propTypes[i];
        prop.type = propType;
        error = cam.GetProperty( &prop );

        if (error != PGRERROR_OK)
        {
            puts("16");
            PrintError( error );
            return -1;
        }

        if(prop.present) {
            propPresent[i] = 1;

            
            // Turns off frame rate
            if(propType==FRAME_RATE || propType==SHARPNESS) {
                prop.onOff = false;
            }
            // Turns on all other properties
            else {
                prop.onOff = true;
            }      

            if(propType== GAMMA) {
                //set gamma to 0
                //but lowest it will go is 0.5!
                puts("setting gamma val to 0");
                prop.absValue = 0.0;
            }

            // Turns auto mode off for shutter
            if(propType == SHUTTER || propType==SHARPNESS) {
                    prop.autoManualMode = false;
            } else
            {
                prop.autoManualMode = true;
            }
            


            error = cam.SetProperty( &prop );

            if (error != PGRERROR_OK)
            {
                puts("17");
                PrintError( error );
                return -1;
            }
        }

        else {
            propPresent[i] = 0;
        }

        
    }
    // Retrieve shutter property
    Property shutter;
    shutter.type = SHUTTER;
    error = cam.GetProperty( &shutter );

    if (error != PGRERROR_OK)
    {
        puts("18");
        PrintError( error );
        return -1;
    }

    shutter.absValue = ms;
    error = cam.SetProperty( &shutter );

    if (error != PGRERROR_OK)
    {
        puts("19");
        PrintError( error );
        return -1;
    }
    
    // Let it update...it takes a while...
    // Otherwise the first pictures comes out 
        //with the same shutter value as the old value
    sleep(4);

    //make directory for this number of milliseconds
    char msdir[512];
    const char * d = dir.c_str();
    sprintf( msdir, "%s/%d-ms", d, ms );

    if(mkdir(msdir,0777)==-1) {
        printf("error in creating directory \n");
    } 





    //take 3 images at this certain shutter val
    const int k_numImages = 3;
    Image rawImage; 
    int imageCnt=1;   
    while(imageCnt <= k_numImages)
    {                
        // Retrieve an image
        error = cam.RetrieveBuffer( &rawImage );
        if (error != PGRERROR_OK)
        {
            puts("20");
            PrintError( error );
            continue;
        }

        printf( "Grabbed image %d with a %d ms shutter.  \n", imageCnt, ms );

        //print frame rate info
        ofstream myfile;
        propType = FRAME_RATE;
        prop.type = propType;
        error = cam.GetProperty( &prop );

        if (error != PGRERROR_OK)
        {
            puts("20.5");
            PrintError( error );
            return -1;
        }

        char propertyfilename[512];
        const char * c = dir.c_str();
        sprintf( propertyfilename, "%s/%d-ms/img-%d-camsettings.txt", c, ms, imageCnt);
        myfile.open(propertyfilename);


        char info[2048];

        for(int i=0; i < 15; i++) {
        
            propType = propTypes[i];
            prop.type = propType;
            error = cam.GetProperty( &prop );

            if (error != PGRERROR_OK)
            {
                puts("20.75");
                PrintError( error );
                return -1;
            }

            propAbsValues[i] = prop.absValue;
            


        
        }

        //uses 0 and 1 for true and false of whether property is present


        sprintf(
        info, 
        "Property\t\t\tValue\t\t\t\t(Is Property Present?)\n"
        "---------------------------------------------------------------\n"
        "Brightness\t\t\t%9f percent\t(%d)\n"
        "Auto exposure\t\t%9f EV\t\t(%d)\n"
        "Sharpness\t\t\t%9f\t\t\t(%d)\n"
        "White balance\t\t%9f\t\t\t(%d)\n"
        "Hue\t\t\t\t\t%9f\t\t\t(%d)\n"
        "Saturation\t\t\t%9f\t\t\t(%d)\n"
        "Gamma\t\t\t\t%9f\t\t\t(%d)\n"
        "Iris\t\t\t\t%9f\t\t\t(%d)\n"
        "Focus\t\t\t\t%9f\t\t\t(%d)\n"
        "Zoom\t\t\t\t%9f\t\t\t(%d)\n"
        "Pan\t\t\t\t\t%9f\t\t\t(%d)\n"
        "Tilt\t\t\t\t%9f\t\t\t(%d)\n"
        "Shutter\t\t\t\t%9f ms\t\t(%d)\n"
        "Gain\t\t\t\t%9f dB\t\t(%d)\n"
        "Frame rate\t\t\t%9f fps\t\t(%d)\n",
        propAbsValues[0],
        propPresent[0],
        propAbsValues[1],
        propPresent[1],
        propAbsValues[2],
        propPresent[2],
        propAbsValues[3],
        propPresent[3],
        propAbsValues[4],
        propPresent[4],
        propAbsValues[5],
        propPresent[5],
        propAbsValues[6],
        propPresent[6],
        propAbsValues[7],
        propPresent[7],
        propAbsValues[8],
        propPresent[8],
        propAbsValues[9],
        propPresent[9],
        propAbsValues[10],
        propPresent[10],
        propAbsValues[11],
        propPresent[11],
        propAbsValues[12],
        propPresent[12],
        propAbsValues[13],
        propPresent[13],
        propAbsValues[14],
        propPresent[14]

        // pCamInfo\t>serialNumber,
        // pCamInfo\t>modelName,
        // pCamInfo\t>vendorName,
        // pCamInfo\t>sensorInfo,
        // pCamInfo\t>sensorResolution,
        // pCamInfo\t>firmwareVersion,
        // pCamInfo\t>firmwareBuildTime,
        // pCamInfo\t>gigEMajorVersion,
        // pCamInfo\t>gigEMinorVersion,
        // pCamInfo\t>userDefinedName,
        // pCamInfo\t>xmlURL1,
        // pCamInfo\t>xmlURL2,
        // macAddress,
        // ipAddress,
        // subnetMask,
        // defaultGateway 
        );

        //myfile << prop.absValue;
        
        myfile << info;
        myfile.close();

        // Create a converted image
        Image convertedImage;

        // Convert the raw image
        error = rawImage.Convert( PIXEL_FORMAT_MONO8, &convertedImage );
        if (error != PGRERROR_OK)
        {
            puts("21");
            PrintError( error );
            return -1;
        }  

        // Create a unique filename
        char filename[512];
        // turn dir from string to char*
        sprintf( filename, "%s/%d-ms/img-%d.png", c, ms, imageCnt );
        // puts(filename);
        // Save the image. If a file format is not passed in, then the file
        // extension is parsed to attempt to determine the file format.
        error = convertedImage.Save( filename );
        if (error != PGRERROR_OK)
        {
            puts("22");
            PrintError( error );
            return -1;
        }
        imageCnt++;
    }            


    return 0;
}

int gigESetup(GigECamera& cam) {
    Error error;

    unsigned int numStreamChannels = 0;
    error = cam.GetNumStreamChannels( &numStreamChannels );
    if (error != PGRERROR_OK)
    {
        puts("1");
        PrintError( error );
        return -1;
    }

    for (unsigned int i=0; i < numStreamChannels; i++)
    {
        // get it
        GigEStreamChannel streamChannel;
        error = cam.GetGigEStreamChannelInfo( i, &streamChannel );
        if (error != PGRERROR_OK)
        {
            puts("2");
            PrintError( error );
            return -1;
        }

        // modify it
        unsigned int pkt;
        unsigned int dly;

        cout << "Enter the packet size.  Lower is safer, but slower. (576 to 9000): \n";
        cin >> pkt;

        cout << "Enter the delay.  Bigger is safer, but slower.  (0 to 6250): \n";
        cin >> dly;


        streamChannel.packetSize = pkt;
        streamChannel.interPacketDelay = dly;

        // set it
        error = cam.SetGigEStreamChannelInfo( i, &streamChannel );
        if (error != PGRERROR_OK)
        {
            puts("3");
            PrintError( error );
            return -1;
        }



        printf( "\nPrinting stream channel information for channel %u:\n", i );
        PrintStreamChannelInfo( &streamChannel );
    }    

    printf( "Querying GigE image setting information...\n" );

    GigEImageSettingsInfo imageSettingsInfo;
    error = cam.GetGigEImageSettingsInfo( &imageSettingsInfo );
    if (error != PGRERROR_OK)
    {
        puts("4");
        PrintError( error );
        return -1;
    }

    GigEImageSettings imageSettings;
    imageSettings.offsetX = 0;
    imageSettings.offsetY = 0;
    imageSettings.height = imageSettingsInfo.maxHeight;
    imageSettings.width = imageSettingsInfo.maxWidth;
    imageSettings.pixelFormat = PIXEL_FORMAT_MONO8;

    printf( "Setting GigE image settings...\n" );

    error = cam.SetGigEImageSettings( &imageSettings );
    if (error != PGRERROR_OK)
    {
        puts("5");
        PrintError( error );
        return -1;
    }
}

int runSingleCamera(PGRGuid guid, CameraBase& cam, InterfaceType interfaceType) {

    Error error;

    // Connect to a camera
    error = cam.Connect(&guid);
    if (error != PGRERROR_OK)
    {
        puts("6");
        PrintError( error );
        return -1;
    }

    // Get the camera information
    CameraInfo camInfo;
    error = cam.GetCameraInfo(&camInfo);
    if (error != PGRERROR_OK)
    {
        puts("7");
        PrintError( error );
        return -1;
    }

    // Get paths
    string dir = getDir(&camInfo);
    string path = getPath(dir);

    //Print cam info
    if(interfaceType == INTERFACE_GIGE) {
        PrintGigECameraInfo(&camInfo, dir);
        gigESetup(dynamic_cast<GigECamera&>(cam)); //get packet info, print stream info
    }
    else {
        PrintUSBCameraInfo(&camInfo, dir);
    }
    

    // Start capturing images
    error = cam.StartCapture();
    if (error != PGRERROR_OK)
    {
        puts("8");
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
        puts("9");
        PrintError( error );
        return -1;
    }      

    // Disconnect the camera
    error = cam.Disconnect();
    if (error != PGRERROR_OK)
    {
        puts("10");
        PrintError( error );
        return -1;
    }

    return 0;
}


int main(int /*argc*/, char** /*argv*/)
{   

    PrintBuildInfo();

    Error error;

    //auto force ip
    error = BusManager::ForceAllIPAddressesAutomatically();
    if (error != PGRERROR_OK)
    {
        puts("11");
        PrintError( error );
        return -1;
    }

    //what flycapture gui does after this line

    sleep(3);
        

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

    CameraInfo camInfo[10];
    unsigned int numCamInfo = 10;
    error = BusManager::DiscoverGigECameras( camInfo, &numCamInfo );
    if (error != PGRERROR_OK)
    {
        puts("12");
        PrintError( error );
        return -1;
    }

        printf( "Number of GigE cameras discovered: %u\n", numCamInfo );

        // for (unsigned int i=0; i < numCamInfo; i++)
        // {
        //     PrintGigECameraInfo( &camInfo[i] );
        // }

        unsigned int numCameras;
        error = busMgr.GetNumOfCameras(&numCameras);
        if (error != PGRERROR_OK)
        {
            puts("13");
            PrintError( error );
            return -1;
        }

        printf( "Number of cameras enumerated: %u\n", numCameras );

        if (numCameras == 0) {
            puts("Exiting...");
            return -1;
        }
        // for (unsigned int i=0; i < numCameras; i++)
        // {
        int i = 0;

        PGRGuid guid;
        error = busMgr.GetCameraFromIndex(i, &guid);
        if (error != PGRERROR_OK)
        {
            puts("14");
            PrintError( error );
            return -1;
        }

        InterfaceType interfaceType;
        error = busMgr.GetInterfaceTypeFromGuid( &guid, &interfaceType );
        if ( error != PGRERROR_OK )
        {
            puts("15");
            PrintError( error );
            return -1;
        }

        if ( interfaceType == INTERFACE_GIGE )
        {
            puts("GigE Interface");
            GigECamera cam;
            runSingleCamera(guid, cam, interfaceType);
        }

        else {
            puts("USB Interface");
            Camera cam;
            runSingleCamera(guid, cam, interfaceType);

        }
        // }

        

        printf( "Done! Press Enter to exit...\n" );
        getchar();

    

    return 0;
}

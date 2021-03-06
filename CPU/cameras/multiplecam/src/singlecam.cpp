//============================================================================
// Name        : singlecam.cpp
// Author      : Sara Turriziani
// Version     : 1.0 
// Copyright   : Mini-EUSO copyright notice
// Description : Single Camera Acquisition Module in C++, ANSI-style, for linux
//============================================================================


#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <sstream>
#include <fstream>
#include <math.h>
#include <time.h>
#include <ctime>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include "/usr/include/flycapture/FlyCapture2.h"
using namespace FlyCapture2;
using namespace std;

// using global pointer/handler for test
//Camera *pCameras;
FlyCapture2::CallbackHandle m_resetHandle;
FlyCapture2::CallbackHandle m_arrivalHandle;
FlyCapture2::CallbackHandle m_removalHandle;

volatile sig_atomic_t done = 0;



void term(int signum)
{
    done = 1;
}

unsigned createMask(unsigned a, unsigned b)
{
	unsigned int r = 0;
	for (unsigned i = a; i < b; i++)
	{
		r = r+1;
		r  = r*2;
	}
	r = r + 1;
	return r;
}

// test time in ms
const std::string currentDateTime2() {
 timeval curTime;
 gettimeofday(&curTime, NULL);
 int milli = curTime.tv_usec / 1000;
 struct tm timeinfo;
 char buffer [80];
 strftime(buffer, sizeof(buffer), "%Y-%m-%d.%H-%M-%S", localtime_r(&curTime.tv_sec, &timeinfo));
 char currentTime[84] = "";
 sprintf(currentTime, "%s.%03d", buffer, milli);

 return currentTime;

  }

inline void delay( unsigned long ms )
    {
    usleep( ms * 1000 );
    }



void PrintBuildInfo()
{
    FC2Version fc2Version;
    Utilities::GetLibraryVersion( &fc2Version );
    char version[128];
    sprintf(
        version,
        "FlyCapture2 library version: %d.%d.%d.%d\n",
        fc2Version.major, fc2Version.minor, fc2Version.type, fc2Version.build );

    printf( version );

    char timeStamp[512];
    sprintf( timeStamp, "Application build date: %s %s\n\n", __DATE__, __TIME__ );

    printf( timeStamp );
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

enum ExtendedShutterType
{
    NO_EXTENDED_SHUTTER,
    GENERAL_EXTENDED_SHUTTER
};

std::string GetCurrentTimeString() {
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime( &rawtime );

	std::ostringstream formatTime;
	formatTime << (timeinfo->tm_year + 1900) << "-"
		<< (timeinfo->tm_mon + 1) << "-"
		<< (timeinfo->tm_mday) << " "
		<< (timeinfo->tm_hour) << ":"
		<< (timeinfo->tm_min) << ":"
		<< (timeinfo->tm_sec);

	return formatTime.str();
}


void OnBusReset( void* pParam, unsigned int serialNumber )
{
	std::cout << GetCurrentTimeString() << " - *** BUS RESET ***" << std::endl;
	
//	//delete[] cam;
//	exit(EXIT_SUCCESS);
	exit(EXIT_FAILURE);

}

void OnBusArrival( void* pParam, unsigned int serialNumber )
{
	std::cout << GetCurrentTimeString() << " - *** BUS ARRIVAL (" << serialNumber << ") ***" << std::endl;

//	//delete[] cam;
//	exit(EXIT_SUCCESS);
	exit(EXIT_FAILURE);

}

void OnBusRemoval( void* pParam, unsigned int serialNumber )
{
	std::cout << GetCurrentTimeString() << " - *** BUS REMOVAL (" << serialNumber << ") ***" << std::endl;

//	//delete[] cam;
//	exit(EXIT_SUCCESS);
	exit(EXIT_FAILURE);

}
 

int BusResetLoop()
{
	BusManager busMgr;

//	FlyCapture2::CallbackHandle m_resetHandle;
//	FlyCapture2::CallbackHandle m_arrivalHandle;
//	FlyCapture2::CallbackHandle m_removalHandle;

	// Register bus events
	Error error;
	error = busMgr.RegisterCallback(&OnBusReset,BUS_RESET,NULL,&m_resetHandle );
	if ( error != PGRERROR_OK )
	{
		PrintError( error );
		return -1;
	}

	error = busMgr.RegisterCallback(&OnBusArrival,ARRIVAL,NULL,&m_arrivalHandle );
	if ( error != PGRERROR_OK )
	{
		PrintError( error );
		return -1;
	}

	error = busMgr.RegisterCallback(&OnBusRemoval, REMOVAL,NULL,&m_removalHandle );
	if ( error != PGRERROR_OK )
	{
		PrintError( error );
		return -1;
	}

	return 0;
}


//int main(int /*argc*/, char** /*argv*/)
int main(int argc, char* argv[])
{

    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    if (sigaction(SIGTERM, &action, NULL) == -1) {
        perror("Error: cannot handle SIGTERM"); // Should not happen
    }   

    if (sigaction(SIGINT, &action, NULL) == -1) {
        perror("Error: cannot handle SIGINT"); // Should not happen
    }

        if (sigaction(SIGHUP, &action, NULL) == -1) {
        perror("Error: cannot handle SIGHUP"); // Should not happen
    }


	unsigned int ulValue;
	CameraInfo camInfo;

	// Check the number of parameters
	    if (argc < 5) {
	        // Tell the user how to run the program if the user enters the command incorrectly.
	        std::cerr << "Usage: " << argv[0] << " PARFILESPATH" << " currentparfilestatus" <<  " IMAGEFILESPATH" << "SERIAL NUMBER " << std::endl; // Usage message
	        return 1;
	    }

	//    cout << "Input: " << argv[1] << endl; // uncomment for debugging

	    std:string pardir = argv[1];
	    const char*  filestatus = argv[2];
	    string imagedir = argv[3];
            const char* camtype = argv[4];
   
            unsigned int serial;   
            stringstream strValue;
            strValue << camtype;

            strValue >> serial;

 //           cout << camtype << endl; // uncomment for debugging
 //           cout << serial << endl;  // uncomment for debugging

            int lengthOfStringimagedir; //hold the number of characters in the string
            lengthOfStringimagedir = imagedir.length();; 
	    

           

	//    cout << filestatus << endl; // uncomment for debugging
	

    PrintBuildInfo();
    Error error;
    // Since this application saves images in the imagedir folder
    // we must ensure that we have permission to write to this folder.
    // If we do not have permission, fail right away.

        string testfilename;
        ifstream testfile;                
        stringstream tf;
        tf << imagedir  << "test.txt";
        testfilename = tf.str();
  
	FILE* tempFile = fopen(testfilename.c_str(), "w+");
	if (tempFile == NULL)
	{
		printf("Failed to create file in %s folder.  Please check permissions.\n", imagedir.c_str());
		return -1;
	}
        else
        {
          printf("Success: you have permission to write file in %s folder.\n", imagedir.c_str());
        }
	fclose(tempFile);
	remove(testfilename.c_str());

    BusManager busMgr;
    unsigned int numCameras;

    int retValue = BusResetLoop();
	if ( retValue != 0 )
	{
        PrintError( error );	
	return -1;
	}

    delay(5);

    error = busMgr.GetNumOfCameras(&numCameras);
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }


    if (numCameras < 1)
       {
       printf( "No camera was detected\n"); 
       return -1;
       }
    else 
        {
          printf( "Number of cameras detected: %u\n", numCameras );
        }

   

    Camera cam; // initialize a single camera
    
    
            PGRGuid guid;           
            error = busMgr.GetCameraFromSerialNumber(serial, &guid);
            if (error != PGRERROR_OK)
             {
                PrintError( error );
                cout << "The camera you requested is not available" << endl; 
                //delete[] cam;
                return -1;
             } 
            

              error = cam.Connect(&guid); // connect the desidered camera
              if (error != PGRERROR_OK)
             {
                PrintError( error );
                //delete[] cam;
                return -1;
             } 
             
           



          error = cam.GetCameraInfo(&camInfo);
          if (error != PGRERROR_OK)
           {
             PrintError(error);
             //delete[] cam;
             return -1;
            }

          PrintCameraInfo(&camInfo);

          // Turn Timestamp on

          EmbeddedImageInfo imageInfo;
          imageInfo.timestamp.onOff = true;
          error = cam.SetEmbeddedImageInfo(&imageInfo);
          if (error != PGRERROR_OK)
           {
             PrintError(error);
             //delete[] cam;
             return -1;
           }

          // read from the camera register the default factory value for each parameter

          Property frmRate;
          frmRate.type = FRAME_RATE;
          error = cam.GetProperty( &frmRate );
          if (error != PGRERROR_OK)
            {
              PrintError( error );
              //delete[] cam;
              return -1;
            }

          PropertyInfo frRate;
          frRate.type = FRAME_RATE;
          error = cam.GetPropertyInfo( &frRate );
          if (error != PGRERROR_OK)
            {
              PrintError( error );
              //delete[] cam;
              return -1;
            }
          float minfrmRate = frRate.absMin;
          float maxfrmRate = frRate.absMax;

          printf( "Default FRAME_RATE is %3.1f fps. \n" , frmRate.absValue );

        //  printf("Test name %s\n", camInfo.modelName);

             char* name1;
             name1 = strtok(camInfo.modelName, " ");

           //  cout << name1 << '\n'; uncomment for debugging

           if ( strcmp("Chameleon", name1) == 0)
            {
        	   cout << "This is NIR camera" << endl;
           }

          if ( strcmp("Chameleon", name1) == 0) // read the temperature register only for NIR camera
            {
              error = cam.ReadRegister(0x82C, &ulValue); // read the temperature register
              if (error != PGRERROR_OK)
                {
                  PrintError( error );
                  //delete[] cam;
                  return -1;
                }

              unsigned r = createMask(20, 31); // extract the bits of interest
              unsigned int res = r & ulValue;  // here you have already the Kelvin temperature * 10
              cout << "Initial Temperature is " << res / 10.0 << " K - " << res / 10.0	- 273.15 << " Celsius" << endl;
          }


    // Get the camera configuration
    FC2Config config;
    error = cam.GetConfiguration(&config);
    if (error != PGRERROR_OK)
      {
        PrintError(error);
        return -1;
     }


          //Update the register of each camera reading from parfile

          std::string line;
          std::string parfilename;
          std::string parfilename1;

          unsigned int checklines = 0;

          float frate, shutt, gain, autoexpo;
          char frset[3], brset[3], autexpset[3];
          int desiredBrightness;

          ifstream parfile;
          string destination;

          if ( strcmp("Chameleon", name1) == 0)
            {
        	  std::stringstream pf;
        	  pf << pardir  << "/defaultNIR.ini";
        	  parfilename = pf.str();
        	  parfile.open(parfilename.c_str());
        	  destination = "NIR";
            }
          else
          {
        	  std::stringstream pf;
        	  pf << pardir  << "/defaultVIS.ini";
        	  parfilename = pf.str();
              parfile.open(parfilename.c_str());
        	  destination = "VIS";
          }


          // Set the new values from the parameters reading them from the parameter file
          if (parfile.is_open())
           {
              printf( "Reading from parfile: %s \n", parfilename.c_str() );
              while ( getline (parfile,line) )
                   {
                     std::istringstream in(line);      //make a stream for the line itself
                     std::string type;
                     in >> type;                  //and read the first whitespace-separated token


                     if(type == "FRAMERATE")       //and check its value
                      {
                        in >> frate;       //now read the whitespace-separated floats
                        cout << type << " " << frate << " fps " << endl;
                      }
                     else if(type == "SHUTTER")
                      {
                        in >> shutt;
                        cout << type << " " << shutt << " ms " << endl;
                      }
                     else if((type == "GAIN"))
                      {
                        in >> gain;
                        cout << type << " " << gain << endl;
                      }
                     else if((type == "BRIGHTNESS"))
                      {
                       in >> desiredBrightness;
                       cout << type << " " << desiredBrightness << endl;
                      }
                     else if((type == "AUTO_EXPOSURE"))
                      {
                        in >> autoexpo;
                        cout << type << " " << autoexpo << endl;
                      }

                     if(type == "FRAMERATESET")
                      {
                        in >> frset;
                  //    cout << "Setting the frame rate " << frset << endl; // uncomment for debugging purposes
                      }
                     if(type == "BRIGHTSET")
                      {
                       in >> brset;
                    //            cout << "Setting the brightness " << brset << endl; // uncomment for debugging purposes
                      }
                     if(type == "AUTOEXPOSET")
                       {
                          in >> autexpset;
                      //            cout << "Setting the autoexposure " << autexpset << endl; // uncomment for debugging purposes
                       }

                }
            parfile.close();

           }
            else
                {
                 printf( "Unable to open parfile!!! \n" );
                 return -1;
                }

          ifstream parfile1;
          if ( strcmp("Chameleon", name1) == 0)
            {
              std::stringstream pf1;
              pf1 << pardir  << "/currentNIR.ini";
              parfilename1 = pf1.str();
              parfile1.open(parfilename1.c_str());
              
            }
          else
            {
              std::stringstream pf1;
              pf1 << pardir  << "/currentVIS.ini";
              parfilename1 = pf1.str();
              parfile1.open(parfilename1.c_str());
              
            }

          float frate1, shutt1, gain1, autoexpo1;
          char frset1[3], brset1[3], autexpset1[3];
          int desiredBrightness1;

          // Set the new values from the parameters reading them from the current parameter file
                    if (parfile1.is_open())
                     {
                        printf( "Reading from parfile: %s \n", parfilename1.c_str() );
                        while ( getline (parfile1,line) )
                             {
                               std::istringstream in(line);      //make a stream for the line itself
                               std::string type;
                               in >> type;                  //and read the first whitespace-separated token


                               if(type == "FRAMERATE")       //and check its value
                                {
                                  in >> frate1;       //now read the whitespace-separated floats
                                  cout << type << " " << frate1 << " fps " << endl;
                                  checklines = checklines +1;
                                }
                               else if(type == "SHUTTER")
                                {
                                  in >> shutt1;
                                  cout << type << " " << shutt1 << " ms " << endl;
                                  checklines = checklines +1;
                                }
                               else if((type == "GAIN"))
                                {
                                  in >> gain1;
                                  cout << type << " " << gain1 << endl;
                                  checklines = checklines +1;
                                }
                               else if((type == "BRIGHTNESS"))
                                {
                                 in >> desiredBrightness1;
                                 cout << type << " " << desiredBrightness1 << endl;
                                 checklines = checklines +1;
                                }
                               else if((type == "AUTO_EXPOSURE"))
                                {
                                  in >> autoexpo1;
                                  cout << type << " " << autoexpo1 << endl;
                                  checklines = checklines +1;
                                }

                               if(type == "FRAMERATESET")
                                {
                                  in >> frset1;
                                  checklines = checklines +1;
                            //    cout << "Setting the frame rate " << frset1 << endl; // uncomment for debugging purposes
                                }
                               if(type == "BRIGHTSET")
                                {
                                 in >> brset1;
                                 checklines = checklines +1;
                              //            cout << "Setting the brightness " << brset1 << endl; // uncomment for debugging purposes
                                }
                               if(type == "AUTOEXPOSET")
                                 {
                                    in >> autexpset1;
                                    checklines = checklines +1;
                                //            cout << "Setting the autoexposure " << autexpset1 << endl; // uncomment for debugging purposes
                                 }

                          }
                      parfile1.close();
                     }
                    else
                      {
                    	if (strcmp("Y", filestatus) == 0)
                    	{
                         printf( "Unable to open current parfile!!! Using default parfile \n" );
                    	}
                    	else
                    	{
                         printf( "As current parfile does not exist, continuing with default parfile \n" );
                    	}
                      }


                    if (checklines != 8)
                              {
                               filestatus = "N";
                               printf( "The current parfile has not passed the screening check, continuing with default parfile \n" );
                              }


          // Turn off SHARPNESS if the camera supports this property
           PropertyInfo propInfo;
           propInfo.type = SHARPNESS;
           error =  cam.GetPropertyInfo( &propInfo );
           if (error != PGRERROR_OK)
             {
                PrintError( error );
                //delete[] cam;
                return -1;
             }

           if ( propInfo.present == true )
             {
              // Then turn off SHARPNESS
                Property prop;
                prop.type = SHARPNESS;
                prop.autoManualMode = false;
                prop.onOff = false;

                error = cam.SetProperty( &prop );
                if (error != PGRERROR_OK)
                       {
                         PrintError( error );
                         //delete[] cam;
                         return -1;
                       }
                cout << "Setting off SHARPNESS" << endl;
             }
           else
             {
               printf( "This camera does not support SHARPNESS \n");
             }

           // Turn off SATURATION if the camera supports this property

                      propInfo.type = SATURATION;
                      error =  cam.GetPropertyInfo( &propInfo );
                      if (error != PGRERROR_OK)
                        {
                           PrintError( error );
                           //delete[] cam;
                           return -1;
                        }

                      if ( propInfo.present == true )
                        {
                         // Then turn off SATURATION
                           Property prop;
                           prop.type = SATURATION;
                           prop.autoManualMode = false;
                           prop.onOff = false;

                           error = cam.SetProperty( &prop );
                           if (error != PGRERROR_OK)
                                  {
                                    PrintError( error );
                                    //delete[] cam;
                                    return -1;
                                  }
                           cout << "Setting off SATURATION" << endl;
                        }
                      else
                        {
                          printf( "This camera does not support SATURATION \n");
                        }

                      // Turn off IRIS if the camera supports this property

                                 propInfo.type = IRIS;
                                 error =  cam.GetPropertyInfo( &propInfo );
                                 if (error != PGRERROR_OK)
                                   {
                                      PrintError( error );
                                      //delete[] cam;
                                      return -1;
                                   }

                                 if ( propInfo.present == true )
                                   {
                                    // Then turn off IRIS
                                      Property prop;
                                      prop.type = IRIS;
                                      prop.autoManualMode = false;
                                      prop.onOff = false;

                                      error = cam.SetProperty( &prop );
                                      if (error != PGRERROR_OK)
                                             {
                                               PrintError( error );
                                               //delete[] cam;
                                               return -1;
                                             }
                                      cout << "Setting off IRIS" << endl;
                                   }
                                 else
                                   {
                                     printf( "This camera does not support IRIS \n");
                                   }


           // Turn off WHITE_BALANCE if the camera supports this property

            propInfo.type = WHITE_BALANCE;
            
             error =  cam.GetPropertyInfo( &propInfo );
                    if (error != PGRERROR_OK)
                      {
                        PrintError( error );
                        //delete[] cam;
                        return -1;
                      }

                    if ( propInfo.present == true )
                     {
                      // Then turn off WHITE_BALANCE
                      Property prop;
                      prop.type = WHITE_BALANCE;
                      prop.autoManualMode = false;
                      prop.onOff = false;

                      error = cam.SetProperty( &prop );
                      if (error != PGRERROR_OK)
                        {
                          PrintError( error );
                          //delete[] cam;
                          return -1;
                        }
                      cout << "Setting off WHITE_BALANCE" << endl;
                     }
                    else
                       {
                         printf( "This camera does not support WHITE_BALANCE \n");
                       }

                    // Turn off HUE if the camera supports this property

                     propInfo.type = HUE;
                     error =  cam.GetPropertyInfo( &propInfo );
                     if (error != PGRERROR_OK)
                       {
                         PrintError( error );
                         //delete[] cam;
                         return -1;
                       }

                     if ( propInfo.present == true )
                       {
                         // Then turn off HUE
                        Property prop;
                        prop.type = HUE;
                        prop.autoManualMode = false;
                        prop.onOff = false;

                        error = cam.SetProperty( &prop );
                        if (error != PGRERROR_OK)
                          {
                            PrintError( error );
                            //delete[] cam;
                            return -1;
                           }
                        cout << "Setting off HUE" << endl;
                        }
                     else
                     {
                    	 printf( "This camera does not support HUE \n");
                     }

                     // Turn off GAMMA if the camera supports this property

                      propInfo.type = GAMMA;
                      error =  cam.GetPropertyInfo( &propInfo );
                      if (error != PGRERROR_OK)
                        {
                          PrintError( error );
                          //delete[] cam;
                          return -1;
                        }

                       if ( propInfo.present == true )
                         {
                          // Then turn off GAMMA
                             Property prop;
                             prop.type = GAMMA;
                             prop.autoManualMode = false;
                             prop.onOff = false;

                             error = cam.SetProperty( &prop );
                             if (error != PGRERROR_OK)
                               {
                                 PrintError( error );
                                 //delete[] cam;
                                 return -1;
                               }
                                cout << "Setting off GAMMA" << endl;
                               }
                         else
                              {
                                printf( "This camera does not support GAMMA \n");
                              }
                       // Turn off PAN if the camera supports this property

                        propInfo.type = PAN;
                        error =  cam.GetPropertyInfo( &propInfo );
                        if (error != PGRERROR_OK)
                          {
                                    PrintError( error );
                                    //delete[] cam;
                                    return -1;
                          }

                     if ( propInfo.present == true )
                       {
                                  // Then turn off PAN
                          Property prop;
                          prop.type = PAN;
                          prop.autoManualMode = false;
                          prop.onOff = false;

                          error = cam.SetProperty( &prop );
                          if (error != PGRERROR_OK)
                            {
                               PrintError( error );
                               //delete[] cam;
                               return -1;
                            }
                           cout << "Setting off PAN" << endl;
                        }
                    else
                        {
                          printf( "This camera does not support PAN \n");
                        }

                     // Turn off TILT if the camera supports this property

                      propInfo.type = TILT;
                      error =  cam.GetPropertyInfo( &propInfo );
                      if (error != PGRERROR_OK)
                        {
                                  PrintError( error );
                                  //delete[] cam;
                                  return -1;
                        }

                      if ( propInfo.present == true )
                               {
                                // Then turn off TILT
                                Property prop;
                                prop.type = TILT;
                                prop.autoManualMode = false;
                                prop.onOff = false;

                                error = cam.SetProperty( &prop );
                                if (error != PGRERROR_OK)
                                  {
                                    PrintError( error );
                                    //delete[] cam;
                                    return -1;
                                  }
                                cout << "Setting off TILT" << endl;
                               }
                      else
                                   {
                                    printf( "This camera does not support TILT \n");
                                   }

                      // Turn off ZOOM if the camera supports this property

                       propInfo.type = ZOOM;
                               error =  cam.GetPropertyInfo( &propInfo );
                               if (error != PGRERROR_OK)
                                 {
                                   PrintError( error );
                                   //delete[] cam;
                                   return -1;
                                 }

                               if ( propInfo.present == true )
                                {
                                 // Then turn off ZOOM
                                 Property prop;
                                 prop.type = ZOOM;
                                 prop.autoManualMode = false;
                                 prop.onOff = false;

                                 error = cam.SetProperty( &prop );
                                 if (error != PGRERROR_OK)
                                   {
                                     PrintError( error );
                                     //delete[] cam;
                                     return -1;
                                   }
                                 cout << "Setting off ZOOM" << endl;
                                }
                                else
                                    {
                                     printf( "This camera does not support ZOOM \n");
                                    }

                               // Turn off TRIGGER_MODE if the camera supports this property

                                propInfo.type = TRIGGER_MODE;
                                        error =  cam.GetPropertyInfo( &propInfo );
                                        if (error != PGRERROR_OK)
                                          {
                                            PrintError( error );
                                            //delete[] cam;
                                            return -1;
                                          }

                                        if ( propInfo.present == true )
                                         {
                                          // Then turn off TRIGGER_MODE
                                          Property prop;
                                          prop.type = TRIGGER_MODE;
                                          prop.autoManualMode = false;
                                          prop.onOff = false;

                                          error = cam.SetProperty( &prop );
                                          if (error != PGRERROR_OK)
                                            {
                                              PrintError( error );
                                              //delete[] cam;
                                              return -1;
                                            }
                                          cout << "Setting off TRIGGER_MODE" << endl;
                                         }
                                         else
                                             {
                                              printf( "This camera does not support TRIGGER_MODE \n");
                                             }

                                        // Turn off TRIGGER_DELAY if the camera supports this property

                                         propInfo.type = TRIGGER_DELAY;
                                                 error =  cam.GetPropertyInfo( &propInfo );
                                                 if (error != PGRERROR_OK)
                                                   {
                                                     PrintError( error );
                                                     //delete[] cam;
                                                     return -1;
                                                   }

                                                 if ( propInfo.present == true )
                                                  {
                                                   // Then turn off TRIGGER_DELAY
                                                   Property prop;
                                                   prop.type = TRIGGER_DELAY;
                                                   prop.autoManualMode = false;
                                                   prop.onOff = false;

                                                   error = cam.SetProperty( &prop );
                                                   if (error != PGRERROR_OK)
                                                     {
                                                       PrintError( error );
                                                       //delete[] cam;
                                                       return -1;
                                                     }
                                                   cout << "Setting off TRIGGER_DELAY" << endl;
                                                  }
                                                  else
                                                      {
                                                       printf( "This camera does not support TRIGGER_DELAY \n");
                                                      }


           //  check if the camera supports the FRAME_RATE property

               propInfo.type = FRAME_RATE;
               error =  cam.GetPropertyInfo( &propInfo );
               if (error != PGRERROR_OK)
                 {
                   PrintError( error );
                   //delete[] cam;
                   return -1;
                 }

          if ( propInfo.present == true && strcmp("Y", filestatus) == 0)
            {
              if ( strcmp("OFF",frset1) == 0 || strcmp("OFf",frset1)  == 0|| strcmp("OfF",frset1)  == 0 || strcmp("Off",frset1)  == 0 || strcmp("oFF",frset1)  == 0  || strcmp("oFf",frset1) == 0 || strcmp("ofF",frset1) == 0 || strcmp("off",frset1) == 0)
                {
                  cout << "Setting " << frset1 << " FRAME_RATE as requested from current parfile"  << endl;

                  ExtendedShutterType shutterType = NO_EXTENDED_SHUTTER;
                  // Then turn off frame rate
                  Property prop;
                  prop.type = FRAME_RATE;
                  error =  cam.GetProperty( &prop );
                  if (error != PGRERROR_OK)
                    {
                      PrintError( error );
                      //delete[] cam;
                      return -1;
                    }

                 prop.autoManualMode = false;
                 prop.onOff = false;
                 error =  cam.SetProperty( &prop );
                 if (error != PGRERROR_OK)
                   {
                     PrintError( error );
                     //delete[] cam;
                     return -1;
                   }
                 shutterType = GENERAL_EXTENDED_SHUTTER;
                 }
                 else
                     {
                       PropertyInfo frRate;
                       frRate.type = FRAME_RATE;
                       error = cam.GetPropertyInfo( &frRate );
                       if (error != PGRERROR_OK)
                         {
                           PrintError( error );
                           //delete[] cam;
                           return -1;
                         }
                float minfrmRate = frRate.absMin;
                float maxfrmRate = frRate.absMax;

                Property frmRate;
                frmRate.type = FRAME_RATE;
                frmRate.absControl = true;
                frmRate.onePush = false;
                frmRate.autoManualMode = false;
                frmRate.onOff = true;

                if (frate1 >= minfrmRate && frate1 <= maxfrmRate)
                  {
                   printf( "Frame rate set to %3.2f fps from current parfile \n", frate1 );
                   frmRate.absValue = frate1;
                  }
                else{
                     if ( strcmp("OFF",frset) == 0 || strcmp("OFf",frset)  == 0|| strcmp("OfF",frset)  == 0 || strcmp("Off",frset)  == 0 || strcmp("oFF",frset)  == 0  || strcmp("oFf",frset) == 0 || strcmp("ofF",frset) == 0 || strcmp("off",frset) == 0)
                       {
                         cout << "Setting " << frset << " FRAME_RATE as requested in default file"  << endl;
                         ExtendedShutterType shutterType = NO_EXTENDED_SHUTTER;
                         // Then turn off frame rate
                         Property prop;
                         prop.type = FRAME_RATE;
                         error =  cam.GetProperty( &prop );
                         if (error != PGRERROR_OK)
                           {
                             PrintError( error );
                             //delete[] cam;
                             return -1;
                            }

                         prop.autoManualMode = false;
                         prop.onOff = false;
                         error =  cam.SetProperty( &prop );
                         if (error != PGRERROR_OK)
                           {
                             PrintError( error );
                             //delete[] cam;
                             return -1;
                           }
                         shutterType = GENERAL_EXTENDED_SHUTTER;
                       }
                       else
                           {
                             PropertyInfo frRate;
                             frRate.type = FRAME_RATE;
                             error = cam.GetPropertyInfo( &frRate );
                             if (error != PGRERROR_OK)
                               {
                                 PrintError( error );
                                 //delete[] cam;
                                 return -1;
                               }
                             float minfrmRate = frRate.absMin;
                             float maxfrmRate = frRate.absMax;

                             Property frmRate;
                             frmRate.type = FRAME_RATE;
                             frmRate.absControl = true;
                             frmRate.onePush = false;
                             frmRate.autoManualMode = false;
                             frmRate.onOff = true;

                             if (frate >= minfrmRate && frate <= maxfrmRate)
                               {
                                 cout << "Problems with the FRAME_RATE in current parfile. Continuing with default parfile." << endl;
                                 printf( "Frame rate set to %3.2f fps from default file \n", frate );
                                 frmRate.absValue = frate;
                               }

                             else{
                                   printf( "Frame Rate outside allowed range.  \n" );
                                   //delete[] cam;
                                   return -1;
                                 }
                         }

                         error = cam.SetProperty( &frmRate );
                         if (error != PGRERROR_OK)
                           {
                             PrintError( error );
                             //delete[] cam;
                             return -1;
                           }
                       }
                                                                               }
                    }
                    else
                    {
                    	  if ( propInfo.present == false )
                    	  {
                           printf( "Frame rate and extended shutter are not supported... exiting\n" );
                           //delete[] cam;
                           return -1;
                    	  }
                    	  else
                    	  {
                    		  if ( strcmp("OFF",frset) == 0 || strcmp("OFf",frset)  == 0|| strcmp("OfF",frset)  == 0 || strcmp("Off",frset)  == 0 || strcmp("oFF",frset)  == 0  || strcmp("oFf",frset) == 0 || strcmp("ofF",frset) == 0 || strcmp("off",frset) == 0)
                    		    {
                    		       cout << "Setting " << frset << " FRAME_RATE as requested in default file"  << endl;
                    		       ExtendedShutterType shutterType = NO_EXTENDED_SHUTTER;
                    		       // Then turn off frame rate
                    		       Property prop;
                    		       prop.type = FRAME_RATE;
                    		       error =  cam.GetProperty( &prop );
                    		       if (error != PGRERROR_OK)
                    		         {
                    		           PrintError( error );
                    		           //delete[] cam;
                    		           return -1;
                    		         }

                    		       prop.autoManualMode = false;
                    		       prop.onOff = false;
                    		       error =  cam.SetProperty( &prop );
                    		       if (error != PGRERROR_OK)
                    		         {
                    		           PrintError( error );
                    		           //delete[] cam;
                    		           return -1;
                    		         }
                    		       shutterType = GENERAL_EXTENDED_SHUTTER;
                    		     }
                    		  else{
                    			    PropertyInfo frRate;
                    			    frRate.type = FRAME_RATE;
                    			    error = cam.GetPropertyInfo( &frRate );
                    			    if (error != PGRERROR_OK)
                    			      {
                    			        PrintError( error );
                    			        //delete[] cam;
                    			        return -1;
                    			       }
                    			    float minfrmRate = frRate.absMin;
                    			    float maxfrmRate = frRate.absMax;

                    			    Property frmRate;
                    			    frmRate.type = FRAME_RATE;
                    			    frmRate.absControl = true;
                    			    frmRate.onePush = false;
                    			    frmRate.autoManualMode = false;
                    			    frmRate.onOff = true;

                    			    if (frate >= minfrmRate && frate <= maxfrmRate)
                    			      {
                    			         printf( "Frame rate set to %3.2f fps from default file \n", frate );
                    			         frmRate.absValue = frate;
                    			      }
                    			   else{
                    			         printf( "Frame Rate outside allowed range.  \n" );
                    			         //delete[] cam;
                    			         return -1;
                           			  }

                  		       error = cam.SetProperty( &frmRate );
                    	    if (error != PGRERROR_OK)
                    		  {
                    			 PrintError( error );
                    			 //delete[] cam;
                    			 return -1;
                    	      }
                    	}
                    }
                  }

            // Set the shutter property of the camera

            PropertyInfo Shut;
            Shut.type = SHUTTER;
            error = cam.GetPropertyInfo( &Shut );
            if (error != PGRERROR_OK)
              {
                PrintError( error );
                //delete[] cam;
                return -1;
              }
            float minShutter = Shut.absMin;
            float maxShutter = Shut.absMax;

            // printf( "Min %3.1f ms Max %3.1f ms  \n" , Shut.absMin, Shut.absMax ); // uncomment this line to debug

             Property shutter;
             shutter.type = SHUTTER;
             shutter.absControl = true;
             shutter.onePush = false;
             shutter.autoManualMode = false;
             shutter.onOff = true;


             if (shutt1 >= minShutter && shutt1 <= maxShutter && strcmp("Y", filestatus) == 0)
               {
                 shutter.absValue = shutt1;
                 printf( "Shutter time set to %3.2f ms from current parfile \n", shutt1 );
                 // Set the grab timeout according to the shutter
            	config.grabTimeout = 3*shutt1;
               }
             else
             {
              if (shutt >= minShutter && shutt <= maxShutter && strcmp("Y", filestatus) == 0)
                {
            	 shutter.absValue = shutt;
                 printf( "WARNING! Shutter outside allowed range in current parfile: shutter time set to %3.2f ms using default parfile\n", shutt );
                // Set the grab timeout according to the shutter
            	config.grabTimeout = 3*shutt;
                }
              else if(shutt >= minShutter && shutt <= maxShutter && strcmp("Y", filestatus) != 0)
              {
            	 shutter.absValue = shutt;
              	printf( "Shutter time set to %3.2f ms from default parfile \n", shutt );
                // Set the grab timeout according to the shutter
            	config.grabTimeout = 3*shutt;
              }
             else{
                  printf( "WARNING! Shutter outside allowed range: setting it to the maximum allowed value %3.2f ms \n", maxShutter );
                  shutter.absValue = maxShutter;
                  // Set the grab timeout according to the shutter
            	config.grabTimeout = 3*maxShutter;
                 }
             }

           error = cam.SetProperty( &shutter );
           if (error != PGRERROR_OK)
             {
               PrintError( error );
               //delete[] cam;
               return -1;
             }

        // Set the camera configuration
	error = cam.SetConfiguration(&config);
	if (error != PGRERROR_OK)
	  {
		PrintError(error);
                //delete[] cam;
		return -1;
	  }



           //  check if the camera supports the AUTO_EXPOSURE property

            propInfo.type = AUTO_EXPOSURE;
            error =  cam.GetPropertyInfo( &propInfo );
            if (error != PGRERROR_OK)
              {
                 PrintError( error );
                 //delete[] cam;
                 return -1;
              }

            propInfo.type = AUTO_EXPOSURE;
              error =  cam.GetPropertyInfo( &propInfo );
              if (error != PGRERROR_OK)
                {
                   PrintError( error );
                   //delete[] cam;
                   return -1;
                }

             if ( propInfo.present == true &&  (strcmp("Y", filestatus) == 0))
              {
                 if ( strcmp("OFF",autexpset1) == 0 || strcmp("OFf",autexpset1)  == 0|| strcmp("OfF",autexpset1)  == 0 || strcmp("Off",autexpset1)  == 0 || strcmp("oFF",autexpset1)  == 0  || strcmp("oFf",autexpset1) == 0 || strcmp("ofF",autexpset1) == 0 || strcmp("off",autexpset1) == 0)
                  {
                     cout << "Setting " << autexpset1 << " AUTO_EXPOSURE "  << endl;
                    // Then turn off AUTO_EXPOSURE
                       Property prop;
                       prop.type = AUTO_EXPOSURE;
                       error =  cam.GetProperty( &prop );
                       if (error != PGRERROR_OK)
                         {
                           PrintError( error );
                           //delete[] cam;
                           return -1;
                         }

                        prop.autoManualMode = false;
                        prop.onOff = false;
                        error =  cam.SetProperty( &prop );
                        if (error != PGRERROR_OK)
                        {
                          PrintError( error );
                          //delete[] cam;
                          return -1;
                        }

                    }
                   else
                    {
                       PropertyInfo propinfo;
                       propinfo.type = AUTO_EXPOSURE;
                       error = cam.GetPropertyInfo( &propinfo );
                       if (error != PGRERROR_OK)
                        {
                         PrintError( error );
                         //delete[] cam;
                         return -1;
                         }

                        Property prop;
                        prop.absControl = true;
                        prop.type = AUTO_EXPOSURE;
                        prop.onePush = false;
                        prop.autoManualMode = false;
                        prop.onOff = true;

                        if (autoexpo1 >= propInfo.absMin && autoexpo1 <= propInfo.absMax)
                         {
                           prop.absValue = autoexpo1;
                           error = cam.SetProperty( &prop );
                           if (error != PGRERROR_OK)
                             {
                               PrintError( error );
                               //delete[] cam;
                               return -1;
                             }
                           printf( "AUTO_EXPOSURE set to %3.3f from current parfile \n", autoexpo1 );
                         }
                       else
                        {
                          if ( strcmp("OFF",autexpset) == 0 || strcmp("OFf",autexpset)  == 0|| strcmp("OfF",autexpset)  == 0 || strcmp("Off",autexpset)  == 0 || strcmp("oFF",autexpset)  == 0  || strcmp("oFf",autexpset) == 0 || strcmp("ofF",autexpset) == 0 || strcmp("off",autexpset) == 0)
                           {
                                cout << "Setting " << autexpset << " AUTO_EXPOSURE as requested in current parfile"  << endl;

                                // Then turn off AUTO_EXPOSURE
                                Property prop;
                                prop.type = AUTO_EXPOSURE;
                                error =  cam.GetProperty( &prop );
                                if (error != PGRERROR_OK)
                                 {
                                  PrintError( error );
                                  //delete[] cam;
                                  return -1;
                                 }

                                prop.autoManualMode = false;
                                prop.onOff = false;
                                error =  cam.SetProperty( &prop );
                                if (error != PGRERROR_OK)
                                  {
                                    PrintError( error );
                                    //delete[] cam;
                                    return -1;
                                   }

                               }
                             else
                               {
                                 PropertyInfo propinfo;
                                 propinfo.type = AUTO_EXPOSURE;
                                 error = cam.GetPropertyInfo( &propinfo );
                                 if (error != PGRERROR_OK)
                                   {
                                     PrintError( error );
                                     //delete[] cam;
                                     return -1;
                                   }

                                 Property prop;
                                 prop.absControl = true;
                                 prop.type = AUTO_EXPOSURE;
                                 prop.onePush = false;
                                 prop.autoManualMode = false;
                                 prop.onOff = true;

                                 if (autoexpo >= propInfo.absMin && autoexpo <= propInfo.absMax)
                                   {
                                     prop.absValue = autoexpo;
                                     error = cam.SetProperty( &prop );
                                     if (error != PGRERROR_OK)
                                       {
                                         PrintError( error );
                                         //delete[] cam;
                                         return -1;
                                       }
                                      printf( "AUTO_EXPOSURE set to %3.3f  from default parfile \n", autoexpo );
                                    }
                                      else
                                       {
                                         printf( "WARNING! AUTO_EXPOSURE outside allowed range [%3.2f, %3.2f] leaving it to the default camera register value \n", propInfo.absMin, propInfo.absMax);
                                       }
                                    }
                     }
                    }
                 }
                  else
                      {
                	  if (propInfo.present == false )
                	    {
                	     printf( "Warning: AUTO_EXPOSURE is not supported... \n" );
                	    }
                	  else
                	  {
                	     if ( strcmp("OFF",autexpset) == 0 || strcmp("OFf",autexpset)  == 0|| strcmp("OfF",autexpset)  == 0 || strcmp("Off",autexpset)  == 0 || strcmp("oFF",autexpset)  == 0  || strcmp("oFf",autexpset) == 0 || strcmp("ofF",autexpset) == 0 || strcmp("off",autexpset) == 0)
                	       {
                	         cout << "Setting " << autexpset << " AUTO_EXPOSURE as requested in default parfile"  << endl;

                	         // Then turn off AUTO_EXPOSURE
                	         Property prop;
                	         prop.type = AUTO_EXPOSURE;
                	         error =  cam.GetProperty( &prop );
                	         if (error != PGRERROR_OK)
                	           {
                	             PrintError( error );
                	             //delete[] cam;
                	             return -1;
                	           }

                	           prop.autoManualMode = false;
                	           prop.onOff = false;
                	           error =  cam.SetProperty( &prop );
                	           if (error != PGRERROR_OK)
                	             {
                	               PrintError( error );
                	               //delete[] cam;
                	               return -1;
                	             }
                	       }
                	        else
                	           {
                	        	PropertyInfo propinfo;
                	        	propinfo.type = AUTO_EXPOSURE;
                	        	error = cam.GetPropertyInfo( &propinfo );
                	        	if (error != PGRERROR_OK)
                	        	  {
                	        	    PrintError( error );
                	        	    //delete[] cam;
                	        	    return -1;
                	        	  }

                	        	 Property prop;
                	        	 prop.absControl = true;
                	        	 prop.type = AUTO_EXPOSURE;
                	        	 prop.onePush = false;
                	        	 prop.autoManualMode = false;
                	        	 prop.onOff = true;

                	        	 if (autoexpo >= propInfo.absMin && autoexpo <= propInfo.absMax)
                	        	   {
                	        	     prop.absValue = autoexpo;
                	        	     error = cam.SetProperty( &prop );
                	        	     if (error != PGRERROR_OK)
                	        	       {
                	        	         PrintError( error );
                	        	         //delete[] cam;
                	        	         return -1;
                	        	       }
                	            printf( "AUTO_EXPOSURE set to %3.3f  from default parfile \n", autoexpo );
                	           }
                	        	 else
                	        	     {
                	        	       printf( "WARNING! AUTO_EXPOSURE outside allowed range [%3.2f, %3.2f] leaving it to the default camera register value \n", propInfo.absMin, propInfo.absMax);
                	        	     }
                	           }

                	  }
                	  }


           //Set gain checking if the property is supported

             propInfo.type = GAIN;
             error =  cam.GetPropertyInfo( &propInfo );
             if (error != PGRERROR_OK)
               {
                 PrintError( error );
                 //delete[] cam;
                 return -1;
               }
             if ( propInfo.present == true &&  (strcmp("Y", filestatus) == 0))
              {
                  Property prop;
                  prop.type = GAIN;
                  prop.absControl = true;
                  prop.onePush = false;
                  prop.autoManualMode = false;
                  prop.onOff = true;


                  if (gain1 >= propInfo.absMin && gain1 <= propInfo.absMax)
                         {
                           	 prop.absValue = gain1;
                             error = cam.SetProperty( &prop );
                             if (error != PGRERROR_OK)
                                  {
                                    PrintError( error );
                                    //delete[] cam;
                                    return -1;
                                  }
                             printf( "Gain set to %3.2f dB from current parfile \n", gain1 );
                          }
                  else
                  {
                	  if (gain >= propInfo.absMin && gain <= propInfo.absMax)
                	     {
                	       prop.absValue = gain;
                	       error = cam.SetProperty( &prop );
                	       if (error != PGRERROR_OK)
                	         {
                	           PrintError( error );
                	           //delete[] cam;
                	           return -1;
                	         }
                	        printf( "Gain set to %3.2f dB from default parfile \n", gain );
                	       }
                   else
                       {
                          printf( "WARNING! Gain outside allowed range [%3.2f, %3.2f] leaving it to default camera register value \n", propInfo.absMin, propInfo.absMax);
                       }
                    }
                   }
            	else
            	    {
                  		Property prop;
            		    prop.type = GAIN;
            		    prop.absControl = true;
            		    prop.onePush = false;
            		    prop.autoManualMode = false;
            		    prop.onOff = true;

            		  if (gain >= propInfo.absMin && gain <= propInfo.absMax)
            		    {
            		      prop.absValue = gain;
            		      error = cam.SetProperty( &prop );
            		      if (error != PGRERROR_OK)
            		        {
            		          PrintError( error );
            		          //delete[] cam;
            		          return -1;
            		        }
            		     printf( "Gain set to %3.2f dB from default parfile \n", gain );
            		    }
                  else
            	       {
            		     printf( "WARNING! Gain outside allowed range [%3.2f, %3.2f] leaving it to default camera register value \n", propInfo.absMin, propInfo.absMax);
            		   }
            //	     }
                  }


             //  check if the camera supports the BRIGHTNESS property

                           propInfo.type = BRIGHTNESS;
                           error =  cam.GetPropertyInfo( &propInfo );
                           if (error != PGRERROR_OK)
                             {
                               PrintError( error );
                               //delete[] cam;
                               return -1;
                              }

                        if ( propInfo.present == true &&  (strcmp("Y", filestatus) == 0) )
                         {
                            if ( strcmp("OFF",brset1) == 0 || strcmp("OFf",brset1)  == 0|| strcmp("OfF",brset1)  == 0 || strcmp("Off",brset1)  == 0 || strcmp("oFF",brset1)  == 0  || strcmp("oFf",brset1) == 0 || strcmp("ofF",brset1) == 0 || strcmp("off",brset1) == 0)
                               {
                                cout << "Setting " << brset1 << " BRIGHTNESS as requested in current parfile"  << endl;


                              // Then turn off BRIGHTNESS
                              Property prop;
                              prop.type = BRIGHTNESS;
                              error =  cam.GetProperty( &prop );
                              if (error != PGRERROR_OK)
                                {
                                  PrintError( error );
                                  //delete[] cam;
                                  return -1;
                                }

                              prop.autoManualMode = false;
                              prop.onOff = false;
                              error =  cam.SetProperty( &prop );
                              if (error != PGRERROR_OK)
                                {
                                  PrintError( error );
                                  //delete[] cam;
                                  return -1;
                                }

                          }
                            else
                                {
                                  PropertyInfo propinfo;
                                  propinfo.type = BRIGHTNESS;
                                  error = cam.GetPropertyInfo( &propinfo );
                                  if (error != PGRERROR_OK)
                                    {
                                      PrintError( error );
                                      //delete[] cam;
                                      return -1;
                                     }


              			   int minbr = propinfo.min;
              			   int maxbr = propinfo.max;
              			// cout << "Minimum: " << minbr << "  - maximum:  " << maxbr << endl; // uncomment for debugging

                                     Property prop;
                                     prop.type = BRIGHTNESS;
                                     prop.onePush = false;
                                     prop.autoManualMode = false;
                                     prop.onOff = true;

                                     if (desiredBrightness1 >= minbr && desiredBrightness1 <= maxbr)
                                       {
                                         prop.valueA = desiredBrightness1;
                                         printf( "BRIGHTNESS set to %4d  from current file\n", desiredBrightness1 );
                                         error = cam.SetProperty( &prop );
                                         if (error != PGRERROR_OK)
                                           {
                                             PrintError( error );
                                             //delete[] cam;
                                             return -1;
                                           }
                                       }
                                    else{
                                         cout << "There is a problems with current parfile. Using default parfile." << endl;
                                         if ( strcmp("OFF",brset) == 0 || strcmp("OFf",brset)  == 0|| strcmp("OfF",brset)  == 0 || strcmp("Off",brset)  == 0 || strcmp("oFF",brset)  == 0  || strcmp("oFf",brset) == 0 || strcmp("ofF",brset) == 0 || strcmp("off",brset) == 0)
                               {
                                cout << "Setting " << brset << " BRIGHTNESS as requested in default parfile"  << endl;

                              // Then turn off BRIGHTNESS
                              Property prop;
                              prop.type = BRIGHTNESS;
                              error =  cam.GetProperty( &prop );
                              if (error != PGRERROR_OK)
                                {
                                  PrintError( error );
                                  //delete[] cam;
                                  return -1;
                                }

                              prop.autoManualMode = false;
                              prop.onOff = false;
                              error =  cam.SetProperty( &prop );
                              if (error != PGRERROR_OK)
                                {
                                  PrintError( error );
                                  //delete[] cam;
                                  return -1;
                                }

                            }
                            else
                                {
                                  PropertyInfo propinfo;
                                  propinfo.type = BRIGHTNESS;
                                  error = cam.GetPropertyInfo( &propinfo );
                                  if (error != PGRERROR_OK)
                                    {
                                      PrintError( error );
                                      //delete[] cam;
                                      return -1;
                                     }


              			   int minbr = propinfo.min;
              			   int maxbr = propinfo.max;
              			// cout << "Minimum: " << minbr << "  - maximum:  " << maxbr << endl; // uncomment for debugging

                                     Property prop;
                                     prop.type = BRIGHTNESS;
                                     prop.onePush = false;
                                     prop.autoManualMode = false;
                                     prop.onOff = true;

                                     if (desiredBrightness >= minbr && desiredBrightness <= maxbr)
                                       {
                                         prop.valueA = desiredBrightness;
                                         printf( "BRIGHTNESS set to %4d from default file  \n", desiredBrightness );
                                         error = cam.SetProperty( &prop );
                                         if (error != PGRERROR_OK)
                                           {
                                             PrintError( error );
                                             //delete[] cam;
                                             return -1;
                                           }
                                       }
                                    else{
                                          printf( "WARNING! BRIGHTNESS outside allowed range [%4d, %4d] leaving it to default value \n", minbr, maxbr );
                                        }
                                  }

                                    }

                                }
                         }
                        else
                            {
                             if (propInfo.present == false )
                             {
                               printf( "Warning: BRIGHTNESS is not supported... \n" );
                             }
                             else
                             {
                              if ( strcmp("OFF",brset) == 0 || strcmp("OFf",brset)  == 0|| strcmp("OfF",brset)  == 0 || strcmp("Off",brset)  == 0 || strcmp("oFF",brset)  == 0  || strcmp("oFf",brset) == 0 || strcmp("ofF",brset) == 0 || strcmp("off",brset) == 0)
                               {
                                 cout << "Setting " << brset << " BRIGHTNESS as requested in default parfile"  << endl;
                                 // Then turn off BRIGHTNESS
                                 Property prop;
                                 prop.type = BRIGHTNESS;
                                 error =  cam.GetProperty( &prop );
                                 if (error != PGRERROR_OK)
                                   {
                                    PrintError( error );
                                    //delete[] cam;
                                    return -1;
                                   }

                                   prop.autoManualMode = false;
                                   prop.onOff = false;
                                   error =  cam.SetProperty( &prop );
                                   if (error != PGRERROR_OK)
                                     {
                                       PrintError( error );
                                       //delete[] cam;
                                       return -1;
                                     }
                                 }
                              else
                              {

                            	  PropertyInfo propinfo;
                            	  propinfo.type = BRIGHTNESS;
                            	  error = cam.GetPropertyInfo( &propinfo );
                            	  if (error != PGRERROR_OK)
                            	    {
                            	      PrintError( error );
                            	      //delete[] cam;
                            	      return -1;
                            	    }

                            	    int minbr = propinfo.min;
                            	    int maxbr = propinfo.max;
                            	    // cout << "Minimum: " << minbr << "  - maximum:  " << maxbr << endl; // uncomment for debugging

                            	   Property prop;
                            	   prop.type = BRIGHTNESS;
                            	   prop.onePush = false;
                            	   prop.autoManualMode = false;
                            	   prop.onOff = true;

                            	   if (desiredBrightness >= minbr && desiredBrightness <= maxbr)
                            	     {
                            	       prop.valueA = desiredBrightness;
                            	       printf( "BRIGHTNESS set to %4d from default file  \n", desiredBrightness );
                            	       error = cam.SetProperty( &prop );
                            	       if (error != PGRERROR_OK)
                            	         {
                            	           PrintError( error );
                            	           //delete[] cam;
                            	           return -1;
                            	         }
                            	      }
                            	   else{
                            	         printf( "WARNING! BRIGHTNESS outside allowed range [%4d, %4d] leaving it to default value \n", minbr, maxbr );
                            	       }
                              }
                            }
                           
                           }
            // Start streaming on camera
            std::stringstream ss;
            ss << currentDateTime2();
            std::string st = ss.str();
            char pippo[st.length()];
            sprintf(pippo, "%s" , st.c_str() );
            printf( "Start time %s \n", pippo );


            error = cam.StartCapture();
            if (error != PGRERROR_OK)
              {
                PrintError(error);
                //delete[] cam;
                return -1;
              }
          


   

  //  for ( int imageCnt=0; imageCnt < k_numImages; imageCnt++ )
    int imageCnt=0; // uncomment this and other lines beginning with /// & comment previous line to get an indefinite loop
 ////   for ( ; ;  )    
 while (!done)
     {
    	      Image image;
    	      error = cam.RetrieveBuffer(&image);
              

    	      if (error != PGRERROR_OK)
    	        {
    	          PrintError(error);
    	          //delete[] cam;
    	          return -1;
    	        }

               

    	      // Display the timestamps of the images grabbed for each camera
    	         TimeStamp timestamp = image.GetTimeStamp();
    	         cout << "Camera " << " Frame " << imageCnt << " - TimeStamp [" << timestamp.cycleSeconds << " " << timestamp.cycleCount << "]"<< endl;

    	         // Save the file

    	      unsigned int res =	 0; // for initialization purposes

    	      Property shutter;
    	      shutter.type = SHUTTER;
    	      error = cam.GetProperty( &shutter );
    	      if (error != PGRERROR_OK)
    	        {
    	          PrintError( error );
    	          return -1;
    	        }


    	      error = cam.GetCameraInfo(&camInfo);
    	      if (error != PGRERROR_OK)
    	        {
    	          PrintError(error);
    	          //delete[] cam;
    	          return -1;
    	        }

    	      char* name1;
    	      name1 = strtok(camInfo.modelName, " ");

    	       if ( strcmp("Chameleon", name1) == 0)
    	         {
    	           error = cam.ReadRegister(0x82C, &ulValue); // read the temperature register for the NIR camera only
    	           unsigned r  = createMask(20, 31); // extract the bits of interest
   	               res = r & ulValue;  // here you have already the Kelvin temperature * 10
    	           if (error != PGRERROR_OK)
    	              {
    	                PrintError( error );
    	                //delete[] cam;
    	                return -1;
    	              }
   	              }
    	        PixelFormat pixFormat;
    	        unsigned int rows, cols, stride;
    	        image.GetDimensions( &rows, &cols, &stride, &pixFormat );

    	       // Create a unique filename

    	       std::string str;     //temporary string to hold the filename
    	       int lengthOfString1; //hold the number of characters in the string
    	       std::stringstream sstm;
    	       std::string head;
    	       if ( strcmp("Chameleon", name1) == 0)
    	         {
    	          head = "NIR";
    	         }
    	       else
    	          {
    	    	   head = "VIS";
    	          }

    	       sstm  << currentDateTime2();
    	       str = sstm.str();
    	       lengthOfString1=str.length();

    	       int lenghtsum = lengthOfStringimagedir + lengthOfString1 + 4  + 4 + 3;
    	       char filename[lenghtsum];
    	       sprintf(filename,"%s/%s/%s-%s.raw", imagedir.c_str(), head.c_str(), head.c_str(), str.c_str() );
    	           cout << filename << endl; // uncomment for testing purposes

    	       unsigned int iImageSize = image.GetDataSize();
    	       printf( "Grabbed image %s \n", filename );
//    	                 printf( "Frame rate is %3.1f fps\n", frmRate.absValue );
    	       printf( "Shutter is %3.1f ms\n", shutter.absValue );
    	       if ( strcmp("Chameleon", name1) == 0)
    	         {
    	           cout << "Temperature is " << res / 10.0 << " K - " << res / 10.0	- 273.15 << " Celsius" << endl;
    	         }
    	       cout << "Raw Image Dimensions: " << rows  << " x " << cols << " Image Stride: " << stride << endl;
    	       cout << "Image Size: " << iImageSize << endl;

    	      // Save the image. If a file format is not passed in, then the file
    	     // extension is parsed to attempt to determine the file format.
    	       error = image.Save( filename );
    	       if (error != PGRERROR_OK)
    	         {
    	           PrintError( error );
    	           //delete[] cam;
    	           return -1;
    	         }
      
   	 imageCnt++;
     
    }

    std::stringstream ss1;
    ss1 << currentDateTime2();
    std::string st1= ss1.str();
    char pippo1[st1.length()];
    sprintf(pippo1 , "%s" , st1.c_str() );
    printf("Kill signal received. Exiting. \n");
    printf( "End time time %s \n", pippo1 );

    // disconnect the camera
    
          cam.StopCapture();
          cam.Disconnect();
    
        //delete[] cam;

    return 0;
}

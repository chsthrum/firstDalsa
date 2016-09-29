// -----------------------------------------------------------------------------------------
// Sapera++ console grab example
//
//    This program shows how to grab images from a camera into a buffer in the host
//    computer's memory, using Sapera++ Acquisition and Buffer objects, and a Transfer
//    object to link them.  Also, a View object is used to display the buffer.
//
// -----------------------------------------------------------------------------------------

// Disable deprecated function warnings with Visual Studio 2005
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(disable: 4995)
#endif

#include "stdio.h"
#include "conio.h"
#include "sapclassbasic.h"
#include "../ExampleUtilsFromDalsa/exampleutils.h"

// Restore deprecated function warnings with Visual Studio 2005
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(default: 4995)
#endif

// Static Functions
static void AcqCallback(SapXferCallbackInfo *pInfo);
static void StartFrameCallback(SapAcqCallbackInfo *pInfo);
static BOOL GetOptions(int argc, char *argv[], char *acqServerName, UINT32 *pAcqDeviceIndex, char *configFileName);
static BOOL GetOptionsFromCommandLine(int argc, char *argv[], char *acqServerName, UINT32 *pAcqDeviceIndex, char *configFileName);



int start,end,duration,fps  = 0;
BOOL firstFrame = TRUE;

/*
*  Callback function - StartOfFrame event
*  The function will record the timestamp(in ms) of the system when
*  each callback is received and then calculate an approximate FPS rate
*  from two consecutive StartOfframe events.
*  NOTE : The FPS is only an approximation due to delays in callbacks
*  and it should not be used as the actual FPS achieved by the system.
*/
static void StartFrameCallback(SapAcqCallbackInfo *pInfo)
{
   //for the first start of frame record the start time.
   if (firstFrame)
   {
      firstFrame = FALSE;
      start = GetTickCount();
      return;
   }
   end = GetTickCount();
   duration = end - start;
   start = end;

   //update FPS only if the value changed. 1000 is used because the duration is in ms.
   if (fps != 1000/duration)
   {
      fps = 1000/duration;
      printf("Approximate FPS = %d \r",fps);
   }

}


int main(int argc, char* argv[])
{
   SapAcquisition		*Acq=NULL;
   SapAcqDevice			*AcqDevice=NULL;
   SapBuffer			*Buffers=NULL;
   SapTransfer			*Xfer=NULL;
   SapView				*View=NULL;


   //char     acqServerName[CORSERVER_MAX_STRLEN], configFilename[MAX_PATH];
   UINT32   acqDeviceNumber;
   char*    acqServerName=new char[CORSERVER_MAX_STRLEN];
    char*    configFilename=new char[MAX_PATH];

   printf("Sapera Console Grab Example (C++ version)\n");

   // Call GetOptions to determine which acquisition device to use and which config
   // file (CCF) should be loaded to configure it.
   // Note: if this were an MFC-enabled application, we could have replaced the lengthy GetOptions
   // function with the CAcqConfigDlg dialog of the Sapera++ GUI Classes (see GrabMFC example)
   if (!GetOptions(argc, argv, acqServerName, &acqDeviceNumber, configFilename))
   {
      printf("\nPress any key to terminate\n");
      CorGetch();
      return 0;
   }

   SapLocation loc(acqServerName, acqDeviceNumber);

   if (SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq) > 0)
   {
        Acq			= new SapAcquisition(loc, configFilename);
        Buffers		= new SapBuffer(1, Acq);
        View		= new SapView(Buffers, SapHwndAutomatic);
        Xfer		= new SapAcqToBuf(Acq, Buffers, AcqCallback, View);

      // Create acquisition object
      if (Acq && !*Acq && !Acq->Create())
         goto FreeHandles;

   }

   //register an acquisition callback
   if (Acq)
      Acq->RegisterCallback(SapAcquisition::EventStartOfFrame,StartFrameCallback,NULL);

   if (SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcqDevice) > 0)
   {
       if (strcmp(configFilename,"NoFile") == 0)
           AcqDevice	= new SapAcqDevice(loc, FALSE);
       else
           AcqDevice	= new SapAcqDevice(loc, configFilename);

       Buffers		= new SapBufferWithTrash(2, AcqDevice);
       View		   = new SapView(Buffers, SapHwndAutomatic);
       Xfer		   = new SapAcqDeviceToBuf(AcqDevice, Buffers, AcqCallback, View);

      // Create acquisition object
      if (AcqDevice && !*AcqDevice && !AcqDevice->Create())
         goto FreeHandles;

   }

   // Create buffer object
   if (Buffers && !*Buffers && !Buffers->Create())
      goto FreeHandles;

   // Create transfer object
   if (Xfer && !*Xfer && !Xfer->Create())
      goto FreeHandles;

   // Create view object
   if (View && !*View && !View->Create())
      goto FreeHandles;


   // Start continous grab
   Xfer->Grab();


   printf("Press any key to stop grab\n");
   CorGetch();

   // Stop grab
   Xfer->Freeze();
   if (!Xfer->Wait(5000))
      printf("Grab could not stop properly.\n");

FreeHandles:
   printf("Press any key to terminate\n");
   CorGetch();

    //unregister the acquisition callback
   if (Acq)
       Acq->UnregisterCallback();

    // Destroy view object
    if (View && *View && !View->Destroy()) return FALSE;

    // Destroy transfer object
    if (Xfer && *Xfer && !Xfer->Destroy()) return FALSE;

    // Destroy buffer object
    if (Buffers && *Buffers && !Buffers->Destroy()) return FALSE;

    // Destroy acquisition object
    if (Acq && *Acq && !Acq->Destroy()) return FALSE;

    // Destroy acquisition object
    if (AcqDevice && *AcqDevice && !AcqDevice->Destroy()) return FALSE;

    // Delete all objects
    if (View)		delete View;
    if (Xfer)		delete Xfer;
    if (Buffers)	delete Buffers;
    if (Acq)		delete Acq;
    if (AcqDevice)	delete AcqDevice;

   return 0;
}

static void AcqCallback(SapXferCallbackInfo *pInfo)
{
    SapView *pView= (SapView *) pInfo->GetContext();

    // Resfresh view
    pView->Show();
}

static BOOL GetOptions(int argc, char *argv[], char *acqServerName, UINT32 *pAcqDeviceIndex, char *configFileName)
{
   // Check if arguments were passed
   if (argc > 1)
      return GetOptionsFromCommandLine(argc, argv, acqServerName, pAcqDeviceIndex, configFileName);
   else
      return GetOptionsFromQuestions(acqServerName, pAcqDeviceIndex, configFileName);
}

static BOOL GetOptionsFromCommandLine(int argc, char *argv[], char *acqServerName, UINT32 *pAcqDeviceIndex, char *configFileName)
{
   // Check the command line for user commands
   if ((strcmp(argv[1], "/?") == 0) || (strcmp(argv[1], "-?") == 0))
   {
      // print help
      printf("Usage:\n");
      printf("GrabCPP [<acquisition server name> <acquisition device index> <config filename>]\n");
      return FALSE;
   }

   // Check if enough arguments were passed
   if (argc < 4)
   {
      printf("Invalid command line!\n");
      return FALSE;
   }

   // Validate server name
   if (SapManager::GetServerIndex(argv[1]) < 0)
   {
      printf("Invalid acquisition server name!\n");
      return FALSE;
   }

   // Does the server support acquisition?
   int deviceCount = SapManager::GetResourceCount(argv[1], SapManager::ResourceAcq);
   int cameraCount = SapManager::GetResourceCount(argv[1], SapManager::ResourceAcqDevice);

   if (deviceCount+cameraCount == 0)
   {
      printf("This server does not support acquisition!\n");
      return FALSE;
   }

   // Validate device index
   if (atoi(argv[2]) < 0 || atoi(argv[2]) >= deviceCount+cameraCount)
   {
      printf("Invalid acquisition device index!\n");
      return FALSE;
   }

    if (cameraCount==0)
    {
     // Verify that the specified config file exist
     OFSTRUCT of = {0};
     if (OpenFile(argv[3], &of, OF_EXIST) == HFILE_ERROR)
     {
          printf("The specified config file (%s) is invalid!\n", argv[3]);
          return FALSE;
     }
    }

   // Fill-in output variables
   CorStrncpy(acqServerName, argv[1], CORSERVER_MAX_STRLEN);
   *pAcqDeviceIndex = atoi(argv[2]);
    if (cameraCount==0)
   CorStrncpy(configFileName, argv[3], MAX_PATH);

   return TRUE;
}


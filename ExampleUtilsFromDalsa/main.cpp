// -----------------------------------------------------------------------------------------
// Sapera Find Camera Example
//
//		This program shows how to list all cameras detected when more
//		than one cameras are present, listing them by user name, serial number, model name or
//		server name.
//
// -----------------------------------------------------------------------------------------

// Disable deprecated function warnings with Visual Studio 2005
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(disable: 4995)
#endif

#include <iostream>
#include <string>
#include <sstream>

#include "stdio.h"
#include "conio.h"
#include "sapclassbasic.h"
#include "ExampleUtils.h"

#include <vector>

// Restore deprecated function warnings with Visual Studio 2005
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(default: 4995)
#endif


static BOOL GetStartOptionsFromQuestions();

int main(int argc, char* argv[])
{
   printf("Sapera Console Find Camera Example (C++ version)\n");

   // This prevents unnecessary unloading and reloading of the Sapera LT libraries
   // during execution of this program
   SapManager::Open();

   if (!GetStartOptionsFromQuestions())
   {
      printf("\nPress any key to terminate\n");
      CorGetch();
      SapManager::Close();
      return 0;
   }

   /*

   //
   //
   // If you uncomment this part, you will be able to change the user defined name of a camera.
   //
   //
   SapAcqDevice* m_pAcqDevice=NULL;
   char acqServerName[CORSERVER_MAX_STRLEN]="";
   UINT32	acqDeviceNumber=0;


   if (!GetCorAcqDeviceOptionsFromQuestions(acqServerName,&acqDeviceNumber))
   {
   printf("\nPress any key to terminate\n");
   CorGetch();
   return 0;
   }

   char fullName[STRING_LENGTH];
   std::cout << "Enter a new user defined name for this device:\n";
   std::cout << "(Maximum length for User Name is 15)\n";
   std::cout << ">>>";
   std::cin.get(fullName, 16);

   // Allocate acquisition object
   m_pAcqDevice = new SapAcqDevice(acqServerName, FALSE);  // uses camera default settings

   // Create resources for all objects
   if (!m_pAcqDevice->Create())
   goto FreeHandles;

   if (!m_pAcqDevice->SetFeatureValue("DeviceUserID", fullName))
   {
   printf("\nUser Name Setting failed.\n");
   printf("\nPress any key to terminate\n");
   CorGetch();
   return 0;
   }
   else
   printf("\nNew User Name has been changed.\n");


   FreeHandles:
   printf("Press any key to terminate\n");
   CorGetch();

   // Destroy acquisition device object
   if (m_pAcqDevice && *m_pAcqDevice && !m_pAcqDevice->Destroy()) return FALSE;

   // Delete all objects
   if (m_pAcqDevice)			delete m_pAcqDevice;

   return 0;

   //
   //
   // End of commented part.
   //
   //

   */

   SapManager::Close();
   return 1;
}


static BOOL GetStartOptionsFromQuestions()
{
   //////// Ask questions to user to select acquisition board/device and config file ////////

   // Get total number of boards in the system
   int serverCount = SapManager::GetServerCount();
   int GenieIndex = 0;
   BOOL serverFound = FALSE;
   char serverName[CORSERVER_MAX_STRLEN];
   SapManager::SetDisplayStatusMode(SapManager::StatusLog);
   if (serverCount == 0)
   {
      printf("No device found!\n");
      return FALSE;
   }

   for (int serverIndex = 0; serverIndex < serverCount; serverIndex++)
   {
      if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcqDevice) != 0)
         serverFound = TRUE;
   }

Find_Genie_Menu:
   GenieIndex = 0;


   // At least one server must be available
   if (!serverFound)
   {
      printf("No camera found!\n");
      return FALSE;
   }
   else
    {
         printf("\nShow me the camera(s) detected: Press 1 to 4 or 'd' to detect new servers or 'q' to quit.\n");

      printf("\n%d: %s\n", 1, "By User defined Name.");
      printf("%d: %s\n", 2, "By Serial Number.");
      printf("%d: %s\n", 3, "By Server Name.");
      printf("%d: %s\n", 4, "By Model Name.");

      printf("\n........................................\n");

      char key = (char)CorGetch();
      if (key != 0)
      {
         // quit
         if (key == 'q')
            return FALSE;

         // detect
         if (key == 'd')
         {
            printf("\nDetecting new servers... ");
            if (SapManager::DetectAllServers(SapManager::DetectServerAll))
            {
               // let time for the detection to execute
               Sleep(5000);

               // get the new server count
               serverCount = SapManager::GetServerCount();

               printf("complete.\n\n");
            }
            else
            {
               printf("failed.\n\n");
            }
            goto Find_Genie_Menu;
         }

         // 1-4
         int serverNum = key - '0'; // char-to-int conversion
         if ((serverNum >= 1) && (serverNum < 5))
         {
            switch(serverNum)
            {
            case 1:
               {
                  printf("\nCameras listed by User Defined Name:\n\n");

                  char userDefinedName[STRING_LENGTH];
                  for (int serverIndex = 0; serverIndex < serverCount; serverIndex++)
                  {
                     if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcqDevice) != 0)
                     {
                        SapManager::GetServerName(serverIndex, serverName, sizeof(serverName));

                        SapAcqDevice camera(serverName);
                        BOOL status = camera.Create();
                        if (status)
                        {
                           // Get user defined name
                           status = camera.GetFeatureValue("DeviceUserID", userDefinedName, sizeof(userDefinedName));
                           if (status)
                              printf("%d/ %s\n", GenieIndex+1, userDefinedName);
                           else
                              printf("%d/ %s\n", GenieIndex+1, "N/A");
                           GenieIndex++;
                        }

                        // Destroy acquisition device object
                        if (!camera.Destroy()) return FALSE;
                     }
                  }
                  break;
               }
            case 2:
               {
                  printf("\nCameras listed by Serial Number:\n\n");
                  char serialNumberName[STRING_LENGTH];
                  for (int serverIndex = 0; serverIndex < serverCount; serverIndex++)
                  {
                     if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcqDevice) != 0)
                     {
                        SapManager::GetServerName(serverIndex, serverName, sizeof(serverName));

                        SapAcqDevice camera(serverName);
                        BOOL status = camera.Create();
                        if (status)
                        {
                           // Get Serial Number Feature Value
                           status = camera.GetFeatureValue("DeviceID", serialNumberName, sizeof(serialNumberName));
                           if (status)
                              printf("%d/ %s\n", GenieIndex+1, serialNumberName);
                           else
                              printf("%d/ %s\n", GenieIndex+1, "N/A");
                           GenieIndex++;
                        }

                        // Destroy acquisition device object
                        if (!camera.Destroy()) return FALSE;

                     }
                  }

                  break;
               }
            case 3:
               {
                  printf("\nCameras listed by Server Name:\n\n");
                  for (int serverIndex = 0; serverIndex < serverCount; serverIndex++)
                  {
                     if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcqDevice) != 0)
                     {
                        // Get Server Name Value
                        SapManager::GetServerName(serverIndex, serverName, sizeof(serverName));
                        SapAcqDevice camera(serverName);
                        BOOL status = camera.Create();
                        if (status)
                        {
                           printf("%d/ %s\n", GenieIndex+1, serverName);
                           GenieIndex++;
                        }
                        // Destroy acquisition device object
                        if (!camera.Destroy()) return FALSE;
                     }
                  }
                  break;
               }
            case 4:
               {
                  printf("\nCameras listed by Model Name:\n\n");
                  char deviceModelName[STRING_LENGTH];
                  for (int serverIndex = 0; serverIndex < serverCount; serverIndex++)
                  {
                     if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcqDevice) != 0)
                     {
                        SapManager::GetServerName(serverIndex, serverName, sizeof(serverName));

                        SapAcqDevice camera(serverName);
                        BOOL status = camera.Create();
                        if (status)
                        {
                           // Get Model Name
                           status = camera.GetFeatureValue("DeviceModelName", deviceModelName, sizeof(deviceModelName));
                           if (status)
                              printf("%d/ %s\n", GenieIndex+1, deviceModelName);
                           else
                              printf("%d/ %s\n", GenieIndex+1, "N/A");
                           GenieIndex++;
                        }
                        // Destroy acquisition device object
                        if (!camera.Destroy()) return FALSE;
                     }
                  }
                  break;
               }
            }

            goto Find_Genie_Menu;
         }
         else
         {
            printf("Invalid selection!\n");
            goto Find_Genie_Menu;
         }
      }
      else
      {
         printf("Invalid selection!\n");
         goto Find_Genie_Menu;
      }
   }

   //printf("\n");
   //return TRUE;
}

// Credits: https://stackoverflow.com/questions/36959508/nvidia-graphics-driver-causing-noticeable-frame-stuttering
//  From answer: https://stackoverflow.com/a/37632948/11659763 (subGlitch)

#include <iostream>
#include <nvapi.h>
#include <NvApiDriverSettings.h>

#define returnIfError if(HasError(status)) return 1;
#define testPtr(ptr) if (ptr == nullptr) { std::cerr << "Failed to allocate pointer..." << std::endl; return 1; }

bool HasError(NvAPI_Status status)
{
    if (status == NVAPI_OK)
        return false;

    NvAPI_ShortString szDesc = { 0 };
    NvAPI_GetErrorMessage(status, szDesc);
    std::cerr << "NVAPI error: " << szDesc << std::endl;
    return true;
}


void SetNVUstring(NvAPI_UnicodeString& nvStr, const char* cStr)
{
    for (int i = 0; i < NVAPI_UNICODE_STRING_MAX; i++)
        nvStr[i] = 0;

    int i = 0;
    while (cStr[i] != 0 && i < NVAPI_UNICODE_STRING_MAX)
    {
        nvStr[i] = cStr[i];
        i++;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cerr << "Wrong number of argument, expected 3 got " << (argc - 1) << std::endl;
        std::cerr << "Usage: SetNvidiaSettings.exe \"ProfileName\" \"executable.exe\" \"FriendlyName\"" << std::endl;
        return 1;
    }
    const char* profileName = argv[1];
    const char* appName = argv[2];
    const char* appFriendlyName = argv[3];

    NvAPI_Status status;
    NvDRSSessionHandle hSession;

    status = NvAPI_Initialize();
    returnIfError;

    status = NvAPI_DRS_CreateSession(&hSession);
    returnIfError;

    status = NvAPI_DRS_LoadSettings(hSession);
    returnIfError;


    // Fill Profile Info
    NVDRS_PROFILE* profileInfo = (NVDRS_PROFILE*)calloc(1, sizeof(NVDRS_PROFILE));
    testPtr(profileInfo);
    profileInfo->version = NVDRS_PROFILE_VER;
    profileInfo->isPredefined = 0;
    SetNVUstring(profileInfo->profileName, profileName);

    // Create Profile
    NvDRSProfileHandle hProfile;
    status = NvAPI_DRS_CreateProfile(hSession, profileInfo, &hProfile);
    returnIfError;

    // Fill Application Info
    NVDRS_APPLICATION* app = (NVDRS_APPLICATION*)calloc(1, sizeof(NVDRS_APPLICATION));
    testPtr(app);
    app->version = NVDRS_APPLICATION_VER_V1;
    app->isPredefined = 0;
    SetNVUstring(app->appName, appName);
    SetNVUstring(app->userFriendlyName, appFriendlyName);
    SetNVUstring(app->launcher, "");
    SetNVUstring(app->fileInFolder, "");

    // Create Application
    status = NvAPI_DRS_CreateApplication(hSession, hProfile, app);
    returnIfError;


    // Fill Setting Info
    NVDRS_SETTING setting = { 0 };
    setting.version = NVDRS_SETTING_VER;
    setting.settingId = OGL_THREAD_CONTROL_ID;
    setting.settingType = NVDRS_DWORD_TYPE;
    setting.settingLocation = NVDRS_CURRENT_PROFILE_LOCATION;
    setting.isCurrentPredefined = 0;
    setting.isPredefinedValid = 0;
    setting.u32CurrentValue = OGL_THREAD_CONTROL_DISABLE;
    setting.u32PredefinedValue = OGL_THREAD_CONTROL_DISABLE;

    // Set Setting
    status = NvAPI_DRS_SetSetting(hSession, hProfile, &setting);
    returnIfError;


    // Apply (or save) our changes to the system
    status = NvAPI_DRS_SaveSettings(hSession);
    returnIfError;


    std::cout << "Success!" << std::endl;

    NvAPI_DRS_DestroySession(hSession);

    free(profileInfo);
    free(app);
    return 0;
}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Astuces pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.

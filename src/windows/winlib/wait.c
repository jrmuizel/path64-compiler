/*
 * Copyright (C) 2010. PathScale Inc. All Rights Reserved.
 */

/*

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

*/

#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <windows.h>
#include <tlhelp32.h>

#define MAX_HANDLES 100

DWORD WINAPI GetProcessId(HANDLE Process);

pid_t wait(int *stat_loc)
{
    HANDLE hSnapShot, ahChildren[MAX_HANDLES];
    PROCESSENTRY32 pe32;
    DWORD Status, dwCurrentProcessId, dwPid, dwExitCode;
    ULONG i, cChildren = 0;

    hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hSnapShot == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "CreateToolhelp32Snapshot failed");
        return -1;
    }

    pe32.dwSize = sizeof(pe32);
    if(!Process32First(hSnapShot, &pe32))
    {
        fprintf(stderr, "Process32First failed");
        CloseHandle(hSnapShot);
        return -1;
    }

    /* Get the ID of the current (parent) process */
    dwCurrentProcessId = GetCurrentProcessId();

    do
    {
        /* Check if the process is a child of the current process */
        if (pe32.th32ParentProcessID == dwCurrentProcessId)
        {
            ahChildren[cChildren] = OpenProcess(READ_CONTROL,
                                                FALSE,
                                                pe32.th32ProcessID);

            if (ahChildren[cChildren] != NULL) cChildren++;
            if (cChildren == MAX_HANDLES) break;
        }
    }
    while (Process32Next(hSnapShot, &pe32));

    /* Close the snapshot */
    CloseHandle(hSnapShot);

    /* Wait for any of the enumerated child processes */
    Status = WaitForMultipleObjects(cChildren, ahChildren, FALSE, INFINITE);

    /* Check for success */
    if (Status >= WAIT_OBJECT_0 && Status <= WAIT_OBJECT_0 + cChildren - 1)
    {
        /* Get the process ID and exit code */
        dwPid = GetProcessId(ahChildren[Status - WAIT_OBJECT_0]);
        GetExitCodeProcess(ahChildren[Status - WAIT_OBJECT_0], &dwExitCode);
    }
    else
    {
        dwPid = -1;
        dwExitCode = 0;
    }

    /* Close all child process handles */
    for (i = 0; i < cChildren; i++)
    {
        CloseHandle(ahChildren[i]);
    }

    /* Return the process ID */
    if (stat_loc) *stat_loc = dwExitCode;
    return dwPid;
}

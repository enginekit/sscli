// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==

#ifndef __HOSTIMPL_H__
#define __HOSTIMPL_H__

#ifdef SELF_NO_HOST
extern HANDLE g_ExecutableHeapHandle;
#endif

// We have an internal class that is used to make sure the hosting api
// is forwarded to the os. This is a must for the shim because mscorwks
// which normally contains the implementation of the hosting api has not 
// been loaded yet. In fact the shim is the one component responsible 
// for that loading
class UtilExecutionEngine : public IExecutionEngine, public IEEMemoryManager
{
private:

    //***************************************************************************
    // IUnknown methods
    //***************************************************************************

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID id, void **pInterface);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    //***************************************************************************
    // IExecutionEngine methods for TLS
    //***************************************************************************

    // Associate a callback for cleanup with a TLS slot
    VOID  STDMETHODCALLTYPE TLS_AssociateCallback(DWORD slot, PTLS_CALLBACK_FUNCTION callback);
    // Get the master TLS slot index
    DWORD STDMETHODCALLTYPE TLS_GetMasterSlotIndex();

    // Get the value at a slot
    LPVOID STDMETHODCALLTYPE TLS_GetValue(DWORD slot);
    
    // Get the value at a slot, return FALSE if TLS info block doesn't exist
    BOOL STDMETHODCALLTYPE TLS_CheckValue(DWORD slot, LPVOID * pValue);
    // Set the value at a slot
    VOID STDMETHODCALLTYPE TLS_SetValue(DWORD slot, LPVOID pData);
    // Free TLS memory block and make callback
    VOID STDMETHODCALLTYPE TLS_ThreadDetaching();

    //***************************************************************************
    // IExecutionEngine methods for locking
    //***************************************************************************

    CRITSEC_COOKIE STDMETHODCALLTYPE CreateLock(LPCSTR szTag, LPCSTR level, CrstFlags flags);
    void STDMETHODCALLTYPE DestroyLock(CRITSEC_COOKIE lock);
    void STDMETHODCALLTYPE AcquireLock(CRITSEC_COOKIE lock);
    void STDMETHODCALLTYPE ReleaseLock(CRITSEC_COOKIE lock);
    
    EVENT_COOKIE STDMETHODCALLTYPE CreateAutoEvent(BOOL bInitialState);
    EVENT_COOKIE STDMETHODCALLTYPE CreateManualEvent(BOOL bInitialState);
    void STDMETHODCALLTYPE CloseEvent(EVENT_COOKIE event);
    BOOL STDMETHODCALLTYPE ClrSetEvent(EVENT_COOKIE event);
    BOOL STDMETHODCALLTYPE ClrResetEvent(EVENT_COOKIE event);
    DWORD STDMETHODCALLTYPE WaitForEvent(EVENT_COOKIE event, DWORD dwMilliseconds, BOOL bAlertable);
    DWORD STDMETHODCALLTYPE WaitForSingleObject(HANDLE handle, DWORD dwMilliseconds);

    SEMAPHORE_COOKIE STDMETHODCALLTYPE ClrCreateSemaphore(DWORD dwInitial, DWORD dwMax);
    void STDMETHODCALLTYPE ClrCloseSemaphore(SEMAPHORE_COOKIE semaphore);
    DWORD STDMETHODCALLTYPE ClrWaitForSemaphore(SEMAPHORE_COOKIE semaphore, DWORD dwMilliseconds, BOOL bAlertable);
    BOOL STDMETHODCALLTYPE ClrReleaseSemaphore(SEMAPHORE_COOKIE semaphore, LONG lReleaseCount, LONG *lpPreviousCount);

    MUTEX_COOKIE STDMETHODCALLTYPE ClrCreateMutex(LPSECURITY_ATTRIBUTES lpMutexAttributes,
                                                  BOOL bInitialOwner,
                                                  LPCTSTR lpName);
    void STDMETHODCALLTYPE ClrCloseMutex(MUTEX_COOKIE mutex);
    BOOL STDMETHODCALLTYPE ClrReleaseMutex(MUTEX_COOKIE mutex);
    DWORD STDMETHODCALLTYPE ClrWaitForMutex(MUTEX_COOKIE mutex,
                                            DWORD dwMilliseconds,
                                            BOOL bAlertable);

    DWORD STDMETHODCALLTYPE ClrSleepEx(DWORD dwMilliseconds, BOOL bAlertable);

    BOOL STDMETHODCALLTYPE ClrAllocationDisallowed();

    void STDMETHODCALLTYPE GetLastThrownObjectExceptionFromThread(void **ppvException);

    //***************************************************************************
    // IEEMemoryManager methods for locking
    //***************************************************************************
    LPVOID STDMETHODCALLTYPE ClrVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
    BOOL STDMETHODCALLTYPE ClrVirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);
    SIZE_T STDMETHODCALLTYPE ClrVirtualQuery(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength);
    BOOL STDMETHODCALLTYPE ClrVirtualProtect(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);
    HANDLE STDMETHODCALLTYPE ClrGetProcessHeap();
    HANDLE STDMETHODCALLTYPE ClrHeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize);
    BOOL STDMETHODCALLTYPE ClrHeapDestroy(HANDLE hHeap);
    LPVOID STDMETHODCALLTYPE ClrHeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
    BOOL STDMETHODCALLTYPE ClrHeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
    BOOL STDMETHODCALLTYPE ClrHeapValidate(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem);
    HANDLE STDMETHODCALLTYPE ClrGetProcessExecutableHeap();
    
};

#endif


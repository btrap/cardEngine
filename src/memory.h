#pragma once

#include "variable_defines.h"
#include "system_defines.h"

#include <cstdlib> // malloc, realloc, calloc, free

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <memoryapi.h> // win32 VirtualAlloc
#undef far
#undef near
#undef pascal

#ifdef DEBUG_ACTIVE
struct DEBUG_Memory_Data
{
    umax permanent_Used;
};
DEBUG_Memory_Data debug_Memory_Data = {0};
#endif

struct Memory_Pool
{
    umax storage_Size;
    void* storage_Current;
    void* storage_Start;
};
Memory_Pool temp_memory = {0}; // temp data
Memory_Pool permanent_memory = {0}; // static data

inline void SetupMemoryPools()
{
    temp_memory.storage_Size = MEGABYTES(1);
    permanent_memory.storage_Size = MEGABYTES(16);

    LPVOID base_Address = 0;
#ifdef DEBUG_ACTIVE
    base_Address = (LPVOID)TERABYTES(2);
#endif
    permanent_memory.storage_Start = VirtualAlloc(base_Address, permanent_memory.storage_Size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    permanent_memory.storage_Current = permanent_memory.storage_Start;

#ifdef DEBUG_ACTIVE
    base_Address = (LPVOID)TERABYTES(3);
#endif
    temp_memory.storage_Start = VirtualAlloc(base_Address, temp_memory.storage_Size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    temp_memory.storage_Current = temp_memory.storage_Start;
}

inline void ClearMemoryPools()
{
    VirtualFree(permanent_memory.storage_Start, 0, MEM_RELEASE);
    VirtualFree(temp_memory.storage_Start, 0, MEM_RELEASE);
}

inline void StoreData_(void **data, umax sizeof_Data, void **memory)
{
    *data = *memory;
    *memory = (void*)((char*)*memory + sizeof_Data);
}
#define STORE_DATA(data, count, memory) StoreData_( (void **)(&data), ( (count) * sizeof(*data) ), (&memory) );

inline void StorePermanentData_(void **data, umax sizeof_Data)
{
#ifdef DEBUG_ACTIVE
    ASSERT((debug_Memory_Data.permanent_Used + sizeof_Data) < permanent_memory.storage_Size);
    debug_Memory_Data.permanent_Used += sizeof_Data;
#endif
    //ASSERT( (permanent_memory.storage_Current + sizeof_Data) < (permanent_memory.storage_Start + permanent_memory.storage_Size) );

    *data = permanent_memory.storage_Current;
    permanent_memory.storage_Current = (void*)((char*)permanent_memory.storage_Current + sizeof_Data);
}
#define STORE_PERM_DATA(data, count) StorePermanentData_( (void **)(&data), ( (count) * sizeof(*data) ));

inline void StoreTempData_(void **data, umax sizeof_Data)
{
    ASSERT(sizeof_Data < temp_memory.storage_Size);

    *data = temp_memory.storage_Current;
}
#define STORE_TEMP_DATA(data, count) StoreTempData_( (void **)(&data), ( (count) * sizeof(*data) ));


// dyamic data
inline umax GetData_(void **data, umax sizeof_Data)
{
    umax result = 0;

    char* new_Data = (char*)malloc((size_t)(sizeof_Data));
    if(new_Data)
    {
        result = sizeof_Data;
        *data = new_Data;
    }
    else if(*data)
    {
        // TODO handle memory issues
        PRINT_CONSOLE("malloc failed in GrowData_\n");
        ASSERT_NOW();
    }

    return result;
}
#define GET_DATA(data, count) GetData_( (void **)(&data), ( (count) * sizeof(*data) ) );

inline umax GetClearData_(void **data, umax count, umax sizeof_Data)
{
    umax result = 0;

    char* new_Data = (char*)calloc(count, (size_t)(sizeof_Data));
    if(new_Data)
    {
        result = sizeof_Data;
        *data = new_Data;
    }
    else if(*data)
    {
        // TODO handle memory issues
        PRINT_CONSOLE("malloc failed in GrowData_\n");
        ASSERT_NOW();
    }

    return result;
}
#define GET_CLEAR_DATA(data, count) GetClearData_( (void **)(&data), (count), ( (count) * sizeof(*data) ) );


inline umax GrowData_(void **data, umax sizeof_Data)
{
    umax result = 0;

    char* new_Data = (char*)realloc(*data, (size_t)(sizeof_Data));
    if(new_Data)
    {
        result = sizeof_Data;
        *data = new_Data;
    }
    else if(*data)
    {
        // TODO handle memory issues
        PRINT_CONSOLE("realloc failed in GrowData_\n");
        ASSERT_NOW();
    }

    return result;
}
#define GROW_DATA(data, count) GrowData_( (void **)(&data), ( (count) * sizeof(*data) ) );

inline void FreeData_(void **data)
{
    free(data);
    *data = 0;
}
#define FREE_DATA(data, count) FreeData_( (void **)(&data) );

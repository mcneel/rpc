// This header provides an interface for a client application
// to supply the memory allocation routines for the RPC API.
// Use GetProcAddress() after loading RPCapi.dll to get
// the function specified here.  Be sure to call this function
// before creating your RPCapi object.

#ifndef _RPC_API_MEM_H_
#define _RPC_API_MEM_H_


// This is the typedef describing a memory allocation function.
typedef void *(*RPCmemoryAllocationFuncType)
		(size_t size, const char *file, int line);

// This is the typedef describing a memory deallocation function.
typedef void (*RPCmemoryDeallocationFuncType)
		(void *ptr, const char *file, int line);

// This is the typedef describing the function exported by RPCapi.dll
// to receive memory allocation functions from a client application.
// Use GetProcAddress(), or equivalent, to get this function.
typedef void (*RPCsetMemoryAllocationFuncType)
		(RPCmemoryAllocationFuncType ma,
		RPCmemoryDeallocationFuncType da);
extern "C" void RPCsetMemoryAllocation(
		RPCmemoryAllocationFuncType ma,
		RPCmemoryDeallocationFuncType da);

#endif	// ifndef _RPC_API_MEM_H_

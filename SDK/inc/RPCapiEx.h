// This header file exists to provide multilanguage functionality
// for the RPC API.  An RPCapi object (from RPCapi.h) can be
// nwm Create a new wrapper class RPCapi2

// dynamically cast to an RPCapiML object in order to
// access this functionality.

#ifndef RPC_API_CLIENT_INTERFACE_H
#define RPC_API_CLIENT_INTERFACE_H

#include "RPCapi.h"


class RPCapiEx {
public:
	class ClientEx {
	public:
		virtual ~ClientEx(void) {}
		// This is a catch-all routine that allows the API to
		// inform the client about various events.
		// Parameters:
		//		event:	The event that occurred.  Possible values:
		//				6:  a new RPC ID was added to the global
		//				    selection table as a result of automatic
		//					repair during RPCapi::Instance::fromStream
		//					or RPCapi::Instance::setRPCFilename.
		//				    args:  const RPCapiEx::IDex *newId
		//				7:  RPCapi wants to display a modal window,
		//				    and needs a parent HWND.
		//					args:  HWND *phwnd
		//		...:	Additional arguments, dependent on event,
		//				as documented above.
		// Returns:
		//		Dependent on the event, above.
		virtual void *RPCnotify(int event, ...) = 0;
	};	// class ClientEx


	#ifdef RPC_CONTENT_CLIENT
	class ClientInterface;
	class ClientInterface : public RPCapi::BaseObject
	{
	public:
		// This is the dummy destructor for this interface.
		virtual ~ClientInterface(void) {}

		// The RpcApiClientInterface::Window class encapsulates all of the
		// different window type codes.
		// To use in your code, say
		// "RpcApiClientInterface::Window::ACM_CONFIGURATION", 
		// or whatever window type is appropriate.
		class Window {
		public:
			typedef enum WINDOW_CODE {
				NONE = -1,
				ACM_CONFIGURATION = 1,	// The dialog that allows the user to select
										// the ACM that this client points to
			} WINDOW_CODE_T;	// end typedef enum WINDOW_CODE
		};	// end class RPCapi::InstanceInterface::Window

		#ifdef PORT_WINDOWS
			// This routine displays the specified dialog to the screen.
			// Parameters:
			//		parentWindow:		handle to the parent window.
			//		windowToDisplay:	the indicator of which window to display
			//							see UserInterface::Window for options
			virtual int show(HWND parentWindow, int windowToDisplay) = 0;
		#endif	// ifdef PORT_WINDOWS

		#ifdef PORT_MAC
			// This routine displays the specified dialog to the screen.
			// Parameters:
			//		parentWindow:		handle to the parent window.
			//		windowToDisplay:	the indicator of which window to display
			//							see UserInterface::Window for options
			virtual int show(WindowRef parentWindow, int windowToDisplay) = 0;
		#endif // ifdef PORT_MAC

		// This routine hides this dialog.
		virtual int hide(void) = 0;

		// This routine sets the Client which this ClientInterface edits.
		// Parameters:
		//		client:	The RPCapi::Client for this Interface.
		virtual void setClient(RPCapi::Client *client) = 0;

		// The following methods will be used to set user interface attributes
		// however they are not currently supported
		virtual int setBackgroundColor(int color) = 0;
		virtual int setForegroundColor(int color) = 0;
		virtual int setWidth(int newWidth) = 0;
		virtual int setHeight(int newHeight) = 0;

	};	// end class ClientInterface
	#endif	// ifdef RPC_CONTENT_CLIENT


	// This extends the interface for the unique IDs assigned to each RPC file
	// by introducing directy string conversion routines.
	class IDex : public RPCapi::ID {
	public:
		typedef enum TYPE_CODE_TYPE {
			TYPE_CODE = 18
		} TYPE_CODE_T;
		virtual ~IDex(void) {}

		// This routine reads this from a human-readable string.
		// Parameters:
		//		str:	The string to read.
		virtual void fromString(RPCapi::TStringArg str) = 0;

		// This routine converts this into a newly-allocated string.
		// Parameters:
		//		buffer:	The buffer to copy this to (not deleted).
		//		offset:	Offset to write to in buffer,
		//				returned as offset after end of written portion.
		// Returns:
		//		A string representing this, or the number of bytes
		//		printed to buffer.
		virtual RPCapi::TString *toString(void) const = 0;

	};	// class IDex


	class InstanceInterfaceEx : public RPCapi::InstanceInterface {
	public:

		class WindowEx : public RPCapi::InstanceInterface::Window
		{
		public:
			typedef enum MODE_CODE {
				MODELESS = 1,		// Shows window inside parent
				FLOATING = 2,		// Shows window in its own border
				MODAL = 3			// Modal dialog
			} MODE_CODE_T;
		};

        using RPCapi::InstanceInterface::show;

		virtual int show(HWND parentWindow, int windowToDisplay, int mode,
			int x = 0, int y = 0, int w = 0, int h = 0, HWND z = NULL) = 0;
	};  // class InstanceInterfaceEx


	// This routine determines if the API will
	// automatically attempt to acquire content
	// when the first RPCapi::Instance is created.
	// (By default, it will).
	// Parameters:
	//		a:	If true, auto-connect.
	// Returns:
	//		The previous value.
	virtual bool acmAutoConnect(bool a) = 0;

	// This routine gets the ACM ID that this API last connected to.
	// Parameters:
	//		acmName:	If not NULL, assigned a newly-allocated
	//					string containing the mathcing ACM name.
	virtual const RPCapi::ID *acmIDget(RPCapi::TString **acmName = NULL) = 0;

	// This routine performs an import by file across the network
	// Parameters:
	//		importDataBuffer:		The "file" to import
	//		importDataBufferSize:	The size of the import data buffer
	//		notify:					Show status messages to the user
	// Returns:
	//		int:				0 on success
	//							Error code on failure
	virtual int networkImportPerform(const unsigned char *importDataBuffer, unsigned int importDataBufferSize, bool notify = false) = 0;

	// This routine causes this client to send a request to
	// its ACM for some reason.
	// Parameters:
	//		serverId:	Identifies the web server to
	//					send the request to.  If NULL,
	//					the request is sent to ArchVision.
	//		request:	The request to send.
	//		errmsg:		If not NULL, this will point to
	//					a newly-allocated error message
	//					(if an error occurred).
	// Returns:
	//		1 on success, or an error code.
	virtual int acmSendRequest(RPCapi::TStringArg serverId,
		RPCapi::TStringArg request, RPCapi::TString **errmsg) = 0;

	// This routine pings an ACM to determine if it exists in the currently
	// configured location.
	// Returns:
	//		true on success, false on failure
	virtual bool acmPing(void) = 0;

	// This routine scans for ACMs on the local network.
	// All returned objects are newly-allocated, and should
	// be deleted by the caller.
	// Parameters:
	//		names:	Nicknames of the matching ACMs.
	//		ids:	IDs of the matching ACMs.
	//		hosts:	Hostnames of the matching ACMs.
	//		ports:	Ports the matching ACMs are running on.
	// Returns:
	//		The number of matches, which is the size
	//		of the returned arrays, or an error code.
	virtual int acmScan(RPCapi::TString **&names, RPCapi::ID **&ids,
		RPCapi::TString **&hosts, short *&ports) const = 0;

	// These functions get this modules new and compare params functions,
	// or (un)register additional new and compare functions.
	typedef RPCapi::Param *(*NewFunc)(int typeCode);
	virtual NewFunc newFuncGet(void) = 0;
	virtual void newFuncRegister(NewFunc nf, bool reg) = 0;
	typedef int (*CompareFunc)(const RPCapi::Param *param1,
		const RPCapi::Param *param2);
	virtual CompareFunc compFuncGet(void) = 0;
	virtual void compFuncRegister(CompareFunc cf, bool reg) = 0;

	// This is used to force a reread of the ini file 
	virtual void contentClientIniRead(void) = 0;

	// This routine retrieves the module handle for RPCapi.dll.
	virtual HMODULE getModuleHandle(void) const = 0;
};

/*! \function int getMaterials(Material**& materials) const
*   \brief Retrieves the instance's materials.
*
* This routine retrieves the materials associated with this Instance.
* Parameters:
*       materials:      Array of materials, newly allocated and returned by reference.
* Returns:
*       Number of materials
*/
int RPCgetMaterials(const RPCapi::Instance* rpcInstance, RPCapi::Material*** materials);

// Returns true, if RPC is licensed by ACM
bool RPCisLicensed(const RPCapi::Instance* rpcInstance);

#endif	// ifndef RPC_API_CLIENT_INTERFACE_H

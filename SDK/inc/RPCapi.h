// This is the RPC interface.  This header file (and its associated
// .lib and .dll files) are used by applications / plugins to
// read data from RPC files.
// The RPC API definitions are below; search for "class RPCapi"
// NOTE:  Be sure to correctly define which PORT this file is being
// compiled in.  This is particularly important in distinguishing
// Windows static compiles from Windows DLL imports
// NOTE:  If you are using RPCapi as a static library rather than
//        as a dynamic library, be sure to define RPC_STATIC_LIB
//        before including RPCapi.h.


// ///////////////////////////////////////////////////////////////// //
//                 NOTE    NOTE    NOTE    NOTE    NOTE              //
//                                                                   //
//   -To properly use the RPCapi dll, your application MUST use      //
//   the Multithreaded run-time DLL.                                 //
//   In Visual C++, go to  Project->Settings                         //
//   On C/C++ tab, in the Category tab box, select Code Generation.  //
//   In the User run-time library tab box, select Multithreaded DLL  //
//   or Debug Multithreaded DLL (if the latter is chosen, be sure    //
//   you are linking with a DEBUG version of the RPCapi dll          //
//																	 //
//	 -Your application must also support Run-Time Type Information	 //
//	 or (RTTI).														 //
//	 To enable this in Visual C++ go to Project->Settings			 //
//	 On C/C++ tab, in the Category tab box, select C++ Language.	 //
//	 Select Enable Run-Time Type Information(RTTI).					 //
// ///////////////////////////////////////////////////////////////// //

#ifndef RPCAPI_H
#define RPCAPI_H


// This tells RPCapiInternal to get its content from a CLS
#define RPC_CONTENT_CLIENT
#define IMPLEMENT_THUMBNAIL_BROWSER
#define RPC_API_ML	// Multilanguage support
#define RPC_ACM_BULLETIN
#define DASHBOARD_LINK //include dashboard link beside project combo box

// *********************************************************************** //
//             BEGIN   PLATFORM SPECIFIC DEFINITIONS SECTION               //
// *********************************************************************** //

// Define the OS and build here.
#ifdef WIN32
#define PORT_WINDOWS
#endif  // ifdef WIN32
#ifdef linux
#define PORT_LINUX
#define PORT_UNIX
#define PORT_MAC_OR_UNIX
#endif  // ifdef linux
#ifdef sgi
#define PORT_IRIX
#define PORT_UNIX
#define PORT_MAC_OR_UNIX
#endif  // ifdef sgi
#ifdef macintosh
#define PORT_MAC
#define PORT_MAC_OR_UNIX
#endif	// ifdef macintosh


// Select the supported and default character widths here.
#ifdef PORT_WINDOWS
#ifndef _UNICODE
#define _UNICODE
#endif	// ifndef _UNICODE
#ifdef RPCAPI_SRC
#define UNICODE
#endif	// ifdef RPCAPI_SRC
#endif	// ifdef PORT_WINDOWS
#ifdef _UNICODE
#include <wchar.h>
#endif	// ifdef _UNICODE


#ifdef PORT_WINDOWS
#ifdef RPCAPI_SRC
#define _WINSOCKAPI_
#endif	// ifdef RPCAPI_SRC
#ifndef USE_MF
#include "windows.h"
#endif	// ifndef USE_MFC
#endif	// ifdef PORT_WINDOWS


// We must select stdlibc++ include files based on our compiler version:
#if (__GNUC__ >= 3)
#include <iostream>
#define RPC_STL3
//  Until the stream problems can be worked out with the STL3 build use
//  the old STL libraries
#elif (defined (_MSC_VER) && _MSC_VER >= 1310) // Visual Studio .NET 2003 or later
#include <iostream>
#define RPC_STL3
#else
#include <iostream.h>
#endif	// if (__GNUC__ >= 3)


#ifdef PORT_MAC
#include <QuickDraw.h>
// CodeWarrior / Mac made verify a keyword.  Weird.
#undef verify
#define RPC_STL3
#endif	// ifdef PORT_MAC


// *********************************************************************** //
//                  BEGIN   RPC    API   SECTION                           //
// *********************************************************************** //


/*! \class RPCapi
*	\brief This class represents the entire RPC API
*
*	This class represents the entire RPC API.  Get a single instance of
*	this class using the RPCgetAPI function (defined below), store it statically,
*	and let it live for the duration of an execution of your program.
*/
class RPCapi {
public:

	// These are the interfaces defined by the RPCapi.  It is the
	// job of the client code developer to implement these interfaces.
	class Client;			// The client-code counterpart to class RPCapi.
	// The client code should define this class and
	// create a single instantiation of it, and pass
	// a pointer of that instantiation to the global
	// RPCapi object.
	class ClientInstance;	// The client-code counterpart to RPCapi::Instance
	// (see class Instance, below).

	// These are the classes exported by the RPCapi:
	class Param;				// Base class of all parameters of an RPC.
	class Checksum;				// A checksum exists in every RPC file.
	class Content;				// Wrapper class for the content type codes
	class ContentMgr;			// manages the content for this RPCapi.dll
	class Creator;				// Used to create an RPC.  Only usable if
	// the API has a Creator License.
	class Error;				// Wrapper class for the error codes in RPCapi
	class ID;					// Unique RPC identifier.  All Instances of
	// an RPC share the same ID, but different
	// RPC files have different IDs.
	class Instance;				// Single instance of an RPC in a scene.
	class InstanceInterface;	// Used to edit a single Instance
	class License;				// Represents the licenses of the RPCs which
	// the client app and this API can read
	class MassEditInterface;	// Used to edit many Instances at once
	class Mesh;					// Geometric mesh.  Contains vertices and faces.
	template<class Data>
	class Prim;				// Class for primitives (int,char,double, ...)
	class ParamList;			// All RPC files are are ParamLists
	// and contain sub ParamLists.
	class PrintOptions;			// Used to store options for printing when
	// print is called.
	class String;				// Encapsulating class for strings.
	class Texture;				// The class which encapsulates all image
	// data retrieved from an Instance.
	class TextureMesh;			// Texture mesh.  Contains texture vertices
	// and texture faces.
	class Vector;				// A 3D vector, with x, y, and z components.



	/*! \class ObjectCodes
	*	\brief Param codes for classes in RPCapi
	*
	* These are the param codes for all classes in the RPCapi which
	* can be retrieved by the client code using RPCapi::newObject:
	* NOTE:  These are NOT necessarily the same as the type codes
	* returned by RPCapi::Param::typeCode().
	*
	* To create a new, empty Instance, for example, use
	*	RPCapi::Instance *newInstance =
	*	RPCapi::newObject(RPCapi::ObjectCodes::INSTANCE);
	*/
	class ObjectCodes {
	public:
		typedef enum OBJECT_CODE {
			PRIM_BOOL = 1,
			PRIM_CHAR = 2,
			PRIM_UCHAR = 3,
			PRIM_SHORT = 4,
			PRIM_USHORT = 5,
			PRIM_INT = 6,
			PRIM_UINT = 7,
			PRIM_FLOAT = 8,
			PRIM_DOUBLE = 9,
			CHECKSUM = 10,
			CREATOR = 11,	// only good in Creator-licensed API
			ID = 12,
			INSTANCE = 13,
			INSTANCE_INTERFACE = 14,
			LICENSE = 15,
			MASS_EDIT_INTERFACE = 16,
			PARAM_LIST = 17,	// stream-accessing param list
			PRINT_OPTIONS = 18,
			STRING = 19,
			VECTOR = 20,
			CLIENT_INTERFACE = 21
		} OBJECT_CODE_T;	// end typedef enum OBJECT_CODE
	};	// end class ObjectCodes


	/*! \class BaseObject
	*	\brief Base class for all classes of the RPC API
	*
	*
	*  This is the base class for all classes of the RPC API.
	*
	*/

	class BaseObject {
	public:

		/*! \function ~BaseObject(void).
		*	\brief Destructor
		*
		* This is a dummy destructor; C++ compilers like to have
		* destructors for all classes, even abstract classes.
		*/
		virtual ~BaseObject(void) {}
	};	// end class BaseObject





	class TString;

	/*! \class Param
	*   \brief Param of BaseObject
	*
	*  An RPC contains many different objects, including its ID,
	* geometric meshes, textures, and primitives.  All of these objects
	* are subclasses of Param.  When a Param is retrieved, you can use
	* either its typecode or the C++ dynamic_cast operator to determine
	* if it is of the desired type.
	*
	*/
	class Param : public BaseObject {
	public:
		/*! \function virtual ~Param(void)
		*	\brief Destructor.
		*
		* This is a dummy destructor; C++ compilers like to have
		* destructors for all classes, even abstract classes.
		*/
		virtual ~Param(void) {}

		/*! \function virtual const TString &className(void)
		*	\brief Gets class name of object.
		*
		* This routine returns the class name of this object as a string.
		* Returns:
		*		The class name of this object.
		*/
		virtual const TString &className(void) const = 0;

		/*! \function virtual Param *copy(void)
		*	\brief
		* This routine copies this Param object into a new object with
		* the same value.
		* Return:
		*		A pointer to the new Param object.
		*/
		virtual Param *copy(void) const = 0;

		/*! \function virtual bool fromCreator(Creator *creator)
		*	\brief Reads params from a creator.
		*
		* This routine reads this Param's value from the stream provided by
		* the input creator.  That stream is in a .fmt format, rather than a
		* binary format.
		* Parameters:
		*		creator:	The Creator object to use to create this Param.
		* Returns:
		*		true on success, false on failure.
		*/
		virtual bool fromCreator(Creator *creator) = 0;

		/*! \function virtual int fromStream(istream &stream, int offset, int typeCode)
		*	\brief Converts stream to object data.
		*
		* This routine converts the input stream to the data of this object.
		* Parameters:
		*		stream:		stream from which to extract this object.
		*		offset:		offset in stream to begin reading this object.
		*		typeCode:	the typeCode that was read for this object.
		*					This dictates how this object unserializes itself.
		*					If typeCode is negative, this object unserializes
		*					itself in a default fashion.
		* Return:
		*		The number of bytes read, or an error code (which is < 0).
		*/
#ifndef RPC_STL3
		virtual int fromStream(istream &stream, int offset, int typeCode) = 0;
#else
		virtual int fromStream(std::istream &stream, int offset, int typeCode) = 0;
#endif

		/*! \function virtual void print(ostream &os = cout, int tab = 0, PrintOptions *options = NULL)
		*	\brief Prints Param
		*
		* This routine outputs this Param in a human-readable format.
		* For Param's which have no simple human-readable format (such as
		* Textures), the class name of the object is printed.
		* Parameters:
		*		os:		The stream to print to.
		*		tab:	The number of times to indent.
		*		options:	The options for printing.
		*/
#ifndef RPC_STL3
		virtual void print(ostream &os = cout, int tab = 0,
			PrintOptions *options = NULL) const = 0;
#else
		virtual void print(std::ostream &os = std::cout, int tab = 0,
			PrintOptions *options = NULL) const = 0;
#endif

		/*! \function virtual int size(void)
		*	\brief Gets size of serial object.
		*
		* This routine determines how many bytes this object will
		* require to serialize itself.
		* Return:
		*		The number of bytes required to serialize this object,
		*		or an error code (which is < 0).
		*/
		virtual int size(void) const = 0;

		/*! \function virtual int toStream(ostream &stream, int offset)
		*	\brief Writes object to a stream.
		*
		* This routine writes this object's bytes to a stream.
		* Parameters:
		*		stream:		The stream to which this object should be written.
		*		offset:		offset in stream to begin writing this object
		* Return:
		*		The number of bytes written, or an error code (which is < 0).
		*/
#ifndef RPC_STL3
		virtual int toStream(ostream &stream, int offset) const = 0;
#else
		virtual int toStream(std::ostream &stream, int offset) const = 0;
#endif

		/*! \function virtual int typeCode(void)
		*	\brief Gets type code of the object.
		*
		* This routine returns the type code of this object.
		* Return:
		*		The unique integer type code of this object's class.
		*/
		virtual int typeCode(void) const = 0;

	};	// end class RPCapi::Param


	/*! \class TString
	*	\brief Wraps string arguments that are passed to/from RPCapi functions/
	*
	* This class exists to wrap string arguments that are passed
	* to / from RPCapi functions.  It provides functionality for
	* multiple character widths.
	*
	* Client programs are responsible for deallocating any non-const
	* TStrings that the RPCapi returns to them.
	*
	* RPCapi functions take as input TStringArgs that can be constructed
	* from character strings.
	*
	* NOTE on ellipsis (...) operator:
	* When passing a TLabel or TString reference to an ellipsis operator,
	* use the TSA macro.
	* When retrieving a TLabel or TString argument, use the TSAU macro.
	* This avoids nasty type problems (like the fact that a TString reference
	* is not the same size as a TLabel).
	*
	*/
#ifdef _UNICODE
#define RPC_WIDE_STRINGS
#ifdef UNICODE
#define RPC_WIDE_DISPLAY
#endif	// ifdef UNICODE
#endif	// ifdef _UNICODE
	class TStringArg;
	class TString : public Param {
	public:
		// These are the possible character types for this class.
		// Ensure that a character width can represent all lower
		// character widths.
		typedef enum _CHAR_TYPE_T {
			ACHAR = 1,	// ANSI, single-byte character
			WCHAR = 2	// wide character (2 bytes on Windows, 4 on Linux).
		} _CHAR_TYPE;

	public:
		// Destructor.
		virtual ~TString(void) { }

		/*! \function virtual int compareS(TStringArg str, bool icase = false, int num = -1, int start = -1, int sstart = -1)
		*	\brief  Determines ordering relative to input string.
		*
		* This routine determines the ordering of this TString
		* relative to the input string.
		* Parameters:
		*		str:	The string to compare this to.
		*		icase:	If true, do case-insensitive comparison.
		*		num:	If non-negative, the number of characters to compare.
		*		start:	If non-negative, the offset to start comparing
		*				in this TString.
		*		sstart:	If non-negative, the offset to start comparing
		*				in the input string.
		* Returns:
		*		-1 if this < str
		*		 0 if this == str
		*		+1 if this > str
		*/
		virtual int compareS(TStringArg str, bool icase = false,
			int num = -1, int start = -1, int sstart = -1) const = 0;

		/*! \function virtual void *extractV(void)
		*	\brief
		*
		* These routines extract the string pointer from this TString.
		* These routines either convert this TString's string pointer
		* to the desired type and delete it, or copy the pointer and
		* then clear it.  Either way, the caller assumes deallocation
		* responsibility for the return, and this TString has a NULL str
		* at the end of the call.
		*/
		virtual void *extractV(void) = 0;
		virtual char *extractA(void) = 0;
#ifdef RPC_WIDE_STRINGS
		virtual wchar_t *extractW(void) = 0;
#endif	// ifdef RPC_WIDE_STRINGS

		/*! \function virtual const void *getV(void)
		*	\brief Get a const pointer to the string.
		*
		* These routines return a constant pointer to
		* this TString's string.
		* Parameters:
		*		force:	If true, and this TString has a NULL pointer,
		*				a pointer to an empty string will be returned.
		* NOTE:  Even though these functions are const, if this TString's
		*        str is of a different character width from that requested,
		*        this TString converts itself in the call.
		*/
		virtual const void *getV(void) const = 0;
		virtual const char *getA(bool force = false) const = 0;
#ifdef RPC_WIDE_STRINGS
		virtual const wchar_t *getW(bool force = false) const = 0;
#endif	// ifdef RPC_WIDE_STRINGS
		virtual char *getPtrA(void) = 0;
#ifdef RPC_WIDE_STRINGS
		virtual wchar_t *getPtrW(void) = 0;
#endif	// ifdef RPC_WIDE_STRINGS

		/*! \function virtual char *getCopyA(void)
		*	\brief Returns a copy of the string.
		*
		* These routines return a copy of this TString's string.
		* NOTE:  The caller assumes deallocation responsibility
		*        for the returned string.
		*/
		virtual char *getCopyA(void) const = 0;
#ifdef RPC_WIDE_STRINGS
		virtual wchar_t *getCopyW(void) const = 0;
#endif	// ifdef RPC_WIDE_STRINGS

		/*! \function virtual char *getGAllocA(void) const = 0;
		*	\brief Returns a copy of the string allocated on the global heap
		*
		*	Note: The Caller assumes deallocation responsibility
		*		  for the returned string.
		*/
		virtual char *getGlobalAllocA(void) const = 0;
#ifdef RPC_WIDE_STRINGS
		virtual wchar_t *getGlobalAllocW(void) const = 0;
#endif

		/*! \function virtual int length(void)
		*	\brief  Get length of the TLabel string.
		*
		* This routine returns the length of this TLabel's string, in characters.
		* The return does not include the null-terminator.
		*/
		virtual int length(void) const = 0;

		/*! \function virtual void own(bool ownit)
		*	\brief Sets ownership.
		*
		* These routines get /set whether this object owns
		* its string.  If it is changed to own, it will make a
		* copy of its current unowned string.
		*/
		virtual void own(bool ownit) = 0;
		virtual bool own(void) const = 0;

		/*! \function virtual void setTSA(const TStringArg &str)
		*	\brief Set the string value.
		*
		* These routines set this string's value.
		*/
		virtual void setTSA(const TStringArg &str) = 0;
		virtual void setPtrTS(TString &str) = 0;
		virtual void setConstPtrTS(const TString &str) = 0;
		virtual void setA(const char *str) = 0;
		virtual void setPtrA(char *str) = 0;
#ifdef RPC_WIDE_STRINGS
		virtual void setW(const wchar_t *str) = 0;
		virtual void setPtrW(wchar_t *str) = 0;
#endif	// ifdef RPC_WIDE_STRINGS

		/*! \function virtual int type(void)
		*	\brief Get the type of string.
		*
		* These routines get /set the current type of this string.
		* (ACHAR, WCHAR, etc).
		* Parameters:
		*		stype:	The type to switch to.
		*		p:		If true, preserve the current contents.
		*/
		virtual int type(void) const = 0;
		virtual void type(int stype, bool p = false) = 0;

	};	// class TString



	/*! \class TStringArg
	*	\brief Used to pass variable width strings
	*
	*	This inlined class is intended to make it easy to pass
	*	in strings of various character widths.
	*/
	class TStringArg {
	public:
		const void *strptr;		// The string contained by this object.
		unsigned char strtype;	// Type of string in strptr (see TString::CHAR_TYPE).

		/*! \function inline void init(unsigned char itype, const void *iptr)
		*	\brief Initializes.
		*
		* This routine initializes this object.
		* Call it in every constructor.
		*/
		inline void init(unsigned char itype, const void *iptr) {
			strtype = itype;
			strptr = iptr;
		}

		// Constructors.
		inline TStringArg(void) {
			init(TString::ACHAR, NULL);
		}
		inline TStringArg(const int istr) {	// Always means NULL.
			init(TString::ACHAR, NULL);
		}
		inline TStringArg(const char *istr) {
			init(TString::ACHAR, (const void *)istr);
		}
#ifdef RPC_WIDE_STRINGS
		inline TStringArg(const wchar_t *istr) {
			init(TString::WCHAR, (const void *)istr);
		}
#endif	// ifdef RPC_WIDE_STRINGS
		inline TStringArg(const TStringArg &istr) {
			init(istr.strtype, istr.strptr);
		}
		inline TStringArg(const TString &istr) {
			init(istr.type(), istr.getV());
		}

		// Assignment operators.
		inline TStringArg &operator =(const int istr) {	// Always means NULL.
			init(TString::ACHAR, NULL);
			return *this;
		}
		inline TStringArg &operator =(const char *istr) {
			init(TString::ACHAR, (const void *)istr);
			return *this;
		}
#ifdef RPC_WIDE_STRINGS
		inline TStringArg &operator =(const wchar_t *istr) {
			init(TString::WCHAR, (const void *)istr);
			return *this;
		}
#endif	// ifdef RPC_WIDE_STRINGS
		inline TStringArg &operator =(const TStringArg &istr) {
			init(istr.strtype, istr.strptr);
			return *this;
		}
		inline TStringArg &operator =(const TString &istr) {
			init(istr.type(), istr.getV());
			return *this;
		}

	};	// class TStringArg


	/*! \class Checksum
	* \brief This is the interface for the Checksum class
	*
	*	This is the interface for the Checksum class
	*/
	class Checksum : public Param {
	public:
		/*	\function virtual ~Checksum(void)
		*	\brief Destructor.
		*
		* This is the dummy destructor for this interface
		*/
		virtual ~Checksum(void) {}

		/*	\function virtual void calculate(const ParamList &list, const License &license)
		*	\brief	Calculates the Checksum
		*
		* This routine calculates this Checksum over the input ParamList.
		* Parameters:
		*		list:			The List over which this checksum should be
		*						calculated.
		*		license:		The license of the content, which determines which
		*						method of calculating the checksum to use.
		*/
		virtual void calculate(const ParamList &list,
			const License &license) = 0;

		/*	\function virtual bool verify(const ParamList &list, const License &license)
		*	\brief Checks input params against the Checksum
		*
		* This routine checks the input ParamList against this checksum.
		* Parameters:
		*		list:			The List over which this checksum should be
		*						calculated.
		*		license:		The license of the content, which determines which
		*						method of calculating the checksum to use.
		* Returns:
		*		true iff the input ParamList is verified by this checksum.
		*/
		virtual bool verify(const ParamList &list,
			const License &license) const = 0;

	};	// end class Checksum


	/*! \class Client
	*	\brief Interface between global API and client code.
	*
	*  This is the interface that the client code must implement
	* and pair with the global API object obtainable by calling
	* the function RPCgetAPI().
	*/

	class Client {
	public:
		/*! \class Mode
		*	\brief Mode of the host program.
		*
		* The RPCapi::Client::Mode represents the mode of the host
		* program.  If the host program is in a normal mode that allows
		* editing of the scene the the state should be
		* RPCapi::Client::Mode::EDIT.  If the host program is in a render
		* slave mode that does not allow the scene to be edited then it
		* should return RPCapi::Client::Mode::NO_EDIT.
		*/
		class Mode {
		public:
			typedef enum MODE_TYPE {
				NO_EDIT = 0,
				EDIT = 1
			} MODE_TYPE_T;	// end typedef enum MODE_TYPE

		};	// end class RPCapi::Mode


		/*! \function
		*	\brief
		*
		* This is the dummy destructor for this interface.
		*/
		virtual ~Client(void) {}

		/*! \function
		*	\brief
		*
		* This routine returns the string for the client code which should
		* be displayed in the RPC api's About Dialog.
		* Returns:
		*		A string containing the client code's version information.
		*/
		virtual TStringArg RPCgetAboutString(void) = 0;

		/*! \function
		*	\brief
		*
		* This routine retrieves a pointer to the API object paired
		* with this Client object.
		* Returns:
		*		A pointer to the API object paired with this Client object.
		*/
		virtual RPCapi *RPCgetAPI(void) = 0;

#ifndef RPC_CONTENT_CLIENT
		/*! \function
		*	\brief
		*
		* This routine retrieves the license of this Client object.
		* Returns:
		*		A pointer to this Client object's license.
		*/
		virtual const License *RPCgetLicense(void) = 0;
#endif	// RPC_CONTENT_CLIENT

		/*! \function
		*	\brief
		*
		* This routine returns the mode of the host program.
		* If the host program is in a render slave mode that
		* does not allow editing of a scene then this method
		* should return "RPCapi::Client::State::NO_EDIT".
		* If the host is in a normal mode that allows editing
		* of the scene then this method should return
		* "RPCapi::Client::Mode::EDIT".
		*/
		virtual int RPCgetMode() = 0;

		/*! \function
		*	\brief
		*
		* This routine returns the list of paths that should be
		* searched for RPC content.  This method will be called when
		* the API needs to know where to look for RPC content.
		* Parameters:
		*		start:		If true, this call means that the API is
		*					going to scane the paths it retrieves from
		*					this function.  This process could take
		*					a while, so the client code may want to
		*					take appropriate action (like displaying
		*					a dialog or making the mouse cursor an hour glass).
		*					If false, this call means that the API
		*					is done scanning the paths, and that this
		*					function does not need to return any paths.
		*		numPaths:	The number of paths returned.
		* Returns:
		*		A pointer to an array of strings, each of which
		*		is the name of a directory.
		*/
		virtual const TStringArg *RPCgetPaths(bool start, int &numPaths) = 0;

#ifdef RPC_CONTENT_CLIENT
		/*
		* This routine returns the path to the RPCapi.ini file.
		* If this routine returns NULL, the RPCapi.dll will
		* look for the INI file in the path that was current
		* when the RPCapi object was created.
		* Returns:
		*		The path to the INI file.
		*/
		virtual TStringArg RPCiniPath(void) = 0;

		/*! \function
		*	\brief
		*
		* This routine is called whenever the license provided
		* by the content client for this plugin has changed.
		* NOTE:  This routine will be called anytime the licenses
		*        are updated for this client from the ACM.  This
		*        occurs when RPCapi::updateACM() is called, when the
		*        first RPCapi::Instance is created, or asynchronously
		*        when the client loses its connection with the ACM.
		* Parameters:
		*		licensed:	true if the plugin is considered licensed.
		*					false otherwise.
		*		acm:		true iff this client is connected to the ACM.
		*/
		virtual void RPClicenseChange(bool licensed, bool acm) = 0;

		/*! \function
		*	\brief
		*
		* This routine returns the globally-unique identifier for
		* the Client class.  This id, used with RPCpluginMetadata,
		* should uniquely determine any program or plugin that uses the
		* RPCapi.  It should be of the format S-XXXX-XXXX-XXXX...,
		* where S is the number of bytes in the ID, and each
		* hyphen-separated word (XXXX) represents 20 bits, where X can
		* range from 0 to 9 and from A to Z (excluding I, O, and Z).
		* 8 words (20 bytes) is a good size.
		* Returns:
		*		The human-readable GUID of this client.
		*/
		virtual TStringArg RPCpluginId(void) = 0;

		/*! \function
		*	\brief
		*
		* This routine gets the metadata for the Client class.
		* This metadata, usedwith RPClicenseID, identifies and
		* describes any program or plugin that uses the RPCapi.
		* Metadata should include the pair "author", "<plugin publisher>".
		* Parameters:
		*		num:	The number of metadata items, returned by refernece.
		*		keys:	The metadata keys, returned by reference.
		*		values:	The metadata values, returned by reference.
		*/
		virtual void RPCpluginMetadata(int &num, const TStringArg *&keys,
			const Param **&values) = 0;

		/*! \function
		*	\brief
		*
		* This routine determines what kinds of content this plugin
		* supports.  Each element of setsize / keys / values represents
		* a set of metadata.  A piece of content must match all key / value
		* pairs in at least one set in order to match.  If no
		* sets are returned, all content matches.
		* NOTE:  The caller will not deallocate the returned values.
		* Parameters:
		*		req:		Request type.
		*					1 == metadata of content the plugin supports.
		*					If a piece of content does not match, it is
		*					not retrieved and made available.
		*					NOTE:  The return values for this request type
		*					       should always be the same for a give plugin.
		*					2 == metadata of content to get licenses for.
		*		numsets:	Number of sets of permissions.
		*		setsize:	Array containing size of each set.
		*		keys:		Array of arrays of keys.
		*		values:		Array of arrays of values.
		*/
		virtual void RPCcontentMetadata(int req, int &numsets,
			const int *&setsize, const TStringArg **&keys,
			const Param ***&values) = 0;
#endif	// ifdef RPC_CONTENT_CLIENT

#ifdef RPC_CONTENT_CLIENT
		/*! \function
		*	\brief
		*
		* The RPC API calls this routine when it has some message
		* that it wishes to display to the user.
		* Parameters:
		*		msgType:	The type of this message.
		*					1:  generic information.
		*					2:  warning
		*					3:  error.
		*					4:  catastrophic error (we're going to die).
		*					5:  a time-consuming task is about to begin.
		*					    no window needs to be displayed, but
		*					    the API will be unresponsive
		*					    (an hour-glass would be good).
		*					6:  a time-consuming task is done.
		*					7:  more verbose warning or error.
		*		ret:		If true, we need a yes-no response.
		*		title:	If a window will display the message, use this title.
		*		msg:	The message to display.
		* Returns:
		*		If ret is true, true or false (depending on the user's response).
		*		If the message will not be displayed, or if ret is false,
		*		return false.
		*/
		virtual bool RPCuserMessage(int msgType, bool ret,
			const TString &title, const TString &msg) = 0;
#endif	// ifdef RPC_CONTENT_CLIENT

	};	// end class Client


	/*! \class ClientInstance
	*   \brief Interface between client code and each RPCapi instance
	*
	* This is the interface that the client code must implement
	* and pair with each RPCapi::Instance object it uses.
	*/
	class ClientInstance {
	public:
		/*! \function virtual ~ClientInstance(void)
		*	\brief Destructor.
		*
		* This is the dummy destructor for this interface.
		*/
		virtual ~ClientInstance(void) {}

		/*! \function virtual Instance *RPCgetInstance(void)
		*	\brief Gets corresponding RPCapi instance.
		*
		* This method is called when a procedure needs access to
		* the RPCapi::Instance that corresponds to this ClientInstance object.
		* Returns:
		*		 a pointer to the corresponding RPCapi::Instance
		*/
		virtual Instance *RPCgetInstance(void) = 0;


		/*! \function virtual TStringArg RPCgetName(void)
		*	\brief Gets the name of the ClientInstnace.
		*
		* This routine returns the name of the ClientInstance.
		* Returns:
		*		The human-readable string the ClientInstance is
		*		using to identify itself.
		*/
		virtual TStringArg RPCgetName(void) = 0;

		/*! \function virtual void RPCgetPivot(double px, double py, double pz,
		double &distance, double &dx, double &dy, double &dz)
		*	\brief Converts the pivot into new pivot/direction.
		*
		* This routine converts the input pivot position
		* into a new pivot position and a direction.
		* Parameters:
		*		px, py, pz:		The current pivot position.
		*		position:		The world-coordinates position of the
		*						indicated pivot, returned by reference.
		*		distance:		The distance in inches the pivot is
		*						from the Instance's pivot at time 0.
		*						This may be an arbitrary value, since
		*						pivot may have followed a curved path
		*						(the distance is measured along the curve).
		*		dx, dy, dz:		The world-coordinates direction of motion
		*						of the indicated pivot, returned by reference.
		*/
		virtual void RPCgetPivot(double px, double py, double pz,
			double &distance, double &dx, double &dy, double &dz) = 0;

		/*! \function virtual int RPCgetTime(void)
		*	\brief Gets the current time in frames.
		*
		* This routine returns the current time,
		* in frames (1/30th of a second).  This routine is used
		* primarily at render time during animations.
		* Returns:
		*		The current time, in frames.
		*/
		virtual int RPCgetTime(void) = 0;

		/*! \function virtual bool RPCisSelected(void)
		*	\brief Is the ClientInstance selected?
		*
		* This routine determines whether or not the ClientInstance
		* is currently selected in the scene.
		* Returns:
		*		true iff the ClientInstance is currently selected.
		*/
		virtual bool RPCisSelected(void) = 0;

		/*! \function virtual void RPCparameterChangeNotification(bool newInstance,
		const TString **params, int num)
		*	\brief  Called when parameters are changed by user.
		*
		* This is a method that the plug in developer implements.  This method
		* will be called when a set of parameters is changed via the UserInterface
		* Parameters:
		*		newInstance : if true then the old data for the rpc needs to be
		*						removed and replaced with the data for this instance.
		*						true will usually appear during the creation process
		*						when the user changes which RPC they want to add to
		*						a scene.  It can also appear if the user switches the
		*						RPC after placement.
		*		params:		These are the keys of the transforms
		*					(retrievable through Instance::getTransform())
		*					that have changed.
		*		num:		The number of params.
		*/
		virtual void RPCparameterChangeNotification(bool newInstance,
			const TString **params, int num) = 0;

		/*! \function virtual void RPCselect(bool select)
		*	\brief Selects/deselects client instance.
		*
		* This routine causes this client instance to become
		* selected or deselected.
		* Parameters:
		*		select:		true if the client instance is to be selected.
		*					false if the client instance is to be unselected.
		*/
		virtual void RPCselect(bool select) = 0;

	};	// end class ClientInstance


	/*! \class Content
	*	\brief Type of content of an RPC
	*
	*
	* The RPCapi::Content represents the type of content
	* of an RPC.  The current types of content are
	* 2D, 2.5D, 3D, 3.5D, and Smart.
	* The content type of an RPC is stored in /metadata/content type
	*
	*/
	class Content {
	public:
		typedef enum CONTENT_TYPE {
			_2D = 1,
			_2_5D = 2,
			_3D = 3,
			_3_5D = 4,
			SMART = 5
		} CONTENT_TYPE_T;	// end typedef enum CONTENT_TYPE

	};	// end class RPCapi::Content

	/*! \class Creator
	*	\brief Base interface for Creator
	*
	*
	* This is the base interface for a Creator object which recognizes
	* a format file of a particular style.
	* NOTE:  This class is used in the creation of RPC files.
	*        This class is only usable if the RPCapi.dll file you are
	*        using has the Creator license enabled.
	*/
	class Creator : public BaseObject {
	public:
		/*! \function virtual ~Creator(void)
		*	\brief Destructor.
		*
		* This is the dummy destructor for this interface
		*/
		virtual ~Creator(void) {}

		/*! \function virtual bool createRPC(int argc, char **argv)
		*	\brief Creates the RPC.
		*
		* This routine builds an RPC from this Creator
		* Parameters:
		*		argc, argv
		* Returns:
		*		true iff rpc was created.
		*/
		virtual bool createRPC(int argc, char **argv) = 0;

		/*! \function  virtual bool readTString(TString *rs)
		*	\brief Reads a TString from the Creator.
		*
		* This routine reads a TString from this Creator.
		*/
		virtual bool readTString(TString *rs) = 0;

	};	// end class Creator


	/*! \class Error
	*	\brief All error codes by any function.
	*
	*
	* The RPCapi::Error class encapsulates all of the error
	* codes which may be returned by any RPCapi function.
	* All error codes are < 0.
	* To use in your code, say "RPCapi::Error::NONE", or
	* whatever error code is appropriate.
	*
	*/
	class Error {
	public:
		typedef enum ERROR_CODE {
			NONE = 0,				// no error
			UNKNOWN = -1,			// error of unknown type
			STREAM_OVERFLOW = -10,	// tried to read/write past end of stream
			PARSING = -11,			// format error in a stream or object
			READING_STREAM = -12,	// could not read from a stream
			WRITING_STREAM = -13,	// could not write to a stream
			MEMORY_ALLOC = -14,		// could not allocate memory
			NO_READ_ACCESS = -15,	// did not have read access on stream
			NO_WRITE_ACCESS = -16,	// did not have write access on stream
			NO_STREAM = -17,		// there was no stream to access
			INVALID_SCALING = -18,	// unsupported scaling mode used
			NETWORK = -19,			// network error
			UNKNOWN_TYPE_CODE = -20,// unkown type code
			CRYPTO_CHECK = -21,		// cryptographic checksum failed

			// The following errors can be returned by acmSendRequest().
			ACM_REQ_CONNECTION = -22,	// unable to connect to an ACM
			ACM_REQ_NETWORK = -23,		// error in net communication w/ ACM
			ACM_REQ_BAD_SERVER_ID = -24,// unable to dereference source server ID
			ACM_REQ_SERVER_ERROR = -25	// source server reported an error

		} ERROR_CODE_T;	// end typedef enum ERROR_CODE

	};	// end class RPCapi::Error


	/*! \class ID
	*	\brief This is the interface for the unique IDs assigned to each RPC file.
	*
	*	This is the interface for the unique IDs assigned to each RPC file.
	*/
	class ID : public Param {
	public:
		// This is the dummy destructor for this interface
		virtual ~ID(void) {}

		/*! \function
		*	\brief
		*
		* This routine returns a constant pointer to the bytes of this ID.
		* Returns:
		*		The bytes of this ID.
		*/
		virtual const unsigned char *bytes(void) const = 0;

		/*!	\function  virtual int compare(const ID &inputID)
		*	\brief Compares IDs.
		*
		* This routine compares two IDs.
		* Parameters:
		*		inputID:	The ID with which to compare this ID.
		* Returns:
		*		-1 if this ID  < inputID;
		*		 0 if this ID == input ID;
		*		+1 if this ID  > input ID
		*/
		virtual int compare(const ID &inputID) const = 0;

		/*!	\function virtual int numBytes(void)
		*	\brief Gets size of ID.
		*
		* This routine returns the number of bytes in this ID.
		* Returns:
		*		number of bytes in this ID's representation.
		*/
		virtual int numBytes(void) const = 0;

		/*!	\function virtual void setBytes(const unsigned char *inBytes, int numBytes)
		*	\brief Set the data.
		*
		* Set the data of this ID to be that contained in the
		* input byte array.
		* Parameters:
		*		inBytes:		bytes to set this ID to
		*		numBytes:		number of bytes in inBytes
		*/
		virtual void setBytes(const unsigned char *inBytes, int numBytes) = 0;

	};	// end class ID


	/*! \class Image
	*	\brief The Image interface encapsulates any texture image retrieved from an RPC.
	*
	*
	*	The Image interface encapsulates any texture image retrieved from an RPC.
	*	 NOTE:  Unless you are using ParamLists, you should NOT
	*	use this class.  See class Texture.
	*
	*/
	class Image : public Param {
	public:
		/*!	\function  virtual ~Image(void)
		*	\brief Destructor.;
		*
		* This is the dummy destructor for this interface
		*/
		virtual ~Image(void) {}

		/*!	\function virtual int data(unsigned char *&buffer,
		bool allocBuffer,
		unsigned char channels,
		int scaleMode,
		int &width,
		int &height,
		int x1 = 0,
		int y1 = 0,
		int x2 = 0,
		int y2 = 0
		)
		*	\brief Retrieves image data.
		*
		* This routine retrieves data from this Image.
		* This routine allocates the data array used to store
		* this Image, fills it in with the appropriate data,
		* and returns a pointer to the array.
		* It is the client application's responsibility to
		* deallocate the data array when finished with it.
		* This routine returns the size of the buffer array.
		* Parameters:
		*		buffer:		buffer in which the image data is placed.
		*					This buffer may be allocated inside this
		*					function, and a pointer to it is returned by
		*					reference.
		*		alloc:		if true, this routine allocates buffer.
		*					if false, this routine fills in the buffer passed
		*					to it, or returns the size needed if that buffer
		*					is NULL.
		*		channels:	specifies which channels to include in the
		*					texture data returned (see RPCapi::Channels).
		*		scaleMode:	specifies which scale mode to use.
		*					(see RPCapi::Scale).
		*		width:		The width to which the texture is to be scaled.
		*					If ScaleMode is set to SCALE_FAST, the returned
		*					texture's width will be scaled to the nearest
		*					power of 2.  Set to 0 if you do not want the
		*					texture to be scaled.
		*		height:		The height to which the texture is to be scaled.
		*					If ScaleMode is set to SCALE_FAST, the returned
		*					texture's height will be scaled to the nearest
		*					power of 2.  Set to 0 if you do not want the
		*					texture to be scaled.
		*		x1:			x coordinate of the top left corner of the
		*					rectangle of the texture you want returned.  Set
		*					to 0 if you want the entire texture.
		*		y1:			y coordinate of the top left corner of the
		*					rectangle of the texture you want returned.  Set
		*					to 0 if you want the entire texture.
		*		x2:			x coordinate of the bottom right corner of the
		*					rectangle of the texture you want returned.  Set
		*					to 0 if you want the entire texture.
		*		y2:			y coordinate of the bottom right corner of the
		*					rectangle of the texture you want returned.  Set
		*					to 0 if you want the entire texture.
		* Returns:
		*		The number of bytes used or required to store the requested,
		*		image, or < 0 on an error.
		*/
		virtual int data(unsigned char *&buffer,
			bool allocBuffer,
			unsigned char channels,
			int scaleMode,
			int &width,
			int &height,
			int x1 = 0,
			int y1 = 0,
			int x2 = 0,
			int y2 = 0
			) = 0;

#ifdef PORT_WINDOWS
		/*!	\function virtual HBITMAP getHBitmap(void)
		*	\brief Create the HBITMAP from the image.
		*
		* This routine creates a screen compatible HBITMAP from
		* the data in this image accessor
		* Return:
		*		HBITMAP of this image accessor
		*/
		virtual HBITMAP getHBitmap(void) = 0;
#endif	// ifdef PORT_WINDOWS

		/*!	\function virtual int horizontalResolution(void)
		*	\brief Get the image width.
		*
		* This routine returns the width of this Image in pixels.
		* Returns:
		*		The pixel-width of this Image.
		*/
		virtual int horizontalResolution(void) const = 0;

		/*!	\function virtual int verticalResolution(void)
		*	\brief Get the image height.
		*
		* This routine returns the height of this Image in pixels.
		* Returns:
		*		The pixel-height of this Image.
		*/
		virtual int verticalResolution(void) const = 0;

	};	// end class Image	



	/*! \class Instance
	*	\brief  Single instance of an RPC in a scene.
	*
	*
	* An Instance object exists for every instance of an RPC in a scene.
	* For example, if you are using "bmw.rpc", and have created
	* two BMWs in your scene, then two Instance objects exist,
	* one for each BMW to be rendered.
	*/
	class Instance : public Param {
	public:
		/*! \function virtual ~Instance(void)
		*	\brief Destructor.
		*
		* This is the dummy destructor for this interface.
		*/
		virtual ~Instance(void) {}

		/*! \function virtual bool calculatePivots(bool editMesh)
		*	\brief Recaulculates pivots.
		*
		* Calling this routine tells the Instance that it should
		* recalculate its pivots.  This routine uses the RPCObject::getPivot()
		* function to aid in the calculation.
		*		editMesh:	true if the pivots are being calculated for
		*					an edit mesh, false if they are being
		*					calculated for a render mesh.
		* Returns:
		*		true iff any pivot information in the Instance which
		*		affects the geometry has changed.
		*/
		virtual bool calculatePivots(bool editMesh) = 0;

		/*! \function virtual int contentType(void)
		*	\brief Returns the instance's content type.
		*
		* This routine calculates the content type of this Instance.
		* Returns:
		*		The content type of this Instance, or 0 if it cannot be determined.
		*		(see typedef enum RPCapi::Content::CONTENT_TYPE).
		*/
		virtual int contentType(void) const = 0;

		/*! \function virtual bool editMeshHasChanged(bool value)
		*	\brief Checks if the mesh has changed.
		*
		* This routine returns true iff the edit mesh for this Instance
		* has changed since the last time this function was called
		* with a value of false.
		* Parameters:
		*		value:	The new value of the edit mesh change flag.
		* Returns:
		*		true iff the edit mesh has changed.
		*/
		virtual bool editMeshHasChanged(bool value) = 0;

		/*! \function virtual Mesh *getEditMesh(void)
		*	\brief Retrieves the instance's edit mesh.
		*
		* This routine retrieves the edit mesh for this Instance.
		* Returns:
		*		A pointer to a newly allocated copy of the edit mesh of
		*		this Instance, or NULL if an error occurred.
		*/
		virtual Mesh *getEditMesh(void) const = 0;

		/*! \function virtual const ID *getID(void) const
		*	\brief Retrieves the RPC's ID.
		*
		* This routine retrieves the unique ID of this Instance's RPC file.
		* Returns:
		*		This Instance's RPC file's unique identifier,
		*		or NULL on an error.
		*/
		virtual const ID *getID(void) const = 0;


		/*! \function virtual Mesh *getMesh(Mesh *mesh,
		double cx = 0, double cy = 0, double cz = 0)
		*	\brief Gets the adjusted render mesh.
		*
		* This routine retrieves the render mesh for this
		* Instance, adjusted for its current transforms.
		* Parameters:
		*		mesh:	A pointer to a render mesh.  If this is NULL,
		*				this routine will allocate a new render mesh.
		*				If it is not NULL, this routine will modify the
		*				input mesh and return a pointer to it.
		*		NOTE:  The next parameter is only required by
		*				RPCs whose render meshes are view-dependent.
		*		cx, cy, cz:			The location of the camer in
		*							the RPC's mesh space.  This location
		*							can be derived from the camera's location
		*							in world space, the RPC's location in
		*							world space, adn the RPC's orientation
		*							in world space.
		* Returns:
		*		A pointer to the render mesh of this Instance, or NULL
		*		if an error occurred.
		*/
		virtual Mesh *getMesh(Mesh *mesh,
			double cx = 0, double cy = 0, double cz = 0) const = 0;
		virtual Mesh *getMesh(Mesh *mesh,
			const Vector *cameraLocation = NULL) const = 0;


		/*! \function virtual TString *getName(void)
		*	\brief Returns the Instance' RPC name.
		*
		* This routine returns the name of this Instance's RPC.
		* Returns:
		*		A pointer to a newly-allocated string containing
		*		this Instance's RPC's name, or NULL if the name
		*		could not be retrieved.
		*/
		virtual TString *getName(void) const = 0;

		/*! \function virtual Texture *getPreviewTexture(void)
		*	\brief Gets a preview texture.
		*
		* This routine returns a preview texture.  This Texture is not intended
		* for painting, but will "look" like those Textures which are
		* used for painting.
		* Returns:
		*		A pointer to the newly-allocated preview texture, or
		*		NULL if an error occurred.
		*/
		virtual Texture *getPreviewTexture(void) const = 0;

		/*! \function virtual RPCapi *getRPCapi(void)
		*	\brief Gets the associated RPCapi object.
		*
		* This routine returns the RPCapi object associated
		* with this Instance.  This will be the object used to
		* generate this Instance, or the object tied to the Client
		* associated with this Instance's ClientInstance.
		* Returns:
		*		A pointer to this Instance's RPCapi object.
		*/
		virtual RPCapi *getRPCapi(void) const = 0;

		/*! \function virtual const TString &getRPCFileName(void)
		*	\brief Gets the RPC file name.
		*
		* This routine retrieves the name of the RPC file
		* for this RPC Instance.
		* Returns:
		*		The name of this Instance's RPC file.
		*/
		virtual const TString &getRPCFileName(void) const = 0;

		/*! \function virtual ClientInstance *getClientInstance(void)
		*	\brief Gets the ClientInstance pointer.
		*
		* This routine returns a pointer to this Instance's
		* Client Instance pointer.
		* Returns:
		*		A pointer to this Instance's Client Instance.
		*/
		virtual ClientInstance *getClientInstance(void) const = 0;

		/*! \function  virtual ParamList *getRPCFile(void)
		*	\brief Gets the ParalList for the RPC file.
		*
		* This routine opens the RPC file that this Instance points to
		* and returns the ParamList pointing to the file.  For
		* more details, see class ParamList.
		* Returns:
		*		A pointer to the newly-allocated ParamList poiting
		*		to the RPC file of this Instance.
		*/
		virtual ParamList *getRPCFile(void) const = 0;

		/*! \function virtual bool getTextures(double cx, double cy, double cz,
		int &numTextures, Texture **&textures,
		TextureMesh *&textureMesh)
		*	\brief Gets current trequired extures and meshes for the instance.
		*
		* This routine retrieves the Textures and TextureMeshes required to
		* render this Instance given the current camera location.
		* All of the TextureMesh faces' indices index into the returned array
		* of Textures, and all TextureMesh vertices' indices index into the
		* global uber-mesh returned earlier by this Instance, so no further
		* index swizelling is required after this routine returns.
		* Parameters:
		*		cx, cy, cz:			The location of the camera in this
		*							Instance's space.
		*		numTextures:		The number of Textures in the array.
		*		textures:			The array of Textures, newly allocated,
		*							and returned by reference.
		*		textureMesh:		The global TextureMesh, newly allocated
		*							and returned by reference.
		* Returns:
		*		true iff the Textures and TextureMeshes
		*		were successfully retrieved
		*/
		virtual bool getTextures(double cx, double cy, double cz,
			int &numTextures, Texture **&textures,
			TextureMesh *&textureMesh) const = 0;
		virtual bool getTextures(const Vector &cameraLocation,
			int &numTextures, Texture **&textures,
			TextureMesh *&textureMesh) const = 0;

		/*! \function virtual const Param *getTransform(TStringArg key)
		*	\brief Gets a single transform pointer.
		*
		* This routine gets a Transform from this Instance.
		* Parameters:
		*		key:	The key of the Transform to get.
		* Returns:
		*		A constant pointer to the associated value of the Transform,
		*		or NULL if it does not exist.
		*/
		virtual const Param *getTransform(TStringArg key) const = 0;

		/*! \function virtual int getTransformKeys(const TString **&keys)
		*	\brief Gets an array of transform pointers.
		*
		* This routine allocates an array of strings,
		* fills in the array with pointers to the keys of
		* this Instance's Transforms, and returns a pointer
		* to the array.
		* NOTE:  Deallocate the returned array, but NOT the returned TStrings.
		* NOTE:  On an error, or in the event that there are no keys,
		* the returned keys pointer is NULL.
		* Parameters:
		*		keys:			Reference pointer through which is
		*						returned the array of keys.
		* Returns:
		*		The number of keys, or < 0 if an error occurred.
		*/
		virtual int getTransformKeys(const TString **&keys) const = 0;

		/*! \function virtual int getTransformUnitType(TStringArg key)
		*	\brief Gets the unit type for the specified transform.
		*
		* This routine returns the unit type for a particular transform
		* (eg LINEAR_UNITS).
		* Parameters:
		*		key:	The name of the transform in question.
		* Returns:
		*		The unit type of the transform, or Units::NO_UNITS
		*		if the Transform does not exist.
		*/
		virtual int getTransformUnitType(TStringArg key) const = 0;

		/*! \function virtual int has(int prop)
		*	\brief Does the RPC have the given property.
		*
		* This routine returns true iff this RPC Instance has
		* the indicated property.  Note that these properties include
		* geometry and speed, so this function with those properties
		* is interchangable with hasGeometry and hasSpeed.
		* NOTE:  A return of true indicates only that this Instance
		* has the potential to use an indicated property, and not
		* that it absolutely will.
		* NOTE:  None of these properties are mutually exclusive.
		* Parameters:
		*		prop:	The property being queried.
		* Returns:
		*		non-zero if this Instance has the input property.
		*/
		typedef enum PROPERTY {
			GEOMETRY = 1,
			SPEED = 2,
			VIEW_DEPENDENT_MESH = 3,
			VIEW_INDEPENDENT_MESH = 4,
			VIEW_DEPENDENT_TEXTURE = 5,
			VIEW_INDEPENDENT_TEXTURE = 6,
			TIME_DEPENDENT_MESH = 7,
			TIME_INDEPENDENT_MESH = 8,
			TIME_DEPENDENT_TEXTURE = 9,
			TIME_INDEPENDENT_TEXTURE = 10
		} PROPERTY_T;
		virtual int has(int prop) const = 0;

		/*! \function virtual bool hasGeometry(void)
		*	\brief Does the instance have geometry.
		*
		* This routine retuns true iff this RPC Instance has geometry.
		* Old RPCs do not have geometry; they create view-dependent
		* planes at render time and paint textures on them.
		* New RPCs may have true meshes.
		* Returns:
		*		true iff this Instance has a geometric mesh.
		*/
		virtual bool hasGeometry(void) const = 0;

		/*! \function virtual bool hasSpeed(void)
		*	\brief Does the instance move or not.
		*
		* This routine returns true iff this RPC Instance has
		* a speed; that is, the RPC object will move along a
		* client-defined path during animation.
		* Returns:
		*		true iff this Instance has speed.
		*/
		virtual bool hasSpeed(void) const = 0;

		/*! \function virtual bool setRpcId(const ID *id)
		*	\brief Sets the RPD's ID.
		*
		* This routine sets this Instance's RPC Id to the input bytes
		* Parameters:
		*		id:		The ID of the RPC file to point this Instance to.
		* Returns:
		*		true iff the ID was successfully set.  Note that if the
		*		license of the plugin or the API do not match that of
		*		the RPC with the input ID, this function will fail.
		*/
		virtual bool setRpcId(const ID *id) = 0;

		/*! \function virtual bool setRPCFileName(TStringArg rpcFileName)
		*	\brief Sets the file name.
		*
		* This routine sets this Instance's RPC File.
		* It causes the global table of RPC paths to be updated so that
		* it contains the directory containing the input RPC file
		* (if the file exists and can be read).
		* Parameters:
		*		rpcFileName:	The name of the RPC file.
		* Returns:
		*		true iff the ID of this RPC was successfully set to
		*		the ID contained in the input file.
		*/
		virtual bool setRPCFileName(TStringArg rpcFileName) = 0;

		/*! \function virtual void setClientInstance(ClientInstance *CI)
		*	\brief Sets the pointer to the instance.
		*
		* This routine sets the pointer to this Instance's ClientInstance.
		* Parameters:
		*		CI:		A pointer to the client code Instance with which this
		*				Instance should be paired.
		*/
		virtual void setClientInstance(ClientInstance *CI) = 0;


		/*! \function virtual bool setTransform(TStringArg key, const Param *value)
		*	\brief Set a Transform on the instance.
		*
		* This routine sets a Transform of this Instance.
		* Parameters:
		*		key:	The key of the Transform to set.
		*		value:	The new value of the Transform.
		* Returns:
		*		true iff the Transform's value  was successfully set.
		*/
		virtual bool setTransform(TStringArg key, const Param *value) = 0;

		/*! \function virtual double speed(void)
		*	\brief Get the instance speed.
		*
		* This routine returns the speed with which this Instance
		* moves along the user-specified path.  If this Instance
		* does not move, this routine returns 0.
		* Returns:
		*		The speed of this Instance, in inches per frame.
		*/
		virtual double speed(void) const = 0;

	};	// end class Instance


	/*! \class Units
	*	\brief Types of units.
	*
	* These are the types of units which can be set
	* in the setUnits routine.
	*
	*/
	class Units {
	public:
		typedef enum UNIT_TYPES {
			NO_UNITS = 0,
			LINEAR_UNITS = 1,	// base inches
			ANGULAR_UNITS = 2,	// base angles
			VELOCITY_UNITS = 3	// base inches per frame
		} UNIT_TYPES_T;	// end typedef enum UNIT_TYPES
	};	// end class Units


	/*! \class InstanceInterface
	*	\brief Allows user to edit options of an Instance.
	*
	*
	* The InstanceInterface can be displayed, allowing the user
	* to edit options of an RPC Instance.
	*
	*/
	class InstanceInterface : public BaseObject {
	public:
		/*! \function virtual ~InstanceInterface(void)
		*	\brief Destructor.
		* This is the dummy destructor for this interface.
		*/
		virtual ~InstanceInterface(void) {}

		/*!	\class Window
		*	\brief Contains window type codes.
		*
		* The RPCapi::InstanceInterface::Window class encapsulates all of the
		* different window type codes.
		* To use in your code, say
		* "RPCapi::InstanceInterface::Window::SELECTION",
		* or whatever window type is appropriate.
		*/
		class Window {
		public:
			typedef enum WINDOW_CODE {
				NONE = -1,
				SELECTION = 1,	// The dialog that allows the user to select
				// the RPC file used by this Interface's
				// Instance
				PARAMETERS = 2,	// The dialog that allows the user to change
				// the transforms of the currently selected
				// Instance.
				SELECTION_PARAMETERS = 3,	// This dialog displays both the
				// SELECTION and PARAMETERS windows.
				// It is not yet supported
#ifdef IMPLEMENT_THUMBNAIL_BROWSER
				SELECTION_THUMBS = 4	// Selection Window that displays
				// all previews in selected category.
#endif	// ifdef IMPLEMENT_THUMBNAIL_BROWSER
			} WINDOW_CODE_T;	// end typedef enum WINDOW_CODE
			// OR these w/ the window code:
			typedef enum WINDOW_OPTIONS {
				// By default, set initial selection
				// from the RPC Instance.
				NO_SELECTION = 0x00010000,	// Do not use global selection or
				// instance to set initial selection.
				GLOBAL_SELECTION = 0x00020000	// Use globally stored
				// last selection to set
				// initial selection
			} WINDOW_OPTIONS_T;	// end typedef enum WINDOW_OPTIONS

			typedef enum MODE_CODE {
				MODELESS = 1,		// Shows window inside parent
				FLOATING = 2,		// Shows window in its own border
				MODAL = 3			// Modal dialog
			} MODE_CODE_T;
		};	// end class RPCapi::InstanceInterface::Window

#ifdef PORT_WINDOWS
		/*!	\function virtual int show(HWND parentWindow, int windowToDisplay, int mode,
				int x = 0, int y = 0, int w = 0, int h = 0, HWND z = NULL)
		*	\brief Displays the dialog.
		*
		* This routine displays the specified dialog to the screen.
		* Parameters:
		*		parentWindow:		handle to the parent window.
		*		windowToDisplay:	the indicator of which window to display
		*							see Window::WINDOE_CODE for options
		*		mode:				the mode of the window to display
		&							see Window::MODE_CODE
		*/
		virtual int show(HWND parentWindow, int windowToDisplay, int mode,
			int x = 0, int y = 0, int w = 0, int h = 0, HWND z = NULL) = 0;
#endif	// ifdef PORT_WINDOWS

#ifdef PORT_MAC
		/*!	\function virtual int show(WindowRef parentWindow, int windowToDisplay)
		*	\brief	Display the dialog for Mac.
		* This routine displays the specified dialog to the screen.
		* Parameters:
		*		parentWindow:		handle to the parent window.
		*		windowToDisplay:	the indicator of which window to display
		*							see UserInterface::Window for options
		*/
		virtual int show(WindowRef parentWindow, int windowToDisplay) = 0;
#endif // ifdef PORT_MAC

		/*!	\function
		*	\brief
		*
		* This routine hides this dialog.
		*/
		virtual int hide(void) = 0;

		/*!	\function
		*	\brief
		*
		* This routine sets the Instance which this InstanceInterface edits.
		* Parameters:
		*		instance:	The new Instance for this Interface.
		*/
		virtual void setInstance(Instance *instance) = 0;

		/*!	\function
		*	\brief
		*
		* This routine sets the units used to display this Interface's
		* unit-possessing Controls.
		* Parameters:
		*		unitType:	The type of units to be changed
		*		scale:		The multiplicative factor to apply to
		*					the displayed values of controls
		*					with the input unitType.
		*					The scale is relative to the default units
		*					for the indicated unitType.
		*					The default unit for linear is inches,
		*					and the default unit for angular is degrees.
		*		unitSuffix:	A NULL terminiated character string that can
		*					be appended after a number to indicate the
		*					displayed units
		*/
		virtual void setUnits(int unitType, double scale, TStringArg unitSuffix = NULL) = 0;

		/*!	\function virtual int setBackgroundColor(int color)
		*	\brief Set interface attribute.
		*
		* The following methods will be used to set user interface attributes
		* however they are not currently supported
		*/
		virtual int setBackgroundColor(int color) = 0;
		virtual int setForegroundColor(int color) = 0;
		virtual int setWidth(int newWidth) = 0;
		virtual int setHeight(int newHeight) = 0;

	};	// end class InstanceInterface


	/*! \class License
	*	\brief Represents the licenses of the RPCs which the client app and this API can read.
	*
	* The License class encapsulates all of the different license type codes.
	* A License object can represent one or more license types simultaneously.
	* For example, you might be writing a commercial plugin, in which
	* case you should create a License object and set it to
	* types COMMERCIAL and FREE.  (All licenses should be set to FREE,
	* indicating that the software can read RPC content which is freely
	* available).
	*
	*/
	class License : public Param {
	public:
		// These are the supported license types
		typedef enum LICENSE_CODE {
			NONE = 0,	// used to indicate no license type
			COMMERCIAL = 1,
			EDUCATIONAL = 2,
			FREE = 3,
			REAL_TIME = 5,
			CREATOR = 32
		} LICENSE_CODE_T;	// end typedef enum LICENSE_CODE

		/*!	\function virtual ~License(void)
		*	\brief Destructor.
		*
		* This is the default destructor
		*/
		virtual ~License(void) {}

#ifdef RPC_CONTENT_CLIENT
		/*!	\function virtual unsigned int hash(void)
		*	\brief Hashes the object.
		*
		* This routine returns a simple hash over this object.
		* Returns:
		*		A simple hash over this object.
		*/
		virtual unsigned int hash(void) const = 0;
#endif	// ifdef RPC_CONTENT_CLIENT

		/*!	\function virtual bool is(int license)
		*	\brief Check if the passed license type is included.
		*
		* This routine determines if this License type includes
		* the input license.  A License object can represent
		* multiple license types (for example, Commercial AND Free).
		* Parameters:
		*		license:	the type of License to be queried for.
		* Returns:
		*		true iff this License includes the input license type.
		*/
		virtual bool is(int license) const = 0;

		/*!	\function virtual void mask(const License &license)
		*	\brief  Masks existing with input license.
		*
		* This routine masks this License with the input License.
		* This means that any licenses supported by this License but
		* not supported by the input License are no longer supported
		* by this License.
		* Parameters:
		*		license:	The License to mask this one with.
		*/
		virtual void mask(const License &license) = 0;

		/*!	\function virtual bool match(const License &license)
		*	\brief Does the passed license match the existing.
		*
		* This routine determines whether or not the input License
		* matches this License.
		* Paramters:
		*		license:	The license to match with this one.
		*		mode:		If 0, the input license matches this one
		*					if they share at least one license.
		*					If 1, the licenses match iff they
		*					are identical.
		*					If 2, the input license matches this one
		*					if this has at least all the licenses in license.
		* Returns:
		*		true iff the input license matches.
		*/
		virtual bool match(const License &license
#ifdef RPC_CONTENT_CLIENT
			, int mode = 0
#endif	// ifdef RPC_CONTENT_CLIENT
			) const = 0;

		/*!	\function virtual void set(bool has, int num, ...)
		*	\brief Checks to see if license type is included
		*
		* This routine sets whether or not this License object includes
		* the input license types.
		* Parameters:
		*		has:	if true, this function causes this License to
		*				include the input license types.  If false,
		*				it causes this License object to not include them.
		*		num:	The number of license types input to this function.
		*		...:	The license types.  If this number is -1, it
		*				sets ALL of the license types to the value of has.
		*				Example call:
		*				set(true, 2,
		*					RPCapi::License::COMMERCIAL, RPCapi::License::FREE)
		*/
		virtual void set(bool has, int num, ...) = 0;

	};	// end class RPCapi::License


	/*! \class MassEditInterface
	*	\brief Edit parameters of many instances at once.
	*
	*
	* A MassEditInterface is used to edit the parameters of
	* many Instances at once.
	*
	*/
	class MassEditInterface : public BaseObject {
	public:
		/*!	\function virtual ~MassEditInterface(void)
		*	\brief Destructor.
		* This is the dummy destructor for this interface.
		*/
		virtual ~MassEditInterface(void) { }

		/*!	\class Window
		*	\brief Contains window type codes.
		*
		* The RPCapi::MassEditInterface::Window class encapsulates all of the
		* different window type codes.
		* To use in your code, say
		* "RPCapi::MassEditInterface::Window::EDIT",
		* or whatever window type is appropriate.
		*/
		class Window {
		public:
			typedef enum WINDOW_CODE {
				NONE = -1,
				EDIT = 0,		// The dialog that allows the user to edit the 
				// properties of many RPCapi::Instance objects 
				// at once
				SELECTION = 1	// The dialog that allows the user to select
				// many RPC files to be used 
			} WINDOW_CODE_T;	// end typedef enum WINDOW_CODE
			// OR these w/ the window code:
			typedef enum WINDOW_OPTIONS {
				// By default, set initial selection
				// from the RPC Instance.
				NO_SELECTION = 0x00010000,	// Do not use global selection or
				// instance to set initial selection.
				GLOBAL_SELECTION = 0x00020000	// Use globally stored
				// last selection to set
				// initial selection
			} WINDOW_OPTIONS_T;	// end typedef enum WINDOW_OPTIONS
		};	// end class RPCapi::MassEditInterface::Window

#ifdef PORT_WINDOWS
		/*!	\function virtual int show(HWND hWndParent, int windowToDisplay)
		*	\brief Displays dialog.
		*
		* This routine displays the specified dialog to the screen.
		* Parameters:
		*		hWndParent:		handle to the parent window.
		*		windowToDisplay:	the indicator of which window to display
		*							see MassEditInterface::Window for options
		*/
		virtual int show(HWND hWndParent, int windowToDisplay) = 0;

		/*!	\function virtual int hide(void)
		*	\brief Hides the dialog.
		*
		* This routine hides this dialog.
		*/
		virtual int hide(void) = 0;
#endif	// ifdef PORT_WINDOWS

		/*!	\function virtual void addInstance(Instance *instance)
		*	\brief Adds an instances to the interface.
		*
		* This routine adds an Instance to be edited by this interface.
		* Parameters:
		*		instance:	The Instance to be added.
		*/
		virtual void addInstance(Instance *instance) = 0;

		/*!	\function virtual void removeInstance(Instance *instance)
		*	\brief Removes one instance from the list.
		*
		* This routine removes an Instance that is currently being edited
		* by this interface.  It does not delete the Instance; it just
		* removed it from this interface's list of Instances.
		* Parameters:
		*		instance:	The instance to be removed.
		*/
		virtual void removeInstance(Instance *instance) = 0;

		/*!	\function virtual void removeAllInstances(void)
		*	\brief Removes all instances from the list.
		*
		* This routine removes all of the Instances currently being
		* editted by this interface.  It does not delete them; it just
		* removes them from this interface's list of Instances.
		*/
		virtual void removeAllInstances(void) = 0;

		/*!	\function virtual void setUnits(int unitType, double scale, TStringArg unitSuffix = NULL
		*	\brief Sets the display units.
		*
		* This routine sets the units used to display this Interface's
		* unit-possessing Controls.
		* Parameters:
		*		unitType:	The type of units to be changed
		*		scale:		The multiplicative factor to apply to
		*					the displayed values of controls
		*					with the input unitType.
		*					The scale is relative to the default units
		*					for the indicated unitType.
		*					The default unit for linear is inches,
		*					and the default unit for angular is degrees.
		*		unitSuffix:	A NULL terminated character string that can
		*					be appended after a number to indicate the
		*					displayed units
		*/
		virtual void setUnits(int unitType, double scale, TStringArg unitSuffix = NULL) = 0;

		/*!	\function virtual int setBackgroundColor(int color)
		*	\brief Set the interface attribute.
		*
		* The following methods will be used to set user interface attributes
		* however they are not currently supported
		*/
		virtual int setBackgroundColor(int color) = 0;
		virtual int setForegroundColor(int color) = 0;
		virtual int setWidth(int newWidth) = 0;
		virtual int setHeight(int newHeight) = 0;
	};	// end class MassEditInterface


	/*! \class Mesh
	*	\brief Geometric mesh.  Contains vertices and faces.
	*
	*
	* This is the base interface for any Mesh contained in an RPC.
	* A Mesh is composed of an array of 3D Vertices and an array
	* of Faces.
	*
	*/
	class Mesh : public Param {
	public:

		/*!	\class Face
		*	\brief Vertices and visibilities of a face.
		*
		* A Face object contains six integers.  The first three integers
		* are the indices of this Face's vertices in the Vertex
		* array returned by this Face's parent Mesh.
		* The second three integers determine the visibility of
		* the edges of this face.  e0 == visibility of edge v0-v1,
		* e1 == visibility of edge v1-v2, and e2 == visibility of edge v2-v0
		*/
		class Face {
		public:
			int v0, v1, v2;
			int e0, e1, e2;

		};	// end class Mesh::Face

		/*!	\class Vertex
		*	\brief 3-D Coordinate.
		*
		* A Vertex object contains the 3-D coordinates of a vertex.
		*/
		class Vertex {
		public:
			double x, y, z;

		};	// end class Mesh::Vertex


		/*!	\function virtual ~Mesh(void)
		*	\brief Destructor.
		*
		* This is the dummy destructor for this interface.
		*/
		virtual ~Mesh(void) {}

		/*!	\function virtual double smoothingAngle(void)
		*	\brief Get the smooth angle.
		*
		* This routine gets the angle, in degrees,
		* to use when smoothing this mesh.
		* If the return is negative, no smoothing should be performed.
		*/
		virtual double smoothingAngle(void) const = 0;

		/*!	\function virtual void smoothingAngle(double a)
		*	\brief Set the smooth angle.
		*
		* This routine sets the angle, in degrees,
		* to use when smoothing this mesh.
		*/
		virtual void smoothingAngle(double a) = 0;

		/*!	\function virtual const Face *getConstFaces(void)
		*	\brief Gets the array of faces (const).
		*
		* This routine gets this Mesh's array of faces.
		* Returns:
		*		The pointer to this Mesh's Faces array.  Note that if
		*		this Mesh is a Stream Accessor, this routine can
		*		only return NULL.
		*/
		virtual const Face *getConstFaces(void) const = 0;

		/*!	\function virtual const Vertex *getConstVertices(void)
		*	\brief Get the array of vertices.
		*
		* This routine gets this Mesh's array of vertices.
		* Returns:
		*		The pointer to this Mesh's Vertices array.  Note that if
		*		this Mesh is a Stream Accessor, this routine can
		*		only return NULL.
		*/
		virtual const Vertex *getConstVertices(void) const = 0;

		/*!	\function virtual Face *getFaces(void)
		*	\brief Get the array of faces.
		*
		* This routine gets this Mesh's array of faces.
		* Returns:
		*		A newly allocated array of faces, which the caller is
		*		responsible for deallocating, or NULL if an error occurred.
		*/
		virtual Face *getFaces(void) const = 0;

		/*!	\function virtual int getNumFaces(void)
		*	\brief Get the number of faces.
		*
		* This routine gets the number of faces in this Mesh
		* Returns:
		*		The number of faces in this Mesh,
		*		or < 0 if an error occurred.
		*/
		virtual int getNumFaces(void) const = 0;

		/*!	\function virtual int getNumVerts(void)
		*	\brief Gets the number of vertices.
		*
		* This routine gets the number of vertices in this Mesh
		* Returns:
		*		The number of vertices in this Mesh,
		*		or < 0 if an error occurred.
		*/
		virtual int getNumVerts(void) const = 0;

		/*!	\function virtual Vertex *getVerts(void)
		*	\brief Gets the vertices.
		*
		* This routine gets this Mesh's array of vertices.
		* Returns:
		*		A newly allocated array of vertices, which the caller is
		*		responsible for deallocating, or NULL if an error occurred.
		*/
		virtual Vertex *getVerts(void) const = 0;

		/*!	\function virtual bool setFaces(Face *faces, int numFaces)
		*	\brief Sets the faces.
		*
		* This routine sets the faces in this Mesh
		* Note that if this Mesh is a Stream Accessor,
		* this routine will do nothing.
		* Parameters:
		*		faces:		The faces to store in this Mesh
		*		numFaces:	The number of faces in the input faces array
		* Returns:
		*		true iff the faces were successfully stored.
		*/
		virtual bool setFaces(Face *faces, int numFaces) = 0;

		/*!	\function virtual bool setVerts(Vertex *verts, int numVerts)
		*	\brief Sets the vertices.
		*
		* This routine sets the vertices in this Mesh
		* Note that if this Mesh is a Stream Accessor,
		* this routine will do nothing.
		* Parameters:
		*		verts:		The vertices to store in this Mesh
		*		numVerts:	The number of vertices in the input faces array
		* Returns:
		*		true iff the vertices were successfully stored.
		*/
		virtual bool setVerts(Vertex *verts, int numVerts) = 0;

	};	// end class Mesh


	/*! \class ParamList
	*	\brief List of Params keyed by strings.
	*
	*
	* An RPC file, after its version number and the typecode,
	* is composed entirely of ParamLists.
	* ParamLists contain Params, which are keyed by strings.
	* This is the base class for all RPCapi ParamLists.
	* There are two basic implementations
	* of ParamList:  one which accesses a ParamList in memory,
	* and one which accesses a ParamList in a stream (like a byte
	* array or a file).  The only difference is that the latter
	* implementation will always return NULL when getConst() is called.
	*
	*/
	class ParamList : public Param {
	public:
		/*!	\function virtual ~ParamList(void)
		*	\brief Destructor.
		*
		* This is the dummy destructor for this interface.
		*/
		virtual ~ParamList(void) {}

		/*!	\function virtual bool contains(TStringArg key, ...)
		*	\brief Does the list contain the parameter?
		*
		* This routine determines if this List contains a Parameter
		* matching the input key.
		* Parameters:
		*		key:	The key of the Parameter to be checked.
		*		...:	Additional arguments used in creating the key string.
		*				key and ... are of the same format as arguments
		*				to printf().
		*				NOTE:  Use %g when specifying floating point
		*				or double numbers!
		* Returns:
		*		true iff this List contains the input key.
		*/
		virtual bool contains(TStringArg key, ...) const = 0;

		/*!	\function virtual int count(void)
		*	\brief Gets the number of params in the list.
		*
		* This routine returns the number of parameters in this ParamList.
		* Returns:
		*		number of parameters in this List.
		*/
		virtual int count(void) const = 0;

		/*!	\function virtual Param *get(TStringArg key, ...)
		*	\brief Gets a Param matching the input key.
		*
		* This routine retrieves a Param matching the input Key.
		* It allocates a new copy of the parameter and returns a pointer to it.
		* Parameters:
		*		key:	The key of the Parameter to be retrieved.
		*		...:	Additional arguments used in creating the key string.
		*				key and ... are of the same format as arguments
		*				to printf().
		*				NOTE:  Use %g when specifying floating point or
		*				double numbers!
		* Returns:
		*		A pointer to a newly created copy of the desired Parameter,
		*		or NULL if it is not found or an error occurs.
		*/
		virtual Param *get(TStringArg key, ...) const = 0;

		/*!	\function virtual const Param *getConst(TStringArg key, ...)
		*	\brief Gets the const pointer to the param of the passed key.
		*
		* This routine retrieves a constant pointer to the Param matching
		* the input Key.  This routine will return NULL if this ParamList
		* is a stream accessor (since no Param of this List exists in
		* memory).
		* Parameters:
		*		key:	The key of the Parameter to be retrieved.
		*		...:	Additional arguments used in creating the key string.
		*				key and ... are of the same format as arguments
		*				to printf().
		*				NOTE:  Use %g when specifying floating point
		*				or double numbers!
		* Returns:
		*		A pointer to the desired Parameter, or NULL if it is not
		*		found, or if this List is a StreamAccessor.
		*/
		virtual const Param *getConst(TStringArg key, ...) const = 0;

		/*!	\function virtual int getKeys(const TString **&keys, int reserved = 0)
		*	\brief Gets an array of keys.
		*
		* This routine allocates an array of character strings,
		* fills in the array with new strings which are copies
		* of the keys of this List, and returns a pointer to the array.
		* NOTE:  On an error, or in the event that there are no keys,
		* the returned keys pointer is NULL.
		* Parameters:
		*		keys:			Reference pointer through which is
		*						returned the array of keys.
		*						Delete the array. Only delete keys from getKeyPtrs.
		*		reserved:		Reserved internal parameter.
		* Returns:
		*		The number of keys, or < 0 if an error occurred.
		*/
		virtual int getKeys(const TString **&keys, int reserved = 0) const = 0;
		virtual int getKeyCopies(TString **&keys, int reserved = 0) const = 0;

		/*!	\function virtual const TString &getPath(void)
		*	\brief Get the path to the ParamList.
		*
		* This routine gets the path of this ParamList.
		* Returns:
		*		The path of this ParamList.
		*/
		virtual const TString &getPath(void) const = 0;

		/*!	\function virtual bool insert(TStringArg key, Param &data, bool insertCopy = true)
		*	\brief Insets key and data to the list.
		*
		* This routine uniquely inserts a key-data pair into this List.
		* It creates copies of the key and the Param, and inserts
		* these copies.
		* Parameters:
		*		key:	The key of the Parameter to be inserted.
		*		data:	The Parameter to be inserted
		*		insertCopy:		create a copy of the data, and insert
		*						the copy, if this flag is true.
		* Returns:
		*		true iff the key and data were successfully inserted.
		*/
		virtual bool insert(TStringArg key, Param &data,
			bool insertCopy = true) = 0;

		/*!	\function virtual bool remove(TStringArg key)
		*	\brief Deletes the key and data.
		*
		* This routine deletes the indicated key and its corresponding data.
		* Note that this function will deallocate the memory associated
		* with the key and its data.
		* Parameters:
		*		key:	The key of the Parameter to be removed.
		* Returns:
		*		true iff the key was found and the pair was successfully removed.
		*/
		virtual bool remove(TStringArg key) = 0;

		/*!	\function virtual void setPath(TStringArg newPath)
		*	\brief Sets the paramList path.
		*
		* This routine sets the path of this ParamList.
		* Parameters:
		*		inPath:		The new path of this ParamList.
		*/
		virtual void setPath(TStringArg newPath) = 0;

		/*!	\function virtual bool update(TStringArg key, Param &data, bool insertCopy = true)
		*	\brief Update the input key.
		*
		* This routine updates the value of the input key.
		* Note that this routine is semantically equivalent to calling
		* remove(); if (get() != NULL) insert()
		* Parameters:
		*		key:	The key of the Parameter to be updated.
		*		data:	The new value the Param should take
		*		insertCopy:		create a copy of the data, and insert
		*						the copy, if this flag is true.
		* Returns:
		*		true iff the key's Param was found and updated
		*/
		virtual bool update(TStringArg key, Param &data,
			bool insertCopy = true) = 0;

	};	// end class ParamList


	/*! \class PrimP
	*	\brief Template for every primitive type in an RPC.
	*
	* This template class is used to represent every primitive type
	* stored in an RPC (int, char, double, ....)
	* I apologize for not calling this class Prim, but Visual C++
	* won't compile if I use that name.
	*
	*/
	template<class Data>
	class PrimP : public Param {
	public:
		/*!	\function virtual ~PrimP(void)
		*	\brief Destructor.
		*
		* This is the dummy destructor for this class
		*/
		virtual ~PrimP(void) { }

		/*!	\function virtual operator Data ()
		*	\brief Casts as Data.
		*
		* This routine casts this object as a primitive of type Data.
		* This routine allows functions to treat this object
		* as if it were a primitive of type data.
		* Return:
		*		primitive value of this object.
		*/
		virtual operator Data () = 0;

		/*!	\function virtual Data getValue(void)
		*	\brief Gets the value.
		*
		* This routine returns this Prim object's primitive value.
		* This function is useful when
		* C++ won't let us cast a Prim<Data> as a Data primitive.
		* This can happen when we dereference a pointer to a
		* Prim<Data> object.
		* Returns:
		*		The primitive value of this Prim.
		*/
		virtual Data getValue(void) const = 0;

		/*!	\function virtual void setValue(Data inValue)
		*	\brief Sets the value.
		*
		* This routine sets the value.
		* Parameters:
		*		inValue:		the value to set this PrimParam to.
		*/
		virtual void setValue(Data inValue) = 0;

	};	// end class PrimP


	/*! \class PrintOptions
	*	\brief  Print options for an RPC
	*
	*
	*
	* The PrintOptions class encapsulates several different test options.
	* At most one of these objects should exist in a program execution;
	* It will be pointed to by the static PrintOptions pointer.
	* Param classes' print() functions reference the PrintOptions
	* to see how to print themselves.
	*
	*/
	class PrintOptions : public BaseObject {
	public:
		/*!	\function virtual ~PrintOptions(void)
		*	\brief Destructor.
		*
		* This is the destructor.
		*/
		virtual ~PrintOptions(void) {}

		/*!	\function virtual const TString &getUseString(void)
		*	\brief Gets the print options available.
		*
		* This routine gets a string specifying the options available
		* to this PrintOptions implementation.  These are the options
		* which can be passed to the setOptions routine.
		* Returns:
		*		A pointer to the string specifying this PrintOption's
		*		possible settings.
		*/
		virtual const TString &getUseString(void) const = 0;

		/*!	\function virtual void reset(void)
		*	\brief Reset PrintOptions.
		*
		* This routine resets the options in this PrintOptions object.
		*/
		virtual void reset(void) = 0;

		/*!	\function virtual bool setOptions(int argc, const char **argv)
		*	\brief Sets the options from the passed array.
		*
		* This routine sets the options in this PrintOptions object.
		* It parses the input arg array, ignoring arguments it does
		* not understand.  The arguments possible are:
		*	-vm      == print Meshes' vertices and faces
		*	-vtm     == print TextureMeshes' vertices and faces
		*	-op name == prefix for output preview files
		*	-na #    == Number of angles from which to take test textures
		*	-aa #    == Specific angle from which to take test texture
		*	-nt #    == Number of time steps from which to take test textures
		*	-ot name == prefix for output texture files
		*	-c #     == maximum number of entries per list to print
		*	-ex name == list entries with this name will not be printed
		* Parameters:
		*		argc:	The number of arguments in the array
		*		argv:	The arguments to parse.
		* Returns:
		*		true iff the argument array was successfully parsed.
		*/
		virtual bool setOptions(int argc, const char **argv) = 0;

		/*!	\function virtual void setRPC(ParamList *rpc, TStringArg fileName)
		*	\brief Sets the main RPC list and file name.
		*
		* This routine sets the main RPC list and the file name
		* of the RPC which will be printed.
		* Parameters:
		*		rpc:		The main ParamList of the RPC.
		*		fileName:	The name of the RPC file.
		*/
		virtual void setRPC(ParamList *rpc, TStringArg fileName) = 0;

	};	// end class PrintOptions



	/*! \class Texture
	*	\brief bitmap image of an instance
	*
	*
	* A Texture represents a bitmap image retrieved from an Instance.
	*
	*
	*/
	class Texture : public Param {
	public:
		// The Scale class wraps all of the scale modes
		// which may be used when retrieving texture images from an RPC.
		// FAST indicates that the image should be scaled by a power of two
		// which most closely matches the user's input dimensions.
		// SLOW indicates that the user's exact dimensions should be used.
		// To use in your code, say "RPCapi::Texture::Scale::FAST", or
		// whatever scale mode is appropriate.
		class Scale {
		public:
			typedef enum SCALE_CODE {
				FAST = 1,
				SLOW = 2
			} SCALE_CODE_T;	// end typedef enum SCALE_CODE

		};	// end class Scale

		// These are the possible channels which can be retrieved.
		// To use in your code, say "RPCapi::Texture::Channel::RGB"
		class Channel {
		public:
			typedef enum CHANNEL_CODE {
				RGB = 0,
				ALPHA = 1
			} CHANNEL_CODE_T;	// end typedef enum CHANNEL_CODE
		};	// end class Channel

		// These are the possible values for the FIELD parameter
		// of the property routine below:
		typedef enum PROPERTY_NAMES {
			SHININESS = 0,
			SHININESS_STRENGTH = 1,
			SELF_ILLUMINATION = 2,
			TWO_SIDED = 3
		} PROPERTY_NAMES_T;	// end typedef enum PROPERTY_NAMES


		/*!	\function virtual ~Texture(void)
		*	\brief Destructor.
		*
		* This is the dummy destructor for this interface
		*/
		virtual ~Texture(void) {}

		/*!	\function virtual Texture *backFaceTexture(void)
		*	\brief Gets the texture for backfaces.
		*
		* This routine returns a pointer to the texture for backfaces,
		* if this Texture possesses such.  If not, NULL is returned.
		* Returns:
		*		A pointer to the back-face Texture, or NULL
		*		if there is no back-face Texture.
		*/
		virtual Texture *backFaceTexture(void) = 0;

		/*!	\function virtual int data(unsigned char *&buffer, bool allocBuffer, int channel, int scaleMode, int &width, int &height)
		*	\brief Retrieves texture data.
		*
		* This routine retrieves data from this Texture.
		* This routine allocates the data array used to store
		* this Texture, fills it in with the appropriate data,
		* and returns a pointer to the array.
		* It is the client application's responsibility to
		* deallocate the data array when finished with it.
		* This routine returns the size of the buffer array.
		* Parameters:
		*		buffer:		buffer in which the image data is placed.
		*					This buffer is allocated inside this function,
		*					and a pointer to it is returned by reference.
		*		allocBuffer:if true, this routine allocates buffer on its own;
		*					if false, this routine fills in the buffer passed
		*					to it, or returns the size needed if that buffer
		*					is NULL.
		*		channels:	specifies which channels to include in the
		*					texture data returned (see typedef Channels above).
		*		scaleMode:	specifies which scale mode to use.
		*					(see typedef ScaleMode above).
		*		width:		The width to which the texture is to be scaled.
		*					If ScaleMode is set to SCALE_FAST, the returned
		*					texture's width will be scaled to the nearest power
		*					of 2.  Set to 0 if you do not want the texture
		*					to be scaled.
		*		height:		The height to which the texture is to be scaled.
		*					If ScaleMode is set to SCALE_FAST, the returned
		*					texture's height will be scaled to the nearest power
		*					of 2.  Set to 0 if you do not want the texture
		*					to be scaled.
		* Returns:
		*		The number of bytes used or required to store the requested,
		*		image, or < 0 on an error.
		*/
		virtual int data(unsigned char *&buffer,
			bool allocBuffer,
			int channel,
			int scaleMode,
			int &width,
			int &height
			) = 0;


		/*!	\function virtual const TString &getID(void)
		*	\brief Gets the texture identifier.
		*
		* This routine retrieves the unique identifier for this Texture.
		* This identifier includes all information used in producing
		* the image data of this Texture.
		* Returns:
		*		The unique identifier string for this Texture.
		*/
		virtual const TString &getID(void) const = 0;

		/*!	\function virtual bool hasChannel(int channel)
		*	\brief Does the texture have an alpha channel?
		*
		* This routine determines whether this Texture has an Alpha channel.
		* Parameters:
		*		channel:	The channel to check for (can be RGB or ALPHA).
		* Returns:S
		*		true iff this Texture contains the specified channel.
		*/
		virtual bool hasChannel(int channel) const = 0;

		/*!	\function virtual bool property(int FIELD, double &value)
		*	\brief is the field specified by this texture?
		*
		* This routine returns true if the indicated field is specified
		* by this Texture, and false if it is not.  If the field is
		* specified, its value is cast to a double and returned by
		* reference, and should be applied by the client application.
		* Parameters:
		*		FIELD:	specifies which field to check
		*				The valid values for field are specified above.
		*		value:	If this Texture contains the indicated property,
		*				its value is returned as a double through value.
		* Returns:
		*		true iff this Texture specifies the indicated property.
		*/
		virtual bool property(int FIELD, double &value) const = 0;

	};	// end class Texture


	/*! \class TextureMesh
	*	\brief Array of 2D vertecies and faces
	*
	* This is the base interface for the Texture Meshes contained in an RPC.
	* A TextureMesh is composed of an array of 2D texture vertices
	* (that contain an index into a Mesh that refers to a specific
	* 3D Vertex) and 2D texture faces.  Each face is composed of three
	* texture vertices and a texture index.
	*
	*
	*/
	class TextureMesh : public Param {
	public:

		/*!	\class Face
		*	\brief Contains vertices and identifier.
		*
		* A Face object contains three integers, which are the indices
		* of the texture vertices composing the face, and the identifier
		* for the texture which should be used to paint the face.
		* If the TextureMesh is retrieved directly from an RPC file,
		* this identifier will be a float (corresponding to the
		* horizontal viewing angle from which the texture was taken).
		* If the TextureMesh is retrieved from an RPCapi::Instance using
		* getTextures(), the identifier is an int, corresponding to
		* the index in the array of Textures that points to the Texture
		* to use to paint the face.
		*/
		class Face {
		public:
			int v0, v1, v2;
			union {
				int textureIndex;
				float textureAngle;
			} u;

		};	// end class TextureMesh::Face

		/*!	\class Vertex
		*	\brief 2D coordinate of the vertex.
		* A Vertex object contains the 2-D coordinates of a texture vertex
		* and the index of the vertex to which the texture Vertex corresponds.
		*/
		class Vertex {
		public:
			double x, y;

		};	// end class TextureMesh::Vertex


		/*!	\function virtual ~TextureMesh(void)
		*	\brief Destructor.
		*
		* This is the dummy destructor for this class.
		*/
		virtual ~TextureMesh(void) {}

		/*!	\function virtual const Face *getConstFaces(void)
		*	\brief Gets the faces (const)
		*
		* This routine gets this TextureMesh's array of faces.
		* Returns:
		*		The pointer to this TextureMesh's Faces array.  Note that if
		*		this TextureMesh is an Accessor, this routine
		*		can only return NULL.
		*/
		virtual const Face *getConstFaces(void) const = 0;

		/*!	\function virtual const Vertex *getConstVertices(void)
		*	\brief Gets the vertices (const).
		*
		* This routine gets this TextureMesh's array of vertices.
		* Returns:
		*		The pointer to this TextureMesh's Vertices array.  Note that if
		*		this TextureMesh is an Accessor, this routine
		*		can only return NULL.
		*/
		virtual const Vertex *getConstVertices(void) const = 0;

		/*!	\function virtual Face *getFaces(void)
		*	\brief Gets the faces.
		*
		* This routine gets this TextureMesh's array of faces.
		* Returns:
		*		A newly allocated array of faces, which the caller is
		*		responsible for deallocating, or NULL if an error occurred.
		*/
		virtual Face *getFaces(void) const = 0;

		/*!	\function virtual int getNumFaces(void)
		*	\brief Gets the number of faces.
		*
		* This routine gets the number of faces in this TextureMesh
		* Returns:
		*		The number of faces in this TextureMesh,
		*		or < 0 if an error occurred.
		*/
		virtual int getNumFaces(void) const = 0;

		/*!	\function virtual int getNumVerts(void)
		*	\brief Returns the number of vertices.
		*
		* This routine gets the number of vertices in this TextureMesh
		* Returns:
		*		The number of vertices in this TextureMesh,
		*		or < 0 if an error occurred.
		*/
		virtual int getNumVerts(void) const = 0;

		/*!	\function virtual Vertex *getVerts(void)
		*	\brief Gets the vertices.
		*
		* This routine gets this TextureMesh's array of vertices.
		* Returns:
		*		A newly allocated array of vertices, which the caller is
		*		responsible for deallocating, or NULL if an error occurred.
		*/
		virtual Vertex *getVerts(void) const = 0;

		/*!	\function virtual bool setFaces(Face *faces, int numFaces)
		*	\brief Sets the faces.
		*
		* This routine sets the faces in this TextureMesh
		* Note that if this TextureMesh is a Stream Accessor,
		* this routine will do nothing.
		* Parameters:
		*		faces:		The faces to store in this TextureMesh
		*		numFaces:	The number of faces in the input faces array
		* Returns:
		*		true iff the faces were successfully stored.
		*/
		virtual bool setFaces(Face *faces, int numFaces) = 0;

		/*!	\function virtual bool setVerts(Vertex *verts, int numVerts)
		*	\brief Sets the verticies.
		*
		* This routine sets the vertices in this TextureMesh
		* Note that if this TextureMesh is a Stream Accessor,
		* this routine will do nothing.
		* Parameters:
		*		verts:		The vertices to store in this TextureMesh
		*		numVerts:	The number of vertices in the input faces array
		* Returns:
		*		true iff the vertices were successfully stored.
		*/
		virtual bool setVerts(Vertex *verts, int numVerts) = 0;

	};	// end class TextureMesh


	/*! \class Vector
	*	\brief 3D vector
	*
	*
	* 3D vector, with x, y, and z components.
	*
	*/
	class Vector : public Param {
	public:
		// Destructor
		virtual ~Vector(void) {}

		// These routines get / set the components of the vector.
		virtual void get(double &ox, double &oy, double &oz) const = 0;
		virtual void set(double ix, double iy, double iz) = 0;
		virtual double xget(void) const = 0;
		virtual void xset(double n) = 0;
		virtual double yget(void) const = 0;
		virtual void yset(double n) = 0;
		virtual double zget(void) const = 0;
		virtual void zset(double n) = 0;

	};	// end class Vector


	// This is the destructor
	virtual ~RPCapi(void) {}

#ifdef RPC_CONTENT_CLIENT
	/*! \function virtual void about(TString *product, TString *version, TString *copyright, TString *etc)
	*	\brief Retrieves info from the module.
	*
	* This routine retrieves about info from this module.
	* If a requested component (product, version, etc...) is
	* NULL, then it is ignored.
	* Parameters:
	*		product:	Product name / description.
	*		version:	Version number.
	*		copyright:	Copyright info.
	*		etc:		Additional info.
	*
	*/
	virtual void about(TString *product, TString *version,
		TString *copyright, TString *etc) const = 0;
#endif	// ifdef RPC_CONTENT_CLIENT


	/*! \function bitmapToMSBitmap
	*
	*	\brief Converts RGB bitmap into MS bitmap.
	*
	*
	* This routine converts an RGB bitmap into an MS-format bitmap.
	* It allocates the new bitmap memory, so the caller is responsible
	* for deallocation.
	* NOTE:  The data returned can be written directly to a file,
	* which is then viewable as a MicroSoft bitmap.
	* This routine returns the size of the bitmap array.
	* Parameters:
	*		bitmap:		The bitmap bytes.
	*		width:		The horizontal resolution of the bitmap.
	*		height:		The vertical resolution of the bitmap.
	*		msBitmap:	The new MS-format bitmap is allocated
	*					by this routine and returned through this
	*					reference parameter.
	* Returns:
	*		The size in bytes of msBitmap.
	*
	*/
	virtual int bitmapToMSBitmap(unsigned char *bitmap, int width, int height,
		unsigned char *&msBitmap) const = 0;

	/*! \function clearErrorLog(void)
	*	\brief Clears the error log.
	*
	* This routine clears the error log for this RPCapi object.
	*/
	virtual void clearErrorLog(void) = 0;

	/*! \function virtualClient *getClient(void)
	*	\brief Gets the pointer to the client object.
	*
	* This routine returns a pointer to the client of this API object.
	* Returns:
	*		A pointer to this API's client object.
	*/
	virtual Client *getClient(void) const = 0;

	/*! \function virtual const TString &getErrorLog(void)
	*	\brief Returns error log for an RPCapi object.
	*
	* This routine returns the error log for this RPCapi object.
	* The error log may contain useful debugging information should
	* the RPCapi return an unexpected error.
	* Returns:
	*		The current error log, as a string.
	*/
	virtual const TString &getErrorLog(void) const = 0;

	/*! \function virtual const License *getLicense(void)
	*	\brief Gets a pointer to a license.
	*
	* This routine returns a pointer to the License of the current DLL.
	* The returned License contains the license information about this DLL,
	* specifying COMMERCIAL, EDUCATIONAL, etc.  See class RPCapi::License
	* for details.
	* Returns:
	*		A pointer to this DLL's License.
	*/
	virtual const License *getLicense(void) const = 0;

	/*! \function virtual BaseObject *newObject(int objectCode)
	*	\brief Makes a new object.
	*
	* This routine allocates a new object of the input object code
	* and returns a pointer to it.  The param codes for implementations
	* of each of the available classes are listed near the top
	* of this file, just after the class declarations.
	* Parameters:
	*		objectCode:	The object code of the desired object.
	*					See class ObjectCode, above.
	* Returns:
	*		A pointer to the newly-allocated object, or NULL if
	*		an error occured (such as an unrecognized object code).
	*/
	virtual BaseObject *newObject(int objectCode) const = 0;

	/*! \function virtual Param *newParam(int typeCode)
	*	\brief  Makes a new Param object.
	*
	* This routine allocates a new object of the input type code and
	* returns a pointer to it.
	* Parameters:
	*		typeCode:	The type code of the desired object.
	* Returns:
	*		A pointer to the newly-allocated object, or NULL if
	*		an error occured (such as an unrecognized typeCode).
	*/
	virtual Param *newParam(int typeCode) const = 0;

	/*! \function virtual TString *newTString(void)
	*	\brief Makes a new TString object.
	*
	* These routines allocate a new TString object.
	* Parameters:
	*		str:	The constant string value to assign the TString.
	*		strp:	The string pointer for the new TString.
	*/
	virtual TString *newTString(void) const = 0;
	virtual TString *newTString(const char *str) const = 0;
	virtual TString *newTStringPtr(char *str) const = 0;
#ifdef RPC_WIDE_STRINGS
	virtual TString *newTString(const wchar_t *str) const = 0;
	virtual TString *newTStringPtr(wchar_t *str) const = 0;
#endif	// ifdef RPC_WIDE_STRINGS

#ifdef RPC_CONTENT_CLIENT
	/*!	\function virtual void *notify(int event, ...)
	*	\brief
	*
	* This is a catch-all routine that allows the client to
	* inform the API about various events.  Any events are purely optional.
	* Parameters:
	*		event:	The event that occurred.  Possible values:
	*				0:  no event.
	*				1:  a scene is beginning to load from a save file.
	*				2:  a scene has finished loading from a save file.
	*				3:  suppress initial updateACM call.
	*				4:  set this API to connect to an ACM
	*				    if Texture::data() cannot open a file from a
	*				    deserialized Texture.
	*				5:  unset the flag set in notify(4).
	*				6:  Flag RPCapi to initialize its ACM settings
	*				    according to this flow when it first attempts
	*				    to read its INI file:
	*				    If an INI file is present, read it.  Otherwise, ...
	*				    If a local ACM is found, use it.  Otherwise, ...
	*				    Scan for ACMs.
	*					If exactly 1 is found, use it.
	*				    If > 1 is found, display ACM selection dialog.
	*				    If < 1 is found, display content settings dialog.
	*				7:  launch a dialog to configure ACM location
	*				    w/out contacting the ACM.
	*					args:  phwnd (parent window)
	*		...:	Additional arguments, dependent on event,
	*				as documented above.
	* Returns:
	*		Dependent on the event, above.
	*
	*/
	virtual void *notify(int event, ...) = 0;
#endif	// ifdef RPC_CONTENT_CLIENT

	/*! \function virtual RPCapi::ParamList *openRPCFile(TStringArg fileName)
	*	\brief Returns Opens an RPC and gets Params.
	*
	* This routine opens an RPC file and returns the ParamList associated
	* with the open stream.
	* Parameters:
	*		fileName:	The name of the RPC file.
	* Returns:
	*		A pointer to the newly allocated ParamList, or NULL on an error.
	*/
	virtual RPCapi::ParamList *openRPCFile(TStringArg fileName) const = 0;

	/*! \function virtual void setClient(Client *CI)
	*	\brief Sets pointer to ClientInstance.
	*
	* This routine sets the pointer to this Instance's ClientInstance.
	* Parameters:
	*		client:	A pointer to the client code with which this
	*				API object should be paired.
	*/
	virtual void setClient(Client *CI) = 0;

	/*! \function virtual void setCreator(Creator *creator)
	*	\brief Sets creator being used.
	*
	* This routine sets the Creator currently being used to create an RPC.
	* Note that this routine can only be used if the license of the
	* RPCapi.dll includes the Creator license.
	* Parameters:
	*		creator:		A pointer to the Creator being used.
	*/
	virtual void setCreator(Creator *creator) = 0;

#ifdef RPC_CONTENT_CLIENT
	/*! \function virtual void updateACM(int acmType = 0, TStringArg acmExe = NULL, TStringArg acmDNS = NULL, short acmPort = 0)
	*	\brief Reconnects to ACM to get updates.
	*
	* This function tells the global RPCapi object that it should
	* reconnect to the ACM and request content.
	* NOTE:  This function will cause Client::RPCgetPaths() to be called.
	* Parameters:
	*		acmType:	Type of the ACM.
	*					0 == unspecified.
	*					1 == locally started by client.
	*					2 == network.
	*					3 == none.
	*					4 == locally started by client if not already specified.
	*					5 == network if not already specified.
	*					6 == none if not already specified.
	*		acmExe:		If not NULL, exe of ACM to launch locally.
	*		acmDNS:		If not NULL, the network location of the ACM.
	*		acmPort:	If not 0, the port of the ACM.
	*/
	virtual void updateACM(int acmType = 0, TStringArg acmExe = NULL,
		TStringArg acmDNS = NULL, short acmPort = 0) = 0;
#endif	// ifdef RPC_CONTENT_CLIENT

	/*! \function virtual void updatePaths(void)
	*	\brief Updates path list to get new RPC files.
	*
	* This function tells the global RPCapi object that it should update
	* its paths containing RPC files.  The client code should call
	* this function whenever it desires the RPCapi to look in new directories
	* for RPC files.
	* NOTE:  This function will cause Client::RPCgetPaths() to be called.
	*/
	virtual void updatePaths(void) = 0;

#ifdef RPC_CONTENT_CLIENT
	/*! \function virtual bool userMessage(int msgType, bool ret, TStringArg title, TStringArg msg, ...)
	*	\brief Display a message to the user.
	*
	* The RPC API calls this routine when it has some message
	* that it wishes to display to the user.
	* Parameters:
	*		msgType:	The type of this message.
	*					1:  generic information.
	*					2:  warning
	*					3:  error.
	*					4:  catastrophic error (we're going to die).
	*		ret:		If true, we need a yes-no response.
	*		title:		If a window will display the message, use this title.
	*		msg:		The message to display.
	*		.../args:	Additional message args, printf style.
	* Returns:
	*		If ret is true, true or false (depending on the user's response).
	*		If the message will not be displayed, or if ret is false,
	*		return false.
	*/
	virtual bool userMessage(int msgType, bool ret,
		TStringArg title, TStringArg msg, ...) const = 0;
	virtual bool userMessageV(int msgType, bool ret,
		TStringArg title, TStringArg msg, va_list args) const = 0;
#endif	// ifdef RPC_CONTENT_CLIENT

	// This is the typedef describing the function exported by the
	// RPCapi dynmamically-linked library.
#if (defined (_MSC_VER) && _MSC_VER >= 1310)
	typedef RPCapi *(*RPCgetAPIFuncType)(void *rpcCLS);
#else
	typedef RPCapi *(*RPCgetAPIFuncType)(void *rpcCLS = NULL);
#endif

#ifdef PORT_WINDOWS
	/*!	\function static RPCapi *RPCgetAPI(void *rpcCLS = NULL)
	*	\brief Makes a new RPCapi object.
	*
	* This function allocates a new RPCapi object and returns
	* a pointer to it.  Be sure to delete this pointer on
	* program termination.
	* To use this function, use code similar to the following
	* (in Windows):
	*		HMODULE rpcDLL = LoadLibrary("RPCapi.dll");
	*		RPCapi::RPCgetAPIFuncType RPCgetAPIFilePtr =
	*			(RPCapi::RPCgetAPIFuncType)
	*			GetProcAddress(rpcDLL, "RPCgetAPI");
	*		...
	*		FreeLibrary(rpcDLL);
	* Parameters:
	*		rpcCLS:	FOR INTERNAL USE ONLY.
	*				Pointer to the ContentClient associated with
	*				this RPCapi.  If NULL, the returned RPCapi will
	*				spawn an RPCcls with which it will obtain content.
	* Returns:
	*		A pointer to the global API object.
	*/
	static RPCapi *RPCgetAPI(void *rpcCLS = NULL);

#endif	// ifdef PORT_WINDOWS

};	// end class RPCapi


/*!	\function extern "C" RPCapi *RPCgetAPI(void *rpcCLS = NULL)
*	\brief  Makes a new RPCapi object.
*
* This function allocates a new RPCapi object and returns
* a pointer to it.  Be sure to delete this pointer on
* program termination.
* To use this function, use code similar to the following
* (in Unix):
*		void *rpcDLL = dlopen("libRPCapi.so", RTLD_LAZY);
*		RPCapi::RPCgetAPIFuncType RPCgetAPIFilePtr =
*			(RPCapi::RPCgetAPIFuncType)
*			dlsym(rpcDLL, "RPCgetAPI");
*		...
*		dlclose(rpcDLL);
* Parameters:
* Parameters:
*		rpcCLS:	FOR INTERNAL USE ONLY.
*				Pointer to the ContentClient associated with
*				this RPCapi.  If NULL, the returned RPCapi will
*				spawn an RPCcls with which it will obtain content.
* Returns:
*		A pointer to the global API object.
*/
#ifdef PORT_MAC_OR_UNIX
#ifdef PORT_MAC
#pragma export on
#endif	// ifdef PORT_MAC
extern "C" RPCapi *RPCgetAPI(void *rpcCLS = NULL);
#ifdef PORT_MAC
#pragma export off
#endif	// ifdef PORT_MAC
#endif	// ifdef PORT_MAC_OR_UNIX


#ifdef PORT_WINDOWS
const char BeginRPCapiKeyboardControl[] =
"RPCapi Begin Keyboard Control";
const char EndRPCapiKeyboardControl[] =
"RPCapi End Keyboard Control";
#ifdef IMPLEMENT_THUMBNAIL_BROWSER
const char RegisterModelessDialogWithClient[] =
"RPCapi Register Modeless Dialog With Client";
#endif  // ifdef IMPLEMENT_THUMBNAIL_BROWSER
#endif	// ifdef PORT_WINDOWS


typedef RPCapi::TString TString;
typedef RPCapi::TStringArg TStringArg;

// These macros encode and extract a TString from an ellipsis argument.
// str is a TString reference.
inline const TString *TSA(const TString &ts) { return &ts; }
inline TString *TSAU(va_list &args) { return va_arg(args, TString *); }

#endif	// RPCAPI_H
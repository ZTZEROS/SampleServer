#pragma once

//#define _CRT_SECURE_NO_WARNINGS
//#define _CRT_SECURE_NO_DEPRECATE
//#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN

#define CARRAGE_RETURN_AND_LINE_FEED "\r\n" //const char const* CRLF = "\r\n";
#define CRALF CARRAGE_RETURN_AND_LINE_FEED
#define CRLF CRALF

//#define UNICODE
//#define _UNICODE

#define STRINGIZING(text) (#text)
#define WIDE_STRINGIZING(text) (L## #text)
#define MERGE_TEXT(leftText, rightText) (leftText##rightText)
#define SHOW_TEXT(text) cout << text << endl

#define IF_ERROR(function) if(function) { cout << STRINGIZING(function) << " failed." << "(" << WSAGetLastError() << ", "<< __LINE__ << ")" << endl; }
#define IF_NULL(function) if(function == NULL) { cout << STRINGIZING(function) << " failed." << "(" << WSAGetLastError() << ", "<< __LINE__ << ")" << endl; }
#define IF_INVALID_SOCKET(socket) if(socket == INVALID_SOCKET) { cout << STRINGIZING(socket) << " is invalid." << "(" << WSAGetLastError() << ", "<< __LINE__ << ")" << endl; }
#define IF_SOCKET_ERROR(function) if(function == SOCKET_ERROR) { cout << STRINGIZING(function) << " failed." << "(" << WSAGetLastError() << ", "<< __LINE__ << ")" << endl; }
#define IF_WSAEWOULDBLOCK(function) if(function == SOCKET_ERROR) { if(WSAGetLastError() == WSAEWOULDBLOCK) { } else { cout << STRINGIZING(function) << " failed." << "(" << WSAGetLastError() << ", "<< __LINE__ << ")" << endl; }}
#define IF_WSA_IO_PENDING(function) if(function == SOCKET_ERROR) { if(WSAGetLastError() == WSA_IO_PENDING) { } else { cout << STRINGIZING(function) << " failed." << "(" << WSAGetLastError() << ", "<< __LINE__ << ")" << endl; }}

#define ELSE_IF(condition) else if(condition)
#define ELSE else

#define CHECK_ERROR(function) { if(function) { cout << STRINGIZING(function) << " failed." << "(" << WSAGetLastError() << ", "<< __LINE__ << ")"<< endl; return 1; } }
#define CHECK_NULL(function) if(!function) { cout << STRINGIZING(function) << " failed." << "(" << WSAGetLastError() << ", "<< __LINE__ << ")" << endl; }
#define CHECK_INVALID_SOCKET(socket) { if(socket == INVALID_SOCKET) { cout << STRINGIZING(socket) << " is invalid." << "(" << WSAGetLastError() << ", "<< __LINE__ << ")" << endl; return 1; } }
#define CHECK_SOCKET_ERROR(function) { if(function == SOCKET_ERROR) { cout << STRINGIZING(function) << " failed." << "(" << WSAGetLastError() << ", "<< __LINE__ << ")"<< endl; return 1; } }
#define CHECK_WSAEWOULDBLOCK(function) if(function == SOCKET_ERROR) { if(WSAGetLastError() == WSAEWOULDBLOCK) { } else { cout << STRINGIZING(function) << " failed." << "(" << WSAGetLastError() << ", "<< __LINE__ << ")" << endl; return 1; } }

//#define CHECK_ERROR(function) { IF_ERROR(function) }
//#define CHECK_INVALID_SOCKET(socket) { IF_INVALID_SOCKET(socket) }
//#define CHECK_SOCKET_ERROR(function) { IF_SOCKET_ERROR(function) }

//#define CHECK_ERROR(function) { if(function) SHOW_TEXT(MERGE_TEXT(STRINGIZING(function), " failed.")); }

#define FOR(currentCount, maximumCount) for(unsigned int currentCount = 0; currentCount < maximumCount; ++currentCount)

#define GETTER(datumType, variableName) datumType Get##variableName##() { return variableName; }
#define SETTER(datumType, variableName) void Set##variableName##(datumType value) { variableName = value; }

#define PRINT_ERROR(errorLevel, errorDetail)
#define LOG_ERROR(errorLevel, errorDetail)

#define ALLOCATE_MEMORY(datumType, count) { AllocateMemory<datumType>(count, __FILE__, __LINE__); }
#define DEALLOCATE_MEMORY(address) { DeallocateMemory(address, __FILE__, __LINE__); }

#define x86_64
//#define x86_32
//#define ADDRESS64 unsigned long long int
//#define ADDRESS32 unsigned long int

#ifdef x86_64
#define ADDRESS ADDRESS64
#else
#define ADDRESS ADDRESS32
#endif

//#ifdef FD_SETSIZE
//#define FD_SETSIZE CLIENT_SOCKET_COUNT
//#endif
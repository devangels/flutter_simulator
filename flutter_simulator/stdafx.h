// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include "include\dart_api.h"

#if defined(GLFW_TRACING)
#include "include\dart_tools_api.h"
#define TRACE_START(name)                                \
  Dart_TimelineEvent(#name, Dart_TimelineGetMicros(), 0, \
                     Dart_Timeline_Event_Begin, 0, NULL, NULL)
#define TRACE_END(name)                                  \
  Dart_TimelineEvent(#name, Dart_TimelineGetMicros(), 0, \
                     Dart_Timeline_Event_End, 0, NULL, NULL)
#else
#define TRACE_START(name) \
  do {                    \
  } while (0)
#define TRACE_END(name) \
  do {                  \
  } while (0)
#endif

Dart_Handle HandleError(Dart_Handle handle);

#if defined(GLFW_TESTING)
#define HANDLE(handle) HandleError(handle)
#else
#define HANDLE(handle) handle
#endif

// Extracts a pointer to a GLFW object from a Dart wrapper class.
template <typename T>
T* GetNativePointer(Dart_Handle obj) {
	if (Dart_IsNull(obj)) {
		return nullptr;
	}
	Dart_Handle wrapper = HANDLE(Dart_GetField(obj, Dart_NewStringFromCString("_nativePtr")));
	int64_t native_value;
	HANDLE(Dart_IntegerToInt64(wrapper, &native_value));
	intptr_t ptr_value = static_cast<intptr_t>(native_value);
	return reinterpret_cast<T*>(ptr_value);
}

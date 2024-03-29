#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "GLFW\glfw3.h"
#include "GLFW\glfw3native.h"
#include "flutter_embedder.h"


Dart_NativeFunction ResolveName(Dart_Handle name, int argc, bool* auto_setup_scope);
Dart_Handle HandleError(Dart_Handle handle);

static Dart_Handle DesktopSimulatorLibrary;
static Dart_Handle CoreLibrary;
static Dart_Handle MathLibrary;

static Dart_Handle makeCurrentString;

DART_EXPORT Dart_Handle flutter_simulator_Init(Dart_Handle parent_library)
{
	if (Dart_IsError(parent_library)) {
		return parent_library;
	}

	Dart_Handle result_code = Dart_SetNativeResolver(parent_library, ResolveName, NULL);
	if (Dart_IsError(result_code)) {
		return result_code;
	}

	DesktopSimulatorLibrary = HandleError(Dart_NewPersistentHandle(HandleError(
		Dart_LookupLibrary(Dart_NewStringFromCString("package:desktop_simulator/src/glfw.dart"))
	)));

	CoreLibrary = HandleError(Dart_NewPersistentHandle(HandleError(
		Dart_LookupLibrary(Dart_NewStringFromCString("dart:core"))
	)));

	MathLibrary = HandleError(Dart_NewPersistentHandle(HandleError(
		Dart_LookupLibrary(Dart_NewStringFromCString("dart:math"))
	)));

	makeCurrentString = Dart_NewPersistentHandle(
		HANDLE(Dart_NewStringFromCString("makeCurrent"))
	);

	return Dart_Null();
}

Dart_Handle NewGlfwWindow(GLFWwindow *window)
{
	if (window == NULL) return Dart_Null();

	Dart_Handle GLFWwindow_type = HandleError(Dart_GetType(
		DesktopSimulatorLibrary, Dart_NewStringFromCString("Window"), 0, NULL));

	Dart_Handle args[1];
	intptr_t ptr_value = reinterpret_cast<intptr_t>(window);
	args[0] = Dart_NewInteger(static_cast<int64_t>(ptr_value));
	return HandleError(Dart_New(GLFWwindow_type, Dart_Null(), 1, args));
}

Dart_Handle NewGlfwMonitor(GLFWmonitor *monitor)
{
	Dart_Handle GLFWmonitor_type = HandleError(Dart_GetType(
		DesktopSimulatorLibrary, Dart_NewStringFromCString("Monitor"), 0, NULL));

	Dart_Handle args[1];
	intptr_t ptr_value = reinterpret_cast<intptr_t>(monitor);
	args[0] = Dart_NewInteger(static_cast<int64_t>(ptr_value));
	return HandleError(Dart_New(GLFWmonitor_type, Dart_Null(), 1, args));
}

Dart_Handle NewGlfwVideoMode(const GLFWvidmode *vidmode)
{
	Dart_Handle GLFWvidmode_type = HandleError(Dart_GetType(
		DesktopSimulatorLibrary, Dart_NewStringFromCString("VideoMode"), 0, NULL));

	Dart_Handle arguments[6];
	arguments[0] = Dart_NewInteger(vidmode->width);
	arguments[1] = Dart_NewInteger(vidmode->height);
	arguments[2] = Dart_NewInteger(vidmode->redBits);
	arguments[3] = Dart_NewInteger(vidmode->greenBits);
	arguments[4] = Dart_NewInteger(vidmode->blueBits);
	arguments[5] = Dart_NewInteger(vidmode->refreshRate);

	return HandleError(Dart_New(GLFWvidmode_type, Dart_Null(), 6, arguments));
}

Dart_Handle NewPoint(double x, double y)
{
	Dart_Handle double_type = HandleError(
		Dart_GetType(CoreLibrary, Dart_NewStringFromCString("double"), 0, NULL));
	Dart_Handle type_args = HandleError(Dart_NewList(1));
	HandleError(Dart_ListSetAt(type_args, 0, double_type));
	Dart_Handle Point_type = HandleError(Dart_GetType(
		MathLibrary, Dart_NewStringFromCString("Point"), 1, &type_args));

	Dart_Handle arguments[2];
	arguments[0] = Dart_NewDouble(x);
	arguments[1] = Dart_NewDouble(y);
	return HandleError(Dart_New(Point_type, Dart_Null(), 2, arguments));
}

Dart_Handle NewRectangle(int left, int top, int width, int height)
{
	Dart_Handle int_type = HandleError(
		Dart_GetType(CoreLibrary, Dart_NewStringFromCString("int"), 0, NULL));
	Dart_Handle type_args = HandleError(Dart_NewList(1));
	HandleError(Dart_ListSetAt(type_args, 0, int_type));
	Dart_Handle Rectangle_type = HandleError(Dart_GetType(
		MathLibrary, Dart_NewStringFromCString("Rectangle"), 1, &type_args));

	Dart_Handle arguments[4];
	arguments[0] = Dart_NewInteger(left);
	arguments[1] = Dart_NewInteger(top);
	arguments[2] = Dart_NewInteger(width);
	arguments[3] = Dart_NewInteger(height);

	return HandleError(Dart_New(Rectangle_type, Dart_Null(), 4, arguments));
}

void glfwInit_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwInit_);

	int ret = glfwInit();
	Dart_SetBooleanReturnValue(arguments, ret);

	TRACE_END(glfwInit_);
}

void glfwTerminate_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwTerminate_);

	glfwTerminate();

	TRACE_END(glfwTerminate_);
}

void glfwGetVersionString_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwGetVersionString_);

	const char* ret = glfwGetVersionString();
	Dart_SetReturnValue(arguments, HANDLE(Dart_NewStringFromCString(ret)));

	TRACE_END(glfwGetVersionString_);
}

void glfwGetTime_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwGetTime_);

	double ret = glfwGetTime();
	Dart_SetDoubleReturnValue(arguments, ret);

	TRACE_END(glfwGetTime_);
}

void glfwGetPrimaryMonitor_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwGetPrimaryMonitor_);

	GLFWmonitor* ret = glfwGetPrimaryMonitor();
	Dart_SetReturnValue(arguments, HANDLE(NewGlfwMonitor(ret)));

	TRACE_END(glfwGetPrimaryMonitor_);
}

void glfwGetVideoMode_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwGetVideoMode_);

	Dart_Handle monitor_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWmonitor* monitor = GetNativePointer<GLFWmonitor>(monitor_obj);

	const GLFWvidmode* ret = glfwGetVideoMode(monitor);
	Dart_SetReturnValue(arguments, HANDLE(NewGlfwVideoMode(ret)));

	TRACE_END(glfwGetVideoMode_);
}

void glfwWindowHint_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwWindowHint_);

	int64_t hint;
	HANDLE(Dart_GetNativeIntegerArgument(arguments, 0, &hint));

	int64_t value;
	HANDLE(Dart_GetNativeIntegerArgument(arguments, 1, &value));

	glfwWindowHint(int(hint), int(value));

	TRACE_END(glfwWindowHint_);
}

void glfwCreateWindow_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwCreateWindow_);

	int64_t width;
	HANDLE(Dart_GetNativeIntegerArgument(arguments, 0, &width));

	int64_t height;
	HANDLE(Dart_GetNativeIntegerArgument(arguments, 1, &height));

	void* title_peer = NULL;
	Dart_Handle title_arg = HANDLE(Dart_GetNativeStringArgument(arguments, 2, (void**)&title_peer));
	const char* title = NULL;
	HANDLE(Dart_StringToCString(title_arg, &title));

	Dart_Handle monitor_obj = HANDLE(Dart_GetNativeArgument(arguments, 3));
	GLFWmonitor* monitor = GetNativePointer<GLFWmonitor>(monitor_obj);

	Dart_Handle share_obj = HANDLE(Dart_GetNativeArgument(arguments, 4));
	GLFWwindow* share = GetNativePointer<GLFWwindow>(share_obj);

	GLFWwindow* ret = glfwCreateWindow(int(width), int(height), title, monitor, share);
	Dart_SetReturnValue(arguments, HANDLE(NewGlfwWindow(ret)));

	TRACE_END(glfwCreateWindow_);
}

void glfwSetWindowPos_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwSetWindowPos_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	int64_t xPos;
	HANDLE(Dart_GetNativeIntegerArgument(arguments, 1, &xPos));

	int64_t yPos;
	HANDLE(Dart_GetNativeIntegerArgument(arguments, 2, &yPos));

	glfwSetWindowPos(window, int(xPos), int(yPos));

	TRACE_END(glfwSetWindowPos_);
}

void glfwSetWindowSizeLimits_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwSetWindowSizeLimits_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	int64_t minWidth;
	HANDLE(Dart_GetNativeIntegerArgument(arguments, 1, &minWidth));

	int64_t minHeight;
	HANDLE(Dart_GetNativeIntegerArgument(arguments, 2, &minHeight));

	int64_t maxWidth;
	HANDLE(Dart_GetNativeIntegerArgument(arguments, 3, &maxWidth));

	int64_t maxHeight;
	HANDLE(Dart_GetNativeIntegerArgument(arguments, 4, &maxHeight));

	glfwSetWindowSizeLimits(window, int(minWidth), int(minHeight), int(maxWidth), int(maxHeight));

	TRACE_END(glfwSetWindowSizeLimits_);
}

void glfwSetWindowUserPointer_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwSetWindowUserPointer_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	Dart_Handle object_obj = HANDLE(Dart_GetNativeArgument(arguments, 1));

	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	Dart_PersistentHandle cur = reinterpret_cast<Dart_PersistentHandle>(glfwGetWindowUserPointer(window));
	if (cur != NULL) {
		Dart_DeletePersistentHandle(cur);
	}
	Dart_PersistentHandle new_handle = HANDLE(Dart_NewPersistentHandle(object_obj));
	glfwSetWindowUserPointer(window, new_handle);

	TRACE_END(glfwSetWindowUserPointer_);
}

void glfwGetWindowUserPointer_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwGetWindowUserPointer_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	Dart_PersistentHandle cur = reinterpret_cast<Dart_PersistentHandle>(glfwGetWindowUserPointer(window));
	Dart_SetReturnValue(arguments, (cur == NULL) ? Dart_Null() : cur);
	
	TRACE_END(glfwGetWindowUserPointer_);
}

void glfwGetWindowSize_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwGetWindowSize_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	Dart_SetReturnValue(arguments, NewRectangle(0, 0, width, height));

	TRACE_END(glfwGetWindowSize_);
}

void glfwDestroyWindow_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwDestroyWindow_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	glfwDestroyWindow(window);

	TRACE_END(glfwDestroyWindow_);
}

void glfwWindowShouldClose_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwWindowShouldClose_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	int ret = glfwWindowShouldClose(window);
	Dart_SetBooleanReturnValue(arguments, ret);

	TRACE_END(glfwWindowShouldClose_);
}

void glfwWaitEventsTimeout_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwWaitEventsTimeout_);

	double timeout;
	HANDLE(Dart_GetNativeDoubleArgument(arguments, 0, &timeout));
	glfwWaitEventsTimeout(timeout);

	TRACE_END(glfwWaitEventsTimeout_);
}

void glfwSetWindowTitle_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwSetWindowTitle_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	void* title_peer = NULL;
	Dart_Handle title_arg = HANDLE(Dart_GetNativeStringArgument(arguments, 1, (void**)&title_peer));
	const char* title = NULL;
	HANDLE(Dart_StringToCString(title_arg, &title));
	glfwSetWindowTitle(window, title);

	TRACE_END(glfwSetWindowTitle_);
}

void glfwSetWindowShouldClose_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwSetWindowShouldClose_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	bool value;
	HANDLE(Dart_GetNativeBooleanArgument(arguments, 1, &value));
	glfwSetWindowShouldClose(window, (int)value);

	TRACE_END(glfwSetWindowShouldClose_);
}

void glfwIconifyWindow_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwIconifyWindow_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	glfwIconifyWindow(window);

	TRACE_END(glfwIconifyWindow_);
}

void glfwRestoreWindow_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwRestoreWindow_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	glfwRestoreWindow(window);

	TRACE_END(glfwRestoreWindow_);
}

void glfwMaximizeWindow_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwMaximizeWindow_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	glfwMaximizeWindow(window);

	TRACE_END(glfwMaximizeWindow_);
}

void glfwGetWindowAttrib_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwGetWindowAttrib_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	int64_t attrib;
	HANDLE(Dart_GetNativeIntegerArgument(arguments, 1, &attrib));
	int64_t ret = glfwGetWindowAttrib(window, (int)attrib);
	Dart_SetIntegerReturnValue(arguments, ret);

	TRACE_END(glfwGetWindowAttrib_);
}

void glfwGetWin32Window_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwGetWin32Window_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	HWND windowHandle = glfwGetWin32Window(window);
	Dart_SetIntegerReturnValue(arguments, reinterpret_cast<int64_t>(windowHandle));

	TRACE_END(glfwGetWin32Window_);
}

static Dart_Handle dart_GLFWmousebuttonfun_cb = NULL;

void _GLFWmousebuttonfun_cb(GLFWwindow* window, int button, int action, int mods)
{
	Dart_Handle arguments[4];
	arguments[0] = HANDLE(NewGlfwWindow(window));
	arguments[1] = HANDLE(Dart_NewInteger(button));
	arguments[2] = HANDLE(Dart_NewInteger(action));
	arguments[3] = HANDLE(Dart_NewInteger(mods));
	HANDLE(Dart_InvokeClosure(dart_GLFWmousebuttonfun_cb, 4, arguments));
}

void glfwSetMouseButtonCallback_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwSetMouseButtonCallback_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	Dart_Handle new_GLFWmousebuttonfun_cb = HANDLE(Dart_GetNativeArgument(arguments, 1));
	if (Dart_IsNull(new_GLFWmousebuttonfun_cb)) {
		new_GLFWmousebuttonfun_cb = NULL;
	}
	else {
		new_GLFWmousebuttonfun_cb = HANDLE(Dart_NewPersistentHandle(new_GLFWmousebuttonfun_cb));
	}
	Dart_Handle old_GLFWmousebuttonfun_cb = Dart_Null();
	if (dart_GLFWmousebuttonfun_cb != NULL) {
		old_GLFWmousebuttonfun_cb = HandleError(Dart_HandleFromPersistent(dart_GLFWmousebuttonfun_cb));
		Dart_DeletePersistentHandle(dart_GLFWmousebuttonfun_cb);
	}
	dart_GLFWmousebuttonfun_cb = new_GLFWmousebuttonfun_cb;
	if (dart_GLFWmousebuttonfun_cb == NULL) {
		glfwSetMouseButtonCallback(window, NULL);
	}
	else {
		glfwSetMouseButtonCallback(window, _GLFWmousebuttonfun_cb);
	}
	Dart_SetReturnValue(arguments, old_GLFWmousebuttonfun_cb);

	TRACE_END(glfwSetMouseButtonCallback_);
}

static Dart_Handle dart_GLFWcursorposfun_cb = NULL;

void _GLFWcursorposfun_cb(GLFWwindow* window, double xpos, double ypos)
{
	Dart_Handle arguments[3];
	arguments[0] = HANDLE(NewGlfwWindow(window));
	arguments[1] = HANDLE(Dart_NewDouble(xpos));
	arguments[2] = HANDLE(Dart_NewDouble(ypos));
	HANDLE(Dart_InvokeClosure(dart_GLFWcursorposfun_cb, 3, arguments));
}

void glfwSetCursorPosCallback_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwSetCursorPosCallback_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	Dart_Handle new_GLFWcursorposfun_cb = HANDLE(Dart_GetNativeArgument(arguments, 1));
	if (Dart_IsNull(new_GLFWcursorposfun_cb)) {
		new_GLFWcursorposfun_cb = NULL;
	}
	else {
		new_GLFWcursorposfun_cb = HANDLE(Dart_NewPersistentHandle(new_GLFWcursorposfun_cb));
	}
	Dart_Handle old_GLFWcursorposfun_cb = Dart_Null();
	if (dart_GLFWcursorposfun_cb != NULL) {
		old_GLFWcursorposfun_cb = HandleError(Dart_HandleFromPersistent(dart_GLFWcursorposfun_cb));
		Dart_DeletePersistentHandle(dart_GLFWcursorposfun_cb);
	}
	dart_GLFWcursorposfun_cb = new_GLFWcursorposfun_cb;
	if (dart_GLFWcursorposfun_cb == NULL) {
		glfwSetCursorPosCallback(window, NULL);
	}
	else {
		glfwSetCursorPosCallback(window, _GLFWcursorposfun_cb);
	}
	Dart_SetReturnValue(arguments, old_GLFWcursorposfun_cb);

	TRACE_END(glfwSetCursorPosCallback_);
}

static Dart_Handle dart_GLFWkeyfun_cb = NULL;

void _GLFWkeyfun_cb(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Dart_Handle arguments[5];
	arguments[0] = HANDLE(NewGlfwWindow(window));
	arguments[1] = HANDLE(Dart_NewInteger(key));
	arguments[2] = HANDLE(Dart_NewInteger(scancode));
	arguments[3] = HANDLE(Dart_NewInteger(action));
	arguments[4] = HANDLE(Dart_NewInteger(mods));
	HANDLE(Dart_InvokeClosure(dart_GLFWkeyfun_cb, 5, arguments));
}

void glfwSetKeyCallback_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwSetKeyCallback_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	Dart_Handle new_GLFWkeyfun_cb = HANDLE(Dart_GetNativeArgument(arguments, 1));
	if (Dart_IsNull(new_GLFWkeyfun_cb)) {
		new_GLFWkeyfun_cb = NULL;
	}
	else {
		new_GLFWkeyfun_cb = HANDLE(Dart_NewPersistentHandle(new_GLFWkeyfun_cb));
	}
	Dart_Handle old_GLFWkeyfun_cb = Dart_Null();
	if (dart_GLFWkeyfun_cb != NULL) {
		old_GLFWkeyfun_cb = HandleError(Dart_HandleFromPersistent(dart_GLFWkeyfun_cb));
		Dart_DeletePersistentHandle(dart_GLFWkeyfun_cb);
	}
	dart_GLFWkeyfun_cb = new_GLFWkeyfun_cb;
	if (dart_GLFWkeyfun_cb == NULL) {
		glfwSetKeyCallback(window, NULL);
	}
	else {
		glfwSetKeyCallback(window, _GLFWkeyfun_cb);
	}
	Dart_SetReturnValue(arguments, old_GLFWkeyfun_cb);

	TRACE_END(glfwSetKeyCallback_);
}

static Dart_Handle dart_GLFWwindowsizefun_cb = NULL;

void _GLFWwindowsizefun_cb(GLFWwindow* window, int width, int height)
{
	Dart_Handle arguments[3];
	arguments[0] = HANDLE(NewGlfwWindow(window));
	arguments[1] = HANDLE(Dart_NewInteger(width));
	arguments[2] = HANDLE(Dart_NewInteger(height));
	HANDLE(Dart_InvokeClosure(dart_GLFWwindowsizefun_cb, 3, arguments));
}

void glfwSetWindowSizeCallback_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwSetWindowSizeCallback_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	Dart_Handle new_GLFWwindowsizefun_cb = HANDLE(Dart_GetNativeArgument(arguments, 1));
	if (Dart_IsNull(new_GLFWwindowsizefun_cb)) {
		new_GLFWwindowsizefun_cb = NULL;
	}
	else {
		new_GLFWwindowsizefun_cb = HANDLE(Dart_NewPersistentHandle(new_GLFWwindowsizefun_cb));
	}
	Dart_Handle old_GLFWwindowsizefun_cb = Dart_Null();
	if (dart_GLFWwindowsizefun_cb != NULL) {
		old_GLFWwindowsizefun_cb = HandleError(Dart_HandleFromPersistent(dart_GLFWwindowsizefun_cb));
		Dart_DeletePersistentHandle(dart_GLFWwindowsizefun_cb);
	}
	dart_GLFWwindowsizefun_cb = new_GLFWwindowsizefun_cb;
	if (dart_GLFWwindowsizefun_cb == NULL) {
		glfwSetWindowSizeCallback(window, NULL);
	}
	else {
		glfwSetWindowSizeCallback(window, _GLFWwindowsizefun_cb);
	}
	Dart_SetReturnValue(arguments, old_GLFWwindowsizefun_cb);

	TRACE_END(glfwSetWindowSizeCallback_);
}

static Dart_Handle dart_GLFWscrollfun_cb = NULL;

void _GLFWscrollfun_cb(GLFWwindow* window, double xoffset, double yoffset)
{
	Dart_Handle arguments[3];
	arguments[0] = HANDLE(NewGlfwWindow(window));
	arguments[1] = HANDLE(Dart_NewDouble(xoffset));
	arguments[2] = HANDLE(Dart_NewDouble(yoffset));
	HANDLE(Dart_InvokeClosure(dart_GLFWscrollfun_cb, 3, arguments));
}

void glfwSetScrollCallback_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwSetScrollCallback_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	Dart_Handle new_GLFWscrollfun_cb = HANDLE(Dart_GetNativeArgument(arguments, 1));
	if (Dart_IsNull(new_GLFWscrollfun_cb)) {
		new_GLFWscrollfun_cb = NULL;
	}
	else {
		new_GLFWscrollfun_cb = HANDLE(Dart_NewPersistentHandle(new_GLFWscrollfun_cb));
	}
	Dart_Handle old_GLFWscrollfun_cb = Dart_Null();
	if (dart_GLFWscrollfun_cb != NULL) {
		old_GLFWscrollfun_cb = HandleError(Dart_HandleFromPersistent(dart_GLFWscrollfun_cb));
		Dart_DeletePersistentHandle(dart_GLFWscrollfun_cb);
	}
	dart_GLFWscrollfun_cb = new_GLFWscrollfun_cb;
	if (dart_GLFWscrollfun_cb == NULL) {
		glfwSetScrollCallback(window, NULL);
	}
	else {
		glfwSetScrollCallback(window, _GLFWscrollfun_cb);
	}
	Dart_SetReturnValue(arguments, old_GLFWscrollfun_cb);

	TRACE_END(glfwSetScrollCallback_);
}

void glfwGetCursorPos_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwGetCursorPos_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	Dart_SetReturnValue(arguments, NewPoint(xpos, ypos));

	TRACE_END(glfwGetCursorPos_);
}

void glfwSetCursor_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwSetCursor_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow* window = GetNativePointer<GLFWwindow>(window_obj);

	int64_t cursor_code;
	Dart_GetNativeIntegerArgument(arguments, 1, &cursor_code);

	// TODO is causes a tiny memory leak until glfwTerminate is called. This should be returned to Dart to destroy.
	GLFWcursor* cursor = glfwCreateStandardCursor(int(cursor_code));
	glfwSetCursor(window, cursor);

	TRACE_END(glfwSetCursor_);
}

void glfwSetClipboardString_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwSetClipboardString_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow *window = GetNativePointer<GLFWwindow>(window_obj);

	void *strPeer = NULL;
	Dart_Handle stringArg = HANDLE(Dart_GetNativeStringArgument(arguments, 1, (void **)&strPeer));
	const char *string = NULL;
	HANDLE(Dart_StringToCString(stringArg, &string));
	glfwSetClipboardString(window, string);

	TRACE_END(glfwSetClipboardString_);
}

void glfwGetClipboardString_native(Dart_NativeArguments arguments)
{
	TRACE_START(glfwGetClipboardString_);

	Dart_Handle window_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));
	GLFWwindow *window = GetNativePointer<GLFWwindow>(window_obj);

	const char *ret = glfwGetClipboardString(window);
	Dart_SetReturnValue(arguments, HANDLE(Dart_NewStringFromCString(ret)));

	TRACE_END(glfwGetClipboardString_);
}

typedef struct {
	FlutterEngine engine;
	Dart_Isolate isolate;
	Dart_PersistentHandle delegate;
	GLFWwindow* window;
} FlutterEngineNative;

static void CleanupFlutterEngineNative(void *isolate_callback_data, Dart_WeakPersistentHandle handle, void *peer)
{
	FlutterEngineNative *native = reinterpret_cast<FlutterEngineNative*>(peer);
	Dart_DeletePersistentHandle(native->delegate);

	// proper cleanup

	delete native;
}

static bool feMakeCurrent(void *userData)
{
	FlutterEngineNative *native = reinterpret_cast<FlutterEngineNative*>(userData);
	/*
	Dart_Handle result = HANDLE_INVOKE(Dart_Invoke(native->delegate, makeCurrentString, 0, nullptr));
	bool value = false;
	HANDLE(Dart_BooleanValue(result, &value));
	return value;
	*/
	glfwMakeContextCurrent(native->window);
	return true;
}

static bool feClearCurrent(void *userData)
{
	FlutterEngineNative *native = reinterpret_cast<FlutterEngineNative*>(userData);
	/*
	Dart_Handle result = HANDLE_INVOKE(Dart_Invoke(native->delegate,
		Dart_NewStringFromCString("clearCurrent"), 0, nullptr));
	bool value = false;
	HANDLE(Dart_BooleanValue(result, &value));
	return value;
	*/
	glfwMakeContextCurrent(NULL);
	return true;
}

static bool fePresent(void *userData)
{
	FlutterEngineNative *native = reinterpret_cast<FlutterEngineNative*>(userData);
	/*
	Dart_Handle result = HANDLE_INVOKE(Dart_Invoke(native->delegate,
		Dart_NewStringFromCString("present"), 0, nullptr));
	bool value = false;
	HANDLE(Dart_BooleanValue(result, &value));
	return value;
	*/
	glfwSwapBuffers(native->window);
	return true;
}

static uint32_t feGetActiveFbo(void *userData)
{
	return 0;
}

static void fePlatformMessageCallback(const FlutterPlatformMessage* message, void *userData)
{
	FlutterEngineNative *native = reinterpret_cast<FlutterEngineNative*>(userData);

	Dart_Handle library = HandleError(
		Dart_LookupLibrary(Dart_NewStringFromCString("package:desktop_simulator/src/flutter_engine.dart"))
	);

	Dart_Handle PlatformMessage_type = HandleError(Dart_GetType(
		library, Dart_NewStringFromCString("PlatformMessage"), 0, NULL
	));

	Dart_Handle dataTypedArray = HANDLE(Dart_NewTypedData(Dart_TypedData_kUint8, message->message_size));
	Dart_TypedData_Type typedDataType;
	uint8_t *data;
	intptr_t len;
	HANDLE(Dart_TypedDataAcquireData(dataTypedArray, &typedDataType, reinterpret_cast<void**>(&data), &len));
	memcpy(data, message->message, min(message->message_size, (uint64_t)len));
	HANDLE(Dart_TypedDataReleaseData(dataTypedArray));

	Dart_Handle constructorArgs[2];
	constructorArgs[0] = HANDLE(Dart_NewStringFromCString(message->channel));
	constructorArgs[1] = dataTypedArray;
	Dart_Handle platformMessageObject = HANDLE(Dart_New(PlatformMessage_type,
		Dart_NewStringFromCString("_"), 2, constructorArgs
	));
	HANDLE(Dart_SetNativeInstanceField(platformMessageObject, 0,
		reinterpret_cast<intptr_t>(native)));
	HANDLE(Dart_SetNativeInstanceField(platformMessageObject, 1,
		reinterpret_cast<intptr_t>(message->response_handle)));

	Dart_Handle callbackArgs[1];
	callbackArgs[0] = platformMessageObject;
	HANDLE(Dart_Invoke(native->delegate,
		Dart_NewStringFromCString("platformMessage"), 1, callbackArgs));
}

static const char* Dart_GetStringField(Dart_Handle object, const char* fieldName)
{
	const char *nativeValue;
	Dart_Handle fieldValue = HANDLE(Dart_GetField(object, Dart_NewStringFromCString(fieldName)));
	HANDLE(Dart_StringToCString(fieldValue, &nativeValue));
	return nativeValue;
}

void feCreate_native(Dart_NativeArguments arguments)
{
	TRACE_START(feCreate_);

	Dart_Handle library = HandleError(
		Dart_LookupLibrary(Dart_NewStringFromCString("package:desktop_simulator/src/flutter_engine.dart"))
	);

	Dart_Handle FlutterEngine_type = HandleError(Dart_GetType(
		library, Dart_NewStringFromCString("FlutterEngine"), 0, NULL
	));

	Dart_Handle delegateParam = HANDLE(Dart_GetNativeArgument(arguments, 1));
	FlutterEngineNative* flutterEngineNative = new FlutterEngineNative();
	flutterEngineNative->delegate = HANDLE(Dart_PersistentHandle(delegateParam));
	flutterEngineNative->isolate = Dart_CurrentIsolate();

	Dart_Handle windowObject = HANDLE(Dart_GetNativeArgument(arguments, 0));
	flutterEngineNative->window = GetNativePointer<GLFWwindow>(windowObject);

	Dart_Handle argsParam = HANDLE(Dart_GetNativeArgument(arguments, 2));

	FlutterRendererConfig config = {
		kOpenGL,
		FlutterOpenGLRendererConfig {
			sizeof(FlutterOpenGLRendererConfig),
			feMakeCurrent,
			feClearCurrent,
			fePresent,
			feGetActiveFbo,
			nullptr,
			false,
			nullptr,
			nullptr,
		},
	};

	Dart_Handle commandLineArgs = HANDLE(Dart_GetField(argsParam,
		Dart_NewStringFromCString("commandLineArgs")));

	intptr_t commandLineArgc = 0;
	HANDLE(Dart_ListLength(commandLineArgs, &commandLineArgc));
	const char **commandLineArgv = new const char*[commandLineArgc];
	for (intptr_t i = 0; i < commandLineArgc; i++) {
		Dart_Handle item = HANDLE(Dart_ListGetAt(commandLineArgs, i));
		HANDLE(Dart_StringToCString(item, &commandLineArgv[i]));
	}

	FlutterProjectArgs args = {
		sizeof(FlutterProjectArgs),
		Dart_GetStringField(argsParam, "assetsPath"),
		Dart_GetStringField(argsParam, "mainPath"),
		Dart_GetStringField(argsParam, "packagesPath"),
		Dart_GetStringField(argsParam, "icuDataPath"),
		(int)commandLineArgc,
		commandLineArgv,
		fePlatformMessageCallback,
	};

	FlutterResult result = FlutterEngineRun(FLUTTER_ENGINE_VERSION,
		&config,
		&args,
		reinterpret_cast<void*>(flutterEngineNative),
		&flutterEngineNative->engine);

	delete commandLineArgv;

	if (result != kSuccess) {
		printf("Failed to create Flutter Engine Instance!\n");
		// TODO: throw real error
		Dart_SetReturnValue(arguments, Dart_Null());
	}
	else {
		Dart_Handle flutterEngineObject = HANDLE(Dart_New(FlutterEngine_type,
			Dart_NewStringFromCString("_"), 0, nullptr
		));
		HANDLE(Dart_SetNativeInstanceField(flutterEngineObject, 0,
			reinterpret_cast<intptr_t>(flutterEngineNative)));
		Dart_NewWeakPersistentHandle(flutterEngineObject,
			(void*)flutterEngineNative, sizeof(flutterEngineNative),
			CleanupFlutterEngineNative);
		Dart_SetReturnValue(arguments, flutterEngineObject);
	}

	TRACE_END(feCreate_);
}

void feShutdown_native(Dart_NativeArguments arguments)
{
	TRACE_START(feShutdown_);

	FlutterEngineNative *native = nullptr;
	HANDLE(Dart_GetNativeReceiver(arguments, reinterpret_cast<intptr_t *>(&native)));

	FlutterResult result = FlutterEngineShutdown(native->engine);
	Dart_SetBooleanReturnValue(arguments, result == kSuccess);

	TRACE_END(feShutdown_);
}

void feSendWindowMetricsEvent_native(Dart_NativeArguments arguments)
{
	TRACE_START(feSendWindowMetricsEvent_);

	FlutterEngineNative *native = nullptr;
	HANDLE(Dart_GetNativeReceiver(arguments, reinterpret_cast<intptr_t *>(&native)));

	Dart_Handle eventObject = HANDLE(Dart_GetNativeArgument(arguments, 1));

	uint64_t width;
	Dart_Handle widthValue = HANDLE(Dart_GetField(eventObject, Dart_NewStringFromCString("width")));
	HANDLE(Dart_IntegerToUint64(widthValue, &width));

	uint64_t height;
	Dart_Handle heightValue = HANDLE(Dart_GetField(eventObject, Dart_NewStringFromCString("height")));
	HANDLE(Dart_IntegerToUint64(heightValue, &height));

	double pixelRatio;
	Dart_Handle pixelRatioValue = HANDLE(Dart_GetField(eventObject, Dart_NewStringFromCString("pixelRatio")));
	HANDLE(Dart_DoubleValue(pixelRatioValue, &pixelRatio));

	FlutterWindowMetricsEvent event = {
		sizeof(FlutterWindowMetricsEvent),
		width,
		height,
		pixelRatio,
	};

	FlutterResult result = FlutterEngineSendWindowMetricsEvent(native->engine, &event);
	Dart_SetBooleanReturnValue(arguments, result == kSuccess);

	TRACE_END(feSendWindowMetricsEvent_);
}

void feSendPointerEvent_native(Dart_NativeArguments arguments)
{
	TRACE_START(feSendPointerEvent_);

	FlutterEngineNative *native = nullptr;
	HANDLE(Dart_GetNativeReceiver(arguments, reinterpret_cast<intptr_t *>(&native)));

	Dart_Handle eventObject = HANDLE(Dart_GetNativeArgument(arguments, 1));

	uint64_t phase;
	Dart_Handle phaseField = HANDLE(Dart_GetField(eventObject, Dart_NewStringFromCString("phase")));
	Dart_Handle phaseValue = HANDLE(Dart_GetField(phaseField, Dart_NewStringFromCString("index")));
	HANDLE(Dart_IntegerToUint64(phaseValue, &phase));

	uint64_t timestamp = 0;
	Dart_Handle timestampDuration = HANDLE(Dart_GetField(eventObject, Dart_NewStringFromCString("timestamp")));
	Dart_Handle timestampValue = HANDLE(Dart_GetField(timestampDuration, Dart_NewStringFromCString("inMicroseconds")));
	HANDLE(Dart_IntegerToUint64(timestampValue, &timestamp));

	double x;
	Dart_Handle xValue = HANDLE(Dart_GetField(eventObject, Dart_NewStringFromCString("x")));
	HANDLE(Dart_DoubleValue(xValue, &x));

	double y;
	Dart_Handle yValue = HANDLE(Dart_GetField(eventObject, Dart_NewStringFromCString("y")));
	HANDLE(Dart_DoubleValue(yValue, &y));

	FlutterPointerEvent event = {
		sizeof(FlutterPointerEvent),
		static_cast<FlutterPointerPhase>((int)phase),
		timestamp,
		x,
		y,
	};

	FlutterResult result = FlutterEngineSendPointerEvent(native->engine, &event, 1);
	Dart_SetBooleanReturnValue(arguments, result == kSuccess);

	TRACE_END(feSendPointerEvent_);
}

void feSendPlatformMessage_native(Dart_NativeArguments arguments)
{
	TRACE_START(feSendPlatformMessage_);

	FlutterEngineNative *native = nullptr;
	HANDLE(Dart_GetNativeReceiver(arguments, reinterpret_cast<intptr_t *>(&native)));

	const char *channelName;
	Dart_Handle channelNameValue = HANDLE(Dart_GetNativeArgument(arguments, 1));
	HANDLE(Dart_StringToCString(channelNameValue, &channelName));

	Dart_Handle dataTypedArray = HANDLE(Dart_GetNativeArgument(arguments, 2));
	Dart_TypedData_Type typedDataType;
	uint8_t *messageData;
	intptr_t messageLen;
	HANDLE(Dart_TypedDataAcquireData(dataTypedArray, &typedDataType, reinterpret_cast<void**>(&messageData), &messageLen));

	FlutterPlatformMessage message = {
		sizeof(FlutterPlatformMessage),
		channelName,
		messageData,
		(size_t)messageLen,
		nullptr,
	};

	FlutterResult result = FlutterEngineSendPlatformMessage(native->engine, &message);
	Dart_SetBooleanReturnValue(arguments, result == kSuccess);

	HANDLE(Dart_TypedDataReleaseData(dataTypedArray));

	TRACE_END(feSendPlatformMessage_);
}

void feSendPlatformMessageResponse_native(Dart_NativeArguments arguments)
{
	TRACE_START(feSendPlatformMessageResponse_);

	FlutterEngineNative *native = nullptr;
	Dart_Handle platformMesssageReceiver = HANDLE(Dart_GetNativeArgument(arguments, 0));
	HANDLE(Dart_GetNativeInstanceField(platformMesssageReceiver, 0, reinterpret_cast<intptr_t *>(&native)));

	FlutterPlatformMessageResponseHandle *responseHandle;
	HANDLE(Dart_GetNativeInstanceField(platformMesssageReceiver, 1, reinterpret_cast<intptr_t *>(&responseHandle)));

	uint8_t *data = nullptr;
	uint64_t dataByteLength = 0;

	Dart_Handle dataByteList = HANDLE(Dart_GetNativeArgument(arguments, 1));
	if (!Dart_IsNull(dataByteList)) {
		Dart_Handle dataByteLengthValue = HANDLE(Dart_GetField(dataByteList, Dart_NewStringFromCString("lengthInBytes")));
		HANDLE(Dart_IntegerToUint64(dataByteLengthValue, &dataByteLength));
		data = new uint8_t[dataByteLength];
		HANDLE(Dart_ListGetAsBytes(dataByteList, 0, data, dataByteLength));
	}

	FlutterResult result = FlutterEngineSendPlatformMessageResponse(
		native->engine, responseHandle, data, dataByteLength);
	Dart_SetBooleanReturnValue(arguments, result == kSuccess);

	if (data != nullptr) {
		delete data;
	}

	TRACE_END(feSendPlatformMessageResponse_);
}

void feFlushPendingTasksNow_native(Dart_NativeArguments arguments)
{
	TRACE_START(feFlushPendingTasksNow_);

	FlutterResult result = __FlutterEngineFlushPendingTasksNow();
	Dart_SetBooleanReturnValue(arguments, result == kSuccess);

	TRACE_END(feFlushPendingTasksNow_);
}

typedef struct {
	Dart_PersistentHandle delegate;
	int value;
} CallbackTestNative;

static void CleanupCallbackNativeTest(void *isolate_callback_data, Dart_WeakPersistentHandle handle, void *peer)
{
	printf("cleaning up\n");
	CallbackTestNative *native = reinterpret_cast<CallbackTestNative*>(peer);
	Dart_DeletePersistentHandle(native->delegate);
	delete native;
}

void ctCreate_native(Dart_NativeArguments arguments)
{
	TRACE_START(ctCreate_);

	Dart_Handle library = HandleError(
		Dart_LookupLibrary(Dart_NewStringFromCString("package:desktop_simulator/src/callback_test.dart"))
	);

	Dart_Handle CallbackTest_type = HandleError(Dart_GetType(
		library, Dart_NewStringFromCString("CallbackTest"), 0, NULL
	));

	Dart_Handle delegate_obj = HANDLE(Dart_GetNativeArgument(arguments, 0));

	CallbackTestNative* native = new CallbackTestNative();
	native->delegate = HANDLE(Dart_PersistentHandle(delegate_obj));
	native->value = 10;

	Dart_Handle callbackTest_obj = HANDLE(Dart_New(CallbackTest_type,
		Dart_NewStringFromCString("_"), 0, nullptr
	));
	HANDLE(Dart_SetNativeInstanceField(callbackTest_obj, 0, reinterpret_cast<intptr_t>(native)));
	Dart_NewWeakPersistentHandle(callbackTest_obj,
		(void*)native, sizeof(native), CleanupCallbackNativeTest);
	Dart_SetReturnValue(arguments, callbackTest_obj);

	TRACE_END(ctCreate_);
}

void ctTest_native(Dart_NativeArguments arguments)
{
	TRACE_START(ctTest_);
	CallbackTestNative *native = nullptr;
	HANDLE(Dart_GetNativeReceiver(arguments, reinterpret_cast<intptr_t *>(&native)));
	printf("instance 0: %d\n", native->value);

	Dart_Handle callbackArgs[1];
	callbackArgs[0] = Dart_NewInteger(native->value);
	Dart_Handle result = HANDLE(Dart_Invoke(native->delegate,
		Dart_NewStringFromCString("callbackTest"), 1, callbackArgs));

	int64_t val = 0;
	HANDLE(Dart_IntegerToInt64(result, &val));
	printf("result %I64d\n", val);

	TRACE_END(ctTest_);
}

void win32ReleaseCapture_native(Dart_NativeArguments arguments)
{
	TRACE_START(win32ReleaseCapture_);
	
	ReleaseCapture();

	TRACE_END(win32ReleaseCapture_);
}

void win32SendMessage_native(Dart_NativeArguments arguments)
{
	TRACE_START(win32ReleaseCapture_);

	int64_t hWnd;
	HANDLE(Dart_GetNativeIntegerArgument(arguments, 0, &hWnd));

	int64_t Msg;
	HANDLE(Dart_GetNativeIntegerArgument(arguments, 1, &Msg));
	
	int64_t wParam;
	HANDLE(Dart_GetNativeIntegerArgument(arguments, 2, &wParam));

	int64_t lParam;
	HANDLE(Dart_GetNativeIntegerArgument(arguments, 3, &lParam));

	int64_t result = SendMessage(
		reinterpret_cast<HWND>(hWnd), 
		static_cast<UINT>(Msg) ,
		static_cast<WPARAM>(wParam),
		static_cast<LPARAM>(lParam));

	Dart_SetIntegerReturnValue(arguments, result);

	TRACE_END(win32ReleaseCapture_);
}

struct FunctionLookup {
	const char* name;
	Dart_NativeFunction function;
};

static FunctionLookup resolveFnList[] = {
	// GLFW Library
	{"glfwInit",                      glfwInit_native},
	{"glfwTerminate",                 glfwTerminate_native},
	{"glfwGetVersionString",          glfwGetVersionString_native},
	{"glfwGetTime",                   glfwGetTime_native},
	{"glfwGetPrimaryMonitor",         glfwGetPrimaryMonitor_native},
	{"glfwGetVideoMode",              glfwGetVideoMode_native},
	{"glfwWindowHint",                glfwWindowHint_native},
	{"glfwCreateWindow",              glfwCreateWindow_native},
	{"glfwSetWindowPos",              glfwSetWindowPos_native},
	{"glfwSetWindowSizeLimits",       glfwSetWindowSizeLimits_native},
	{"glfwSetWindowUserPointer",      glfwSetWindowUserPointer_native},
	{"glfwGetWindowUserPointer",      glfwGetWindowUserPointer_native},
	{"glfwGetWindowSize",             glfwGetWindowSize_native},
	{"glfwDestroyWindow",             glfwDestroyWindow_native},
	{"glfwWindowShouldClose",         glfwWindowShouldClose_native},
	{"glfwWaitEventsTimeout",         glfwWaitEventsTimeout_native},
	{"glfwSetWindowTitle",            glfwSetWindowTitle_native},
	{"glfwSetWindowShouldClose",      glfwSetWindowShouldClose_native},
	{"glfwIconifyWindow",             glfwIconifyWindow_native},
	{"glfwRestoreWindow",             glfwRestoreWindow_native},
	{"glfwMaximizeWindow",            glfwMaximizeWindow_native},
	{"glfwGetWindowAttrib",           glfwGetWindowAttrib_native},
	{"glfwGetWin32Window",            glfwGetWin32Window_native},
	{"glfwSetKeyCallback",            glfwSetKeyCallback_native},
	{"glfwSetMouseButtonCallback",    glfwSetMouseButtonCallback_native},
	{"glfwSetCursorPosCallback",      glfwSetCursorPosCallback_native},
	{"glfwSetWindowSizeCallback",     glfwSetWindowSizeCallback_native},
	{"glfwSetScrollCallback",         glfwSetScrollCallback_native},
	{"glfwGetCursorPos",              glfwGetCursorPos_native},
	{"glfwSetCursor",                 glfwSetCursor_native},
	{"glfwSetClipboardString",        glfwSetClipboardString_native},
	{"glfwGetClipboardString",        glfwGetClipboardString_native},
	// Flutter Engine Library
	{"feCreate",                      feCreate_native},
	{"feShutdown",                    feShutdown_native},
	{"feSendWindowMetricsEvent",      feSendWindowMetricsEvent_native},
	{"feSendPointerEvent",            feSendPointerEvent_native},
	{"feSendPlatformMessage",         feSendPlatformMessage_native},
	{"feSendPlatformMessageResponse", feSendPlatformMessageResponse_native},
	{"feFlushPendingTasksNow",        feFlushPendingTasksNow_native},
	// Callback Test
	{"ctCreate",                      ctCreate_native},
	{"ctTest",                        ctTest_native},
	// User32
	{"win32ReleaseCapture",           win32ReleaseCapture_native},
	{"win32SendMessage",              win32SendMessage_native},
	{NULL, NULL},
};

Dart_NativeFunction ResolveName(Dart_Handle name, int argc, bool* auto_setup_scope)
{
	Dart_NativeFunction result = NULL;
	Dart_EnterScope();

	const char *cname;
	HANDLE(Dart_StringToCString(name, &cname));
	for (int i = 0; resolveFnList[i].name != NULL; i++) {
		if (strcmp(resolveFnList[i].name, cname) == 0) {
			*auto_setup_scope = false;
			result = resolveFnList[i].function;
			break;
		}
	}

	Dart_ExitScope();
	return result;
}

Dart_Handle HandleError(Dart_Handle handle)
{
	if (Dart_IsError(handle)) {
		Dart_PropagateError(handle);
	}
	return handle;
}

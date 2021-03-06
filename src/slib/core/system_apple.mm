/*
 *   Copyright (c) 2008-2018 SLIBIO <https://github.com/SLIBIO>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */

#include "slib/core/definition.h"

#if defined(SLIB_PLATFORM_IS_APPLE)

#include "slib/core/system.h"
#include "slib/core/file.h"
#include "slib/core/platform_apple.h"

#include <mach-o/dyld.h>

#define PRIV_PATH_MAX 1024

namespace slib
{

	String System::getApplicationPath()
	{
		char path[PRIV_PATH_MAX] = {0};
		char pathResolved[PRIV_PATH_MAX] = {0};
		uint32_t size = PRIV_PATH_MAX - 1;
		if (_NSGetExecutablePath(path, &size) == 0) {
			realpath(path, pathResolved);
		}
		return pathResolved;
	}

	String System::getApplicationHomeDirectory()
	{
		NSString* path = NSHomeDirectory();
		return Apple::getStringFromNSString(path);
	}

	String System::getCachesDirectory()
	{
		NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
		if ([paths count] > 0) {
			NSString* path = [paths objectAtIndex:0];
			return Apple::getStringFromNSString(path);
		}
		String dir = getApplicationHomeDirectory() + "/caches";
		File::createDirectory(dir);
		return dir;
	}

	String System::getTempDirectory()
	{
		NSString* path = NSTemporaryDirectory();
		return Apple::getStringFromNSString(path);
	}

	void _priv_System_setBundleLoginItemEnabled(const String& path, sl_bool flagEnabled)
	{
#if defined(SLIB_PLATFORM_IS_MACOS)
		if (path.isEmpty()) {
			return;
		}
		
		NSURL *itemURL = [NSURL fileURLWithPath:(Apple::getNSStringFromString(path))];
		LSSharedFileListItemRef existingItem = NULL;
		
		LSSharedFileListRef loginItems = LSSharedFileListCreate(NULL, kLSSharedFileListSessionLoginItems, NULL);
		
		if(loginItems) {
			UInt32 seed = 0U;
			NSArray *currentLoginItems = CFBridgingRelease(LSSharedFileListCopySnapshot(loginItems, &seed));
			for (id itemObject in currentLoginItems) {
				LSSharedFileListItemRef item = (__bridge LSSharedFileListItemRef)itemObject;
				UInt32 resolutionFlags = kLSSharedFileListNoUserInteraction | kLSSharedFileListDoNotMountVolumes;
				CFURLRef URL = NULL;
				OSStatus err = LSSharedFileListItemResolve(item, resolutionFlags, &URL, NULL);
				if (err == noErr) {
					Boolean foundIt = CFEqual(URL, (__bridge CFTypeRef)(itemURL));
					CFRelease(URL);
					if (foundIt) {
						existingItem = item;
						break;
					}
				}
			}
			if (flagEnabled) {
				if (existingItem == NULL) {
					LSSharedFileListInsertItemURL(loginItems, kLSSharedFileListItemBeforeFirst, NULL, NULL, (__bridge CFURLRef)itemURL, NULL, NULL);
				}
			} else {
				if (existingItem != NULL) {
					LSSharedFileListItemRemove(loginItems, existingItem);
				}
			}
			CFRelease(loginItems);
		}
#endif
	}

	void System::registerApplicationRunAtStartup(const String& path)
	{
		_priv_System_setBundleLoginItemEnabled(path, sl_true);
	}

	void System::registerApplicationRunAtStartup()
	{
		_priv_System_setBundleLoginItemEnabled(Apple::getMainBundlePath(), sl_true);
	}

	void System::unregisterApplicationRunAtStartup(const String& path)
	{
		_priv_System_setBundleLoginItemEnabled(path, sl_false);
	}

	void System::unregisterApplicationRunAtStartup()
	{
		_priv_System_setBundleLoginItemEnabled(Apple::getMainBundlePath(), sl_false);
	}

}

#endif

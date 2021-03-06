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

#ifndef CHECKHEADER_SLIB_SOCIAL_FACEBOOK
#define CHECKHEADER_SLIB_SOCIAL_FACEBOOK

#include "definition.h"

#include "../core/function.h"
#include "../core/string.h"
#include "../core/list.h"
#include "../core/time.h"

namespace slib
{
	
	class SLIB_EXPORT FacebookAccessToken
	{
	public:
		String userId;
		String token;
		Time expirationDate;
		Time refreshDate;
		List<String> permissions;
		List<String> declinedPermissions;
		
	public:
		FacebookAccessToken();
		
		SLIB_DECLARE_CLASS_DEFAULT_MEMBERS(FacebookAccessToken)
		
	};
	
	class SLIB_EXPORT FacebookLoginResult
	{
	public:
		sl_bool flagError;
		String errorMessage;

		sl_bool flagCancel;
		
		List<String> grantedPermissions;
		List<String> declinedPermissions;
		
		FacebookAccessToken token;
		
	public:
		FacebookLoginResult();
		
		SLIB_DECLARE_CLASS_DEFAULT_MEMBERS(FacebookLoginResult)

	};
	
	class SLIB_EXPORT FacebookLoginParam
	{
	public:
		sl_bool flagPublishPermissions;
		List<String> permissions;
		
		Function<void(FacebookLoginResult& result)> onComplete;
		
	public:
		FacebookLoginParam();
		
		SLIB_DECLARE_CLASS_DEFAULT_MEMBERS(FacebookLoginParam)
		
	};
	
	class SLIB_EXPORT Facebook
	{
	public:
		// call this function at `onStart()` in your application class
		static void initializeOnStartApp();
		
		static sl_bool getCurrentToken(FacebookAccessToken* _out = sl_null);
		
		static void login(const FacebookLoginParam& param);
		
		static void loginWithReadPermissions(const Function<void(FacebookLoginResult&)>& callback);
		
	};
	
}

#endif

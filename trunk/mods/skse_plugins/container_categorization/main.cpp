/* Copyright (c) 2011 Rick (rick 'at' gibbed 'dot' us)
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would
 *    be appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 */

#include <windows.h>
#include <skse/PluginAPI.h>

#include "patch.hpp"

bool plugin_query(const SKSEInterface *skse, PluginInfo *info)
{
	_MESSAGE("query");

	info->infoVersion = PluginInfo::kInfoVersion;
	info->name = "container categorization";
	info->version = 2;

	if (skse->isEditor != 0)
	{
		_ERROR("no need to categorize containers in the editor!");
		return false;
	}

	if (skse->runtimeVersion != 0x01150000 &&
		skse->runtimeVersion != 0x020C0000)
	{
		_ERROR("unsupported runtime version");
		return false;
	}

	_MESSAGE("runtime version = %08X", skse->runtimeVersion);
	return true;
}

bool plugin_load(const SKSEInterface *skse)
{
	unsigned char nops[] = { 0x90, 0x90 };
	unsigned char original[] = { 0x75, 0x20 };

	HMODULE exe = GetModuleHandle(NULL);

	unsigned int target = 0;

	switch (skse->runtimeVersion)
	{
		case 0x01150000: target = 0x009A0035; break;
		case 0x020C0000: target = 0x009A0AF5; break;
		
		default:
		{
			return false;
		}
	}

	target = adjust_address(exe, target);
	if (memcmp((void *)target, original, 2) != 0)
	{
		_ERROR("code at target is different, bad address?");
		return false;
	}

	_MESSAGE("applying patch");
	return patch_code(target, nops, 2);
}

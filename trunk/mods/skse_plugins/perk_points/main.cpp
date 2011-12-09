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
#include <shlwapi.h>

#include <skse/PluginAPI.h>
#include <skse/skse_version.h>

#include "../skse/SafeWrite.h"

#include "dll.hpp"

struct _perk_points_config
{
	unsigned char points_per_level;
}
config;

void load_config()
{
	config.points_per_level = 2;

	wchar_t path[MAX_PATH];

	if (!GetModuleFileNameW((HMODULE)gDLL, path, MAX_PATH))
	{
		_MESSAGE("could not look up module file name");
		return;
	}
	else if (PathRemoveFileSpec(path) == FALSE)
	{
		_MESSAGE("failed to remove module file name from path");
		return;
	}
	else if (PathAppend(path, L"perk_points.ini") == FALSE)
	{
		_MESSAGE("failed to append ini file name to path");
		return;
	}
	else if (PathFileExists(path) == FALSE)
	{
		_WARNING("ini path does not exist");
		return;
	}

	config.points_per_level = (unsigned char)(GetPrivateProfileIntW(
		L"perk_points", L"points_per_level", 2, path) & 0xFF);
	_MESSAGE("perk points per level = %u", config.points_per_level);
}

bool plugin_query(const SKSEInterface *skse, PluginInfo *info)
{
	_MESSAGE("query");

	info->infoVersion = PluginInfo::kInfoVersion;
	info->name = "perk points";
	info->version = 2;

	if (skse->isEditor != 0)
	{
		_ERROR("no need to funktify perk points in the editor!");
		return false;
	}

	if (skse->runtimeVersion != RUNTIME_VERSION_1_1_21_0 &&
		skse->runtimeVersion != RUNTIME_VERSION_1_2_12_0 &&
		skse->runtimeVersion != RUNTIME_VERSION_1_3_7_0)
	{
		_ERROR("unsupported runtime version");
		return false;
	}

	_MESSAGE("runtime version = %08X", skse->runtimeVersion);
	return true;
}

bool plugin_load(const SKSEInterface *skse)
{
	load_config();

	unsigned char original[] = { 0x83, 0xC2 };

	unsigned int target = 0;

	switch (skse->runtimeVersion)
	{
		case RUNTIME_VERSION_1_1_21_0: target = 0x009E8528; break;
		case RUNTIME_VERSION_1_2_12_0: target = 0x009E885D; break;
		case RUNTIME_VERSION_1_3_7_0: target = 0x009E97ED; break;

		default:
		{
			return false;
		}
	}

	if (memcmp((void *)target, original, 2) != 0)
	{
		_ERROR("code at target is different, bad address?");
		return false;
	}

	_MESSAGE("applying patch");
	SafeWriteBuf(target + 2, &config.points_per_level, sizeof(config.points_per_level));
	return true;
}

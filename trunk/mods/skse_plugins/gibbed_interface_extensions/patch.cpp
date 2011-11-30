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

#include "patch.hpp"

bool patch_bytes(unsigned int address, void *data, int length)
{
	DWORD old, junk;
	if (VirtualProtect((void *)address, length, PAGE_EXECUTE_READWRITE, &old) == FALSE)
	{
		return false;
	}
	memcpy((void *)address, data, length);
	VirtualProtect((void *)address, length, old, &junk);
	return true;
}

bool patch_code(unsigned int address, void *data, int length)
{
	if (patch_bytes(address, data, length) == false)
	{
		return false;
	}

	if (FlushInstructionCache(GetCurrentProcess(), (void *)address, length) == FALSE)
	{
		return false;
	}

	return true;
}

bool patch_jump(unsigned int address, unsigned char opcode, unsigned int target)
{
	unsigned char jump[5];

	jump[0] = opcode;
	*(DWORD *)(&jump[1]) = target;

	return patch_code(address, jump, 5);
}

bool patch_call(unsigned int address, unsigned int target)
{
	return patch_jump(address, 0xE8, target);
}

bool patch_call_absolute(unsigned int address, unsigned int target)
{
	return patch_call(address, target - address - 5);
}

bool patch_function(unsigned int address, unsigned int target)
{
	return patch_jump(address, 0xE9, target);
}

bool patch_function_absolute(unsigned int address, unsigned int target)
{
	return patch_function(address, target - address - 5);
}

unsigned int adjust_address(HMODULE module, unsigned int address)
{
	return (address - 0x00400000) + (unsigned int)module;
}

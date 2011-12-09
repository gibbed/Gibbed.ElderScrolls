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
#include <math.h>
#include <skse/skse_version.h>

#include "../skse/SafeWrite.h"
#include "StandardItemData.hpp"

namespace Scaleform
{
	struct _Addresses
	{
		UInt32 ScaleformCreateBool;
		UInt32 ScaleformCreateNumber;
		UInt32 ScaleformSetMember;
	}
	Addresses;

	void SetupAddresses1_1(void)
	{
		memset(&Addresses, 0xCC, sizeof(Addresses));

		Addresses.ScaleformCreateBool = 0x00999B60;
		Addresses.ScaleformCreateNumber = 0x00999B80;
		Addresses.ScaleformSetMember = 0x00999BD0;
	}
	
	void SetupAddresses1_3(void)
	{
		memset(&Addresses, 0xCC, sizeof(Addresses));

		Addresses.ScaleformCreateBool = 0x0099B8D0;
		Addresses.ScaleformCreateNumber = 0x0099B8F0;
		Addresses.ScaleformSetMember = 0x0099B940;
	}

	typedef struct
	{
		unsigned char unknown[16];
	}
	GFxValue;

	GFxValue *CreateBool(GFxValue *gfxv, bool value)
	{
		UInt32 func = Addresses.ScaleformCreateBool;
		unsigned int ivalue = (unsigned int)value;

		__asm
		{
			push ivalue
			mov ecx, gfxv
			call func
		}
	}

	GFxValue *CreateNumber(GFxValue *gfxv, double value)
	{
		UInt32 func = Addresses.ScaleformCreateNumber;
		unsigned int *ivalue = (unsigned int *)&value;
		unsigned int value_a = ivalue[0];
		unsigned int value_b = ivalue[1];

		__asm
		{
			push value_b
			push value_a
			mov ecx, gfxv
			call func
		}
	}

	void SetMember(void *obj, const char *name, GFxValue *value)
	{
		UInt32 func = Addresses.ScaleformSetMember;
		__asm
		{
			push value
			push name
			mov ecx, obj
			call func
		}
	}

	void SetMemberBool(void *obj, const char *name, bool value)
	{
		GFxValue gfxv;
		CreateBool(&gfxv, value);
		SetMember(obj, name, &gfxv);
	}

	void SetMemberNumber(void *obj, const char *name, double value)
	{
		GFxValue gfxv;
		CreateNumber(&gfxv, value);
		SetMember(obj, name, &gfxv);
	}
}

namespace Game
{
	struct _Addresses
	{
		UInt32 PlayerCharacter;
	
		UInt32 GetFormType;
		UInt32 GetFormWeight;

		UInt32 GetItemValue;
		UInt32 GetItemDamage;
		UInt32 GetItemArmor;

		UInt32 SetupStandardItemData;
		UInt32 GetStandardItemData;
	}
	Addresses;
	
	struct _Constants
	{
		UInt32	FormType_Armor;
		UInt32	FormType_Weapon;
		UInt32	FormType_Ammo;
		UInt32	FormType_Potion;
	}
	Constants;

	void SetupAddresses1_1(void)
	{
		memset(&Addresses, 0xCC, sizeof(Addresses));
		memset(&Constants, 0xCC, sizeof(Constants));

		Addresses.PlayerCharacter = 0x0155C064;

		Addresses.GetFormType = 0x00403100;
		Addresses.GetFormWeight = 0x0047F5C0;

		Addresses.GetItemValue = 0x004A8470;
		Addresses.GetItemDamage = 0x00854A60;
		Addresses.GetItemArmor = 0x00854720;

		Addresses.SetupStandardItemData = 0x00999C40;
		Addresses.GetStandardItemData = 0x009999C0;
		
		Constants.FormType_Armor = 0x1B;
		Constants.FormType_Weapon = 0x2B;
		Constants.FormType_Ammo = 0x2C;
		Constants.FormType_Potion = 0x30;
	}
	
	void SetupAddresses1_3(void)
	{
		memset(&Addresses, 0xCC, sizeof(Addresses));
		memset(&Constants, 0xCC, sizeof(Constants));

		Addresses.PlayerCharacter = 0x0156F334;

		Addresses.GetFormType = 0x00403050;
		Addresses.GetFormWeight = 0x0047F350;

		Addresses.GetItemValue = 0x004A83B0;
		Addresses.GetItemDamage = 0x008560A0;
		Addresses.GetItemArmor = 0x00855D60;

		Addresses.SetupStandardItemData = 0x0099B9B0;
		Addresses.GetStandardItemData = 0x0099B730;
		
		Constants.FormType_Armor = 0x1A;
		Constants.FormType_Weapon = 0x29;
		Constants.FormType_Ammo = 0x2A;
		Constants.FormType_Potion = 0x2E;
	}

	typedef struct
	{
		unsigned char data[1];
	}
	Form;

	unsigned char GetFormType(Form *form)
	{
		UInt32 func = Addresses.GetFormType;
		__asm
		{
			mov ecx, form
			call func
		}
	}

	double GetFormWeight(Form *form)
	{
		UInt32 func = Addresses.GetFormWeight;
		__asm
		{
			push form
			call func
			add esp, 4
		}
	}
	
	typedef struct
	{
		Form *form;
	}
	Item;

	int GetItemValue(Item *item)
	{
		UInt32 func = Addresses.GetItemValue;
		__asm
		{
			mov ecx, item
			call func
		}
	}

	double GetItemDamage(Item *item)
	{
		UInt32 func = Addresses.GetItemDamage;
		void *pc = *(void **)Addresses.PlayerCharacter;

		__asm
		{
			push item
			mov ecx, pc
			call func
		}
	}

	double GetItemArmor(Item *item)
	{
		UInt32 func = Addresses.GetItemArmor;
		void *pc = *(void **)Addresses.PlayerCharacter;

		__asm
		{
			push item
			mov ecx, pc
			call func
		}
	}

	typedef struct
	{
		unsigned int vtable;
		Item *item;
		void *unknown08;
		unsigned int unknown0C;
		unsigned int unknown10;
		unsigned int unknown14;
		unsigned int unknown18;
		unsigned int unknown1C;
	}
	StandardItemData;

	void SetupStandardItemData(StandardItemData *sid, Item *item, int a2)
	{
		UInt32 func = Addresses.SetupStandardItemData;
		__asm
		{
			push a2
			push item
			mov ecx, sid
			call func
		}
	}

	StandardItemData *GetStandardItemData(
		StandardItemData *sid,
		void **callbacks, Item *item, int a4)
	{
		UInt32 func = Addresses.GetStandardItemData;
		__asm
		{
			push a4
			push item
			push callbacks
			mov ecx, sid
			call func
		}
	}
}

double round(double r)
{
	return (r >= 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

Game::StandardItemData * __stdcall MyGetStandardItemData(
	Game::StandardItemData *sid,
	void **callbacks, Game::Item *item, int a4)
{
	Game::StandardItemData *info =
		Game::GetStandardItemData(sid, callbacks, item, a4);
	
	if (*callbacks == NULL ||
		item->form == NULL)
	{
		return info;
	}

	unsigned char formType = Game::GetFormType(item->form);

	double weight = Game::GetFormWeight(item->form);
	weight = max(0.0f, weight);

	int value = Game::GetItemValue(item);

#ifdef _DEBUG
	_MESSAGE("item %u %f %d", formType, weight, value);
#endif

	Scaleform::SetMemberBool(&info->unknown10, "extended", true);
	Scaleform::SetMemberNumber(&info->unknown10, "formType", (double)formType);
	Scaleform::SetMemberNumber(&info->unknown10, "weight", (double)weight);
	Scaleform::SetMemberNumber(&info->unknown10, "value", (double)value);

	if (formType == Game::Constants.FormType_Armor)
	{
		double armor = Game::GetItemArmor(item);
		armor = round(armor);

#ifdef _DEBUG
		_MESSAGE("  armor %f", armor);
#endif

		Scaleform::SetMemberNumber(&info->unknown10, "armor", armor);
	}
	else if (formType == Game::Constants.FormType_Weapon)
	{
		unsigned char weaponType = item->form->data[0xC4+0x31];
		
		double damage = Game::GetItemDamage(item);
		damage = round(damage);

#ifdef _DEBUG
		_MESSAGE("  weapon type %u damage %f", weaponType, damage);
#endif

		Scaleform::SetMemberNumber(&info->unknown10, "weaponType", (double)weaponType);
		Scaleform::SetMemberNumber(&info->unknown10, "damage", damage);
	}
	else if (formType == Game::Constants.FormType_Ammo)
	{
		double damage = Game::GetItemDamage(item);
		damage = round(damage);

#ifdef _DEBUG
		_MESSAGE("  damage %f", damage);
#endif

		Scaleform::SetMemberNumber(&info->unknown10, "damage", damage);
	}
	else if (formType == Game::Constants.FormType_Potion)
	{
		//
	}

	return info;
}

void __declspec(naked) stub_MyGetStandardItemData(void)
{
	__asm
	{
		push ebp
		mov ebp, esp
		push [ebp+16] ; unknown
		push [ebp+12] ; item
		push [ebp+8] ; localizer?
		push ecx ; this
		call MyGetStandardItemData
		pop ebp
		ret 12
	}
}

bool patch_StandardItemData(UInt32 version)
{
	UInt32 functionCall;

	switch (version)
	{
		case RUNTIME_VERSION_1_1_21_0:
		{
			Scaleform::SetupAddresses1_1();
			Game::SetupAddresses1_1();
			functionCall = 0x00999711;

			unsigned char original[] = { 0xE8, 0xAA, 0x02, 0x00, 0x00 };
			if (memcmp((void *)functionCall, original, sizeof(original)) != 0)
			{
				return false;
			}

			break;
		}
		
		case RUNTIME_VERSION_1_3_7_0:
		{
			Scaleform::SetupAddresses1_3();
			Game::SetupAddresses1_3();
			functionCall = 0x0099B511;

			unsigned char original[] = { 0xE8, 0x1A, 0x02, 0x00, 0x00 };
			if (memcmp((void *)functionCall, original, sizeof(original)) != 0)
			{
				return false;
			}

			break;
		}

		default:
		{
			return false;
		}
	}

	WriteRelCall(functionCall, (UInt32)&stub_MyGetStandardItemData);
	return true;
}

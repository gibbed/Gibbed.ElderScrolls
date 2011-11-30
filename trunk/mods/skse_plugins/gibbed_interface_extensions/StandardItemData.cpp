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

#include "patch.hpp"
#include "StandardItemData.hpp"

typedef struct
{
	unsigned char data[1];
}
item_form;

unsigned int ADDRESS_player_character;

unsigned int ADDRESS_get_form_type;
unsigned char get_form_type(item_form *thiz)
{
	__asm
	{
		mov ecx, thiz
		call ADDRESS_get_form_type
	}
}

unsigned int ADDRESS_get_form_weight;
double get_form_weight(item_form *thiz)
{
	__asm
	{
		push thiz
		call ADDRESS_get_form_weight
		add esp, 4
	}
}

typedef struct
{
	item_form *form;
}
item_runtime;

unsigned int ADDRESS_get_item_value;
int get_item_value(item_runtime *thiz)
{
	__asm
	{
		mov ecx, thiz
		call ADDRESS_get_item_value
	}
}

unsigned int ADDRESS_get_item_damage;
double get_item_damage(item_runtime *item)
{
	void *pc = *(void **)ADDRESS_player_character;

	__asm
	{
		push item
		mov ecx, pc
		call ADDRESS_get_item_damage
	}
}

unsigned int ADDRESS_get_item_armor;
double get_item_armor(item_runtime *item)
{
	void *pc = *(void **)ADDRESS_player_character;

	__asm
	{
		push item
		mov ecx, pc
		call ADDRESS_get_item_armor
	}
}

typedef struct
{
	unsigned int vtable;
	item_runtime *item;
	void *unknown08;
	unsigned int unknown0C;
	unsigned int unknown10;
	unsigned int unknown14;
	unsigned int unknown18;
	unsigned int unknown1C;
}
StandardItemData;

typedef struct
{
	unsigned char unknown[16];
}
scaleform_value;

unsigned int ADDRESS_scaleform_create_boolean;
scaleform_value *scaleform_create_boolean(scaleform_value *thiz, bool value)
{
	unsigned int ivalue = (unsigned int)value;

	__asm
	{
		push ivalue
		mov ecx, thiz
		call ADDRESS_scaleform_create_boolean
	}
}


unsigned int ADDRESS_scaleform_create_double;
scaleform_value *scaleform_create_double(scaleform_value *thiz, double value)
{
	unsigned int *ivalue = (unsigned int *)&value;
	unsigned int value_a = ivalue[0];
	unsigned int value_b = ivalue[1];

	__asm
	{
		push value_b
		push value_a
		mov ecx, thiz
		call ADDRESS_scaleform_create_double
	}
}

unsigned int ADDRESS_scaleform_add_variable = 0;
void scaleform_add_variable(void *list, const char *name, scaleform_value *value)
{
	__asm
	{
		push value
		push name
		mov ecx, list
		call ADDRESS_scaleform_add_variable
	}
}

unsigned int ADDRESS_setup_standard_data = 0;
void setup_standard_data(StandardItemData *thiz, item_runtime *item, int a2)
{
	__asm
	{
		push a2
		push item
		mov ecx, thiz
		call ADDRESS_setup_standard_data
	}
}

unsigned int ADDRESS_get_standard_data = 0;
StandardItemData *get_standard_data(
	StandardItemData *thiz,
	void **callbacks, item_runtime *item, int a4)
{
	__asm
	{
		push a4
		push item
		push callbacks
		mov ecx, thiz
		call ADDRESS_get_standard_data
	}
}

void scaleform_add_boolean(void *list, const char *name, bool value)
{
	scaleform_value var;
	scaleform_create_boolean(&var, value);
	scaleform_add_variable(list, name, &var);
}

void scaleform_add_double(void *list, const char *name, double value)
{
	scaleform_value var;
	scaleform_create_double(&var, value);
	scaleform_add_variable(list, name, &var);
}

double round(double r)
{
	return (r >= 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

StandardItemData * __stdcall my_get_standard_data(
	StandardItemData *thiz,
	void **callbacks, item_runtime *item, int a4)
{
	StandardItemData *info = get_standard_data(thiz, callbacks, item, a4);
	
	if (*callbacks == NULL ||
		item->form == NULL)
	{
		return info;
	}

	unsigned char formType = get_form_type(item->form);

	double weight = get_form_weight(item->form);
	weight = max(0.0f, weight);

	int value = get_item_value(item);

	//_MESSAGE("item %u %f %d", formType, weight, value);

	scaleform_add_boolean(&info->unknown10, "extended", true);
	scaleform_add_double(&info->unknown10, "formType", (double)formType);
	scaleform_add_double(&info->unknown10, "weight", (double)weight);
	scaleform_add_double(&info->unknown10, "value", (double)value);

	switch (formType)
	{
		case 27: // armor
		{
			double armor = get_item_armor(item);
			armor = round(armor);

			//_MESSAGE("  armor %f", armor);

			scaleform_add_double(&info->unknown10, "armor", armor);
			break;
		}

		case 43: // weapon
		{
			unsigned char weaponType = item->form->data[0xC4+0x31];
			
			double damage = get_item_damage(item);
			damage = round(damage);

			//_MESSAGE("  weapon type %u damage %f", weaponType, damage);

			scaleform_add_double(&info->unknown10, "weaponType", (double)weaponType);
			scaleform_add_double(&info->unknown10, "damage", damage);
			break;
		}

		case 44: // ammo
		{
			double damage = get_item_damage(item);
			damage = round(damage);

			//_MESSAGE("  damage %f", damage);

			scaleform_add_double(&info->unknown10, "damage", damage);
			break;
		}

		case 48: // potion
		{
			break;
		}
	}

	return info;
}

void __declspec(naked) my_get_standard_data_stub(void)
{
	__asm
	{
		push ebp
		mov ebp, esp
		push [ebp+16] ; unknown
		push [ebp+12] ; item
		push [ebp+8] ; localizer?
		push ecx ; thiz
		call my_get_standard_data
		pop ebp
		ret 12
	}
}

bool patch_StandardItemData(HMODULE module)
{
	unsigned char original[] = { 0xE8, 0xAA, 0x02, 0x00, 0x00 };
	unsigned int target = adjust_address(module, 0x00999711);
	
	if (memcmp((void *)target, original, sizeof(original)) != 0)
	{
		return false;
	}
	
	ADDRESS_player_character = adjust_address(module, 0x0155C064);

	ADDRESS_get_form_type = adjust_address(module, 0x00403100);
	ADDRESS_get_form_weight = adjust_address(module, 0x0047F5C0);

	ADDRESS_get_item_value = adjust_address(module, 0x004A8470);
	ADDRESS_get_item_damage = adjust_address(module, 0x00854A60);
	ADDRESS_get_item_armor = adjust_address(module, 0x00854720);

	ADDRESS_scaleform_create_boolean = adjust_address(module, 0x00999B60);
	ADDRESS_scaleform_create_double = adjust_address(module, 0x00999B80);
	ADDRESS_scaleform_add_variable = adjust_address(module, 0x00999BD0);

	ADDRESS_setup_standard_data = adjust_address(module, 0x00999C40);
	ADDRESS_get_standard_data = adjust_address(module, 0x009999C0);

	return patch_call_absolute(target, (unsigned int)&my_get_standard_data_stub);
}


#include "Core.h"

extern CCore* Core;
extern CItemRandomiser *ItemRandomiser;
extern SCore* CoreStruct;
DWORD pUniqueItems[25];
DWORD pCappedItems[250];
DWORD pSpecialWeapons[170];

VOID fItemRandomiser(UINT_PTR qWorldChrMan, UINT_PTR pItemBuffer, UINT_PTR pItemData, DWORD64 qReturnAddress) {

	if (*(int*)(pItemData) >= 0) ItemRandomiser->RandomiseItem(qWorldChrMan, pItemBuffer, pItemData, qReturnAddress);

	return;
};

VOID CItemRandomiser::RandomiseItem(UINT_PTR qWorldChrMan, UINT_PTR pItemBuffer, UINT_PTR pItemData, DWORD64 qReturnAddress) {

	DWORD dItemAmount = 0;
	DWORD dItemID = 0;
	DWORD dItemQuantity = 0;
	DWORD dItemDurability = 0;

	dItemAmount = *(int*)pItemBuffer;
	pItemBuffer += 4;

	if (dItemAmount > 6) {
		Core->Panic("Too many items!", "...\\Source\\ItemRandomiser\\ItemRandomiser.cpp", FE_AmountTooHigh, 1);
		int3
	};

	while (dItemAmount) {
	
		dItemID = *(int*)(pItemBuffer);
		dItemQuantity = *(int*)(pItemBuffer + 0x04);
		dItemDurability = *(int*)(pItemBuffer + 0x08);
	
		if (!CoreStruct->dRandomiseKeyItems) { //User preference "RandomiseKeys"
			if (IsGameProgressionItem(dItemID)) return;
		};
		if (!CoreStruct->dRandomsieHealItems) { //User preference "RandomiseHeals"
			if ((dItemID == 0x4000085D ) || (dItemID == 0x4000085F)) return;
		};

		if (CoreStruct->pItemArray[0] < MAX_LIST_ITEMS) {
			dItemID = CoreStruct->pItemArray[CoreStruct->pOffsetArray[CoreStruct->pItemArray[0]]]; //Grab new item
			CoreStruct->pOffsetArray[CoreStruct->pItemArray[0]] = 0;
		} 
		else {
			dItemID = CoreStruct->pItemArray[RandomiseNumber(1, MAX_LIST_ITEMS)]; //Default to random item list
		};

		CoreStruct->pItemArray[0]++;

		SortNewItem(&dItemID, &dItemQuantity);

		if ((dItemID == 0x4000085D) || (dItemID == 0x4000085F)) {
			if (!CoreStruct->dRandomsieHealItems) dItemID = 0x400001F4;
		};

		if (!CoreStruct->dRandomiseKeyItems) {
			if (IsGameProgressionItem(dItemID)) dItemID = 0x400001F4;
		};

		DebugItemPrint(*(int*)(pItemBuffer), *(int*)(pItemBuffer + 0x04), dItemID, dItemQuantity);
		
		*(int*)(pItemBuffer) = dItemID;
		*(int*)(pItemBuffer + 0x04) = dItemQuantity;
		*(int*)(pItemBuffer + 0x08) = -1;
	
		dItemAmount--;
		pItemBuffer += 0x0C;
	};

	CoreStruct->dIsListChanged++;

	return;

};

VOID CItemRandomiser::SortNewItem(DWORD* dItem, DWORD* dQuantity) {

	char pBuffer[MAX_PATH];
	UINT_PTR pPlayerPointer = 0;
	DWORD dItemType = 0;
	DWORD dItemSortID = 0;
	BYTE bPlayerUpgradeLevel = 0;
	
	dItemType = (*dItem >> 0x1C);
	dItemSortID = (*dItem & 0x0FFFFFF);

	switch (dItemType) {
	
	case(ItemType_Weapon): {

		*dQuantity = 1;

		if ((*dItem >> 0x10) == 6) {
			*dQuantity = RandomiseNumber(1, 99);
			return;
		};

		if (*dItem == 0x000A87500) return; //Dark Hand

		pPlayerPointer = *(UINT_PTR*)(CoreStruct->qLocalPlayer);
		pPlayerPointer = *(UINT_PTR*)(pPlayerPointer + 0x10);
		bPlayerUpgradeLevel = *(BYTE*)(pPlayerPointer + 0xB3);

		if (!bPlayerUpgradeLevel) return;

		if (IsWeaponSpecialType(*dItem)) {
			bPlayerUpgradeLevel >>= 1;
			*dItem += RandomiseNumber(0, bPlayerUpgradeLevel);
			return;
		};

		*dItem += RandomiseNumber(0, bPlayerUpgradeLevel);
		*dItem += (RandomiseNumber(0, 15) * 100);
	
		return;
	
	};
	case(ItemType_Protector): {
		*dQuantity = 1;
		return;
	};
	case(ItemType_Accessory): {
		*dQuantity = 1;
		return;
	};
	case(ItemType_Goods): {
		*dQuantity = 1;
		if (IsRestrictedGoods(*dItem)) return;
		*dQuantity = RandomiseNumber(1, 10);
		return;
	};
	default: {
		sprintf_s(pBuffer, "Invalid item type: %i (%08X)", dItemType, *dItem);
		Core->Panic(pBuffer, "...\\Source\\ItemRandomiser\\ItemRandomiser.cpp", HE_InvalidItemType, 0);
		*dItem = 0x400001F4;
		*dQuantity = 1;
	};
	
	};

};

BOOL CItemRandomiser::IsGameProgressionItem(DWORD dItemID) {

	int i = 0;

	while (pUniqueItems[i]) {
		if (dItemID == pUniqueItems[i]) return true;
		i++;
	};

	return false;
};

BOOL CItemRandomiser::IsWeaponSpecialType(DWORD dItemID) {

	int i = 0;

	while (pSpecialWeapons[i]) {
		if (dItemID == pSpecialWeapons[i]) return true;
		i++;
	};

	return false;
};

BOOL CItemRandomiser::IsRestrictedGoods(DWORD dItemID) {

	int i = 0;

	while (pCappedItems[i]) {
		if (dItemID == pCappedItems[i]) return true;
		i++;
	};

	return false;
};

DWORD CItemRandomiser::RandomiseNumber(DWORD dMin, DWORD dMax) {

	char pBuffer[MAX_PATH];
	DWORD dGen = 0;

	if (dMin > dMax) {
		sprintf_s(pBuffer, "Defined minimum > maximum! %i > %i", dMin, dMax);
		Core->Panic(pBuffer, "...\\Source\\ItemRandomiser\\ItemRandomiser.cpp", HE_Undefined, 0);
		return 1;
	};

	dGen = (DWORD)(__rdtsc() % dMax);

	if ((!dMin) || (dGen > dMin)) return dGen;

	return dMin;
};

VOID CItemRandomiser::DebugItemPrint(DWORD dOldItem, DWORD dOldQuantity, DWORD dItem, DWORD dQuantity) {
#ifdef DEBUG
	char pBuffer[MAX_PATH];
	sprintf_s(pBuffer, "Item randomised | Old %08X (%i) | New %08X (%i)\n", dOldItem, dOldQuantity, dItem, dQuantity);
	printf_s(pBuffer);
#endif
};


extern DWORD pUniqueItems[25] = {
	0x400000BF,	0x006132D0,	0x40000836,	0x400007D8,
	0x40000859,	0x400007DA,	0x40000846,	0x40000845,
	0x4000083C,	0x400007DC,	0x400007DE,	0x4000086C,
	0x4000086B,	0x400007D5,	0x4000084B,	0x4000084C,
	0x4000084D,	0x4000084E,	0x400007D1,	0x4000086E,
	0x4000085B,	0x400001EA,	0x4000083B,	0x4000083C,
	0x00000000,
};

extern DWORD pCappedItems[250] = {
	0x4000013B,	0x400000BF,
	0x40000181,	0x40000179,
	0x4000017A,	0x40000183,
	0x40000184,	0x4000015F,
	0x40000186,	0x400001EA,
	0x40000173,	0x400001A4,
	0x400001A3,	0x400001A2,
	0x400001A1,	0x400001A0,
	0x4000019F,	0x4000019E,
	0x4000019D,	0x4000019C,
	0x4000019B,	0x4000019A,
	0x40000199,	0x40000198,
	0x40000197,	0x40000196,
	0x40000195,	0x40000194,
	0x400003EB,	0x400002D5,
	0x400007D1,	0x400007D3,
	0x400007D4,	0x400007D5,
	0x400007D7,	0x400007D8,
	0x400007D9,	0x400007DA,
	0x400007DB,	0x400007DC,
	0x400007DD,	0x400007DE,
	0x400007DF,	0x40000835,
	0x40000836,	0x40000837,
	0x40000838, 0x40000839,
	0x4000083A,	0x4000083B,
	0x4000083C,	
	0x4000083D,
	0x4000083E,
	0x4000083F,
	0x40000840,
	0x40000841,
	0x40000842,
	0x40000843,
	0x40000844,
	0x40000845,
	0x40000846,
	0x40000847,
	0x40000848,
	0x40000849,
	0x4000084B,
	0x4000084C,
	0x4000084D,
	0x4000084E,
	0x4000084F,
	0x40000850,
	0x40000851,
	0x40000852,
	0x40000853,
	0x40000854,
	0x40000855,
	0x40000856,
	0x40000857,
	0x40000859,
	0x4000085A,
	0x4000085B,
	0x4000085C,
	0x4000085D,
	0x4000085E,
	0x4000085F,
	0x40000860,
	0x40000861,
	0x40000862,
	0x40000863,
	0x40000864,
	0x40000865,
	0x40000866,
	0x40000867,
	0x40000868,
	0x40000869,
	0x4000086A,
	0x4000086B,
	0x4000086C,
	0x4000086D,
	0x4000086E,
	0x4000086F,
	0x40000208,
	0x40000209,
	0x4000020A,
	0x4000020B,
	0x4000020C,
	0x4000028A,
	0x4000028B,
	0x400002C8,
	0x400002C9,
	0x400002CA,
	0x400002CB,
	0x400002CD,
	0x400002CE,
	0x400002CF,
	0x400002D0,
	0x400002D1,
	0x400002D2,
	0x400002D3,
	0x400002D4,
	0x400002D5,
	0x400002D6,
	0x400002D7,
	0x400002D8,
	0x400002D9,
	0x400002DB,
	0x400002DC,
	0x400002DD,
	0x400002E3,
	0x400002E6,
	0x400002E7,
	0x400002E8,
	0x400002E9,
	0x400002EA,
	0x400002EB,
	0x40124F80,
	0x40127690,
	0x4013D620,
	0x4013DA08,
	0x4013DDF0,
	0x4013E1D8,
	0x4013E5C0,
	0x4013E9A8,
	0x4013ED90,
	0x4013F178,
	0x4013F560,
	0x4013F948,
	0x4013FD30,
	0x40140118,
	0x40140500,
	0x401408E8,
	0x40144B50,
	0x40144F38,
	0x40147260,
	0x40147648,
	0x40149970,
	0x4014A528,
	0x4014A910,
	0x4014ACF8,
	0x4014B0E0,
	0x4014E790,
	0x4014EF60,
	0x4014F348,
	0x4014F730,
	0x4014FB18,
	0x4014FF00,
	0x401875B8,
	0x401879A0,
	0x40189CC8,
	0x4018B820,
	0x40193138,
	0x401A8CE0,
	0x401A90C8,
	0x40249F00,
	0x4024A6D0,
	0x4024AAB8,
	0x4024B288,
	0x4024B670,
	0x4024BA58,
	0x4024C610,
	0x4024C9F8,
	0x4024ED20,
	0x4024F108,
	0x4024F4F0,
	0x40251430,
	0x40251818,
	0x402527B8,
	0x40252BA0,
	0x40253B40,
	0x40256250,
	0x40256638,
	0x40256A20,
	0x40256E08,
	0x402575D8,
	0x402579C0,
	0x40258190,
	0x4025B070,
	0x40257DA8,
	0x402717D0,
	0x4027AC40,
	0x4027D350,
	0x4027FA60,
	0x40282170,
	0x40284880,
	0x40286F90,
	0x402896A0,
	0x403540D0,
	0x403567E0,
	0x40356BC8,
	0x40356FB0,
	0x40357398,
	0x40357780,
	0x40357B68,
	0x40358338,
	0x40358720,
	0x40358B08,
	0x40359AA8,
	0x4035B600,
	0x4035B9E8,
	0x4035DD10,
	0x4035E0F8,
	0x4035E4E0,
	0x40360420,
	0x40362B30,
	0x40362F18,
	0x40363300,
	0x403636E8,
	0x40363AD0,
	0x40363EB8,
	0x403642A0,
	0x40364688,
	0x40365240,
	0x40365628,
	0x40365DF8,
	0x4036A448,
	0x4036C770,
	0x4036CB58,
	0x40378AC0,
	0x40387520,
	0x40389C30,
	0x4038C340,
	0x4038EA50,
	0x40393870,
	0x40395F80,
	0x40398690,
	0x4039ADA0,
	0x00000000,
};

extern DWORD pSpecialWeapons[170] = {
	0x00D63BC0,
	0x00D5EDA0,
	0x00CC9ED0,
	0x00CCC5E0,
	0x003E8FA0,
	0x0135E7F0,
	0x002206F0,
	0x00B80560,
	0x00610BC0,
	0x00F646E0,
	0x00F69500,
	0x00111700,
	0x006132D0,
	0x004CE780,
	0x004C9960,
	0x004D0E90,
	0x014159A0,
	0x01424400,
	0x01504DC0,
	0x01430750,
	0x009A1D20,
	0x0141F5E0,
	0x0144B500,
	0x01509BE0,
	0x015074D0,
	0x006BE130,
	0x00015F90,
	0x00061A80,
	0x00061AE4,
	0x00061B48,
	0x00061BAC,
	0x00061C10,
	0x00061C74,
	0x00061CD8,
	0x00061D3C,
	0x00062250,
	0x00062318,
	0x0006237C,
	0x000623E0,
	0x00062A20,
	0x00062A84,
	0x00062AE8,
	0x00062B4C,
	0x00062BB0,
	0x00062C14,
	0x00062C78,
	0x001053B0,
	0x00107AC0,
	0x00116520,
	0x00203230,
	0x0020A760,
	0x0020F580,
	0x002143A0,
	0x00222E00,
	0x0021DFE0,
	0x00225510,
	0x00227C20,
	0x002E6300,
	0x002E8A10,
	0x003E1A70,
	0x003E4180,
	0x003F04D0,
	0x004D35A0,
	0x005E2590,
	0x005E4CA0,
	0x005E9AC0,
	0x005F0FF0,
	0x005D8950,
	0x005F5E10,
	0x005F8520,
	0x005FAC30,
	0x005FD340,
	0x005FFA50,
	0x00602160,
	0x00604870,
	0x00606F80,
	0x0060BDA0,
	0x0060E4B0,
	0x006C7D70,
	0x006CA480,
	0x006CCB90,
	0x006D40C0,
	0x006D8EE0,
	0x007CAA10,
	0x007CD120,
	0x007CF830,
	0x007D6D60,
	0x007E09A0,
	0x007E30B0,
	0x007E7ED0,
	0x007ECCF0,
	0x007EF400,
	0x008B01F0,
	0x008B5010,
	0x008B7720,
	0x008BC540,
	0x008BEC50,
	0x008C3A70,
	0x008CAFA0,
	0x008CFDC0,
	0x008D24D0,
	0x008D4BE0,
	0x009959D0,
	0x0099A7F0,
	0x009AE070,
	0x009B0780,
	0x009B55A0,
	0x00A84DF0,
	0x00A87500,
	0x00B7B740,
	0x00B82C70,
	0x00C72090,
	0x00C747A0,
	0x00C76EB0,
	0x00C795C0,
	0x00C7E3E0,
	0x00C80AF0,
	0x00C83200,
	0x00C88020,
	0x00C8A730,
	0x00C8CE40,
	0x00C8F550,
	0x00C91C60,
	0x00C94370,
	0x00C96A80,
	0x00C99190,
	0x00C9B8A0,
	0x00C9DFB0,
	0x00CA06C0,
	0x00CA2DD0,
	0x00CA54E0,
	0x00CA7BF0,
	0x00CAA300,
	0x00CACA10,
	0x00CAF120,
	0x00CC77C0,
	0x00CF8500,
	0x00D5C690,
	0x00D662D0,
	0x00D689E0,
	0x00D6B0F0,
	0x00D6FF10,
	0x00D72620,
	0x00D79B50,
	0x00D7C260,
	0x00D7E970,
	0x00D83790,
	0x00D85EA0,
	0x00D885B0,
	0x00F4C040,
	0x00F5F8C0,
	0x013376F0,
	0x014466E0,
	0x0143CAA0,
	0x01432E60,
	0x01450320,
	0x01452A30,
	0x015EF3C0,
	0x015F1AD0,
	0x0150C2F0,
	0x01511110,
	0x01518640,
	0x01437C80,
	0x00000000,
};
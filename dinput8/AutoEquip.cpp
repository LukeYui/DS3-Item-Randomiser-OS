#include "Core.h"

extern CCore* Core;
extern CAutoEquip* AutoEquip;
extern SCore* CoreStruct;

DWORD dRingSlotSelect = 0x11;
DWORD pHelmetList[110];
DWORD pBodyList[105];
DWORD pHandsList[100];
DWORD pLegsList[105];

VOID fAutoEquip(UINT_PTR pItemBuffer, DWORD64 pItemData, DWORD64 qReturnAddress) {
	if (*(DWORD*)(pItemData + 0x04) == 0) AutoEquip->AutoEquipItem(pItemBuffer, qReturnAddress);
	return;
};

VOID CAutoEquip::AutoEquipItem(UINT_PTR pItemBuffer, DWORD64 qReturnAddress) {

	SEquipBuffer pEquipBuffer;
	DWORD dItemAmount = 0;
	DWORD dItemID = 0;
	DWORD dItemQuantity = 0;
	DWORD dItemDurability = 0;

	if (!pItemBuffer) {
		Core->Panic("Null buffer!", "...\\Source\\AutoEquip\\AutoEquip.cpp", FE_NullPtr, 1);
		int3
	};

	dItemAmount = *(int*)pItemBuffer;
	pItemBuffer += 4;

	if (dItemAmount > 6) {
		Core->Panic("Too many items!", "...\\Source\\AutoEquip\\AutoEquip.cpp", FE_AmountTooHigh, 1);
		int3
	};
	
	while (dItemAmount) {

		dItemID = *(int*)(pItemBuffer);
		dItemQuantity = *(int*)(pItemBuffer + 0x04);
		dItemDurability = *(int*)(pItemBuffer + 0x08);

		if (SortItem(dItemID, &pEquipBuffer)) {
			if (!AutoEquip->EquipItem) {
				Core->Panic("Bad function call", "...\\Source\\AutoEquip\\AutoEquip.cpp", FE_BadFunc, 1);
				int3
			};
			AutoEquip->EquipItem(pEquipBuffer.dEquipSlot, &pEquipBuffer);
		};
	
		dItemAmount--;
		pItemBuffer += 0x0C;
	};

	return;
};

BOOL CAutoEquip::SortItem(DWORD dItemID, SEquipBuffer* E) {

	char pBuffer[MAX_PATH];
	DWORD dItemType = 0;
	DWORD dEquipSlot = 0;

	dItemType = (dItemID >> 0x1C);

	switch (dItemType) {
	case(ItemType_Weapon): {
		if ((dItemID >> 0x10) == 6) return false; //Don't equip ammo
		if ((dItemID & 0xFF000000) << 4 != 0x10000000) dEquipSlot = 1; //If these conditions are met, it's a shield.
		break;
	};
	case(ItemType_Protector): {
		if (FindEquipType(dItemID, &pHelmetList[0])) dEquipSlot = 0x0C;
		else if (FindEquipType(dItemID, &pBodyList[0])) dEquipSlot = 0x0D;
		else if (FindEquipType(dItemID, &pHandsList[0])) dEquipSlot = 0x0E;
		else if (FindEquipType(dItemID, &pLegsList[0])) dEquipSlot = 0x0F;
		break;
	};
	case(ItemType_Accessory): {
		if (dRingSlotSelect >= 0x15) dRingSlotSelect = 0x11;
		dEquipSlot = dRingSlotSelect;
		dRingSlotSelect++;
		break;
	};
	case(ItemType_Goods): return false;
	default: {
		sprintf_s(pBuffer, "Invalid item type: %i (%08X)", dItemType, dItemID);
		Core->Panic(pBuffer, "...\\Source\\AutoEquip\\AutoEquip.cpp", HE_InvalidItemType, 0);
		return false;
	};
	
	};

	E->dEquipSlot = dEquipSlot;
	E->dInventorySlot = GetInventorySlotID(dItemID);

	if (E->dInventorySlot < 0) {
		sprintf_s(pBuffer, "Unable to find item: %08X", dItemID);
		Core->Panic(pBuffer, "...\\Source\\AutoEquip\\AutoEquip.cpp", HE_InvalidInventoryEquipID, 0);
		return false;
	};

	return true;
};

BOOL CAutoEquip::FindEquipType(DWORD dItem, DWORD* pArray) {

	int i = 0;

	if (!pArray) {
		Core->Panic("Null array pointer!", "...\\Source\\AutoEquip\\AutoEquip.cpp", FE_NullArray, 1);
		int3
	};

	while (*pArray) {
		if (dItem == *pArray) return true;
		pArray++;
	};

	return false;
};

DWORD CAutoEquip::GetInventorySlotID(DWORD dItemID) {

	DWORD dInventoryID = 0;
	UINT_PTR qInventoryPtr = 0;
	UINT_PTR qInventoryScanPtr = 0;

	qInventoryPtr = *(UINT_PTR*)CoreStruct->qLocalPlayer;
	qInventoryPtr = *(UINT_PTR*)(qInventoryPtr + 0x10);
	qInventoryPtr = *(UINT_PTR*)(qInventoryPtr + 0x470);
	qInventoryPtr = *(UINT_PTR*)(qInventoryPtr + 0x10);
	qInventoryPtr += 0x1B8;

	while (dInventoryID < *(DWORD*)(qInventoryPtr + 0x04)) {
	
		qInventoryScanPtr = (dInventoryID << 0x04);
		qInventoryScanPtr += *(UINT_PTR*)(qInventoryPtr + 0x38);

		if (*(DWORD*)(qInventoryScanPtr + 0x04) == dItemID) {
			return (dInventoryID + *(DWORD*)(qInventoryPtr + 0x14));
		};
	
		dInventoryID++;
	};

	return -1;
};

extern DWORD pHelmetList[110]{

	0x14ADD0A0,
	0x14153A20,
	0x1121EAC0,
	0x11298BE0,
	0x11312D00,
	0x11406F40,
	0x11481060,
	0x114FB180,
	0x115752A0,
	0x115EF3C0,
	0x11607A60,
	0x116694E0,
	0x116E3600,
	0x1175D720,
	0x117D7840,
	0x11851960,
	0x118CBA80,
	0x11945BA0,
	0x119BFCC0,
	0x11A39DE0,
	0x11AB3F00,
	0x11BA8140,
	0x11C9C380,
	0x11D905C0,
	0x11E84800,
	0x11F78A40,
	0x12625A00,
	0x1263E0A0,
	0x12656740,
	0x129020C0,
	0x129F6300,
	0x12A70420,
	0x12AEA540,
	0x12BDE780,
	0x12CD29C0,
	0x12D4CAE0,
	0x12DC6C00,
	0x12EBAE40,
	0x130291A0,
	0x13197500,
	0x1328B740,
	0x1337F980,
	0x133F9AA0,
	0x13473BC0,
	0x13567E00,
	0x1365C040,
	0x136D6160,
	0x13750280,
	0x138BE5E0,
	0x13938700,
	0x13A2C940,
	0x13AA6A60,
	0x13B20B80,
	0x13C14DC0,
	0x13D09000,
	0x13DFD240,
	0x13EF1480,
	0x13FE56C0,
	0x140D9900,
	0x142C1D80,
	0x143B5FC0,
	0x144AA200,
	0x1459E440,
	0x14692680,
	0x1470C7A0,
	0x147868C0,
	0x148009E0,
	0x1487AB00,
	0x148F4C20,
	0x1496ED40,
	0x149E8E60,
	0x12656740,
	0x14B571C0,
	0x14BD12E0,
	0x14C4B400,
	0x14D3F640,
	0x14E33880,
	0x14F27AC0,
	0x1501BD00,
	0x1510FF40,
	0x15204180,
	0x152F83C0,
	0x153EC600,
	0x155D4A80,
	0x156C8CC0,
	0x157BCF00,
	0x158B1140,
	0x159A5380,
	0x15A995C0,
	0x15B8D800,
	0x15C81A40,
	0x15D75C80,
	0x15E69EC0,
	0x137CA3A0,
	0x134EDCE0,
	0x11312D00,
	0x139B2820,
	0x135E1F20,
	0x11C22260,
	0x11E84800,
	0x13C8EEE0,
	0x1405F7E0,
	0x14CC5520,
	0x14DB9760,
	0x14FA1BE0,
	0x14EAD9A0,
	0x15095E20,
	0x00000000,

};		  //All in-game headwear

extern DWORD pBodyList[105]{

	0x14B575A8,
	0x15204568,
	0x156C90A8,
	0x148F5008,
	0x1121EEA8,
	0x11298FC8,
	0x113130E8,
	0x11407328,
	0x11481448,
	0x114FB568,
	0x11575688,
	0x115EF7A8,
	0x11607E48,
	0x116698C8,
	0x116E39E8,
	0x1175DB08,
	0x117D7C28,
	0x11851D48,
	0x118CBE68,
	0x11945F88,
	0x119C00A8,
	0x11A3A1C8,
	0x11AB42E8,
	0x11BA8528,
	0x11C9C768,
	0x11CB4E08,
	0x11D909A8,
	0x11DA9048,
	0x11F78E28,
	0x11DA9048,
	0x129024A8,
	0x129F66E8,
	0x12A70808,
	0x12AEA928,
	0x12BDEB68,
	0x12CD2DA8,
	0x12DC6FE8,
	0x12E41108,
	0x12EBB228,
	0x13029588,
	0x131978E8,
	0x1328BB28,
	0x1337FD68,
	0x133F9E88,
	0x13473FA8,
	0x135681E8,
	0x1365C428,
	0x13750668,
	0x138BE9C8,
	0x13938AE8,
	0x13A2CD28,
	0x13AA6E48,
	0x13B20F68,
	0x13C151A8,
	0x13D093E8,
	0x13DFD628,
	0x13EF1868,
	0x13F6B988,
	0x140D9CE8,
	0x142C2168,
	0x143B63A8,
	0x144AA5E8,
	0x1459E828,
	0x14692A68,
	0x1470CB88,
	0x14786CA8,
	0x14800DC8,
	0x1487AEE8,
	0x1496F128,
	0x149E9248,
	0x14A63368,
	0x14BD16C8,
	0x14C4B7E8,
	0x14D3FA28,
	0x14E33C68,
	0x14F27EA8,
	0x1501C0E8,
	0x15110328,
	0x152F87A8,
	0x153EC9E8,
	0x154E0C28,
	0x155D4E68,
	0x157BD2E8,
	0x158B1528,
	0x159A5768,
	0x15A999A8,
	0x15B8DBE8,
	0x15C81E28,
	0x15D76068,
	0x15E6A2A8,
	0x137CA788,
	0x134EE0C8,
	0x139B2C08,
	0x135E2308,
	0x11B2E408,
	0x11C22648,
	0x11E84BE8,
	0x13C8F2C8,
	0x13D83508,
	0x1405FBC8,
	0x14CC5908,
	0x14DB9B48,
	0x14EADD88,
	0x00000000,

};		  //All in-game chestpieces

extern DWORD pHandsList[100]{

	0x149E9630,
	0x1121F290,
	0x112993B0,
	0x113134D0,
	0x11407710,
	0x11481830,
	0x114FB950,
	0x11575A70,
	0x115EFB90,
	0x11669CB0,
	0x116E3DD0,
	0x1175DEF0,
	0x117D8010,
	0x11852130,
	0x118CC250,
	0x11946370,
	0x119C0490,
	0x11A3A5B0,
	0x11AB46D0,
	0x11BA8910,
	0x11C9CB50,
	0x11D90D90,
	0x11E84FD0,
	0x11F79210,
	0x126261D0,
	0x129F6AD0,
	0x12AEAD10,
	0x12BDEF50,
	0x12CD3190,
	0x12DC73D0,
	0x12EBB610,
	0x13029970,
	0x13197CD0,
	0x1328BF10,
	0x13380150,
	0x133FA270,
	0x13474390,
	0x135685D0,
	0x1365C810,
	0x13750A50,
	0x13938ED0,
	0x13A2D110,
	0x13B21350,
	0x13C15590,
	0x13D097D0,
	0x140DA0D0,
	0x142C2550,
	0x143B6790,
	0x144AA9D0,
	0x1459EC10,
	0x14692E50,
	0x1470CF70,
	0x14787090,
	0x148011B0,
	0x1487B2D0,
	0x148F53F0,
	0x1496F510,
	0x14A63750,
	0x14B57990,
	0x14BD1AB0,
	0x14C4BBD0,
	0x14D3FE10,
	0x14E34050,
	0x14F28290,
	0x1501C4D0,
	0x15110710,
	0x15204950,
	0x152F8B90,
	0x153ECDD0,
	0x154E1010,
	0x155D5250,
	0x156C9490,
	0x157BD6D0,
	0x158B1910,
	0x159A5B50,
	0x15A99D90,
	0x15B8DFD0,
	0x15D76450,
	0x15E6A690,
	0x137CAB70,
	0x134EE4B0,
	0x139B2FF0,
	0x11B2E7F0,
	0x11C22A30,
	0x11E84FD0,
	0x13C8F6B0,
	0x13D838F0,
	0x1405FFB0,
	0x14CC5CF0,
	0x14DB9F30,
	0x14EAE170,
	0x00000000,
};

extern DWORD pLegsList[105]{

	0x1121F678,
	0x11299798,
	0x113138B8,
	0x11407AF8,
	0x11481C18,
	0x114FBD38,
	0x11575E58,
	0x115EFF78,
	0x1166A098,
	0x116E41B8,
	0x1175E2D8,
	0x117D83F8,
	0x11852518,
	0x118CC638,
	0x11946758,
	0x119C0878,
	0x11A3A998,
	0x11AB4AB8,
	0x11BA8CF8,
	0x11C9CF38,
	0x11D91178,
	0x11E853B8,
	0x11F795F8,
	0x126265B8,
	0x129F6EB8,
	0x12AEB0F8,
	0x12BDF338,
	0x12CD3578,
	0x12DC77B8,
	0x12EBB9F8,
	0x13029D58,
	0x131980B8,
	0x1328C2F8,
	0x13380538,
	0x133FA658,
	0x13474778,
	0x135689B8,
	0x1365CBF8,
	0x13750E38,
	0x139392B8,
	0x13A2D4F8,
	0x13AA7618,
	0x13B21738,
	0x13C15978,
	0x13D09BB8,
	0x13EF2038,
	0x13F6C158,
	0x140DA4B8,
	0x142C2938,
	0x143B6B78,
	0x144AADB8,
	0x1459EFF8,
	0x14693238,
	0x1470D358,
	0x14787478,
	0x14801598,
	0x1487B6B8,
	0x148F57D8,
	0x1496F8F8,
	0x149E9A18,
	0x14A63B38,
	0x14B57D78,
	0x14BD1E98,
	0x14C4BFB8,
	0x14D401F8,
	0x14E34438,
	0x14F28678,
	0x1501C8B8,
	0x15110AF8,
	0x15204D38,
	0x152F8F78,
	0x153ED1B8,
	0x154E13F8,
	0x155D5638,
	0x156C9878,
	0x157BDAB8,
	0x158B1CF8,
	0x159A5F38,
	0x15A9A178,
	0x15B8E3B8,
	0x15C825F8,
	0x15D76838,
	0x15E6AA78,
	0x137CAF58,
	0x134EE898,
	0x139B33D8,
	0x135E2AD8,
	0x11B2EBD8,
	0x11C22E18,
	0x11E853B8,
	0x13C8FA98,
	0x13D83CD8,
	0x14060398,
	0x14CC60D8,
	0x14DBA318,
	0x14EAE558,
	0x00000000,
};
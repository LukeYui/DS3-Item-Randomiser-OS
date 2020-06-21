#include "Core.h"

extern CCore* Core;
extern CAutoEquip* AutoEquip;
extern SCore* CoreStruct;
extern CItemHelpers ItemHelpers;

DWORD dRingSlotSelect = 0x11;

VOID fAutoEquip(UINT_PTR pItemBuffer, DWORD64 pItemData, DWORD64 qReturnAddress) {
	if (*(int*)(pItemData) >= 0) AutoEquip->AutoEquipItem(pItemBuffer, qReturnAddress);
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

            if (!IsInventoryItemEquipped(pEquipBuffer.dInventorySlot)) {
                LockUnlockEquipSlots(1);
                AutoEquip->EquipItem(pEquipBuffer.dEquipSlot, &pEquipBuffer);
            }
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
        if (CoreStruct->dCatalystsLeftHand && ItemHelpers.IsCatalyst(dItemID)); //Equip staves,chimes,talismans in left hand
        else if ((dItemID & 0xFF000000) << 4 != 0x10000000) dEquipSlot = 1; //If these conditions are met, it's a shield.
        break;
    };
	case(ItemType_Protector): {
        dEquipSlot = GetEquipSlot(dItemID);
		break;
	};
	case(ItemType_Accessory): {
        if (!ItemHelpers.IsRing(dItemID)) return false;

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

DWORD CAutoEquip::GetEquipSlot(DWORD dItem) {

    // slot is based on decimal value of item id
    DWORD itemActualId = dItem & 0x0FFFFFFF;
    switch (itemActualId % 10000) {
    case 0:
        return 0x0C;
    case 1000:
        return 0x0D;
    case 2000:
        return 0x0E;
    case 3000:
        return 0x0F;
    }

    char pBuffer[MAX_PATH];
    sprintf_s(pBuffer, "Unable to find slot for an item: %08X", dItem);
    Core->Panic(pBuffer, "...\\Source\\AutoEquip\\AutoEquip.cpp", HE_InvalidProtectorItem, 0);
    return 0;
};

DWORD CAutoEquip::GetInventorySlotID(DWORD dItemID) {

	DWORD dInventoryID = 0;
	UINT_PTR qInventoryPtr = 0;
	UINT_PTR qInventoryScanPtr = 0;

	qInventoryPtr = *(UINT_PTR*)CoreStruct->qLocalPlayer;
	qInventoryPtr = *(UINT_PTR*)(qInventoryPtr + 0x10);
	if (!qInventoryPtr) {
		Core->Panic("'Local Player' does not exist", "...\\Source\\AutoEquip\\AutoEquip.cpp", HE_NoPlayerChar, 1);
		int3
	};

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

VOID CAutoEquip::LockUnlockEquipSlots(int iIsUnlock) {

	UINT_PTR qWorldChrMan = 0;
	DWORD dChrEquipAnimFlags = 0;

	qWorldChrMan = *(UINT_PTR*)(CoreStruct->qWorldChrMan);
	if (!qWorldChrMan) {
		Core->Panic("WorldChrMan", "...\\Source\\AutoEquip\\AutoEquip.cpp", HE_NoPlayerChar, 1);
		int3
	};

	qWorldChrMan = *(UINT_PTR*)(qWorldChrMan + 0x80);
	if (!qWorldChrMan) {
		Core->Panic("'WorldChr Player' does not exist", "...\\Source\\AutoEquip\\AutoEquip.cpp", HE_NoPlayerChar, 1);
		int3
	};

	qWorldChrMan = *(UINT_PTR*)(qWorldChrMan + 0x1F90);
	if (!qWorldChrMan) {
		Core->Panic("'WorldChr Data' does not exist", "...\\Source\\AutoEquip\\AutoEquip.cpp", HE_NoPlayerChar, 0);
		return;
	};

	qWorldChrMan = *(UINT_PTR*)(qWorldChrMan);
	if (!qWorldChrMan) {
		Core->Panic("'WorldChr Flags' does not exist", "...\\Source\\AutoEquip\\AutoEquip.cpp", HE_NoPlayerChar, 0);
		return;
	};

	dChrEquipAnimFlags = *(DWORD*)(qWorldChrMan + 0x10);

	if (iIsUnlock) dChrEquipAnimFlags |= 1;
	else dChrEquipAnimFlags &= 0xFFFFFFFE;

	*(DWORD*)(qWorldChrMan + 0x10) = dChrEquipAnimFlags;

	return;
};

bool CAutoEquip::IsInventoryItemEquipped(int inventoryId) {
    auto playerEquips = *(UINT_PTR*)CoreStruct->qLocalPlayer;
    playerEquips = *(UINT_PTR*)(playerEquips + 0x10) + 0x228;

    return FindEquippedSlot(playerEquips, inventoryId) != -1;
}

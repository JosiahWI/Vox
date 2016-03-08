// ******************************************************************************
// Filename:    InventoryManager.cpp
// Project:     Game
// Author:      Steven Ball
//
// Purpose:
//
// Revision History:
//   Initial Revision - 30/07/12
//
// Copyright (c) 2005-2016, Steven Ball
// ******************************************************************************

#pragma warning(disable: 4800) // Disable 'casting to bool' warning C4800

#include "InventoryManager.h"
#include "../Items/Item.h"
#include "../Player/Player.h"
#include "../GameGUI/InventoryGUI.h"
#include "../GameGUI/LootGUI.h"
#include "../GameGUI/ActionBar.h"

#include <algorithm>
#include <fstream>
#include <ostream>
#include <iostream>
#include <string>
using namespace std;


static ItemTextData g_itemData[] = 
{
	// Item				// Filename				// Title			// Description
	eItem_None,			"none",					"None",				"None.",
};


InventoryManager::InventoryManager()
{
	m_pPlayer = NULL;
	m_pInventoryGUI = NULL;
	m_pLootGUI = NULL;
	m_pActionBar = NULL;

    // Reset the inventory mapping array
    for(int i = 0; i < MAX_NUM_INVENTORY_SLOTS; i++)
    {
        m_ItemSlotMapping[i] = NULL;
    }

	for(int i = 0; i < (int)EquipSlot_NumSlots; i++)
	{
		m_equippedSlots[i] = NULL;
	}	

	m_playerName = "";

    m_numCoins = 0;
	m_coinsUpdated = true;

	m_InventoryGUINeedsUpdate = false;
	m_CharacterGUINeedsUpdate = false;
}

InventoryManager::~InventoryManager()
{
	Reset();
}

void InventoryManager::Reset()
{
	ClearInventory();
	ClearEquipped();
	ClearOtherCreatedItems();
}

// Linkage
void InventoryManager::SetPlayer(Player* pPlayer)
{
	m_pPlayer = pPlayer;
}

void InventoryManager::SetInventoryGUI(InventoryGUI* pInventoryGUI)
{
	m_pInventoryGUI = pInventoryGUI;
}

void InventoryManager::SetLootGUI(LootGUI* pLootGUI)
{
	m_pLootGUI = pLootGUI;
}

void InventoryManager::SetActionBar(ActionBar* pActionBar)
{
	m_pActionBar = pActionBar;
}

void InventoryManager::ClearInventory()
{
	for(unsigned int i = 0; i < m_vpInventoryItemList.size(); i++)
	{
		delete m_vpInventoryItemList[i];
		m_vpInventoryItemList[i] = 0;
	}
	m_vpInventoryItemList.clear();

	for(int i = 0; i < MAX_NUM_INVENTORY_SLOTS; i++)
	{
		m_ItemSlotMapping[i] = NULL;
	}
}

void InventoryManager::ClearEquipped()
{
	for(int i = 0; i < (int)EquipSlot_NumSlots; i++)
	{
		m_equippedSlots[i] = NULL;
	}
}

void InventoryManager::ClearOtherCreatedItems()
{
	for(unsigned int i = 0; i < m_vpOtherInventoryItemList.size(); i++)
	{
		delete m_vpOtherInventoryItemList[i];
		m_vpOtherInventoryItemList[i] = 0;
	}
	m_vpOtherInventoryItemList.clear();
}

void InventoryManager::LoadDefaultInventory(string playerName)
{
	ClearInventory();
	ClearEquipped();

	m_playerName = playerName;

	m_numCoins = 0;

	InventoryItem* pPickaxe = AddInventoryItem("media/gamedata/weapons/Pickaxe/Pickaxe.weapon", "media/textures/items/pickaxe.tga", InventoryType_Weapon_Pickaxe, eItem_None, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Pickaxe", "Used for mining and digging the world", 1.0f, 1.0f, 1.0f, -1, -1, -1, -1, -1);
	InventoryItem* pTorch = AddInventoryItem("media/gamedata/weapons/Torch/Torch.weapon", "media/textures/items/torch.tga", InventoryType_Weapon_Torch, eItem_None, ItemStatus_None, EquipSlot_LeftHand, ItemQuality_Common, false, false, "Torch", "A torch to light up the darkness", 1.0f, 1.0f, 1.0f, -1, -1, -1, -1, -1);
	InventoryItem* pSword = AddInventoryItem("media/gamedata/weapons/Sword/Sword.weapon", "media/textures/items/sword.tga", InventoryType_Weapon_Sword, eItem_None, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Uncommon, false, false, "Sword", "Just a standard sword, very useful for killing enemies", 1.0f, 1.0f, 1.0f, -1, -1, -1, -1, -1);
	pSword->AddStatAttribute(AttributeType_Strength, 2);
	InventoryItem* pShield = AddInventoryItem("media/gamedata/weapons/Shield/Shield.weapon", "media/textures/items/shield.tga", InventoryType_Weapon_Shield, eItem_None, ItemStatus_None, EquipSlot_LeftHand, ItemQuality_Uncommon, false, false, "Shield", "Just a standard shield", 1.0f, 1.0f, 1.0f, -1, -1, -1, -1, -1);
	pShield->AddStatAttribute(AttributeType_Armor, 2);
	InventoryItem* pBow = AddInventoryItem("media/gamedata/weapons/Bow/Bow.weapon", "media/textures/items/bow.tga", InventoryType_Weapon_Bow, eItem_None, ItemStatus_None, EquipSlot_LeftHand, ItemQuality_Uncommon, true, true, "Bow", "Shoots arrows.", 1.0f, 1.0f, 1.0f, -1, -1, -1, -1, -1);
	pBow->AddStatAttribute(AttributeType_Dexterity, 1);
	InventoryItem* p2HandedSword = AddInventoryItem("media/gamedata/weapons/2HandedSword/2HandedSword.weapon", "media/textures/items/2handed_sword.tga", InventoryType_Weapon_2HandedSword, eItem_None, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Rare, true, true, "Two Handed Sword", "You need two hands for a reason", 1.0f, 1.0f, 1.0f, -1, -1, -1, -1, -1);
	InventoryItem* pBoomerang = AddInventoryItem("media/gamedata/weapons/Boomerang/Boomerang.weapon", "media/textures/items/boomerang.tga", InventoryType_Weapon_Boomerang, eItem_None, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Magical, false, false, "Boomerang", "It really comes back", 1.0f, 1.0f, 1.0f, -1, -1, -1, -1, -1);
	InventoryItem* pBomb = AddInventoryItem("media/gamedata/items/Bomb/Bomb.item", "media/textures/items/bomb.tga", InventoryType_Weapon_Bomb, eItem_None, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Magical, false, false, "Bomb", "Booooom!", 1.0f, 1.0f, 1.0f, 10, -1, -1, -1, -1);
	InventoryItem* pStaff = AddInventoryItem("media/gamedata/weapons/Staff/Staff.weapon", "media/textures/items/staff.tga", InventoryType_Weapon_Staff, eItem_None, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, true, true, "Staff", "A magical staff", 1.0f, 1.0f, 1.0f, -1, -1, -1, -1, -1);
	pStaff->AddStatAttribute(AttributeType_Intelligence, 25);
	InventoryItem* pDagger1 = AddInventoryItem("media/gamedata/weapons/Knife/Knife.weapon", "media/textures/items/knife.tga", InventoryType_Weapon_Dagger, eItem_None, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Dagger", "Sharp dagger", 1.0f, 1.0f, 1.0f, -1, -1, -1, -1, -1);
	InventoryItem* pDagger2 = AddInventoryItem("media/gamedata/weapons/Knife/Knife.weapon", "media/textures/items/knife.tga", InventoryType_Weapon_Dagger, eItem_None, ItemStatus_None, EquipSlot_LeftHand, ItemQuality_Common, false, false, "Dagger", "Sharp dagger", 1.0f, 1.0f, 1.0f, -1, -1, -1, -1, -1);
	InventoryItem* pFireball1 = AddInventoryItem("media/gamedata/weapons/FireballHands/FireballHandsRight.weapon", "media/textures/items/fireball_hand.tga", InventoryType_Weapon_SpellHands, eItem_None, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Epic, false, false, "Fireball Hands", "Fireball hand", 1.0f, 1.0f, 1.0f, -1, -1, -1, -1, -1);
	InventoryItem* pFireball2 = AddInventoryItem("media/gamedata/weapons/FireballHands/FireballHandsLeft.weapon", "media/textures/items/fireball_hand.tga", InventoryType_Weapon_SpellHands, eItem_None, ItemStatus_None, EquipSlot_LeftHand, ItemQuality_Epic, false, false, "Fireball Hands", "Fireball hand", 1.0f, 1.0f, 1.0f, -1, -1, -1, -1, -1);

	/*
	InventoryItem* pPlacementTorch = AddInventoryItem("media/gamedata/items/Torch/Torch.item", "media/textures/items/torch.tga", InventoryType_Item, eItem_Torch, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Torch", "Provides light", 1.0f, 1.0f, 1.0f, 3, -1, -1, -1, -1);
	InventoryItem* pAnvil = AddInventoryItem("media/gamedata/items/Anvil/Anvil.item", "media/textures/items/anvil.tga", InventoryType_Item, eItem_Anvil, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Anvil", "Anvil for crafting", 1.0f, 1.0f, 1.0f, 1, -1, -1, -1, -1);
	InventoryItem* pChest = AddInventoryItem("media/gamedata/items/Chest/Chest.item", "media/textures/items/chest.tga", InventoryType_Item, eItem_Chest, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Chest", "Chest for storing items", 1.0f, 1.0f, 1.0f, 1, -1, -1, -1, -1);
	InventoryItem* pFurnace = AddInventoryItem("media/gamedata/items/Furnace/Furnace.item", "media/textures/items/furnace.tga", InventoryType_Item, eItem_Furnace, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Furnace", "Used for smelting ore", 1.0f, 1.0f, 1.0f, 1, -1, -1, -1, -1);
	InventoryItem* pBed = AddInventoryItem("media/gamedata/items/Bed/Bed.item", "media/textures/items/question_mark.tga", InventoryType_Item, eItem_Bed, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Bed", "Set spawn position", 1.0f, 1.0f, 1.0f, 1, -1, -1, -1, -1);
	*/

	InventoryItem* pStone = AddInventoryItem("media/gamedata/items/Block_Stone/Block_Stone.item", "media/textures/items/block_stone.tga", InventoryType_Block, eItem_Block_Stone, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Stone Block", "Stone block for world building", 1.0f, 1.0f, 1.0f, 3, -1, -1, -1, -1);
	InventoryItem* pWood = AddInventoryItem("media/gamedata/items/Block_Wood/Block_Wood.item", "media/textures/items/block_wood.tga", InventoryType_Block, eItem_Block_Wood, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Wood Block", "Wood block for world building", 1.0f, 1.0f, 1.0f, 4, -1, -1, -1, -1);
	InventoryItem* pGrass = AddInventoryItem("media/gamedata/items/Block_Grass/Block_Grass.item", "media/textures/items/block_grass.tga", InventoryType_Block, eItem_Block_Grass, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Grass Block", "Grass block for world building", 1.0f, 1.0f, 1.0f, 2, -1, -1, -1, -1);
	InventoryItem* pDirt = AddInventoryItem("media/gamedata/items/Block_Dirt/Block_Dirt.item", "media/textures/items/block_dirt.tga", InventoryType_Block, eItem_Block_Dirt, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Dirt Block", "Dirt block for world building", 1.0f, 1.0f, 1.0f, 2, -1, -1, -1, -1);
	InventoryItem* pLeaf = AddInventoryItem("media/gamedata/items/Block_Leaf/Block_Leaf.item", "media/textures/items/block_leaf.tga", InventoryType_Block, eItem_Block_Leaf, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Leaf Block", "Leaf block for world building", 1.0f, 1.0f, 1.0f, 2, -1, -1, -1, -1);

	/*
	InventoryItem* pShoulders = AddInventoryItem("media/gamedata/equipment/WoodShoulders/WoodShoulders.equipment", "media/textures/items/wood_shoulders.tga", InventoryType_Clothing, eItem_None, ItemStatus_None, EquipSlot_Shoulders, ItemQuality_Uncommon, true, true, "Shoulders", "Shoulder armor.", 1.0f, 1.0f, 1.0f, -1, -1, -1, -1, -1);
	pShoulders->AddStatAttribute(AttributeType_Armor, 1);
	InventoryItem* pHelm = AddInventoryItem("media/gamedata/equipment/WoodHelm/WoodHelm.equipment", "media/textures/items/wood_helm.tga", InventoryType_Clothing, eItem_None, ItemStatus_None, EquipSlot_Head, ItemQuality_Uncommon, false, false, "Helm", "Helm armor.", 1.0f, 1.0f, 1.0f, -1, -1, -1, -1, -1);
	pHelm->AddStatAttribute(AttributeType_Armor, 1);
	InventoryItem* pGloves = AddInventoryItem("media/gamedata/equipment/WoodGloves/WoodGloves.equipment", "media/textures/items/wood_gloves.tga", InventoryType_Clothing, eItem_None, ItemStatus_None, EquipSlot_Hand, ItemQuality_Uncommon, true, true, "Gloves", "Glove armor.", 1.0f, 1.0f, 1.0f, -1, -1, -1, -1, -1);
	pGloves->AddStatAttribute(AttributeType_Armor, 1);
	InventoryItem* pBoots = AddInventoryItem("media/gamedata/equipment/WoodBoots/WoodBoots.equipment", "media/textures/items/wood_boots.tga", InventoryType_Clothing, eItem_None, ItemStatus_None, EquipSlot_Feet, ItemQuality_Uncommon, true, true, "Boots", "Boot armor.", 1.0f, 1.0f, 1.0f, -1, -1, -1, -1, -1);
	pBoots->AddStatAttribute(AttributeType_Armor, 1);
	InventoryItem* pBody = AddInventoryItem("media/gamedata/equipment/WoodBody/WoodBody.equipment", "media/textures/items/wood_armor.tga", InventoryType_Clothing, eItem_None, ItemStatus_None, EquipSlot_Body, ItemQuality_Uncommon, true, true, "Body Armor", "Body armor.", 1.0f, 1.0f, 1.0f, -1, -1, -1, -1, -1);
	pBody->AddStatAttribute(AttributeType_Armor, 1);
	InventoryItem* pLegs = AddInventoryItem("media/gamedata/equipment/WoodPants/WoodPants.equipment", "media/textures/items/wood_pants.tga", InventoryType_Clothing, eItem_None, ItemStatus_None, EquipSlot_Legs, ItemQuality_Uncommon, true, true, "Leg Armor", "Leg armor.", 1.0f, 1.0f, 1.0f, -1, -1, -1, -1, -1);
	pLegs->AddStatAttribute(AttributeType_Armor, 1);
	*/

	ExportInventory(m_playerName);
}

void InventoryManager::SetInventoryGUINeedsUpdate(bool update)
{
	m_InventoryGUINeedsUpdate = update;
}

bool InventoryManager::InventoryGUINeedsUpdate()
{
	return m_InventoryGUINeedsUpdate;
}

void InventoryManager::SetCharacterGUINeedsUpdate(bool update)
{
	m_CharacterGUINeedsUpdate = update;
}

bool InventoryManager::CharacterGUINeedsUpdate()
{
	return m_CharacterGUINeedsUpdate;
}

void InventoryManager::ExportInventory(string playerName)
{
    ofstream exportFile;
    char lfilename[128];
    sprintf(lfilename, "media/characters/%s/%s.inv", playerName.c_str(), playerName.c_str());
    exportFile.open(lfilename, ios_base::binary);

    if(exportFile.is_open())
    {
		// Export the stored inventory items
        exportFile << MAX_NUM_INVENTORY_SLOTS << "|";

        for(int i = 0; i < MAX_NUM_INVENTORY_SLOTS; i++)
        {
            InventoryItem* lpItem = m_ItemSlotMapping[i];

            if(lpItem == NULL)
            {
                exportFile << "0" << "|";
            }
            else
            {
                exportFile << "1" << "|";
                exportFile << lpItem->m_filename << "|";
                exportFile << lpItem->m_Iconfilename << "|";
                exportFile << (int)lpItem->m_itemType << "|";
                exportFile << (int)lpItem->m_item << "|";
				exportFile << (int)lpItem->m_status << "|";
                exportFile << (int)lpItem->m_equipSlot << "|";
				exportFile << (int)lpItem->m_itemQuality << "|";
				exportFile << lpItem->m_left << "|";
				exportFile << lpItem->m_right << "|";
                exportFile << lpItem->m_title << "|";
                exportFile << lpItem->m_description << "|";
				exportFile << lpItem->m_placementR << "|";
				exportFile << lpItem->m_placementG << "|";
				exportFile << lpItem->m_placementB << "|";
				exportFile << lpItem->m_scale << "|";
				exportFile << lpItem->m_offsetX << "|";
				exportFile << lpItem->m_offsetY << "|";
				exportFile << lpItem->m_offsetZ << "|";
				exportFile << lpItem->m_quantity << "|";

				// Also export the stats attributes
				exportFile << (int)lpItem->m_vpStatAttributes.size() << "|";
				for(int j = 0; j < (int)lpItem->m_vpStatAttributes.size(); j++)
				{
					exportFile << (int)lpItem->m_vpStatAttributes[j]->GetType() << "|";
					exportFile << lpItem->m_vpStatAttributes[j]->GetModifyAmount() << "|";
				}
            }
        }

		// Export the equipped items
		for(int i = 0; i < EquipSlot_NumSlots; i++)
		{
			InventoryItem* lpItem = m_equippedSlots[i];

			if(lpItem == NULL)
			{
				exportFile << "0" << "|";
			}
			else
			{
				exportFile << "1" << "|";
				exportFile << lpItem->m_filename << "|";
                exportFile << lpItem->m_Iconfilename << "|";
				exportFile << (int)lpItem->m_itemType << "|";
                exportFile << (int)lpItem->m_item << "|";
				exportFile << (int)lpItem->m_status << "|";
				exportFile << (int)lpItem->m_equipSlot << "|";
				exportFile << (int)lpItem->m_itemQuality << "|";
				exportFile << lpItem->m_left << "|";
				exportFile << lpItem->m_right << "|";
				exportFile << lpItem->m_title << "|";
				exportFile << lpItem->m_description << "|";
				exportFile << lpItem->m_placementR << "|";
				exportFile << lpItem->m_placementG << "|";
				exportFile << lpItem->m_placementB << "|";
				exportFile << lpItem->m_scale << "|";
				exportFile << lpItem->m_offsetX << "|";
				exportFile << lpItem->m_offsetY << "|";
				exportFile << lpItem->m_offsetZ << "|";
				exportFile << lpItem->m_quantity << "|";

				// Also export the stats attributes
				exportFile << (int)lpItem->m_vpStatAttributes.size() << "|";
				for(int j = 0; j < (int)lpItem->m_vpStatAttributes.size(); j++)
				{
					exportFile << (int)lpItem->m_vpStatAttributes[j]->GetType() << "|";
					exportFile << lpItem->m_vpStatAttributes[j]->GetModifyAmount() << "|";
				}
			}			
		}

        // Export the number of coins
        exportFile << m_numCoins << "|";

		// Export the file signature
        exportFile << "signature: " << "\n";

        exportFile.close();
    }
}

void InventoryManager::ImportInventory(string playerName)
{
	m_playerName = playerName;

    ifstream importFile;
    char lfilename[128];
    sprintf(lfilename, "media/characters/%s/%s.inv", playerName.c_str(), playerName.c_str());
    importFile.open(lfilename, ios_base::binary);

    if(importFile.is_open())
    {
        char * input = new char[128];
        char * pEnd;

		// Import the stored inventory items
        importFile.getline(input, 128, '|');
        int numInventory = strtol(input, &pEnd, 10);

        for(int i = 0; i < numInventory; i++)
        {
            importFile.getline(input, 128, '|');
            int itemValid = strtol(input, &pEnd, 10);

            if(itemValid == 1)
            {
                char filename[128];
                importFile.getline(filename, 128, '|');

                char iconFilename[128];
                importFile.getline(iconFilename, 128, '|');

                importFile.getline(input, 128, '|');
                int itemType = strtol(input, &pEnd, 10);

                importFile.getline(input, 128, '|');
                int item = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int status = strtol(input, &pEnd, 10);

                importFile.getline(input, 128, '|');
                int equipSlot = strtol(input, &pEnd, 10);
				
				importFile.getline(input, 128, '|');
				int itemQuality = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int left = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int right = strtol(input, &pEnd, 10);

                char title[128];
                importFile.getline(title, 128, '|');

                char description[128];
                importFile.getline(description, 128, '|');

				float r;
				float g;
				float b;
				importFile.getline(input, 128, '|');
				r = (float)strtod(input, &pEnd);
				importFile.getline(input, 128, '|');
				g = (float)strtod(input, &pEnd);
				importFile.getline(input, 128, '|');
				b = (float)strtod(input, &pEnd);

				float scale;
				float offsetX;
				float offsetY;
				float offsetZ;

				importFile.getline(input, 128, '|');
				scale = (float)strtod(input, &pEnd);
				importFile.getline(input, 128, '|');
				offsetX = (float)strtod(input, &pEnd);
				importFile.getline(input, 128, '|');
				offsetY = (float)strtod(input, &pEnd);
				importFile.getline(input, 128, '|');
				offsetZ = (float)strtod(input, &pEnd);

                importFile.getline(input, 128, '|');
                int quantity = strtol(input, &pEnd, 10);
                
				// Create the new inventory item				
				InventoryItem* pNewItem = new InventoryItem();
				
                pNewItem->m_filename = filename;

                pNewItem->m_Iconfilename = iconFilename;

				pNewItem->m_title = title;
				pNewItem->m_description =  description;

				pNewItem->m_itemType = (InventoryType)itemType;
                pNewItem->m_item = (eItem)item;
				pNewItem->m_status = (ItemStatus)status;
				pNewItem->m_equipSlot = (EquipSlot)equipSlot;
				pNewItem->m_itemQuality = (ItemQuality)itemQuality;

				pNewItem->m_left = (bool)left;
				pNewItem->m_right = (bool)right;

				pNewItem->m_placementR = r;
				pNewItem->m_placementG = g;
				pNewItem->m_placementB = b;

				pNewItem->m_scale = scale;
				pNewItem->m_offsetX = offsetX;
				pNewItem->m_offsetY = offsetY;
				pNewItem->m_offsetZ = offsetZ;

                pNewItem->m_quantity = quantity;

				pNewItem->m_equipped = false;

				pNewItem->m_remove = false;

				// Also import the stats attributes
				int numStatAttributes;
				importFile.getline(input, 128, '|');
				numStatAttributes = strtol(input, &pEnd, 10);
				for(int k = 0; k < numStatAttributes; k++)
				{
					int type;
					int amount;
					importFile.getline(input, 128, '|');
					type = strtol(input, &pEnd, 10);

					importFile.getline(input, 128, '|');
					amount = strtol(input, &pEnd, 10);

					pNewItem->AddStatAttribute((AttributeType)type, amount);
				}

				// Add to the inventory container
				m_vpInventoryItemList.push_back(pNewItem);

				// Store the item mapping pointer, so we can easily get this when we need to find what items map to what slot positions
				m_ItemSlotMapping[i] = pNewItem;

				SetInventoryGUINeedsUpdate(true);
				SetCharacterGUINeedsUpdate(true);
            }
        }

		// Import the equipped items
		for(int i = 0; i < EquipSlot_NumSlots; i++)
		{
			importFile.getline(input, 128, '|');
			int itemValid = strtol(input, &pEnd, 10);

			if(itemValid == 1)
			{
				char filename[128];
				importFile.getline(filename, 128, '|');

                char iconFilename[128];
                importFile.getline(iconFilename, 128, '|');

				importFile.getline(input, 128, '|');
				int itemType = strtol(input, &pEnd, 10);

                importFile.getline(input, 128, '|');
                int item = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int status = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int equipSlot = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int itemQuality = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int left = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int right = strtol(input, &pEnd, 10);

				char title[128];
				importFile.getline(title, 128, '|');

				char description[128];
				importFile.getline(description, 128, '|');

				float r;
				float g;
				float b;
				importFile.getline(input, 128, '|');
				r = (float)strtod(input, &pEnd);
				importFile.getline(input, 128, '|');
				g = (float)strtod(input, &pEnd);
				importFile.getline(input, 128, '|');
				b = (float)strtod(input, &pEnd);

				float scale;
				float offsetX;
				float offsetY;
				float offsetZ;

				importFile.getline(input, 128, '|');
				scale = (float)strtod(input, &pEnd);
				importFile.getline(input, 128, '|');
				offsetX = (float)strtod(input, &pEnd);
				importFile.getline(input, 128, '|');
				offsetY = (float)strtod(input, &pEnd);
				importFile.getline(input, 128, '|');
				offsetZ = (float)strtod(input, &pEnd);

                importFile.getline(input, 128, '|');
                int quantity = strtol(input, &pEnd, 10);

				// Equip the item
				InventoryItem* pNewItem = new InventoryItem();

				pNewItem->m_filename = filename;

                pNewItem->m_Iconfilename = iconFilename;

				pNewItem->m_title = title;
				pNewItem->m_description = description;

				pNewItem->m_itemType = (InventoryType)itemType;
                pNewItem->m_item = (eItem)item;
				pNewItem->m_status = (ItemStatus)status;
				pNewItem->m_equipSlot = (EquipSlot)equipSlot;
				pNewItem->m_itemQuality = (ItemQuality)itemQuality;

				pNewItem->m_left = (bool)left;
				pNewItem->m_right = (bool)right;

				pNewItem->m_placementR = r;
				pNewItem->m_placementG = g;
				pNewItem->m_placementB = b;

				pNewItem->m_scale = scale;
				pNewItem->m_offsetX = offsetX;
				pNewItem->m_offsetY = offsetY;
				pNewItem->m_offsetZ = offsetZ;

                pNewItem->m_quantity = quantity;

				pNewItem->m_equipped = false;

				pNewItem->m_remove = false;

				// Also import the stats attributes
				int numStatAttributes;
				importFile.getline(input, 128, '|');
				numStatAttributes = strtol(input, &pEnd, 10);
				for(int k = 0; k < numStatAttributes; k++)
				{
					int type;
					int amount;
					importFile.getline(input, 128, '|');
					type = strtol(input, &pEnd, 10);

					importFile.getline(input, 128, '|');
					amount = strtol(input, &pEnd, 10);

					pNewItem->AddStatAttribute((AttributeType)type, amount);
				}

				// Add to the inventory container
				m_vpInventoryItemList.push_back(pNewItem);

				m_equippedSlots[equipSlot] = pNewItem;
				m_equippedSlots[equipSlot]->m_equipped = true;

				m_pInventoryGUI->SetEquippedItem((EquipSlot)equipSlot, title);
				m_pPlayer->EquipItem(pNewItem);

				SetInventoryGUINeedsUpdate(true);
				SetCharacterGUINeedsUpdate(true);
			}
		}

        // Import the number of coins
        importFile.getline(input, 128, '|');
        m_numCoins = strtol(input, &pEnd, 10);
		m_coinsUpdated = true;

		// Import the file signature
		string tempString;
		string signature;
		importFile >> tempString >> signature;

		delete input;

        importFile.close();
    }
}

bool InventoryManager::IsInventoryFull()
{
    for(int i = 0; i < MAX_NUM_INVENTORY_SLOTS; i++)
    {
        if(m_ItemSlotMapping[i] == NULL)
        {
			return false;
		}
	}

	return true;
}

InventoryItem* InventoryManager::CreateInventoryItem(InventoryItem* pItem)
{
	return CreateInventoryItem(pItem->m_filename.c_str(), pItem->m_Iconfilename.c_str(), pItem->m_itemType, pItem->m_item, pItem->m_status, pItem->m_equipSlot, pItem->m_itemQuality, pItem->m_left, pItem->m_right, pItem->m_title.c_str(), pItem->m_description.c_str(), pItem->m_placementR, pItem->m_placementG, pItem->m_placementB, pItem->m_quantity, pItem->m_lootSlotX, pItem->m_lootSlotY, -1, -1);
}

InventoryItem* InventoryManager::CreateInventoryItem(const char* filename, const char* iconFilename, InventoryType itemType, eItem item, ItemStatus status, EquipSlot equipSlot, ItemQuality itemQuality, bool left, bool right, const char* title, const char* description, float r, float g, float b, int quantity, int lootSlotX, int lootSlotY, int setInventorySlotX, int setInventorySlotY)
{
	InventoryItem* pNewItem = new InventoryItem();
	pNewItem->m_filename = filename;
	pNewItem->m_Iconfilename = iconFilename;
	pNewItem->m_title = title;
	pNewItem->m_description = description;

	pNewItem->m_itemType = itemType;
	pNewItem->m_item = item;
	pNewItem->m_status = status;
	pNewItem->m_equipSlot = equipSlot;
	pNewItem->m_itemQuality = itemQuality;

	pNewItem->m_left = left;
	pNewItem->m_right = right;

	pNewItem->m_placementR = r;
	pNewItem->m_placementG = g;
	pNewItem->m_placementB = b;

	pNewItem->m_scale = 0.5f;
	pNewItem->m_offsetX = 0.5f;
	pNewItem->m_offsetY = 0.5f;
	pNewItem->m_offsetZ = 0.5f;

	pNewItem->m_lootSlotX = lootSlotX;
	pNewItem->m_lootSlotY = lootSlotY;

	pNewItem->m_equipped = false;

	pNewItem->m_quantity = quantity;

	pNewItem->m_remove = false;

	m_vpOtherInventoryItemList.push_back(pNewItem);

	return pNewItem;
}

ItemTextData* InventoryManager::GetItemTextData(eItem item)
{
	for(int i = 0; i < (sizeof(g_itemData)/sizeof(g_itemData[0])); i++)
	{
		if(item == g_itemData[i].m_item)
		{
			return &g_itemData[i];
		}
	}

	return NULL;
}

bool InventoryManager::CanDestroyItemWithHammer(eItem item)
{
	for(int i = 0; i < (sizeof(g_itemData)/sizeof(g_itemData[0])); i++)
	{
		if(item == g_itemData[i].m_item)
		{
			return true;
		}
	}

	return false;
}

bool InventoryManager::CanAddInventoryItem(const char* title, eItem item, int quantity)
{
	bool addToExistingItem = false;
	if(quantity != -1)
	{
		addToExistingItem = true;
	}

	if(addToExistingItem)
	{
		// First check if the item already exists in the inventory slots, to add to its quantity
		for(int i = 0; i < MAX_NUM_INVENTORY_SLOTS; i++)
		{
			if(m_ItemSlotMapping[i] != NULL && (strcmp(title, m_ItemSlotMapping[i]->m_title.c_str()) == 0) && m_ItemSlotMapping[i]->m_item == item)
			{
				return true;
			}
		}

		// Next check the equipped slots
		for(int i = 0; i < EquipSlot_NumSlots; i++)
		{
			InventoryItem* lpItem = m_equippedSlots[i];

			if(lpItem != NULL && (strcmp(title, lpItem->m_title.c_str()) == 0) && m_equippedSlots[i]->m_item == item)
			{
				return true;
			}
		}
	}

	// Search for a free slot
	for(int i = 0; i < MAX_NUM_INVENTORY_SLOTS; i++)
	{
		if(m_ItemSlotMapping[i] == NULL)
		{
			return true;
		}
	}

	return false;
}

InventoryItem* InventoryManager::AddInventoryItem(const char* filename, const char* iconFilename, InventoryType itemType, eItem item, ItemStatus status, EquipSlot equipSlot, ItemQuality itemQuality, bool left, bool right, const char* title, const char* description, float r, float g, float b, int quantity, int lootSlotX, int lootSlotY, int setInventorySlotX, int setInventorySlotY)
{
    bool addToExistingItem = false;
    bool createNewitem = true;
    if(quantity != -1)
    {
        addToExistingItem = true;
    }

    // First check if the item already exists in the inventory slots, to add to its quantity
    if(addToExistingItem)
    {
        for(int i = 0; i < MAX_NUM_INVENTORY_SLOTS; i++)
        {
            if(m_ItemSlotMapping[i] != NULL && (strcmp(title, m_ItemSlotMapping[i]->m_title.c_str()) == 0) && m_ItemSlotMapping[i]->m_itemType == itemType)
            {
                createNewitem = false;

                m_ItemSlotMapping[i]->m_quantity += quantity;

				SetInventoryGUINeedsUpdate(true);
				SetCharacterGUINeedsUpdate(true);

				// Export the inventory data since we have changed an item quantity
				ExportInventory(m_playerName);

                return m_ItemSlotMapping[i];
            }
        }

        // Next check the equipped slots
        for(int i = 0; i < EquipSlot_NumSlots; i++)
        {
            InventoryItem* lpItem = m_equippedSlots[i];

            if(lpItem != NULL && (strcmp(title, lpItem->m_title.c_str()) == 0) && m_equippedSlots[i]->m_itemType == itemType)
            {
                createNewitem = false;

                lpItem->m_quantity += quantity;

				SetInventoryGUINeedsUpdate(true);
				SetCharacterGUINeedsUpdate(true);

				// Export the inventory data since we have changed an item quantity
				ExportInventory(m_playerName);

                return lpItem;
            }
        }
    }

    // First find any empty slot in the inventory
    if(createNewitem)
    {
		int indexToUse = -1;

		if(setInventorySlotX != -1 && setInventorySlotY != -1)
		{
			// We have specified an index to use
			int convertedIndex = ConvertSlotsToIndex(setInventorySlotX, setInventorySlotY);
			indexToUse = convertedIndex;
		}
		else
		{
			// Search for a free slot
			for(int i = 0; i < MAX_NUM_INVENTORY_SLOTS; i++)
			{
				if(m_ItemSlotMapping[i] == NULL)
				{
					indexToUse = i;
					break;
				}
			}
		}

		// Create the new item
		if(indexToUse != -1)
		{
			if(m_ItemSlotMapping[indexToUse] == NULL)
			{
				// Create the new inventory item
				InventoryItem* pNewItem = new InventoryItem();
				pNewItem->m_filename = filename;
				pNewItem->m_Iconfilename = iconFilename;
				pNewItem->m_title = title;
				pNewItem->m_description = description;

				pNewItem->m_itemType = itemType;
				pNewItem->m_item = item;
				pNewItem->m_status = status;
				pNewItem->m_equipSlot = equipSlot;
				pNewItem->m_itemQuality = itemQuality;

				pNewItem->m_left = left;
				pNewItem->m_right = right;

				pNewItem->m_placementR = r;
				pNewItem->m_placementG = g;
				pNewItem->m_placementB = b;

				pNewItem->m_scale = 0.5f;
				pNewItem->m_offsetX = 0.5f;
				pNewItem->m_offsetY = 0.5f;
				pNewItem->m_offsetZ = 0.5f;

				pNewItem->m_lootSlotX = lootSlotX;
				pNewItem->m_lootSlotY = lootSlotY;

				pNewItem->m_equipped = false;

				pNewItem->m_quantity = quantity;

				pNewItem->m_remove = false;

				// Add to the inventory container
				m_vpInventoryItemList.push_back(pNewItem);

				// Store the item mapping pointer, so we can easily get this when we need to find what items map to what slot positions
				m_ItemSlotMapping[indexToUse] = pNewItem;

				SetInventoryGUINeedsUpdate(true);
				SetCharacterGUINeedsUpdate(true);

				// Export the inventory data since we have added an item
				ExportInventory(m_playerName);

				return pNewItem;
			}
		}
    }
    
	return NULL;
}

void InventoryManager::AddInventoryItem(InventoryItem* pInventoryItem, int inventoryX, int inventoryY)
{
	InventoryItem* pItem = AddInventoryItem(pInventoryItem->m_filename.c_str(), pInventoryItem->m_Iconfilename.c_str(), pInventoryItem->m_itemType, pInventoryItem->m_item,  pInventoryItem->m_status, pInventoryItem->m_equipSlot, pInventoryItem->m_itemQuality, pInventoryItem->m_left, pInventoryItem->m_right, pInventoryItem->m_title.c_str(), pInventoryItem->m_description.c_str(), pInventoryItem->m_placementR, pInventoryItem->m_placementG, pInventoryItem->m_placementB, pInventoryItem->m_quantity, pInventoryItem->m_lootSlotX, pInventoryItem->m_lootSlotY, inventoryX, inventoryY);
	
	for(int i = 0; i < (int)pInventoryItem->m_vpStatAttributes.size(); i++)
	{
		pItem->AddStatAttribute(pInventoryItem->m_vpStatAttributes[i]->GetType(), pInventoryItem->m_vpStatAttributes[i]->GetModifyAmount());
	}

	pItem->m_scale = pInventoryItem->m_scale;
	pItem->m_offsetX = pInventoryItem->m_offsetX;
	pItem->m_offsetY = pInventoryItem->m_offsetY;
	pItem->m_offsetZ = pInventoryItem->m_offsetZ;
}

void InventoryManager::RemoveInventoryItem(const char* title, eItem item, int quantity)
{
	for(int i = 0; i < MAX_NUM_INVENTORY_SLOTS; i++)
	{
		if(m_ItemSlotMapping[i] != NULL && (strcmp(title, m_ItemSlotMapping[i]->m_title.c_str()) == 0) && m_ItemSlotMapping[i]->m_item == item)
		{
			m_ItemSlotMapping[i]->m_quantity -= quantity;

			if(m_ItemSlotMapping[i]->m_quantity <= 0)
			{
				RemoveInventoryItem(i);
			}

			SetInventoryGUINeedsUpdate(true);
			SetCharacterGUINeedsUpdate(true);

			// Export the inventory data since we have changed an item quantity
			ExportInventory(m_playerName);

			return;
		}
	}

	// Next check the equipped slots
	for(int i = 0; i < EquipSlot_NumSlots; i++)
	{
		InventoryItem* lpItem = m_equippedSlots[i];

		if(lpItem != NULL && (strcmp(title, lpItem->m_title.c_str()) == 0) && m_equippedSlots[i]->m_item == item)
		{
			lpItem->m_quantity -= quantity;

			if(lpItem->m_quantity <= 0)
			{
				m_pPlayer->UnequipItem((EquipSlot)i);
				RemoveInventoryItem((EquipSlot)i);
			}

			SetInventoryGUINeedsUpdate(true);
			SetCharacterGUINeedsUpdate(true);

			// Export the inventory data since we have changed an item quantity
			ExportInventory(m_playerName);

			return;
		}
	}
}

void InventoryManager::RemoveInventoryItem(EquipSlot equipSlot)
{
	m_equippedSlots[equipSlot] = NULL;

	SetInventoryGUINeedsUpdate(true);
	SetCharacterGUINeedsUpdate(true);

	// Export the inventory data since we have removed an item
	ExportInventory(m_playerName);
}

void InventoryManager::RemoveInventoryItem(int slotIndex)
{
    InventoryItem* pNewItem = m_ItemSlotMapping[slotIndex];
    pNewItem->m_remove = true;
    m_ItemSlotMapping[slotIndex] = NULL;

	SetInventoryGUINeedsUpdate(true);
	SetCharacterGUINeedsUpdate(true);

	// Export the inventory data since we have removed an item
	ExportInventory(m_playerName);
}

void InventoryManager::RemoveInventoryItem(int xPos, int yPos)
{
    int index = ConvertSlotsToIndex(xPos, yPos);
    RemoveInventoryItem(index);
}

InventoryItem* InventoryManager::GetInventoryItemForSlot(int slotIndex)
{
    return m_ItemSlotMapping[slotIndex];
}

InventoryItem* InventoryManager::GetInventoryItemForSlot(int xPos, int yPos)
{
    int index = ConvertSlotsToIndex(xPos, yPos);
    return GetInventoryItemForSlot(index);
}

InventoryItem* InventoryManager::GetInventoryItemWithTitle(string title)
{
	for(int i = 0; i < MAX_NUM_INVENTORY_SLOTS; i++)
	{
		if(m_ItemSlotMapping[i] != NULL)
		{
			if(strcmp(m_ItemSlotMapping[i]->m_title.c_str(), title.c_str()) == 0)
			{
				return m_ItemSlotMapping[i];
			}
		}
	}

	return NULL;
}

void InventoryManager::SwitchInventoryItems(int slot1, int slot2)
{
    InventoryItem* pTempItem = m_ItemSlotMapping[slot1];
    m_ItemSlotMapping[slot1] = m_ItemSlotMapping[slot2];
    m_ItemSlotMapping[slot2] = pTempItem;

	// Export the inventory data since we have switched items
	ExportInventory(m_playerName);
}

void InventoryManager::SwitchInventoryItems(int x1, int y1, int x2, int y2)
{
    int index1 = ConvertSlotsToIndex(x1, y1);
    int index2 = ConvertSlotsToIndex(x2, y2);
    SwitchInventoryItems(index1, index2);
}

void InventoryManager::SwitchEquippedSlots(EquipSlot equipSlotSrc, EquipSlot equipSlotDst)
{
	InventoryItem* temp = m_equippedSlots[equipSlotDst];
	m_equippedSlots[equipSlotDst] = m_equippedSlots[equipSlotSrc];
	m_equippedSlots[equipSlotSrc] = temp;
}

void InventoryManager::EquipInventoryItem(int slotIndex, EquipSlot equipSlot)
{
	InventoryItem* pEquippedItem = m_equippedSlots[equipSlot];

	// Equip the new item
	m_equippedSlots[equipSlot] = m_ItemSlotMapping[slotIndex];
	m_equippedSlots[equipSlot]->m_equipped = true;

	// If we already have an item equipped in this slot, unequip it
	m_ItemSlotMapping[slotIndex] = pEquippedItem;
	if(m_ItemSlotMapping[slotIndex] != NULL)
	{
		m_ItemSlotMapping[slotIndex]->m_equipped = false;
	}

	// Refresh stat modifier cache since we have equipped an item
	m_pPlayer->RefreshStatModifierCacheValues();

	// Export the inventory data since we have equipped an item
	ExportInventory(m_playerName);
}

void InventoryManager::EquipInventoryItem(int xPos, int yPos, EquipSlot equipSlot)
{
	int index = ConvertSlotsToIndex(xPos, yPos);
	EquipInventoryItem(index, equipSlot);
}

void InventoryManager::EquipLootItem(int xPos, int yPos, InventoryItem* pInventoryItem, EquipSlot equipSlot)
{
	InventoryItem* pEquippedItem = m_equippedSlots[equipSlot];

	InventoryItem* newItem = CreateInventoryItem(pInventoryItem);
	newItem->m_scale = pInventoryItem->m_scale;
	newItem->m_offsetX = pInventoryItem->m_offsetX;
	newItem->m_offsetY = pInventoryItem->m_offsetY;
	newItem->m_offsetZ = pInventoryItem->m_offsetZ;

	for(int i = 0; i < (int)pInventoryItem->m_vpStatAttributes.size(); i++)
	{
		newItem->AddStatAttribute(pInventoryItem->m_vpStatAttributes[i]->GetType(), pInventoryItem->m_vpStatAttributes[i]->GetModifyAmount());
	}

	// Equip the new item
	m_equippedSlots[equipSlot] = newItem;
	m_equippedSlots[equipSlot]->m_equipped = true;

	// Refresh stat modifier cache since we have equipped an item
	m_pPlayer->RefreshStatModifierCacheValues();

	// Export the inventory data since we have equipped an item
	ExportInventory(m_playerName);
}

InventoryItem* InventoryManager::GetInventoryItemForEquipSlot(EquipSlot equipSlot)
{
	return m_equippedSlots[equipSlot];
}

bool InventoryManager::IsEquippedStatus(ItemStatus status)
{
	for(int i = 0; i < (int)EquipSlot_NumSlots; i++)
	{
		if(m_equippedSlots[(ItemStatus)i] != NULL)
		{
			if(m_equippedSlots[(ItemStatus)i]->m_status == status)
			{
				return true;
			}
		}
	}

	return false;
}

void InventoryManager::UnequipItem(int xPos, int yPos, EquipSlot equipSlot)
{
	int index = ConvertSlotsToIndex(xPos, yPos);
	InventoryItem* lpTemp = m_ItemSlotMapping[index];

	InventoryItem* pEquippedItem = m_equippedSlots[equipSlot];
	m_equippedSlots[equipSlot]->m_equipped = false;

	m_ItemSlotMapping[index] = pEquippedItem;
	m_equippedSlots[equipSlot] = lpTemp;
	if(m_equippedSlots[equipSlot] != NULL)
	{
		m_equippedSlots[equipSlot]->m_equipped = true;
	}

	// Refresh stat modifier cache since we have un-equipped an item
	m_pPlayer->RefreshStatModifierCacheValues();

	// Export the inventory data since we unequipped an item
	ExportInventory(m_playerName);
}

void InventoryManager::UnequipItemToLootGUI(EquipSlot equipSlot)
{
	m_equippedSlots[equipSlot]->m_equipped = false;
	m_equippedSlots[equipSlot] = NULL;

	// Refresh stat modifier cache since we have un-equipped an item
	m_pPlayer->RefreshStatModifierCacheValues();

	// Export the inventory data since we unequipped an item
	ExportInventory(m_playerName);
}

bool InventoryManager::UnequipItemToFreeInventorySlot(EquipSlot equipSlot, int *newSlotX, int *newSlotY)
{
	for(int i = 0; i < MAX_NUM_INVENTORY_SLOTS; i++)
	{
		if(m_ItemSlotMapping[i] == NULL)
		{
			InventoryItem* pEquippedItem = m_equippedSlots[equipSlot];
			if(pEquippedItem != NULL)
			{
				m_equippedSlots[equipSlot]->m_equipped = false;
				m_equippedSlots[equipSlot] = NULL;

				m_ItemSlotMapping[i] = pEquippedItem;

				*newSlotX = i % MAX_NUM_SLOTS_HORIZONTAL;
				*newSlotY = i / MAX_NUM_SLOTS_HORIZONTAL;

				// Refresh stat modifier cache since we have un-equipped an item
				m_pPlayer->RefreshStatModifierCacheValues();

				// Export the inventory data since we unequipped an item
				ExportInventory(m_playerName);
			}

			return true;
		}
	}

	*newSlotX = -1;
	*newSlotY = -1;

	return false;
}

int InventoryManager::ConvertSlotsToIndex(int x, int y)
{
	return (MAX_NUM_SLOTS_HORIZONTAL * y) + x;
}

int InventoryManager::GetNumCoins()
{
    return m_numCoins;
}

void InventoryManager::GiveCoins(int numCoins)
{
    m_numCoins += numCoins;

	m_coinsUpdated = true;

	m_InventoryGUINeedsUpdate = true;
	m_CharacterGUINeedsUpdate = true;
}

void InventoryManager::TakeCoins(int numCoins)
{
    m_numCoins -= numCoins;

	m_coinsUpdated = true;
}

bool InventoryManager::CoinsUpdated()
{
	return m_coinsUpdated;
}

void InventoryManager::SetCoinsUpdated(bool set)
{
	m_coinsUpdated = set;
}

bool needs_removing(InventoryItem* aI)
{
	bool needsErase = aI->m_remove;

	if(needsErase == true)
	{
		delete aI;
	}

	return needsErase;
}

void InventoryManager::Update(float dt)
{
    // Remove any items that need to be removed from the vector container
    m_vpInventoryItemList.erase( remove_if(m_vpInventoryItemList.begin(), m_vpInventoryItemList.end(), needs_removing), m_vpInventoryItemList.end() );
}

/**
 * vim: set ts=4 sw=4 tw=99 noet :
 * ======================================================
 * TF2 Dynamic Schema Injector
 * Written by nosoop
 * ======================================================
 */

#include <stdio.h>
#include "mmsplugin.h"
#include "econmanager.h"
#include "natives.h"

#include <map>

DynSchema g_Plugin;

SMEXT_LINK(&g_Plugin);

static KHook::Return<bool> Hook_LevelInitPost(IServerGameDLL *pThis, char const *pMapName, char const *pMapEntities,char const *pOldLevel, char const *pLandmarkName, bool loadGame, bool background);

static KHook::Virtual<IServerGameDLL, bool, char const *, char const *, char const *, char const *, bool, bool> g_LevelInitHook(&IServerGameDLL::LevelInit, nullptr, Hook_LevelInitPost);

bool DynSchema::SDK_OnLoad(char *error, size_t maxlen, bool late)
{
	g_LevelInitHook.Add(gamedll);

	sharesys->AddNatives(myself, g_EconAttributeNatives);
	g_EconInjectedAttributeType = g_pHandleSys->CreateType("EconDynAttr", &g_EconInjectedAttributeHandler, 0, NULL, NULL, myself->GetIdentity(), NULL);

	/* Prepare our manager */
	if (!g_EconManager.Init(error, maxlen)) {
		return false;
	}

	return true;
}

void DynSchema::SDK_OnUnload() {
	g_LevelInitHook.Remove(gamedll);

	g_pHandleSys->RemoveType(g_EconInjectedAttributeType, myself->GetIdentity());
}

static KHook::Return<bool> Hook_LevelInitPost(IServerGameDLL *pThis, char const *pMapName, char const *pMapEntities, char const *pOldLevel, char const *pLandmarkName, bool loadGame, bool background) {
	// reinstall attributes as needed
	g_EconManager.InstallAttributes();

	return { KHook::Action::Ignore };
}

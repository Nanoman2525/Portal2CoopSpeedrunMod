#include <cstring>
#include <stdexcept>
#include "coop_srm.hpp"
#include "logger.hpp"

typedef void* (*CreateInterfaceFn)(const char* pName, int* pReturnCode);

class IServerPluginCallbacks {
public:
    virtual bool Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory) = 0;
    virtual void Unload() = 0;
    virtual void Pause() = 0;
    virtual void UnPause() = 0;
    virtual const char* GetPluginDescription() = 0;
    virtual void LevelInit(char const* pMapName) = 0;
    virtual void ServerActivate(void* pEdictList, int edictCount, int clientMax) = 0;
    virtual void GameFrame(bool simulating) = 0;
    virtual void LevelShutdown() = 0;
    virtual void ClientFullyConnect(void* pEdict) = 0;
    virtual void ClientActive(void* pEntity) = 0;
    virtual void ClientDisconnect(void* pEntity) = 0;
    virtual void ClientPutInServer(void* pEntity, char const* playername) = 0;
    virtual void SetCommandClient(int index) = 0;
    virtual void ClientSettingsChanged(void* pEdict) = 0;
    virtual int ClientConnect(bool* bAllowConnect, void* pEntity, const char* pszName, const char* pszAddress, char* reject, int maxrejectlen) = 0;
    virtual int ClientCommand(void* pEntity, const void*& args) = 0;
    virtual int NetworkIDValidated(const char* pszUserName, const char* pszNetworkID) = 0;
    virtual void OnQueryCvarValueFinished(int iCookie, void* pPlayerEntity, int eStatus, const char* pCvarName, const char* pCvarValue) = 0;
    virtual void OnEdictAllocated(void* edict) = 0;
    virtual void OnEdictFreed(const void* edict) = 0;
};

class P2CoopSRMServerPlugin : public IServerPluginCallbacks {
    bool Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory) override;
    void Unload() override;
    void Pause() override;
    void UnPause() override;
    const char* GetPluginDescription() override;
    void LevelInit(char const* pMapName) override;
    void ServerActivate(void* pEdictList, int edictCount, int clientMax) override;
    void GameFrame(bool simulating) override;
    void LevelShutdown() override;
    void ClientFullyConnect(void* pEdict) override;
    void ClientActive(void* pEntity) override;
    void ClientDisconnect(void* pEntity) override;
    void ClientPutInServer(void* pEntity, char const* playername) override;
    void SetCommandClient(int index) override;
    void ClientSettingsChanged(void* pEdict) override;
    int ClientConnect(bool* bAllowConnect, void* pEntity, const char* pszName, const char* pszAddress, char* reject, int maxrejectlen) override;
    int ClientCommand(void* pEntity, const void*& args) override;
    int NetworkIDValidated(const char* pszUserName, const char* pszNetworkID) override;
    void OnQueryCvarValueFinished(int iCookie, void* pPlayerEntity, int eStatus, const char* pCvarName, const char* pCvarValue) override;
    void OnEdictAllocated(void* edict) override;
    void OnEdictFreed(const void* edict) override;
};

bool P2CoopSRMServerPlugin::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory) {
    return coopSRM.Load();
}

void P2CoopSRMServerPlugin::Unload() {
    coopSRM.Unload();
}

void P2CoopSRMServerPlugin::ServerActivate(void* pEdictList, int edictCount, int clientMax) {
    coopSRM.ServerActivate();
}

void P2CoopSRMServerPlugin::Pause() {
    coopSRM.Pause();
}

void P2CoopSRMServerPlugin::UnPause() {
    coopSRM.Unpause();
}

const char* P2CoopSRMServerPlugin::GetPluginDescription() {
    return coopSRM.Description();
}


void P2CoopSRMServerPlugin::LevelInit(char const* pMapName) { }
void P2CoopSRMServerPlugin::GameFrame(bool simulating) { }
void P2CoopSRMServerPlugin::LevelShutdown() { }
void P2CoopSRMServerPlugin::ClientFullyConnect(void* pEdict) { }
void P2CoopSRMServerPlugin::ClientActive(void* pEntity) { }
void P2CoopSRMServerPlugin::ClientDisconnect(void* pEntity) { }
void P2CoopSRMServerPlugin::ClientPutInServer(void* pEntity, char const* playername) { }
void P2CoopSRMServerPlugin::SetCommandClient(int index) { }
void P2CoopSRMServerPlugin::ClientSettingsChanged(void* pEdict) { }
int P2CoopSRMServerPlugin::ClientConnect(bool* bAllowConnect, void* pEntity, const char* pszName, const char* pszAddress, char* reject, int maxrejectlen) { return 0; }
int P2CoopSRMServerPlugin::ClientCommand(void* pEntity, const void*& args) { return 0; }
int P2CoopSRMServerPlugin::NetworkIDValidated(const char* pszUserName, const char* pszNetworkID) { return 0; }
void P2CoopSRMServerPlugin::OnQueryCvarValueFinished(int iCookie, void* pPlayerEntity, int eStatus, const char* pCvarName, const char* pCvarValue) { }
void P2CoopSRMServerPlugin::OnEdictAllocated(void* edict) { }
void P2CoopSRMServerPlugin::OnEdictFreed(const void* edict) { }

P2CoopSRMServerPlugin p2CoopSRMServerPlugin;

extern "C" __declspec(dllexport) void* CreateInterface(const char* pName, int* pReturnCode) {
    if (strcmp(pName, "ISERVERPLUGINCALLBACKS002") == 0) {
        if (pReturnCode != nullptr) *pReturnCode = 0;
        return reinterpret_cast<void*>(&p2CoopSRMServerPlugin);
    } else {
        if (pReturnCode != nullptr) *pReturnCode = 1;
        return nullptr;
    }
}

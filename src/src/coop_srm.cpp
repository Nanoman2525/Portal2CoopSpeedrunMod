#include "coop_srm.hpp"

#include "modules.hpp"
#include "scanner.hpp"
#include "vscript.hpp"
#include "logger.hpp"
#include <stdexcept>
#include "commandline.hpp"

#include "script.hpp"

bool CoopSRM::Load() {
    if (this->loaded) {
        LOG("Already loaded.\n");
        this->noUnload = true;
        return false;
    }
    this->loaded = true;

// #ifdef _WIN32
// 	// Sixense builds only exist on Windows
//     std::span<uint8_t> tier0 = Memory::Modules::Get("tier0");
//     ICommandLine g_CmdLine = Memory::Scanner::Scan<ICommandLine>(tier0, "E8 ?? ?? ?? ?? 89 85 F0 FB FF FF", 1);
// 	const char* gamedir = (*g_CmdLine).ParmValue("-game", (*g_CmdLine).ParmValue("-defaultgamedir", "hl2"));
// 	if (strcmp(gamedir, "portal2_sixense") == 0) {
// 		clientModuleName = "client_sixense";
// 		serverModuleName = "server_sixense";
// 	}
// #endif

    this->paused = false;
    return true;
}

void CoopSRM::Unload() {
    if (this->noUnload) {
        this->noUnload = false;
        return;
    }
    this->paused = false;
    this->loaded = false;
}

const char* CoopSRM::Description() {
    return "coop_srm by NULLderef, original by Nanoman2525 - " PLUGIN_VERSION;
}

void CoopSRM::Pause() {
    if (this->paused) {
        return;
    }
    this->paused = true;
    LOG("coop_srm: Plugin is now paused. VScript execution will stop.\n");
}

void CoopSRM::Unpause() {
    if (!this->paused) {
        return;
    }
    this->paused = false;
    LOG("coop_srm: Plugin is now unpaused. VScript execution will continue.\n");
}

void CoopSRM::ServerActivate() {
    if (!this->paused) {
        try {
            std::span<uint8_t> server = Memory::Modules::Get(serverModuleName);
            IScriptVM* g_pScriptVM = **Memory::Scanner::Scan<IScriptVM***>(server, "8B 1D ?? ?? ?? ?? 57 85 DB", 2);
            g_pScriptVM->Run(g_pScriptVM->CompileScript(script));
        } catch (const std::exception& ex) {
            ERROR("%s\n", ex.what());
        }
    }
}

CoopSRM coopSRM;

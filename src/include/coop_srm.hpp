#ifndef COOP_SRM_HPP
#define COOP_SRM_HPP

class CoopSRM {
public:
    bool Load();
    void Unload();
    const char* Description();
    void Pause();
    void Unpause();
    void ServerActivate();

private:
    bool noUnload;
    bool running;
    bool loaded;
    bool paused;

    const char* clientModuleName = "client";
    const char* serverModuleName = "server";
    const char* matchmakingModuleName = "matchmaking";
};

extern CoopSRM coopSRM;

#endif // COOP_SRM_HPP

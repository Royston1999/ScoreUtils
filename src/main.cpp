#include "ScoreUtils.hpp"
#include "main.hpp"
#include "Hooks.hpp"

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Returns a logger, useful for printing debug messages
Logger& getMyLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
}

bool isInit = false;

void ScoreUtils::Init(){
    if (!isInit){
        isInit = true;
        setup(modInfo);
        InstallHooks();
        getMyLogger().info("ScoreUtils has been initialised!");
    }
}
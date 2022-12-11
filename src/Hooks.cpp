#include "main.hpp"
#include "Hooks.hpp"
#include "MaxScoreRetriever.hpp"
#include "ScoreUtils.hpp"

using namespace ScoreUtils::MaxScoreRetriever;
using namespace il2cpp_utils;

MAKE_HOOK_FIND_CLASS_UNSAFE_INSTANCE(MultiLevelStart, "", "MultiplayerLevelScenesTransitionSetupDataSO", "Init", void, Il2CppObject* self, StringW gameMode, Il2CppObject* previewBeatmapLevel, Il2CppObject beatmapDifficulty, Il2CppObject* beatmapCharacteristic, Il2CppObject* difficultyBeatmap, Il2CppObject* overrideColorScheme, Il2CppObject* gameplayModifiers, Il2CppObject* playerSpecificSettings, Il2CppObject* practiceSettings, bool useTestNoteCutSoundEffects){
    MultiLevelStart(self, gameMode, previewBeatmapLevel, beatmapDifficulty, beatmapCharacteristic, difficultyBeatmap, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, useTestNoteCutSoundEffects);
    auto* beatmap = *RunMethod(self, "get_difficultyBeatmap");
    currentDifficultyBeatmap = beatmap;
    if (GetRetrievedMaxScoreCallback().size() >= 1){
        auto* method = MakeGenericMethod(FindMethodUnsafe("UnityEngine", "Resources", "FindObjectsOfTypeAll", 0), {GetClassFromName("", "PlayerDataModel")});
        auto* model = RunStaticMethod<ArrayW<Il2CppObject*>, false>(method)->get(0);
        auto* playerData = *RunMethod(model, "get_playerData");
        acquireMaxScore(playerData, beatmap);
    }
}

MAKE_HOOK_FIND_CLASS_INSTANCE(LevelSelect, "", "StandardLevelDetailView", "RefreshContent", void, Il2CppObject* self) {
    LevelSelect(self);
    auto* playerData = *GetFieldValue(self, "_playerData");
    auto* beatmap = *GetFieldValue(self, "_selectedDifficultyBeatmap");
    currentDifficultyBeatmap = beatmap;
    if (GetRetrievedMaxScoreCallback().size() >= 1) acquireMaxScore(playerData, beatmap);
}

MAKE_HOOK_FIND_VERBOSE(TaskResult, THROW_UNLESS(FindMethodUnsafe(MakeGeneric(GetClassFromName("System.Threading.Tasks", "Task`1"), {GetClassFromName("", "IReadonlyBeatmapData")}), "TrySetResult", 1)), bool, Il2CppObject* self, Il2CppObject* result) {
    bool flag = TaskResult(self, result);
    auto inMap = taskMap.find(self);
    if (inMap != taskMap.end()){
        inMap->second(result);
        taskMap.erase(inMap->first);
    }
    return flag;
}

void InstallHooks(){
    INSTALL_HOOK(getMyLogger(), LevelSelect);
    INSTALL_HOOK(getMyLogger(), MultiLevelStart);
    INSTALL_HOOK(getMyLogger(), TaskResult);
}
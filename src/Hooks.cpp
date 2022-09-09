#include "main.hpp"
#include "Hooks.hpp"
#include "MaxScoreRetriever.hpp"
#include "ScoreUtils.hpp"

using namespace ScoreUtils;

MAKE_HOOK_FIND_CLASS_UNSAFE_INSTANCE(MultiLevelStart, "", "MultiplayerLevelScenesTransitionSetupDataSO", "Init", void, Il2CppObject* self, StringW gameMode, Il2CppObject* previewBeatmapLevel, Il2CppObject beatmapDifficulty, Il2CppObject* beatmapCharacteristic, Il2CppObject* difficultyBeatmap, Il2CppObject* overrideColorScheme, Il2CppObject* gameplayModifiers, Il2CppObject* playerSpecificSettings, Il2CppObject* practiceSettings, bool useTestNoteCutSoundEffects){
    MultiLevelStart(self, gameMode, previewBeatmapLevel, beatmapDifficulty, beatmapCharacteristic, difficultyBeatmap, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, useTestNoteCutSoundEffects);
    auto* beatmap = THROW_UNLESS(il2cpp_utils::RunMethod(self, "get_difficultyBeatmap"));
    MaxScoreRetriever::currentDifficultyBeatmap = beatmap;
    if (MaxScoreRetriever::GetRetrievedMaxScoreCallback().size() >= 1){
        auto* internal = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe("UnityEngine", "Resources", "FindObjectsOfTypeAll", 0));
        auto* generic = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(internal, {il2cpp_utils::GetClassFromName("", "PlayerDataModel")}));
        auto* model = THROW_UNLESS(il2cpp_utils::RunStaticMethod<ArrayW<Il2CppObject*>>(generic)).get(0);
        auto* playerData = THROW_UNLESS(il2cpp_utils::RunMethod(model, "get_playerData"));
        MaxScoreRetriever::acquireMaxScore(playerData, beatmap);
    }
}

MAKE_HOOK_FIND_CLASS_INSTANCE(LevelSelect, "", "StandardLevelDetailView", "RefreshContent", void, Il2CppObject* self) {
    LevelSelect(self);
    auto* playerData = THROW_UNLESS(il2cpp_utils::GetFieldValue(self, "_playerData"));
    auto* beatmap = THROW_UNLESS(il2cpp_utils::GetFieldValue(self, "_selectedDifficultyBeatmap"));
    MaxScoreRetriever::currentDifficultyBeatmap = beatmap;
    if (MaxScoreRetriever::GetRetrievedMaxScoreCallback().size() >= 1) MaxScoreRetriever::acquireMaxScore(playerData, beatmap);
}

void InstallHooks(){
    INSTALL_HOOK(getLogger(), LevelSelect);
    INSTALL_HOOK(getLogger(), MultiLevelStart);
}
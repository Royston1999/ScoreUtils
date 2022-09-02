#include "GlobalNamespace/MultiplayerLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/PlayerDataModel.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"

#include "UnityEngine/Resources.hpp"

#include "main.hpp"
#include "Hooks.hpp"
#include "MaxScoreRetriever.hpp"
#include "ScoreUtils.hpp"

bool canRunNoodleCrashHook;

using namespace GlobalNamespace;

MAKE_HOOK_MATCH(LevelSelect, &StandardLevelDetailView::RefreshContent, void, StandardLevelDetailView* self)
{
    LevelSelect(self);
    canRunNoodleCrashHook = false;
    if (ScoreUtils::MaxScoreRetriever::GetRetrievedMaxScoreCallback().size() >= 1) ScoreUtils::MaxScoreRetriever::acquireMaxScore(self->playerData, self->selectedDifficultyBeatmap);
}

MAKE_HOOK_MATCH(MultiLevelStart, &MultiplayerLevelScenesTransitionSetupDataSO::Init, void, MultiplayerLevelScenesTransitionSetupDataSO* self, ::StringW gameMode, ::GlobalNamespace::IPreviewBeatmapLevel* previewBeatmapLevel, ::GlobalNamespace::BeatmapDifficulty beatmapDifficulty, ::GlobalNamespace::BeatmapCharacteristicSO* beatmapCharacteristic, ::GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, ::GlobalNamespace::ColorScheme* overrideColorScheme, ::GlobalNamespace::GameplayModifiers* gameplayModifiers, ::GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, ::GlobalNamespace::PracticeSettings* practiceSettings, bool useTestNoteCutSoundEffects){
    MultiLevelStart(self, gameMode, previewBeatmapLevel, beatmapDifficulty, beatmapCharacteristic, difficultyBeatmap, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, useTestNoteCutSoundEffects);
    canRunNoodleCrashHook = true;
    if (ScoreUtils::MaxScoreRetriever::GetRetrievedMaxScoreCallback().size() >= 1){
        auto* playerData = UnityEngine::Resources::FindObjectsOfTypeAll<PlayerDataModel*>()->get(0)->get_playerData();
        ScoreUtils::MaxScoreRetriever::acquireMaxScore(playerData, difficultyBeatmap);
    }
}

void InstallHooks(){
    INSTALL_HOOK(getLogger(), LevelSelect);
    INSTALL_HOOK(getLogger(), MultiLevelStart);
}
#include "GlobalNamespace/MultiplayerLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/PlayerDataModel.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"

#include "UnityEngine/Resources.hpp"

#include "main.hpp"
#include "Hooks.hpp"
#include "MaxScoreRetriever.hpp"
#include "ScoreUtils.hpp"

using namespace GlobalNamespace;
using namespace ScoreUtils;

MAKE_HOOK_MATCH(LevelSelect, &StandardLevelDetailView::RefreshContent, void, StandardLevelDetailView* self) {
    LevelSelect(self);
    if (MaxScoreRetriever::GetRetrievedMaxScoreCallback().size() >= 1) MaxScoreRetriever::acquireMaxScore(self->playerData, self->selectedDifficultyBeatmap);
}

MAKE_HOOK_MATCH(MultiLevelStart, &MultiplayerLevelScenesTransitionSetupDataSO::Init, void, MultiplayerLevelScenesTransitionSetupDataSO* self, StringW gameMode, IPreviewBeatmapLevel* previewBeatmapLevel, BeatmapDifficulty beatmapDifficulty, BeatmapCharacteristicSO* beatmapCharacteristic, IDifficultyBeatmap* difficultyBeatmap, ColorScheme* overrideColorScheme, GameplayModifiers* gameplayModifiers, PlayerSpecificSettings* playerSpecificSettings, PracticeSettings* practiceSettings, bool useTestNoteCutSoundEffects){
    MultiLevelStart(self, gameMode, previewBeatmapLevel, beatmapDifficulty, beatmapCharacteristic, difficultyBeatmap, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, useTestNoteCutSoundEffects);
    if (MaxScoreRetriever::GetRetrievedMaxScoreCallback().size() >= 1){
        auto* playerData = UnityEngine::Resources::FindObjectsOfTypeAll<PlayerDataModel*>()->get(0)->get_playerData();
        MaxScoreRetriever::acquireMaxScore(playerData, difficultyBeatmap);
    }
}

void InstallHooks(){
    INSTALL_HOOK(getLogger(), LevelSelect);
    INSTALL_HOOK(getLogger(), MultiLevelStart);
}
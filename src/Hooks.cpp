#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/StandardLevelDetailViewController.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapLevelDataExtensions.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "GlobalNamespace/BeatmapObjectsInTimeRowProcessor.hpp"
#include "GlobalNamespace/BeatmapObjectsInTimeRowProcessor_TimeSliceContainer_1.hpp"
#include "GlobalNamespace/StaticBeatmapObjectSpawnMovementData.hpp"
#include "GlobalNamespace/NoteCutDirectionExtensions.hpp"
#include "GlobalNamespace/Vector2Extensions.hpp"
#include "GlobalNamespace/BeatmapObjectsInTimeRowProcessor_SliderTailData.hpp"
#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/PlayerDataModel.hpp"
#include "GlobalNamespace/MultiplayerLevelScenesTransitionSetupDataSO.hpp"
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

MAKE_HOOK_MATCH(NoodleFix_1, &BeatmapObjectsInTimeRowProcessor::HandleCurrentTimeSliceAllNotesAndSlidersDidFinishTimeSlice, void, BeatmapObjectsInTimeRowProcessor* self,
                BeatmapObjectsInTimeRowProcessor::TimeSliceContainer_1<::GlobalNamespace::BeatmapDataItem*>* allObjectsTimeSlice, float nextTimeSliceTime) {      
    if (!canRunNoodleCrashHook) return;
    return NoodleFix_1(self, allObjectsTimeSlice, nextTimeSliceTime);

}

MAKE_HOOK_MATCH(NoodleFix_2, &StandardLevelScenesTransitionSetupDataSO::Init, void, StandardLevelScenesTransitionSetupDataSO* self, StringW gameMode, IDifficultyBeatmap* difficultyBeatmap, IPreviewBeatmapLevel* previewBeatmapLevel, OverrideEnvironmentSettings* overrideEnvironmentSettings, ColorScheme* overrideColorScheme, GameplayModifiers* gameplayModifiers, PlayerSpecificSettings* playerSpecificSettings, PracticeSettings* practiceSettings, StringW backButtonText, bool useTestNoteCutSoundEffects, bool startPaused)
{
    NoodleFix_2(self, gameMode, difficultyBeatmap, previewBeatmapLevel, overrideEnvironmentSettings, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects, startPaused);
    canRunNoodleCrashHook = true;
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
    INSTALL_HOOK(getLogger(), NoodleFix_1);
    INSTALL_HOOK(getLogger(), NoodleFix_2);
}
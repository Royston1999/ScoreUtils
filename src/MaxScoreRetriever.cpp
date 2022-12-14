#include "ScoreUtils.hpp"
#include "MaxScoreRetriever.hpp"
#include "EasyDelegate.hpp"

#include "custom-types/shared/delegate.hpp"

#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapEnvironmentHelper.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/PlayerDataModel.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/ScoreModel.hpp"

#include "UnityEngine/Resources.hpp"

#include "System/Action_1.hpp"
#include "System/Threading/Tasks/Task_1.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Threading::Tasks;
using namespace EasyDelegate;
using namespace System;

namespace ScoreUtils::MaxScoreRetriever{
    ScoreValuesMap maxScoreValues;
    IDifficultyBeatmap* currentlySelectedMap;

    void addMaxScoreData(IDifficultyBeatmap* difficultyBeatmap, int maxScore){
        auto levelID = difficultyBeatmap->get_level()->i_IPreviewBeatmapLevel()->get_levelID();
        auto characteristic = difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->serializedName;
        auto difficulty = (int)difficultyBeatmap->get_difficulty();

        auto foundLevel = maxScoreValues.find(levelID);
        if (foundLevel == maxScoreValues.end()) {
            maxScoreValues.insert(std::make_pair(levelID, CharacMap{std::make_pair(characteristic, DiffMap{std::make_pair(difficulty, maxScore)})}));
            return;
        }
        auto foundCharac = foundLevel->second.find(characteristic);
        if (foundCharac == foundLevel->second.end()) {
            foundLevel->second.insert(std::make_pair(characteristic, DiffMap{std::make_pair(difficulty, maxScore)}));
            return;
        }
        auto foundDiff = foundCharac->second.find(difficulty);
        if (foundDiff == foundCharac->second.end()) {
            foundCharac->second.insert(std::make_pair(difficulty, maxScore));
        }
        else foundDiff->second = maxScore;
    }

    int RetrieveMaxScoreDataFromCache(){
        auto foundLevel = maxScoreValues.find(currentlySelectedMap->get_level()->i_IPreviewBeatmapLevel()->get_levelID());
        if (foundLevel == maxScoreValues.end()) return -1;
        auto foundCharac = foundLevel->second.find(currentlySelectedMap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->serializedName);
        if (foundCharac == foundLevel->second.end()) return -1;
        auto foundDiff = foundCharac->second.find((int)currentlySelectedMap->get_difficulty());
        if (foundDiff == foundCharac->second.end()) return -1;
        int maxScore = foundDiff->second;
        return maxScore;
    }
    
    using MapTask = Task_1<IReadonlyBeatmapData*>;
    using Action = Action_1<Task*>;

    void RetrieveMaxScoreFromMapData(PlayerData* playerData, IDifficultyBeatmap* difficultyBeatmap, function_ptr_t<void, int> callback){
        currentlySelectedMap = difficultyBeatmap;
        EnvironmentInfoSO* envInfo = BeatmapEnvironmentHelper::GetEnvironmentInfo(difficultyBeatmap);
        PlayerSpecificSettings* settings = playerData->playerSpecificSettings;
        reinterpret_cast<Task*>(difficultyBeatmap->GetBeatmapDataAsync(envInfo, settings))->ContinueWith(MakeDelegate<Action*>([=](MapTask* result){
            IReadonlyBeatmapData* mapData = result->get_ResultOnSuccess();
            int maxScore = mapData != nullptr ? ScoreModel::ComputeMaxMultipliedScoreForBeatmap(mapData) : -1;
            addMaxScoreData(difficultyBeatmap, maxScore);
            if (difficultyBeatmap != currentlySelectedMap) return getLogger().info("Not the selected map! blocking callback!");
            announceScoreAcquired(maxScore, callback);
        }));
    }

    void acquireMaxScore(PlayerData* playerData, IDifficultyBeatmap* difficultyBeatmap){
        int score = RetrieveMaxScoreDataFromCache();
        if (score != -1) return announceScoreAcquired(score);
        RetrieveMaxScoreFromMapData(playerData, difficultyBeatmap);
    }

    void RetrieveMaxScoreDataCustomCallback(function_ptr_t<void, int> callback){
        int score = RetrieveMaxScoreDataFromCache();
        if (score != -1) return callback(score);
        auto* playerData = UnityEngine::Resources::FindObjectsOfTypeAll<PlayerDataModel*>()->get(0)->get_playerData();
        RetrieveMaxScoreFromMapData(playerData, currentlySelectedMap, callback);
    }
}
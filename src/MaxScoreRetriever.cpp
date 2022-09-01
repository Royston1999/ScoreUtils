#include "ScoreUtils.hpp"
#include "MaxScoreRetriever.hpp"

#include "custom-types/shared/delegate.hpp"

#include "GlobalNamespace/ScoreModel.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/BeatmapEnvironmentHelper.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/PlayerDataModel.hpp"

#include "UnityEngine/Resources.hpp"

#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Func_2.hpp"
#include "System/Action_1.hpp"

#include "pinkcore/shared/LevelDetailAPI.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;

namespace ScoreUtils::MaxScoreRetriever{

    ScoreValuesMap maxScoreValues;
    bool hasCJD;
    bool hasNoodle;
    GlobalNamespace::MenuTransitionsHelper* menuTrans;

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

    int RetrieveMaxScoreDataFromCache(IDifficultyBeatmap* difficultyBeatmap){
        auto foundLevel = maxScoreValues.find(difficultyBeatmap->get_level()->i_IPreviewBeatmapLevel()->get_levelID());
        if (foundLevel == maxScoreValues.end()) return -1;
        auto foundCharac = foundLevel->second.find(difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->serializedName);
        if (foundCharac == foundLevel->second.end()) return -1;
        auto foundDiff = foundCharac->second.find((int)difficultyBeatmap->get_difficulty());
        if (foundDiff == foundCharac->second.end()) return -1;
        int maxScore = foundDiff->second;
        return maxScore;
    }

    bool hasNoodleRequirement(std::vector<std::string>& requirements){
        return std::count(requirements.begin(), requirements.end(), "Noodle Extensions");
    }

    using MapTask = System::Threading::Tasks::Task_1<IReadonlyBeatmapData*>;
    using Task = System::Threading::Tasks::Task;
    using Action = System::Action_1<Task*>;
    #define TaskComplete(Func) custom_types::MakeDelegate<Action*>(classof(Action*), static_cast<std::function<void(MapTask*)>>(Func)) \

    void RetrieveMaxScoreFromMapData(GlobalNamespace::PlayerData* playerData, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, function_ptr_t<void, int> callback){
        bool requiresNoodle = hasNoodleRequirement(PinkCore::API::GetCurrentMapData().currentRequirements);     
        if (hasCJD && !hasNoodle && requiresNoodle) callback == nullptr ? announceScoreAcquired(-1) : callback(-1);
        else{
            EnvironmentInfoSO* envInfo = BeatmapEnvironmentHelper::GetEnvironmentInfo(difficultyBeatmap);
            reinterpret_cast<Task*>(difficultyBeatmap->GetBeatmapDataAsync(envInfo, playerData->playerSpecificSettings))->ContinueWith(TaskComplete([=](MapTask* result){
                IReadonlyBeatmapData* mapData = result->get_ResultOnSuccess();
                int maxScore = mapData != nullptr ? ScoreModel::ComputeMaxMultipliedScoreForBeatmap(mapData) : -1;
                if (maxScore != -1) addMaxScoreData(difficultyBeatmap, maxScore);
                callback == nullptr ? announceScoreAcquired(maxScore) : callback(maxScore);
            }));
        }
    }

    void acquireMaxScore(PlayerData* playerData, IDifficultyBeatmap* difficultyBeatmap){
        int score = RetrieveMaxScoreDataFromCache(difficultyBeatmap);
        if (score != -1) return announceScoreAcquired(score);
        RetrieveMaxScoreFromMapData(playerData, difficultyBeatmap);
    }

    void RetrieveMaxScoreDataCustomCallback(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, function_ptr_t<void, int> callback){
        int score = RetrieveMaxScoreDataFromCache(difficultyBeatmap);
        if (score != -1) return callback(score);
        auto* playerData = UnityEngine::Resources::FindObjectsOfTypeAll<PlayerDataModel*>()->get(0)->get_playerData();
        RetrieveMaxScoreFromMapData(playerData, difficultyBeatmap, callback);
    }
}
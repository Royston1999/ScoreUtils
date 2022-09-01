#include "main.hpp"
#include "ScoreUtils.hpp"

ScoreUtils::MaxScoreRetriever::RetrievedMaxScoreEvent onGotMaxScore;

ScoreUtils::MaxScoreRetriever::RetrievedMaxScoreEvent& ScoreUtils::MaxScoreRetriever::GetRetrievedMaxScoreCallback(){
    return onGotMaxScore;
}

void announceScoreAcquired(int x){
    onGotMaxScore.invoke(x);
}
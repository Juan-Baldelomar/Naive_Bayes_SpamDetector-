//
// Created by juan on 26/10/20.
//

#ifndef PA1_T10_TEXTCLASSIFIER_H
#define PA1_T10_TEXTCLASSIFIER_H
#include <string>
#include <vector>
#include <iomanip>
#include <iostream>
#include <stdlib.h>
using namespace std;

/*
 * int code parameter in functions refers to trainingData or testData
 * trainingData = 0
 * testData = 1
 * */

class TextClassifier {
private:
    vector<vector<int>> trainingData;
    vector<vector<int>> testData;
    void transformData(int code);
    void startTraining();
    vector<double> P_Y_X; // P(Y|X)
    vector<double> P_Y_NX;
    vector<double> spam;
    int confusionMatrix[2][2];
    int P_X; // # times email is spam
    int P_NX;

    //statistics
    double precision;
    double accuracy;
    double F1score;
    double recall;
    double specificity;


public:
    TextClassifier(string trainingPath, string testPath);
    TextClassifier(string trainingPath);
    int getTrainingSize();
    double getP_X_Y(int i);
    void showData(int code);
    void showVector(int i, int code);
    void showColumn(int j, int code);

    void predict();
    void showPrediction();
};

ostream &operator<<(ostream &os, const vector< vector<int> > &M);
ostream &operator<<(ostream &os, const vector<int> &V);

#endif //PA1_T10_TEXTCLASSIFIER_H

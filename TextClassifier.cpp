//
// Created by juan on 26/10/20.
//
#include <bits/stdc++.h>
#include "TextClassifier.h"

// metodo para parsear CSV
int parseLine(string line, vector<int>&container){
    string token;
    stringstream lineStream(line);

    //skip first two columns
    getline(lineStream, token, ',');
    getline(lineStream, token, ',');
    while(getline(lineStream, token, ',')){
        stringstream tokenAsNum(token);
        int value;
        tokenAsNum >> value;            //cast to int
        container.push_back(value);
    }
    return 0; //status ok
}

// tools to print data
ostream &operator<<(ostream &os, const vector< vector<int> > &M){
    for(int i = 0; i < M.size(); i++){
        for(auto x:M[i]){
            os << setw(12) << x << setw(12);
        }
        os <<endl;
    }
    return os;
}

ostream &operator<<(ostream &os, const vector<int> &V){
    for(auto x:V)
        os << x << " ";
    os <<endl;
    return os;
}

// CONSTRUCTORES
TextClassifier::TextClassifier(string trainingPath) {
    ifstream file(trainingPath);
    string line, token;

    if (!file.is_open()) {
        cout << "ERROR OPENING FILE" << endl;
        return;
    }

    // count lines
    int counter = 0, nLines = 0;
    while (getline(file, line))
        nLines++;

    // reopen file to reset pointer
    file.close();
    file.open(trainingPath);

    //read headers
    if (file.good()) {
        getline(file, line);
        stringstream lineStream(line);
    }

    // read training data
    while (getline(file, line)) {
        vector<int> row;
        parseLine(line, row);
        trainingData.push_back(row);
    }

    // split data into test and training data
    int spamCount = 0;
    int notSpamCount = 0;
    int spamColumn = trainingData[0].size();

    // count spam
    for (int i = 0; i < trainingData.size(); i++){
        if (trainingData[i][spamColumn - 1] == 1)
            spamCount++;
    }
    notSpamCount = trainingData.size()-spamCount;

    // 30% of spam  and not spam used for testing
    int testSpamCount = 0.3* spamCount;
    int testNotSpamCount = 0.3*notSpamCount;

    // remove data from training and insert it to test
    for (int i = 0, count=0; count<testSpamCount; i++){
        vector<int> tmpRow = trainingData[i];
        if (tmpRow[spamColumn-1] == 1){
            testData.push_back(tmpRow);
            trainingData.erase(trainingData.begin()+i);
            i--;                                                                //decrease pointer position because row was removed
            count++;                                                            //increase counter of testData inserted
        }
    }

    for (int i = 0, count=0; count<testNotSpamCount; i++){
        vector<int> tmpRow = trainingData[i];
        if (tmpRow[spamColumn-1] == 0){
            testData.push_back(tmpRow);
            trainingData.erase(trainingData.begin()+i);
            i--;                                                                //decrease pointer position because row was removed
            count++;                                                            //increase counter of testData inserted
        }
    }

    file.close();

    // transform testData
    transformData(1);

    // asign atribute values
    P_Y_X.assign(trainingData[0].size(), 0.0);
    P_Y_NX.assign(trainingData[0].size(), 0.0);
    spam.assign(testData.size(), 0);
    confusionMatrix[0][0] = confusionMatrix[0][1] = confusionMatrix[1][0] = confusionMatrix[1][1] = 0;
    P_X = 0;
    P_NX = 0;

    // start training and prediction
    startTraining();
    predict();
    showPrediction();
}


TextClassifier::TextClassifier(string trainingPath, string testPath) {
    ifstream file(trainingPath);
    string line, token;

    if (!file.is_open()){
        cout << "ERROR OPENING TRAINING FILE" << endl;
        return;
    }
    //read headers
    if (file.good()){
        getline(file, line);
        stringstream lineStream(line);
    }
    // read training data
    while(getline(file, line)){
        vector<int> row;
        parseLine(line, row);
        trainingData.push_back(row);
    }
    file.close();

    file.open(testPath);
    if (!file.is_open()){
        cout << "ERROR OPENING TEST FILE" << endl;
        return;
    }
    //read headers
    if (file.good()){
        getline(file, line);
        stringstream lineStream(line);
    }
    // read training data
    while(getline(file, line)){
        vector<int> row;
        parseLine(line, row);
        testData.push_back(row);
    }
    file.close();
    // transformData
    transformData(1);

    // asign atribute values
    P_Y_X.assign(trainingData[0].size(), 0.0);
    P_Y_NX.assign(trainingData[0].size(), 0.0);
    spam.assign(testData.size(), 0);
    confusionMatrix[0][0] = confusionMatrix[0][1] = confusionMatrix[1][0] = confusionMatrix[1][1] = 0;
    P_X = 0;
    P_NX = 0;

    // start training and prediction
    startTraining();
    predict();
    showPrediction();
}

// METODOS PARA IMPRIMIR DATA DEL OBJETO

void TextClassifier::showData(int code) {
    if (code == 0)
        cout << trainingData << endl;
    else
        cout << testData << endl;
}

void TextClassifier::showVector(int i, int code) {
    if (code == 0)
        cout << trainingData[i] << endl;
    else
        cout << testData[i] << endl;
}

void TextClassifier::showColumn(int j, int code) {
    vector<vector<int>> data;
    if (code == 0)
        data = trainingData;
    else
        data = testData;

    int total = 0;
    int spam = 0;
    for (int i = 0; i<data.size(); i++){
        cout << data[i][j] <<  endl;
    }
}

int TextClassifier::getTrainingSize() {
    return trainingData[0].size();
}

double TextClassifier::getP_X_Y(int i) {
    return P_Y_X[i];
}

// TRANSFORMAR EN VECTOR DE CARACTERISTICAS
void TextClassifier::transformData(int code) {
    vector<vector<int>> *data;
    if (code == 0)
        data = &trainingData;
    else
        data = &testData;

    int n = (*data).size();
    int m = (*data)[0].size();

    for (int i = 0; i<n; i++)
        for (int j = 0; j<m-1; j++)
            (*data)[i][j] = (*data)[i][j] > 0;
}

void TextClassifier::startTraining() {
    int n = trainingData.size();
    int m = trainingData[0].size();

    // count spam and not spam words
    for (int j = 0; j<m-1; j++){
        int spamCountCurrentWord = 0;
        int notSpamCountCurrentWord = 0;
        for (int i = 0; i<n; i++){
            if (trainingData[i][m-1] == 1)
                spamCountCurrentWord += trainingData[i][j];         //count spam words
            else
                notSpamCountCurrentWord += trainingData[i][j];      //count not spam words
        }
        // if there is no word in intersection of both, ignore them
        if (spamCountCurrentWord > 0 && notSpamCountCurrentWord > 0){
            P_X+= spamCountCurrentWord;
            P_NX+= notSpamCountCurrentWord;
        }
    }

    // # times word is in spam and is not in spam
    for (int i = 0; i<n; i++){
        for (int j = 0; j<m-1; j++){
            if (trainingData[i][m-1] == 1 && trainingData[i][j] > 0)           //email is spam and contains word
                    P_Y_X[j] += trainingData[i][j];
            else if (trainingData[i][m-1] == 0 && trainingData[i][j] > 0)      //email is not spam and contains word
                    P_Y_NX[j] += trainingData[i][j];
        }
    }

    // P(Y[i]|X)
    for (int j = 0; j<m-1; j++){
        // if intersection is not null
        if (P_Y_X[j] > 0 && P_Y_NX[j] > 0){
            P_Y_X[j] /= P_X;
            P_Y_NX[j] /= P_NX;
        }else{
            P_Y_X[j] = 0;
            P_Y_NX[j] = 0;
        }
    }
}

void TextClassifier::predict() {
    int training_size = trainingData.size();
    int n = testData.size();
    int m = testData[0].size();
    double p_x = (P_X*1.00) / training_size;
    double p_nx = (P_NX*1.00) / training_size;

    // performo prediction for every element of testData
    for (int i = 0; i<n; i++){
        double spamProd = 0;
        double notSpamProd = 0;
        // calculate with Naives
        for (int j = 0; j<m-1; j++){
            if (testData[i][j] == 1){
                if (P_Y_X[j]!=0){
                    spamProd+= log(P_Y_X[j]);                   // formula modified to use log(x_1*x_2...) = log(x_1) + log(x_2) + ... for stability reasons
                    notSpamProd+= log(P_Y_NX[j]);
                }
            }
        }
        // determine class of testVector
        if (log(p_x) + spamProd > log(p_nx) + notSpamProd)
            spam[i] = 1;
        else
            spam[i] = 0;
    }

    // statistics
    for (int i = 0; i<n; i++){
        if (spam[i] == 1 && testData[i][m-1] == 1)
            confusionMatrix[0][0]++;                             //true positives
        else if (spam[i] == 0 && testData[i][m-1] == 0)
            confusionMatrix[1][1]++;                            //true negatives
        else if (spam[i] == 1 && testData[i][m-1] == 0)
            confusionMatrix[0][1]++;                             //falsePositive
        else
            confusionMatrix[1][0]++;                             //falseNegative
    }

    // statistics
    accuracy = ((confusionMatrix[0][0] + confusionMatrix[1][1])*1.00)/n;
    precision = confusionMatrix[0][0]/((confusionMatrix[0][0] + confusionMatrix[0][1])*1.00);
    recall = confusionMatrix[0][0]/((confusionMatrix[0][0] + confusionMatrix[1][0])*1.00);
    specificity =  confusionMatrix[1][1]/((confusionMatrix[0][1] + confusionMatrix[1][1])*1.00);
    F1score = 2*precision*recall / (precision + recall);
}

void TextClassifier::showPrediction() {
    int n = testData.size();
    int m = testData[0].size();
    cout << setprecision(10);

    cout << "CONFUSION MATRIX: " << endl;
    cout << setw(5) << confusionMatrix[0][0] << setw(5) << confusionMatrix[0][1] << "\n" << setw(5) << confusionMatrix[1][0] << setw(5) << confusionMatrix[1][1] <<endl;
    cout << "accuracy: " <<  accuracy <<endl;
    cout << "precision: " << precision <<endl;
    cout << "recall: " << recall <<endl;
    cout << "Specificity: " << specificity <<endl;
    cout << "F1 Score: " << F1score <<endl;

}



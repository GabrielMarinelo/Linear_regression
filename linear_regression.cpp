#include <iostream>
#include <vector>
#include <getopt.h>
#include <fstream>
#include <TF1.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TApplication.h> 
 
using namespace std;

float loss_function(vector<float> predict, vector<float> actual);
void model(float* b0, float* b1, vector<float> predict, vector<float> actual, vector<float> x, float learning_rate);

int main(int argc, char **argv){ 
    
    TApplication app("app", nullptr, nullptr);	
    TGraph *gr = new TGraph();
    TCanvas *c1 = new TCanvas("canvas","Plot");
    TF1 *f1 = new TF1("f1","[0]+[1]*x", 0, 21);
    
    fstream file;
    int size = 0, option_index;
    float b1 = .01, b0 = .01, learning_rate = 1e-3;
    string content, file_name = "data2.txt";

    while(( option_index = getopt(argc, argv, "hr:n:" )) != -1){
        switch (option_index){
            case 'h':
                cout << "-r to set the learning rate" << endl;
                cout << "-n to set the file's name" << endl;
                break;
            case 'r':
                learning_rate = stod(optarg);
                break;
            case 'n':
                file_name = optarg;
                break;    
            default :
                return 1;
        }
    }

    file.open(file_name);
	while(getline(file, content)) size++;
    file.close();

    vector<float> x(size), y_actual(size), y_predict(size);

    file.open(file_name);
    int i = 0;
    while(1)
    {
        file >> x[i] >> y_actual[i];
        gr -> SetPoint(gr -> GetN(), x[i], y_actual[i]); 
        i++;
        if(file.eof()) break;
    }
    file.close();

    float check;
    do{
        check = loss_function(y_predict, y_actual);
        f1 -> SetParameter(0, b0);
        f1 -> SetParameter(1, b1);
        for(int i = 0; i < size; i++) y_predict[i] = f1 -> Eval(x[i]);
        model(&b0, &b1, y_predict, y_actual, x, learning_rate);  
    }while(loss_function(y_predict, y_actual) != check);

    f1 -> SetParameter(0, b0);
    f1 -> SetParameter(1, b1);

    cout << "Loss: " << loss_function(y_predict, y_actual) << endl;
    cout << "slope: " << b1 << endl;
    cout << "intercept: " << b0 << endl;
    
    f1 -> SetLineColor(2);
    f1 -> SetLineWidth(3);
    
    gr -> SetMarkerColor(4);      
    gr -> SetMarkerStyle(8);   
    gr -> Draw("AP");
    f1 -> Draw("same");

    // app.Run();
    
    return 0;
}

void model(float* b0, float* b1, vector<float> predict, vector<float> actual, vector<float> x, float learning_rate){
    for(int i = 0; i < predict.size(); i++){ 
        *b1 -= learning_rate*2*x[i]*(predict[i]- actual[i])/predict.size();
        *b0 -= learning_rate*2*(predict[i]- actual[i])/predict.size();
    }
}

float loss_function(vector<float> predict, vector<float> actual){
    float sum = 0;
    for(int i = 0; i < predict.size(); i++) sum += pow(predict[i]- actual[i], 2);
    return sum /= predict.size();
}
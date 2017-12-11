#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <iostream>
#include <cmath>

#include "classifier.h"
#include "EasyBMP.h"
#include "linear.h"
#include "argvparser.h"
#include "matrix.h"

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::pair;
using std::make_pair;
using std::cout;
using std::cerr;
using std::endl;

using CommandLineProcessing::ArgvParser;

typedef vector<pair<BMP*, int> > TDataSet;
typedef vector<pair<string, int> > TFileList;
typedef vector<pair<vector<float>, int> > TFeatures;

// Load list of files and its labels from 'data_file' and
// stores it in 'file_list'
void LoadFileList(const string& data_file, TFileList* file_list) {
    ifstream stream(data_file.c_str());

    string filename;
    int label;
    
    int char_idx = data_file.size() - 1;
    for (; char_idx >= 0; --char_idx)
        if (data_file[char_idx] == '/' || data_file[char_idx] == '\\')
            break;
    string data_path = data_file.substr(0,char_idx+1);
    
    while(!stream.eof() && !stream.fail()) {
        stream >> filename >> label;
        if (filename.size())
            file_list->push_back(make_pair(data_path + filename, label));
    }

    stream.close();
}

// Load images by list of files 'file_list' and store them in 'data_set'
void LoadImages(const TFileList& file_list, TDataSet* data_set) {
    for (size_t img_idx = 0; img_idx < file_list.size(); ++img_idx) {
            // Create image
        BMP* image = new BMP();
            // Read image from file
        image->ReadFromFile(file_list[img_idx].first.c_str());
            // Add image and it's label to dataset
        data_set->push_back(make_pair(image, file_list[img_idx].second));
    }
}

// Save result of prediction to file
void SavePredictions(const TFileList& file_list,
                     const TLabels& labels, 
                     const string& prediction_file) {
        // Check that list of files and list of labels has equal size 
    assert(file_list.size() == labels.size());
        // Open 'prediction_file' for writing
    ofstream stream(prediction_file.c_str());

        // Write file names and labels to stream
    for (size_t image_idx = 0; image_idx < file_list.size(); ++image_idx)
        stream << file_list[image_idx].first << " " << labels[image_idx] << endl;
    stream.close();
}

// Exatract features from dataset.
// You should implement this function by yourself =)

float br(BMP* srcImage, int i, int j) {
    RGBApixel pixel = srcImage->GetPixel(i, j);
    float f = 0.299*pixel.Red + 0.587*pixel.Green + 0.114*pixel.Blue;
    return f;
}

void ExtractFeatures(const TDataSet& data_set, TFeatures* features)
{
    std::cout << data_set.size() << " ";
    int N_buckets = 12;

    int X_blocks = 10;
    int Y_blocks = 10;

    for (size_t image_idx = 0; image_idx < data_set.size(); ++image_idx) {

        BMP* Im = data_set[image_idx].first;
        int mark = data_set[image_idx].second;
        int W = (*Im).TellWidth();
        int H = (*Im).TellHeight();
        int w = static_cast<int>(ceil(W / X_blocks));
        int h = static_cast<int>(ceil(H / Y_blocks));
        Matrix<float> g1(W, H), g2(W, H), mod(W, H), img(W, H);
        Matrix<int> dir(W, H), LBP(W, H);
        Matrix < vector < float>> hog(X_blocks, Y_blocks);
        //HOG
        for (int i = 0; i < W; i++) {
            for (int j = 0; j < H; j++) {
                img(i, j) = br(Im, i, j);
                if (i == 0) {
                    g1(i, j) = br(Im, i + 1, j) - br(Im, i, j);
                }
                else if (i == W - 1) {
                    g1(i, j) = br(Im, i, j) - br(Im, i - 1, j);
                }
                else {
                    g1(i, j) = (br(Im, i + 1, j) - br(Im, i - 1, j)) / 2;
                }
                if (j == 0) {
                    g2(i, j) = br(Im, i, j + 1) - br(Im, i, j);
                }
                else if (j == H - 1) {
                    g2(i, j) = br(Im, i, j) - br(Im, i, j - 1);
                }
                else {
                    g2(i, j) = (br(Im, i, j + 1) - br(Im, i, j - 1)) / 2;
                }
            }
        }
        for (int i = 0; i < W; i++) {
            for (int j = 0; j < H; j++) {
                mod(i, j) = sqrt(g1(i, j) * g1(i, j) + g2(i, j) * g2(i, j));
                dir(i, j) = static_cast<int>((atan2(g1(i, j), g2(i, j)) + 3.1415926) * N_buckets / (2 * 3.1415926));
                //std::cout << mod(i, j) << " " << dir(i, j) << std::endl;
            }
        }
        for (int i = 0; i < X_blocks; i++) {
            for (int j = 0; j < Y_blocks; j++) {
                hog(i, j) = std::vector<float>(N_buckets, 0.0);
                float s = 0;
                for (int k = 0; (k < w) && (i * X_blocks + k < W); k++) {
                    for (int l = 0; (l < h) && (j * Y_blocks + l < H); l++) {
                        hog(i, j)[dir(i * X_blocks + k, j * Y_blocks + l)] += mod(i * X_blocks + k, j * Y_blocks + l);
                        //s += mod(i * X_blocks + k, j * Y_blocks + l) * mod(i * X_blocks + k, j * Y_blocks + l);
                    }
                } 
                
                for(int k = 0; k < N_buckets; k++) {
                    s += hog(i, j)[k] * hog(i, j)[k];
                }
                if (s > 1e-8) {
                    for (int k = 0; k < N_buckets; k++) {
                        hog(i, j)[k] /= sqrt(s);
                    }
                }
            }
        }
        std::vector<float> ans;
        for (int i = 0; i < X_blocks; i++) {
            for (int j = 0; j < Y_blocks; j++) {
                ans.insert( ans.end(), hog(i, j).begin(), hog(i, j).end());
            }
        }
        
        // LBP
        img = img.extra_borders(1, 1);
        
        for (int i = 1; i < W + 1; i++) {
            for (int j = 1; j < H + 1; j++) {
                int push = 0;
                int cnt = 0;
                for(int k = -1; k <= 1; k++) {
                    for (int l = -1; l <= 1; l++) {
                        if (0 == l && k == 0) continue;
                        if (img(i, j) <= img(i + k, j + l)) {
                            push += 1 << cnt;
                        }
                        cnt++;
                    }
                }
                LBP(i - 1, j - 1) = push;
                if (push > 255) {std::cout << i << " " << j;}
            }
        }
        float tmp[255]; 
        for(int i = 0; i < 255; i++) tmp[i] = 0.0;
        for (int i = 0; i < X_blocks; i++) {
            for (int j = 0; j < Y_blocks; j++) {
                float s = 0;
                for (int k = 0; (k < w) && (i * X_blocks + k < W); k++) {
                    for (int l = 0; (l < h) && (j * Y_blocks + l < H); l++) {
                        tmp[LBP(i * X_blocks + k, j * Y_blocks + l)] += 1.0; 
                    }
                }
                for (int k = 0; k < 255; k++) {
                    s += tmp[k] * tmp[k];
                }
                if (s > 1e-8) {
                    for (int k = 0; k < 255; k++) {
                        tmp[k] /= sqrt(s); 
                    }
                }
                for(int k = 0; k < 255; k++) {
                    ans.push_back(tmp[k]); 
                    tmp[k] = 0.0;
                }
            }
        }  
        // COLOR
        

        for (int i = 0; i < X_blocks; i++) {
            for (int j = 0; j < Y_blocks; j++) {
                int r = 0;
                int g = 0;
                int b = 0;
                int cnt = 0;
                for (int k = 0; (k < w) && (i * X_blocks + k < W); k++) {
                    for (int l = 0; (l < h) && (j * Y_blocks + l < H); l++) {
                        RGBApixel pixel = Im->GetPixel(i * X_blocks + k, j * X_blocks + l);
                        r += pixel.Red;
                        g += pixel.Green;
                        b += pixel.Blue;
                        cnt++;
                    }
                }
                cnt *= 255;
                if (cnt == 0) {
                    ans.push_back(0.0);
                    ans.push_back(0.0);
                    ans.push_back(0.0);
                    continue;
                }
                ans.push_back((static_cast<float> (r)) / cnt);
                ans.push_back((static_cast<float> (g)) / cnt);
                ans.push_back((static_cast<float> (b)) / cnt);
            }
        }  



        features->push_back(std::pair<vector<float>, int>(ans, mark));
    }
}

// Clear dataset structure
void ClearDataset(TDataSet* data_set) {
        // Delete all images from dataset
    for (size_t image_idx = 0; image_idx < data_set->size(); ++image_idx)
        delete (*data_set)[image_idx].first;
        // Clear dataset
    data_set->clear();
}

// Train SVM classifier using data from 'data_file' and save trained model
// to 'model_file'
void TrainClassifier(const string& data_file, const string& model_file) {
        // List of image file names and its labels
    TFileList file_list;
        // Structure of images and its labels
    TDataSet data_set;
        // Structure of features of images and its labels
    TFeatures features;
        // Model which would be trained
    TModel model;
        // Parameters of classifier
    TClassifierParams params;
    
        // Load list of image file names and its labels
    LoadFileList(data_file, &file_list);
        // Load images
    LoadImages(file_list, &data_set);
        // Extract features from images
    ExtractFeatures(data_set, &features);

        // PLACE YOUR CODE HERE
        // You can change parameters of classifier here
    params.C = 0.01;
    TClassifier classifier(params);
        // Train classifier
    classifier.Train(features, &model);
        // Save model to file
    model.Save(model_file);
        // Clear dataset structure
    ClearDataset(&data_set);
}

// Predict data from 'data_file' using model from 'model_file' and
// save predictions to 'prediction_file'
void PredictData(const string& data_file,
                 const string& model_file,
                 const string& prediction_file) {
        // List of image file names and its labels
    TFileList file_list;
        // Structure of images and its labels
    TDataSet data_set;
        // Structure of features of images and its labels
    TFeatures features;
        // List of image labels
    TLabels labels;

        // Load list of image file names and its labels
    LoadFileList(data_file, &file_list);
        // Load images
    LoadImages(file_list, &data_set);
        // Extract features from images
    ExtractFeatures(data_set, &features);

        // Classifier 
    TClassifier classifier = TClassifier(TClassifierParams());
        // Trained model
    TModel model;
        // Load model from file
    model.Load(model_file);
        // Predict images by its features using 'model' and store predictions
        // to 'labels'
    classifier.Predict(features, model, &labels);

        // Save predictions
    SavePredictions(file_list, labels, prediction_file);
        // Clear dataset structure
    ClearDataset(&data_set);
}

int main(int argc, char** argv) {
    // Command line options parser
    ArgvParser cmd;
        // Description of program
    cmd.setIntroductoryDescription("Machine graphics course, task 2. CMC MSU, 2014.");
        // Add help option
    cmd.setHelpOption("h", "help", "Print this help message");
        // Add other options
    cmd.defineOption("data_set", "File with dataset",
        ArgvParser::OptionRequiresValue | ArgvParser::OptionRequired);
    cmd.defineOption("model", "Path to file to save or load model",
        ArgvParser::OptionRequiresValue | ArgvParser::OptionRequired);
    cmd.defineOption("predicted_labels", "Path to file to save prediction results",
        ArgvParser::OptionRequiresValue);
    cmd.defineOption("train", "Train classifier");
    cmd.defineOption("predict", "Predict dataset");
        
        // Add options aliases
    cmd.defineOptionAlternative("data_set", "d");
    cmd.defineOptionAlternative("model", "m");
    cmd.defineOptionAlternative("predicted_labels", "l");
    cmd.defineOptionAlternative("train", "t");
    cmd.defineOptionAlternative("predict", "p");

        // Parse options
    int result = cmd.parse(argc, argv);

        // Check for errors or help option
    if (result) {
        cout << cmd.parseErrorDescription(result) << endl;
        return result;
    }

        // Get values 
    string data_file = cmd.optionValue("data_set");
    string model_file = cmd.optionValue("model");
    bool train = cmd.foundOption("train");
    bool predict = cmd.foundOption("predict");

        // If we need to train classifier
    if (train)
        TrainClassifier(data_file, model_file);
        // If we need to predict data
    if (predict) {
            // You must declare file to save images
        if (!cmd.foundOption("predicted_labels")) {
            cerr << "Error! Option --predicted_labels not found!" << endl;
            return 1;
        }
            // File to save predictions
        string prediction_file = cmd.optionValue("predicted_labels");
            // Predict data
        PredictData(data_file, model_file, prediction_file);
    }
}
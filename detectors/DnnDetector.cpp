#include <opencv2/core/version.hpp>
#if CV_MAJOR_VERSION > 3
#include "DnnDetector.h"
#include <regex>
#include <fstream> 

using namespace std;
using namespace cv;
using namespace dnn;

namespace eod{
    
    vector<String> getOutputsNames(const Net& net)
	{
	    vector<String> names;  // was static
	    if (names.empty()) // commented because of https://answers.opencv.org/question/202612/using-two-dnn-networks-causes-crash/
	    {
	        //Get the indices of the output layers, i.e. the layers with unconnected outputs
	        vector<int> outLayers = net.getUnconnectedOutLayers();
	        //get the names of all the layers in the network
	        vector<String> layersNames = net.getLayerNames();
	        // Get the names of the output layers in names
	        names.resize(outLayers.size());
	        for (size_t i = 0; i < outLayers.size(); ++i)
	        names[i] = layersNames[outLayers[i] - 1];
	    }
	    return names;
	}
	
	bool readLabelsMap(string fileName, map<int, string> &labelsMap){
        // Read file into a string        
        ifstream t(fileName);
        if (t.bad()){
            printf("Dnn attribute: error reading label map!\n");
            return false;
        }
        
        stringstream buffer;
        buffer << t.rdbuf();
        string fileString = buffer.str();

        // Search entry patterns of type 'item { ... }' and parse each of them
        smatch matcherEntry;
        smatch matcherId;
        smatch matcherName;
        const regex reEntry("item \\{([\\S\\s]*?)\\}");
        const regex reId(" [0-9]+");
        const regex reDispName("\"[a-z\\s]+\"");
        
        string entry;

        auto stringBegin = sregex_iterator(fileString.begin(), fileString.end(), reEntry);
        auto stringEnd = sregex_iterator();

        int id;
        string name;
        for (sregex_iterator i = stringBegin; i != stringEnd; i++) {
            matcherEntry = *i;
            entry = matcherEntry.str();
            regex_search(entry, matcherId, reId);
            if (!matcherId.empty())
                id = stoi(matcherId[0].str());
            else
                continue;
            regex_search(entry, matcherName, reDispName);
            if (!matcherName.empty())
                name = matcherName[0].str().substr(1, matcherName[0].str().length() - 2);
            else
                name = "unknown";
                //continue;
            labelsMap.insert(pair<int, string>(id, name));            
        }
        return true;
    }
    
    // ---------------
    // EXTENDED OBJECT DETECTION LIB INTERFACE
    // ---------------
    
    DnnAttribute::DnnAttribute(){
        Type = DNN_A;        
    }
    
    DnnAttribute::DnnAttribute(string framework_name, string weights_file, string config_file, int inpWidth_, int inpHeight_, string labelmapfile, bool forceCuda ){
        Type = DNN_A;          
        
        transform(framework_name.begin(), framework_name.end(), framework_name.begin(),[](unsigned char c){ return tolower(c); });
        
        this->framework_name = framework_name;
        this->weights_file = weights_file;
        this->config_file = config_file;
        this->label_file = labelmapfile;
        
        if( framework_name == "tensorflow" || framework_name == "tf" ){
            framework = TF_DNN_FW;
            net = readNetFromTensorflow(weights_file, config_file);            
            net.setPreferableBackend(DNN_BACKEND_OPENCV);
            net.setPreferableTarget(DNN_TARGET_CPU);
            if( net.empty() ){
                printf("Unable to load tensorflow net in DnnAttribute\n");
                return;
            }
            inited = true;
        }
        else if( framework_name == "darknet" ){
            framework = DN_DNN_FW;
            net = readNetFromDarknet(config_file, weights_file);
            net.setPreferableBackend(DNN_BACKEND_OPENCV);
            net.setPreferableTarget(DNN_TARGET_CPU);
            if( net.empty() ){
                printf("Unable to load darknet net in DnnAttribute\n");
                return;
            }
            inited = true;
        }                
        else{
            printf("Error: unknown DNN framework '%s' in DnnAtribute\n",framework_name.c_str());
            return;
        }     
        
        if( forceCuda ){
            net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
            net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
        }
    
        inpWidth = inpWidth_;
        inpHeight = inpHeight_;            
        isLabelMap = readLabelsMap(labelmapfile, labelMap);
    }
    
    vector<ExtendedObjectInfo> DnnAttribute::Detect2(const Mat& image, int seq){
        if( prev_seq != seq || seq == 0){        
            // input net
            Mat blob;        
            if( framework == DN_DNN_FW )
                blobFromImage(image, blob, 1/255.0, Size(inpWidth,inpHeight), Scalar(0,0,0), true, false);
            
            else if( framework == TF_DNN_FW )
                blobFromImage(image, blob, 1.0, Size(inpWidth,inpHeight), Scalar(0,0,0), true, false);
            
            net.setInput(blob);        
            // output net
            vector<Mat> outs;        
            net.forward(outs, getOutputsNames(net));        
            
            std::vector<int> outLayers = net.getUnconnectedOutLayers(); // was static
            std::string outLayerType = net.getLayer(outLayers[0])->type; // was static
                            
            saved_answer.clear();
            for (int i = 0; i < outs.size(); i++) {
                float* data = (float*)outs[i].data;
                 if (outLayerType == "Region"){                                     
                    for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
                    {
                        Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
                        Point classIdPoint;
                        double confidence;
                        minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);                                                
                        
                        if( confidence >= Probability ){
                            int centerX = (int)(data[0] * image.cols);
                            int centerY = (int)(data[1] * image.rows);
                            int width = (int)(data[2] * image.cols);
                            int height = (int)(data[3] * image.rows);
                            int left = centerX - width / 2;
                            int top = centerY - height / 2;
                            
                            ExtendedObjectInfo tmp = ExtendedObjectInfo(Rect(left, top, width, height));
                            tmp.setScoreWeight(confidence, Weight);
                            set_extracted_info(tmp, "class_id", classIdPoint.x);                            
                            
                            if(isLabelMap){                                
                                set_extracted_info(tmp, "class_label", labelMap[classIdPoint.x]);                                
                            }
                            
                            saved_answer.push_back(tmp);    
                        }
                    }
        
                }
                else if (outLayerType == "DetectionOutput"){
                    float* data = (float*)outs[i].data;
                    for (size_t k = 0; k < outs[i].total(); k += 7)
                    {
                        float confidence = data[k + 2];                        
                        
                        if( confidence >= Probability ){
                        
                            int left   = (int)data[k + 3];
                            int top    = (int)data[k + 4];
                            int right  = (int)data[k + 5];
                            int bottom = (int)data[k + 6];
                            int width  = right - left + 1;
                            int height = bottom - top + 1;
                            if (width <= 2 || height <= 2)
                            {
                                left   = (int)(data[k + 3] * image.cols);
                                top    = (int)(data[k + 4] * image.rows);
                                right  = (int)(data[k + 5] * image.cols);
                                bottom = (int)(data[k + 6] * image.rows);
                                width  = right - left + 1;
                                height = bottom - top + 1;
                            }
                            ExtendedObjectInfo tmp = ExtendedObjectInfo(Rect(left, top, width, height));
                            tmp.setScoreWeight(confidence, Weight);                            
                            int pos = (int)(data[k + 1]);
                            set_extracted_info(tmp, "class_id", (int)(data[k + 1]));                            
                            
                            if(isLabelMap){                                
                                set_extracted_info(tmp, "class_label", labelMap[(int)(data[k + 1])]);                                
                            }
                            
                            saved_answer.push_back(tmp);
                        }
                        
                    }                        
                }                
            }
            prev_seq = seq;
            return saved_answer;
        }
        else{
            return saved_answer;
        }
    }
    
    bool DnnAttribute::Check2(const Mat& image,ExtendedObjectInfo& rect){
        return false;        
    }
    
    void DnnAttribute::Extract2(const cv::Mat& image, ExtendedObjectInfo& rect){
    }
    
}
#endif // CV_MAJOR_VERSION > 3

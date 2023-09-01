#include "ofApp.h"
cv::Mat intrinsic, distortion;
bool capture_frame = false;
//--charuco
cv::Ptr<cv::aruco::Dictionary> dictionary;
cv::Ptr<cv::aruco::DetectorParameters> params = cv::aruco::DetectorParameters::create();
std::vector<std::vector<cv::Point2f>> allCharucoCorners;
std::vector<std::vector<int>> allCharucoIds;
//------aruco
cv::Ptr<cv::aruco::CharucoBoard> charucoboard = cv::aruco::CharucoBoard::create(7, 7, 26, 22, dictionary);
//cv::Ptr<cv::aruco::Board> board = cv::aruco::GridBoard::create(7, 7, 26.0f, 2.0f, dictionary);

/* [869.5333079784639, 0, 661.7975199681001;
 0, 868.7868814478865, 371.0793139773919;
 0, 0, 1]
[-0.1294788370651657, 0.1304373002783254, -0.002025135372729077, -0.004740136006870072, -0.1360261412827909]*/

//--------------------------------------------------------------
void ofApp::setup(){
    camWidth = 1280;  // try to grab at this size.
    camHeight = 720;
    intrinsic = cv::Mat::eye(3,3, CV_64F);
    cv::Mat intrinsic_ =  (cv::Mat_<double>(3, 3) <<
                  850.0, 0.0, camWidth/2,
                  0.0, 850.0, camHeight/2,
                  0.0, 0.0, 1.0);
    cv::Mat dist = cv::Mat::zeros(1, 4, CV_64F);
    //http://www.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/tech0118.html
    intrinsic_.copyTo(intrinsic);
    ofCropped.allocate(camWidth, camHeight, OF_IMAGE_COLOR);
    vector<ofVideoDevice> devices = vidGrabber.listDevices();
    for(size_t i = 0; i < devices.size(); i++){
        if(devices[i].bAvailable){
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
        }else{
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
        }
    }
    vidGrabber.setDeviceID(0);
    vidGrabber.initGrabber(camWidth, camHeight);
    ofSetVerticalSync(true);
    //---aruco
    dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_250);
    params->cornerRefinementMethod = cv::aruco::CORNER_REFINE_NONE;
    //----charuco
    charucoboard = cv::aruco::CharucoBoard::create(7, 7, 26.0f, 22.0f, dictionary);
}

//--------------------------------------------------------------
void ofApp::update(){
    vidGrabber.update();
    if(vidGrabber.isFrameNew()){
        ofPixels & pixels = vidGrabber.getPixels();
        cv::Mat img, gray;
        img = cv::Mat(camHeight, camWidth, CV_8UC3, pixels.getData());
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
        std::vector<int> markerIds;
        std::vector<std::vector<cv::Point2f> > markerCorners;
        cv::aruco::detectMarkers(gray, charucoboard->dictionary, markerCorners, markerIds, params);
        if( markerIds.size() > 0){
            cv::aruco::drawDetectedMarkers(img, markerCorners, markerIds);
            std::vector<cv::Point2f> charucoCorners;
            std::vector<int> charucoIds;
            cv::aruco::interpolateCornersCharuco(markerCorners, markerIds, img, charucoboard, charucoCorners, charucoIds);
            if (charucoIds.size() > 0)
            {
                cv::aruco::drawDetectedCornersCharuco(img, charucoCorners, charucoIds, cv::Scalar(0, 0, 255));
                if( capture_frame && charucoIds.size() > 10){
                    allCharucoCorners.push_back(charucoCorners);
                    allCharucoIds.push_back(charucoIds);
                    capture_frame = false;
                    std::cout<<charucoCorners.size()<<" "<<charucoIds.size()<<std::endl;
                    
                }
            }
        }
        ofCropped.setFromPixels(img.ptr(), camWidth, camHeight, OF_IMAGE_COLOR, false);
        }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofDisableDepthTest();
    ofCropped.draw(0, 0, 640 ,360);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if( key == 32) {
        capture_frame = true;
    }
    if( key == 16){
        calc();
    }
}
//--------------------------------------------------------------
void ofApp::calc(){
    std::vector<cv::Mat> rvecs, tvecs;
    //----fix principal point
    int calibrationFlags = cv::CALIB_USE_INTRINSIC_GUESS | cv::CALIB_FIX_PRINCIPAL_POINT ;
    double repError = cv::aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, charucoboard, cv::Size(camWidth, camHeight), intrinsic, distortion, rvecs, tvecs, calibrationFlags);
    std::cout << "fix principal point" << std::endl;
    std::cout << intrinsic << std::endl;
    std::cout << distortion << std::endl;
    //---fix principal point & ignore distorion
    calibrationFlags = cv::CALIB_USE_INTRINSIC_GUESS | cv::CALIB_FIX_PRINCIPAL_POINT | cv::CALIB_ZERO_TANGENT_DIST | cv::CALIB_FIX_K1 | cv::CALIB_FIX_K2 | cv::CALIB_FIX_K3;
    repError = cv::aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, charucoboard, cv::Size(camWidth, camHeight), intrinsic, distortion, rvecs, tvecs, calibrationFlags);
    std::cout << "fix principal point & ignore distorion" << std::endl;
    std::cout << intrinsic << std::endl;
    std::cout << distortion << std::endl;
    //---ignore distorion
    calibrationFlags = cv::CALIB_ZERO_TANGENT_DIST | cv::CALIB_FIX_K1 | cv::CALIB_FIX_K2 | cv::CALIB_FIX_K3;
    repError = cv::aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, charucoboard, cv::Size(camWidth, camHeight), intrinsic, distortion, rvecs, tvecs, calibrationFlags);
    std::cout << "ignore distorion" << std::endl;
    std::cout << intrinsic << std::endl;
    std::cout << distortion << std::endl;
    calibrationFlags = cv::CALIB_ZERO_TANGENT_DIST | cv::CALIB_FIX_K1 | cv::CALIB_FIX_K2 | cv::CALIB_FIX_K3;
    //-----standard
    calibrationFlags = 0;
    repError = cv::aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, charucoboard, cv::Size(camWidth, camHeight), intrinsic, distortion, rvecs, tvecs, calibrationFlags);
    std::cout << "standard" << std::endl;
    std::cout << intrinsic << std::endl;
    std::cout << distortion << std::endl;
   
    allCharucoCorners.clear();
    allCharucoIds.clear();
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include "functions.hpp"
#include <wiringPi.h>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_io.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
// SlackMessenger クラス: Slack へのメッセージ送信を管理    

// SlackMessenger クラスのコンストラクタ
// apiUrl: Slack API のエンドポイント URL
// apiToken: Slack API の認証トークン
SlackMessenger::SlackMessenger(const std::string& apiUrl, const std::string& apiToken)
    : slackApiUrl(apiUrl), slackApiToken(apiToken) {}


// Slack へメッセージを送信する
// payload: 送信するメッセージの JSON ファイル
void SlackMessenger::sendMessageToSlack(const std::string& payload) {
    CURL* curl = curl_easy_init();
    CURLcode res;
    if(curl) {
        //curl の設定
        struct curl_slist* headers = NULL;
        //HTTPヘッダーの設定
        headers = curl_slist_append(headers, ("Authorization: Bearer " + slackApiToken).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        //Slack API の URL、ヘッダー、コンテンツ（text）を設定
        curl_easy_setopt(curl, CURLOPT_URL, slackApiUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        
        // HTTP POST の実行
        res = curl_easy_perform(curl);
        // error
        if (res != CURLE_OK) {
            std::cerr << "cURL failed: " << curl_easy_strerror(res) << std::endl;
        }
        // メモリ解放とクリーンアップ
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

// JSON ファイルのペイロードを指定したファイルから読み込む
// filename: 読み込む JSON ファイルのパス
std::string SlackMessenger::readJsonFilename(const std::string& filename) {
    std::ifstream file(filename);
    //error
    if(!file.is_open()) {
        std::cerr << "FAILED TO OPEN " <<filename <<std::endl; 
        return "";
    }
    // ファイルから JSON を読み込む
    std::string payload((std::istreambuf_iterator<char>(file)), 
    std::istreambuf_iterator<char>());

    file.close();// ファイルを閉じる
    return payload;// 読み込んだ JSON ペイロードを返す
}

// UltrasonicSensor クラス: 超音波センサー

// UltrasonicSensor クラスのコンストラクタ
// trigPin: トリガーピンのピン番号
// echoPin: エコーピンのピン番号
UltrasonicSensor::UltrasonicSensor(int trigPin, int echoPin)
: TrigPin(trigPin), EchoPin(echoPin) {
    // WiringPi ライブラリの初期化
    wiringPiSetup();
    
    pinMode(EchoPin, INPUT); // EchoPin を入力に設定
    pinMode(TrigPin, OUTPUT); // TrigPin を出力に設定
}

// 距離を測定する
float UltrasonicSensor::measureDistance() {
    struct timeval tv1;
    struct timeval tv2;
    long time1, time2;
    float dis;

    digitalWrite(TrigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(TrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(TrigPin, LOW);

    while (!(digitalRead(EchoPin) == 1));
    gettimeofday(&tv1, NULL);
    while (!(digitalRead(EchoPin) == 0));
    gettimeofday(&tv2, NULL);

    time1 = tv1.tv_sec * 1000000 + tv1.tv_usec;
    time2 = tv2.tv_sec * 1000000 + tv2.tv_usec;

    dis = (float)(time2 - time1) / 1000000 * 34000 / 2;

    return dis;
}


// FaceDetect クラス: 顔検出
FaceDetect::FaceDetect(){}


// カメラで写真を撮影し、ファイルパスを返す
// imgNum: 画像の番号
std::string FaceDetect::takePic(int imgNum) {
    //mainから貰ったimgNumで画像名を作る
    std::string photoName = "img" + std::to_string(imgNum) + ".jpg";
    //画像が保存されるパスを指定
    std::string imgPath = "../img/" + photoName;
    //system関数で撮影をする命令をする為にcommandを作っておく
    std::string command = "/usr/bin/libcamera-still -n -o " + imgPath;

    std::system(command.c_str());    //画像撮影
    return imgPath;
}

// 画像から顔を検出する
// imgPath: 顔検出を行う画像のファイルパス
int FaceDetect::detect(const std::string& imgPath) {
    //dlibの顔探知機
    dlib::frontal_face_detector faceDetector = dlib::get_frontal_face_detector();
    //openCVのimread関数でパスの画像を読み取り、openCVのMatクラスのimgオブジェクトに格納
    cv::Mat img = cv::imread(imgPath);

    //OpenCVのイメージデータ(cv::Mat img)をdlibで使えるように変換する
    dlib::cv_image<dlib::bgr_pixel> dlibImage(img);
    //dlibの顔探知機を使って顔を検出
    std::vector<dlib::rectangle> faces = faceDetector(dlibImage);

    //検出された場合→facesに中身がある　→　face detect
    //検出されなかった場合→facesが空っぽな状態　→　not FIND!
    if(!faces.empty()) {
        std::cout << "face detect!" << std::endl;
        return 1;
    }else {
        std::cout << "not FIND!" << std::endl;
        return 0;
    }

}
// 画像を削除する
void FaceDetect::deletePicture(const std::string& imgPath) {
    std::string command = "rm " + imgPath;
    std::system(command.c_str());
    std::cout << "image Deleted!" << std::endl;
}
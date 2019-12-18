#include <iostream>
#include "efp/ElasticFrameProtocol.h"
#include "srt/SRTNet.h"

//Convert a stream into individual files -> ffmpeg -i 2000_tivoli.mp4 -f image2 -vframes 3000 -vcodec copy -bsf h264_mp4toannexb "%d.h264"
//Run the above command in the media folder

#define MTU 1456 //SRT-max
#define WORKER_VIDEO_FRAMES 3000

SRTNet mySRTNetClient;
ElasticFrameProtocol myEFPSender(MTU,ElasticFrameMode::sender);

std::vector<uint8_t> getNALUnit(int i) {
    std::string fileName="../media/" + std::to_string(i) + ".h264";

    FILE *f = fopen(fileName.c_str(), "rb");
    if (!f) {
        std::cout << "Failed opening file" << std::endl;
        std::vector<uint8_t> empty;
        return empty;
    }
    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t* pictureBuffer;
    posix_memalign((void**)&pictureBuffer, 32, fsize);
    fread(pictureBuffer, fsize, 1, f);
    std::vector<uint8_t> my_vector(&pictureBuffer[0], &pictureBuffer[fsize]);
    free(pictureBuffer);
    fclose(f);
    return my_vector;
}

int packSize;

void sendData(const std::vector<uint8_t> &subPacket) {

    if (subPacket[0] == 1) {
        packSize += subPacket.size()-myEFPSender.geType1Size();
    }

    if (subPacket[0] == 2) {
        packSize += subPacket.size()-myEFPSender.geType2Size();
        std::cout << "Sent-> " << packSize << std::endl;
        packSize=0;
    }

    SRT_MSGCTRL thisMSGCTRL1 = srt_msgctrl_default;
    bool isBroken=mySRTNetClient.sendData((uint8_t*)subPacket.data(),subPacket.size(), &thisMSGCTRL1);
    if (!isBroken) {
        std::cout << "Should do something" << std::endl;
    }
}

void handleDataClient(std::unique_ptr <std::vector<uint8_t>> &content, SRT_MSGCTRL &msgCtrl, std::shared_ptr<NetworkConnection> ctx, SRTSOCKET serverHandle){
    std::cout << "Got data from server" << std::endl;
}

int main() {
    packSize = 0;

    //Set-up framing protocol
    myEFPSender.sendCallback = std::bind(&sendData, std::placeholders::_1);

    //Set-up SRT
    auto client1Connection=std::make_shared<NetworkConnection>();
    mySRTNetClient.recievedData=std::bind(&handleDataClient, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    if (!mySRTNetClient.startClient("127.0.0.1", 8000, 16, 1000, 100,client1Connection, MTU)) {
        std::cout << "SRT client1 failed starting." << std::endl;
        return EXIT_FAILURE;
    }

    uint64_t pts=0;
    for (int i = 0; i < WORKER_VIDEO_FRAMES ; ++i) {
        std::vector<uint8_t> thisNalData = getNALUnit(i+1);
        std::cout << "SendNAL > " << thisNalData.size() << " pts " << pts << std::endl;
        myEFPSender.packAndSend(thisNalData,ElasticFrameContent::h264,pts,'ANXB',1,NO_FLAGS);
        pts += 90000/60; //fake a pts of 60Hz. FYI.. the codestream is 23.98 (I and P only)
        usleep(1000*16); //sleep for 16ms ~60Hz
    }
    mySRTNetClient.stop();
    std::cout << "Done sending will exit" << std::endl;
    return 0;
}
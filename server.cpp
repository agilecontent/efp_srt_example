#include <iostream>
#include "efp/ElasticFrameProtocol.h"
#include "srt/SRTNet.h"

//Convert a stream into individual files -> ffmpeg -i 2000_tivoli.mp4 -f image2 -vframes 3000 -vcodec copy -bsf h264_mp4toannexb "%d.h264"
//Run the above command in the media folder

#define MTU 1456 //SRT-max
#define EFP_TIME_OUT //20 seconds.. Meaning after 20 seconds we claim a 'unused' EFP reciever number

SRTNet mySRTNetServer;


//**********************************
//Server part
//**********************************

void gotData(ElasticFrameProtocol::pFramePtr &rPacket);

class MyClass {
public:
  virtual ~MyClass() {
    myEFPReceiver.stopReceiver();
    *efpActiveElement = false; //Release active marker
  };
  uint8_t efpId = 0;
  std::atomic_bool *efpActiveElement;
  ElasticFrameProtocol myEFPReceiver;
};

//Array of 256 possible EFP receivers (true == actice)
std::atomic_bool efpActiveList[UINT8_MAX];

uint8_t generateEFPId() {
  for(int i=1;i<UINT8_MAX-1;i++) {
    if (!efpActiveList[i]) {
      efpActiveList[i]=true; //Set active
      return i;
    }
  }
  return UINT8_MAX;
}



//Return a connection object. (Return nullptr if you don't want to connect to that client)
std::shared_ptr<NetworkConnection> validateConnection(struct sockaddr_in* sin) {
    auto* ip = (unsigned char*)&sin->sin_addr.s_addr;
    std::cout << "Connecting IP: " << unsigned(ip[0]) << "." << unsigned(ip[1]) << "." << unsigned(ip[2]) << "." << unsigned(ip[3]) << std::endl;
    //Validate connection.. Do we have resources to accept it? Is it from someone we want to talk to?

    //Yes I know I waste one connection possibility. 254 is still OK for a example.
    uint8_t efpNumber = generateEFPId();
    if (efpNumber == UINT8_MAX) {
      std::cout << "Unable to accept more EFP connections " << std::endl;
      return nullptr;
    }
    //Here we can put whatever into the connection in this case a class containint a uniqie EFP id so that when we recieve data we can
    //Separate the connections to a specifict EFP reciever
    auto a1 = std::make_shared<NetworkConnection>();
    a1->object = std::make_shared<MyClass>();
    auto v = std::any_cast<std::shared_ptr<MyClass>&>(a1 -> object);
    v->efpId = efpNumber;
    v->efpActiveElement = &efpActiveList[efpNumber];
    v->myEFPReceiver.receiveCallback = std::bind(&gotData, std::placeholders::_1); //In this example we aggregate all callbacks..
    //However you could create a decoder/reciever/consumer in the MyClass and consume the data there.
    v->myEFPReceiver.startReceiver(10, 2);
    return a1;
}

//Network data recieved callback.
bool handleData(std::unique_ptr <std::vector<uint8_t>> &content, SRT_MSGCTRL &msgCtrl, std::shared_ptr<NetworkConnection> ctx, SRTSOCKET clientHandle) {
  auto v = std::any_cast<std::shared_ptr<MyClass>&>(ctx -> object);
  v->myEFPReceiver.receiveFragment(*content,v->efpId);
  return true;
}

//ElasticFrameProtocol got data
void gotData(ElasticFrameProtocol::pFramePtr &rPacket) {
    std::cout << "BAM... Got some NAL-units of size " << unsigned(rPacket->mFrameSize) <<
    " pts " << unsigned(rPacket->mPts) <<
    " is broken? " << rPacket->mBroken <<
    " from EFP connection " << unsigned(rPacket->mSource) <<
    std::endl;
}

int main() {

  //make sure all are set to inactive
  for(int i=0;i<UINT8_MAX;i++) {
    efpActiveList[i] = false;
  }

    //Setup and start the SRT server
    mySRTNetServer.clientConnected=std::bind(&validateConnection, std::placeholders::_1);
    mySRTNetServer.recievedData=std::bind(&handleData, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    if (!mySRTNetServer.startServer("0.0.0.0", 8000, 16, 1000, 100, MTU)) {
        std::cout << "SRT Server failed to start." << std::endl;
        return EXIT_FAILURE;
    }

    //Run this server until ........
    while (true) {
        sleep(1);
    }
    

    //When you decide to quit garbage collect and stop threads....
    mySRTNetServer.stop();

    std::cout << "Done serving. Will exit." << std::endl;
    return 0;
}


/*
 * Communication.c
 *
 *  Created on: 29 июн. 2025 г.
 *      Author: Victor
 *
 * Updated from C++ library
 */

#ifndef ESP32_RTSP_SERVER_H
#define ESP32_RTSP_SERVER_H

#include "lwip/sockets.h"
#include "esp_timer.h"
#include <esp_log.h>
//#include <map>

#define MAX_RTSP_BUFFER (512 * 1024)
#define RTP_STACK_SIZE (1024 * 8)
#define RTP_PRI 10
#define RTSP_STACK_SIZE (1024 * 8)
#define RTSP_PRI 10
#define MAX_CLIENTS 10 // max rtsp clients

#define RTSP_BUFFER_SIZE 8092

// Optionally include RTSPConfig.h if available
#ifdef __has_include
  #if __has_include("RTSPConfig.h")
    #include "RTSPConfig.h"
  #endif
#endif

#ifdef RTSP_LOGGING_ENABLED
  #define RTSP_LOGI(tag, format, ...) ESP_LOGI(tag, format, ##__VA_ARGS__)
  #define RTSP_LOGW(tag, format, ...) ESP_LOGW(tag, format, ##__VA_ARGS__)
  #define RTSP_LOGE(tag, format, ...) ESP_LOGE(tag, format, ##__VA_ARGS__)
  #define RTSP_LOGD(tag, format, ...) ESP_LOGD(tag, format, ##__VA_ARGS__)
#else
  #define RTSP_LOGI(tag, format, ...)
  #define RTSP_LOGW(tag, format, ...)
  #define RTSP_LOGE(tag, format, ...)
  #define RTSP_LOGD(tag, format, ...)
#endif

#define MAX_COOKIE_LENGTH 128 // max length of session cookie

struct RTSP_Session {
  uint32_t sessionID;
  int sock;
  int cseq;
  uint16_t cVideoPort;
  uint16_t cAudioPort;
  uint16_t cSrtPort;
  bool isMulticast;
  bool isPlaying;
  bool isTCP;
  bool isHttp;  // Add flag for HTTP tunneling
  int httpSock;  // Add HTTP socket storage
  char sessionCookie[MAX_COOKIE_LENGTH];  // Add storage for session cookie
};

typedef struct RTSP_Session TRTSP_Session;


  enum EVIDEO_ENCODING{
	EN_MJPEG,
	EN_H264,  
  };
  
  enum EServerType{
	  LOCAL,
	  REMOTE,
  } ;
  
 typedef  enum EServerType   TServerType;
  
 typedef  enum EVIDEO_ENCODING TVIDEO_ENCODING; 
  
  enum ETransportType {
    VIDEO_ONLY,
    AUDIO_ONLY,
    SUBTITLES_ONLY,
    VIDEO_AND_AUDIO,
    VIDEO_AND_SUBTITLES,
    AUDIO_AND_SUBTITLES,
    VIDEO_AUDIO_SUBTITLES,
    NONE,
  };
  

typedef  enum ETransportType TTransportType;
typedef  char IPAddress[4];

struct SRTSPServer {
  TTransportType transport;
  TVIDEO_ENCODING VideoEncoding;
  TServerType ServerType;
  uint32_t rtpFps;
  uint32_t sampleRate;
  int rtspPort;
  IPAddress rtpIp;
  uint8_t rtpTTL;
  uint16_t rtpVideoPort;
  uint16_t rtpAudioPort;
  uint16_t rtpSubtitlesPort;
  uint8_t maxRTSPClients;
  int rtspSocket;
  int videoUnicastSocket; 
  int audioUnicastSocket; 
  int subtitlesUnicastSocket; 
  int videoMulticastSocket; 
  int audioMulticastSocket; 
  int subtitlesMulticastSocket;
  uint8_t activeRTSPClients; 
  uint8_t maxClients;
  TaskHandle_t rtpVideoTaskHandle;
  TaskHandle_t rtspTaskHandle;
  //std::map<uint32_t, RTSP_Session> sessions;
  int8_t* rtspStreamBuffer;
  size_t rtspStreamBufferSize;
  bool rtpFrameSent;
  bool rtpAudioSent;
  bool rtpSubtitlesSent;
  uint8_t vQuality;
  uint16_t vWidth;
  uint16_t vHeight;
  uint16_t videoSequenceNumber;
  uint32_t videoTimestamp;
  uint32_t videoSSRC;
  uint16_t audioSequenceNumber;
  uint32_t audioTimestamp;
  uint32_t audioSSRC;
  uint16_t subtitlesSequenceNumber;
  uint32_t subtitlesTimestamp;
  uint32_t subtitlesSSRC;
  uint32_t rtpFrameCount;
  uint32_t lastRtpFPSUpdateTime;
  uint8_t videoCh;
  uint8_t audioCh;
  uint8_t subtitlesCh;
  bool isVideo;
  bool isAudio;
  bool isSubtitles;
  bool isPlaying;
  bool firstClientConnected; 
  bool firstClientIsMulticast; 
  bool firstClientIsTCP;
  bool authEnabled; // Flag to indicate if authentication is enabled
  
  
  SemaphoreHandle_t isPlayingMutex;  // Mutex for protecting access
  SemaphoreHandle_t sendTcpMutex;  // Mutex for protecting TCP send access
  SemaphoreHandle_t maxClientsMutex; // FreeRTOS mutex for maxClients

  esp_timer_handle_t sendSubtitlesTimer;
  char base64Credentials[128]; // Store base64 encoded credentials
};

typedef struct SRTSPServer TRTSPServer;
  
  bool RTSPServerinit(TTransportType transport,TServerType ServerType,TVIDEO_ENCODING EncodingFormat, uint16_t rtspPort, uint32_t sampleRate , uint16_t port1, uint16_t port2, uint16_t port3 , IPAddress rtpIp, uint8_t rtpTTL);  // Defined in ESP32-RTSPServer.cpp
  
  void RTSPServerdeinit();  // Defined in ESP32-RTSPServer.cpp

  bool RTSPServerreinit();  // Defined in ESP32-RTSPServer.cpp

  void RTSPServersendRTSPFrame(const uint8_t* data, size_t len, int quality, int width, int height);  // Defined in rtp.cpp

  void RTSPServersendRTSPAudio(int16_t* data, size_t len);  // Defined in rtp.cpp

  void RTSPServersendRTSPSubtitles(char* data, size_t len);  // Defined in rtp.cpp

  void RTSPServerstartSubtitlesTimer(esp_timer_cb_t userCallback);  // Defined in utils.cpp

  bool RTSPServerreadyToSendFrame() ;  // Defined in utils.cpp

  bool RTSPServerreadyToSendAudio() ;  // Defined in utils.cpp

  bool RTSPServerreadyToSendSubtitles() ;  // Defined in utils.cpp

  bool RTSPServersetCredentials(const char* username, const char* password); // Add method to set credentials

  
  void RTSPServercloseSockets();  // Defined in ESP32-RTSPServer.cpp
  
  void RTSPServersendTcpPacket(const uint8_t* packet, size_t packetSize, int sock);  // Defined in network.cpp

  void RTSPServercheckAndSetupUDP(int * rtpSocket, bool isMulticast, uint16_t rtpPort, IPAddress rtpIp);  // Defined in network.cpp

  void RTSPServersendRtpSubtitles(const char* data, size_t len, int sock, uint16_t sendRtpPort, bool useTCP, bool isMulticast);  // Defined in rtp.cpp

  void RTSPServersendRtpAudio(const int16_t* data, size_t len, int sock, uint16_t sendRtpPort, bool useTCP, bool isMulticast);  // Defined in rtp.cpp

  void RTSPServersendRtpFrame(const uint8_t* data, size_t len, uint8_t quality, uint16_t width, uint16_t height, int sock, uint16_t sendRtpPort, bool useTCP, bool isMulticast);  // Defined in rtp.cpp

  static void RTSPServerrtpVideoTaskWrapper(void* pvParameters);  // Defined in rtp.cpp

  void RTSPServerrtpVideoTask();  // Defined in rtp.cpp

  void RTSPServersetMaxClients(uint8_t newMaxClients);  // Defined in utils.cpp

  uint8_t RTSPServergetMaxClients();  // Defined in utils.cpp

  uint8_t RTSPServergetActiveClients();  // Defined in utils.cpp
  
  void RTSPServerincrementActiveRTSPClients();  // Defined in utils.cpp

  void RTSPServerdecrementActiveRTSPClients();  // Defined in utils.cpp

  uint8_t RTSPServergetActiveRTSPClients();  // Defined in utils.cpp

  void RTSPServerupdateIsPlayingStatus();  // Defined in utils.cpp
  
  void RTSPServersetIsPlaying(bool playing);  // Defined in utils.cpp
  
  bool RTSPServergetIsPlaying();  // Defined in utils.cpp

  int RTSPServercaptureCSeq(char* request);  // Defined in utils.cpp

  uint32_t RTSPServergenerateSessionID();  // Defined in utils.cpp

  uint32_t RTSPServerextractSessionID(char* request);  // Defined in utils.cpp

  const char* RTSPServerdateHeader();  // Defined in utils.cpp

  void RTSPServerhandleOptions(char* request, TRTSP_Session *session);  // Defined in rtsp_requests.cpp

  void RTSPServerhandleDescribe(const TRTSP_Session *session);  // Defined in rtsp_requests.cpp

  void RTSPServerhandleSetup(char* request, TRTSP_Session *session);  // Defined in rtsp_requests.cpp

  void RTSPServerhandlePlay(TRTSP_Session *session);  // Defined in rtsp_requests.cpp

  void RTSPServerhandlePause(TRTSP_Session *session);  // Defined in rtsp_requests.cpp

  void RTSPServerhandleTeardown(TRTSP_Session *session);  // Defined in rtsp_requests.cpp

  bool RTSPServerhandleRTSPRequest(TRTSP_Session *session);  // Defined in rtsp_requests.cpp

  bool RTSPServersetNonBlocking(int sockfd);  // Defined in network.cpp

  bool RTSPServerprepRTSP();  // Defined in ESP32-RTSPServer.cpp

  static void RTSPServerrtspTaskWrapper(void* pvParameters);  // Defined in ESP32-RTSPServer.cpp

  void RTSPServerrtspTask(TRTSPServer* server);  // Defined in ESP32-RTSPServer.cpp

 
  void RTSPServersendUnauthorizedResponse(TRTSP_Session *session); // Add method to send 401 Unauthorized response
  void RTSPServerextractSessionCookie(const char* buffer, char* sessionCookie, size_t maxLen);
  bool RTSPServerisBase64Encoded(const char* buffer, size_t length);
  void RTSPServerhandleRTSPCommand(char* command, TRTSP_Session *session);
  bool RTSPServerdecodeBase64(const char* input, size_t inputLen, char* output, size_t* outputLen);
  void RTSPServerwrapInHTTP(char* buffer, size_t len, char* response, size_t maxLen);  // Add this line
  TRTSP_Session* RTSPServerfindSessionByCookie(const char* cookie);  // Add this line

 
#endif // ESP32_RTSP_SERVER_H

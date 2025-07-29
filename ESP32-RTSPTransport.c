/*
 * Communication.c
 *
 *  Created on: 29 июн. 2025 г.
 *      Author: Victor
 *
 * Updated from C++ library
 */


#include "ESP32-RTSPTransport.h"

#include "esp_mac.h"
#include "esp_mac.h"
#include "esp_mac.h"
#include "libb64/cencode.h" // Include libb64 library
#include "libb64/cdecode.h" // Include libb64 library for decoding

IPAddress CURR_IP={239, 255, 0, 1};

const char* LOG_TAG = "RTSPServer";
TRTSPServer VRTSPServer= { 
    VIDEO_AND_SUBTITLES, // Default transport 
    EN_H264,
    REMOTE,
    0,
    0,
    554,
    {239, 255, 0, 1}, // Default RTP IP 
    64, // Default TTL
    5430,
    5432,
    5434,
    3,
    //
    -1,
    -1,
    -1, 
    -1,
    -1,
    -1,
    -1,
     0,
    1,
    NULL,
    NULL,
    NULL,
    0,
    true,
    true,
    true,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false, // Initialize authEnabled to false
    true,
    true,
    false,
    


   };
TRTSPServer *RTSPServer;

  
void RTSPServerCreate()
{
	if (RTSPServer==NULL)
	 {
		 RTSPServer=&RTSPServer;
		
      }
}

void RTSPServerDestroy() 
{
  // Clean up resources
  	if (RTSPServer==NULL)
	 {
		 RTSPServer=&VRTSPServer;
		
      }
  RTSPServerdeinit();
  vSemaphoreDelete(RTSPServer->isPlayingMutex);
  vSemaphoreDelete(RTSPServer->sendTcpMutex);
  vSemaphoreDelete(RTSPServer->maxClientsMutex);
}

bool RTSPServerinit(TTransportType transport,TServerType ServerType,TVIDEO_ENCODING EncodingFormat, uint16_t rtspPort, uint32_t sampleRate, uint16_t port1, uint16_t port2, uint16_t port3, IPAddress rtpIp, uint8_t rtpTTL)
 {
   	 
  RTSPServer->transport = (transport != NONE) ? transport : RTSPServer->transport;
  RTSPServer->rtspPort = (rtspPort != 0) ? rtspPort : RTSPServer->rtspPort;
  for (uint8_t i=0;i<4;i++)RTSPServer->rtpIp[i] =  rtpIp[1] ;
  RTSPServer->rtpTTL = (rtpTTL != 255) ? rtpTTL : RTSPServer->rtpTTL;

  if (transport == AUDIO_ONLY || transport == VIDEO_AND_AUDIO || transport == AUDIO_AND_SUBTITLES || transport == VIDEO_AUDIO_SUBTITLES) {
    if (RTSPServer->sampleRate == 0 && sampleRate == 0) {
     
      return false;
    }
    if (sampleRate != 0) {
      RTSPServer->sampleRate = sampleRate;
    }
  }

  switch (RTSPServer->transport) {
    case VIDEO_ONLY:
      RTSPServer->rtpVideoPort = (port1 != 0) ? port1 : RTSPServer->rtpVideoPort;
      RTSPServer->isVideo = true;
      break;
    case AUDIO_ONLY:
      RTSPServer->rtpAudioPort = (port1 != 0) ? port1 : RTSPServer->rtpAudioPort;
      RTSPServer->isAudio = true;
      break;
    case SUBTITLES_ONLY:
      RTSPServer->rtpSubtitlesPort = (port1 != 0) ? port1 : RTSPServer->rtpSubtitlesPort;
      RTSPServer->isSubtitles = true;
      break;
    case VIDEO_AND_AUDIO:
      RTSPServer->rtpVideoPort = (port1 != 0) ? port1 : RTSPServer->rtpVideoPort;
      RTSPServer->rtpAudioPort = (port2 != 0) ? port2 : RTSPServer->rtpAudioPort;
      RTSPServer->isVideo = true;
      RTSPServer->isAudio = true;
      break;
    case VIDEO_AND_SUBTITLES:
      RTSPServer->rtpVideoPort = (port1 != 0) ? port1 : RTSPServer->rtpVideoPort;
      RTSPServer->rtpSubtitlesPort = (port2 != 0) ? port2 : RTSPServer->rtpSubtitlesPort;
      RTSPServer->isVideo = true;
      RTSPServer->isSubtitles = true;
      break;
    case AUDIO_AND_SUBTITLES:
      RTSPServer->rtpAudioPort = (port1 != 0) ? port1 : RTSPServer->rtpAudioPort;
      RTSPServer->rtpSubtitlesPort = (port2 != 0) ? port2 : RTSPServer->rtpSubtitlesPort;
      RTSPServer->isAudio = true;
      RTSPServer->isSubtitles = true;
      break;
    case VIDEO_AUDIO_SUBTITLES:
      RTSPServer->rtpVideoPort = (port1 != 0) ? port1 : RTSPServer->rtpVideoPort;
      RTSPServer->rtpAudioPort = (port2 != 0) ? port2 : RTSPServer->rtpAudioPort;
      RTSPServer->rtpSubtitlesPort = (port3 != 0) ? port3 : RTSPServer->rtpSubtitlesPort;
      RTSPServer->isVideo = true;
      RTSPServer->isAudio = true;
      RTSPServer->isSubtitles = true;
      break;
    case NONE:
      RTSP_LOGE(LOG_TAG, "Transport type can not be NONE");
      return false;
    default:
      RTSP_LOGE(LOG_TAG, "Invalid transport type for RTSPServer init method");
      return false;
  }
    RTSPServer->isPlayingMutex=xSemaphoreCreateMutex(); // Initialize the mutex
    RTSPServer->sendTcpMutex=xSemaphoreCreateMutex(); // Initialize the mutex
    RTSPServer->maxClientsMutex=xSemaphoreCreateMutex();
    return RTSPServerprepRTSP();
}

void RTSPServerdeinit() {
  if (RTSPServer->rtspTaskHandle != NULL) {
    vTaskDelete(RTSPServer->rtspTaskHandle);
    RTSPServer->rtspTaskHandle = NULL;
  }
  if (RTSPServer->rtpVideoTaskHandle != NULL) {
    vTaskDelete(RTSPServer->rtpVideoTaskHandle);
    RTSPServer->rtpVideoTaskHandle = NULL;
  }
  if (RTSPServer->rtspSocket >= 0) {
    close(RTSPServer->rtspSocket);
    RTSPServer->rtspSocket = -1;
  }
  
  RTSPServercloseSockets();
  
  if (RTSPServer->rtspStreamBuffer) {
    free(RTSPServer->rtspStreamBuffer);
  }

  RTSP_LOGI(LOG_TAG, "RTSP server deinitialized.");
}

bool RTSPServerreinit() {
  RTSPServerdeinit();
  return RTSPServerinit(VIDEO_AND_SUBTITLES,REMOTE,EN_H264,0,0,0,0,0,CURR_IP,0);
}

void RTSPServercloseSockets() {
  if (VRTSPServer.videoUnicastSocket != -1) {
    close(VRTSPServer.videoUnicastSocket);
    VRTSPServer.videoUnicastSocket = -1;
  }
  if (VRTSPServer.audioUnicastSocket != -1) {
    close(VRTSPServer.audioUnicastSocket);
    VRTSPServer.audioUnicastSocket = -1;
  }
  if (VRTSPServer.subtitlesUnicastSocket != -1) {
    close(VRTSPServer.subtitlesUnicastSocket);
    VRTSPServer.subtitlesUnicastSocket = -1;
  }
  if (VRTSPServer.videoMulticastSocket != -1) {
    close(VRTSPServer.videoMulticastSocket);
    VRTSPServer.videoMulticastSocket = -1;
  }
  if (VRTSPServer.audioMulticastSocket != -1) {
    close(VRTSPServer.audioMulticastSocket);
    VRTSPServer.audioMulticastSocket = -1;
  }
  if (VRTSPServer.subtitlesMulticastSocket != -1) {
    close(VRTSPServer.subtitlesMulticastSocket);
    VRTSPServer.subtitlesMulticastSocket = -1;
  }
}

bool RTSPServerprepRTSP() {
  uint64_t mac;
  esp_efuse_mac_get_default((uint8_t*)&mac);
  RTSPServer->videoSSRC = (uint32_t)(mac & 0xFFFFFFFF);
  RTSPServer->audioSSRC = (uint32_t)((mac >> 32) & 0xFFFFFFFF);
  RTSPServer->subtitlesSSRC = (uint32_t)((mac >> 48) & 0xFFFFFFFF);

  RTSPServer->rtspSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (RTSPServer->rtspSocket < 0) {
    RTSP_LOGE(LOG_TAG, "Failed to create RTSP socket.");
    return false;
  }

  if (!RTSPServersetNonBlocking(RTSPServer->rtspSocket)) {
    RTSP_LOGE(LOG_TAG, "Failed to set RTSP socket to non-blocking mode.");
    close(RTSPServer->rtspSocket);
    return false;
  }

  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(RTSPServer->rtspPort);

  if (bind(RTSPServer->rtspSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    RTSP_LOGE(LOG_TAG, "Failed to bind RTSP socket: %d", RTSPServer->rtspSocket);
    close(RTSPServer->rtspSocket);
    return false;
  }
  
  if (listen(RTSPServer->rtspSocket, 5) < 0) {
    RTSP_LOGE(LOG_TAG, "Failed to listen on RTSP socket.");
    close(RTSPServer->rtspSocket);
    return false;
  }

  if (RTSPServer->rtspTaskHandle == NULL) {
    if (xTaskCreate(RTSPServerrtspTaskWrapper, "rtspTask", RTSP_STACK_SIZE, RTSPServer, RTSP_PRI, &RTSPServer->rtspTaskHandle) != pdPASS) {
      RTSP_LOGE(LOG_TAG, "Failed to create RTSP task.");
      close(RTSPServer->rtspSocket);
      return false;
    }
  }

  RTSP_LOGI(LOG_TAG, "RTSP server setup completed, listening on port: %d", RTSPServer->rtspPort);
  return true;
}

void RTSPServerrtspTaskWrapper(void* pvParameters) {
  TRTSPServer* server = (TRTSPServer*)(pvParameters);
  RTSPServerrtspTask(server);
}

void RTSPServerrtspTask(TRTSPServer* server) {
  struct sockaddr_in clientAddr;
  socklen_t addr_len = sizeof(clientAddr);
  fd_set read_fds;
  int client_sockets[MAX_CLIENTS] = {0};
  int max_sd, activity, client_sock;

  while (true) {
    FD_ZERO(&read_fds);
    FD_SET(RTSPServer->rtspSocket, &read_fds);
    max_sd = RTSPServer->rtspSocket;

    uint8_t currentMaxClients = RTSPServergetMaxClients();

    for (int i = 0; i < currentMaxClients; i++) {
      int sd = client_sockets[i];
      if (sd > 0) FD_SET(sd, &read_fds);
      if (sd > max_sd) max_sd = sd;
    }

    activity = select(max_sd + 1, &read_fds, NULL, NULL, NULL);

    if (activity < 0 && errno != EINTR) {
      RTSP_LOGE(LOG_TAG, "Select error");
      continue;
    }

    if (FD_ISSET(server->rtspSocket, &read_fds)) {
      if (RTSPServergetActiveRTSPClients() >= currentMaxClients) {
        client_sock = accept(RTSPServer->rtspSocket, (struct sockaddr *)&clientAddr, &addr_len);
        if (client_sock < 0) {
          RTSP_LOGE(LOG_TAG, "Accept error");
          continue;
        }

        const char* response = "RTSP/1.0 503 Service Unavailable\r\n\r\n";
        write(client_sock, response, strlen(response));
        close(client_sock);
        RTSP_LOGE(LOG_TAG, "Max clients reached. Sent 503 error to new client.");
        continue;
      }

      client_sock = accept(RTSPServer->rtspSocket, (struct sockaddr *)&clientAddr, &addr_len);
      if (client_sock < 0) {
        RTSP_LOGE(LOG_TAG, "Accept error");
        continue;
      }

      if (!RTSPServersetNonBlocking(client_sock)) {
        RTSP_LOGE(LOG_TAG, "Failed to set RTSP socket to non-blocking mode.");
        close(client_sock);
        continue;
      }

      RTSP_LOGI(LOG_TAG, "New client connected");

      // Create a new session for the new client
      TRTSP_Session session = {
        esp_random(),  // sessionID
        client_sock,   // sock
        0,            // cseq
        0,            // cVideoPort
        0,            // cAudioPort
        0,            // cSrtPort
        false,        // isMulticast
        false,        // isPlaying
        false,        // isTCP
        false,        // isHttp
        -1,           // httpSock
        {0}           // sessionCookie (initialized as empty)
      };
      //sessions[session->sessionID] = session;

      for (int i = 0; i < currentMaxClients; i++) {
        if (client_sockets[i] == 0) {
          client_sockets[i] = client_sock;
          RTSPServerincrementActiveRTSPClients();
          RTSP_LOGI(LOG_TAG, "Added to list of sockets as %d", i);
          break;
        }
      }
    }

    for (int i = 0; i < currentMaxClients; i++) {
      int sd = client_sockets[i];

      if (FD_ISSET(sd, &read_fds)) {
        // Get the session for RTSPServer client
        TRTSP_Session* session = NULL;
        /*
        for (auto& sess : sessions) {
          if (sess.second.sock == sd) {
            session = &sess.second;
            break;
          }
        }*/
        if (session) {
          bool keepConnection = RTSPServerhandleRTSPRequest(session);
          if (!keepConnection) {
            if (RTSPServergetActiveRTSPClients() == 1) {
              RTSPServersetIsPlaying(false);
              RTSPServercloseSockets();
              RTSP_LOGD(LOG_TAG, "All clients disconnected. Resetting firstClientConnected flag."); 
              RTSPServer->firstClientConnected = false; 
              RTSPServer->firstClientIsMulticast = false; 
              RTSPServer->firstClientIsTCP = false; 
            }
            close(sd);
            client_sockets[i] = 0;
            //sessions.erase(session->sessionID); // Remove session when client disconnects
            RTSPServerdecrementActiveRTSPClients();
          }
        }
      }
    }
  }
}

void RTSPServerwrapInHTTP(char* buffer, size_t len, char* response, size_t maxLen) {
    snprintf(response, maxLen,
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/x-rtsp-tunnelled\r\n"
             "Content-Length: %d\r\n"
             "Pragma: no-cache\r\n"
             "Cache-Control: no-cache\r\n"
             "\r\n"
             "%s",
             len, buffer);
}

/**
 * @brief Handles the OPTIONS RTSP request.
 * 
 * @param request The RTSP request.
 * @param session The RTSP session->
 */

void RTSPServerhandleOptions(char* request, TRTSP_Session *session) {
  char* urlStart = strstr(request, "rtsp://");
  if (urlStart) {
    char* pathStart = strchr(urlStart + 7, '/');
    char* pathEnd = strchr(pathStart, ' ');
    if (pathStart && pathEnd) {
      *pathEnd = 0; // Null-terminate the path
      // Path can be processed here if needed
    }
  }
  
  char response[512];
  const char* publicMethods = "Public: OPTIONS, DESCRIBE, SETUP, PLAY, PAUSE, TEARDOWN\r\n\r\n";
  
  snprintf(response, sizeof(response), 
           "RTSP/1.0 200 OK\r\n"
           "CSeq: %d\r\n"
           "%s\r\n"
           "%s",
           session->cseq, 
           RTSPServerdateHeader(), 
           publicMethods);
  
  if (session->isHttp) {
    char httpResponse[1024];
    RTSPServerwrapInHTTP(response, strlen(response), httpResponse, sizeof(httpResponse));
    write(session->httpSock, httpResponse, strlen(httpResponse));
  } else {
    write(session->sock, response, strlen(response));
  }
}

/**
 * @brief Handles the DESCRIBE RTSP request.
 * 
 * @param session The RTSP session->
 */
void RTSPServerhandleDescribe(const TRTSP_Session *session) {
  char sdpDescription[512];
  int sdpLen = snprintf(sdpDescription, sizeof(sdpDescription),
                        "v=0\r\n"
                        "o=- %u 1 IN IP4 %s\r\n"
                        "s=\r\n"
                        "c=IN IP4 0.0.0.0\r\n"
                        "t=0 0\r\n"
                        "a=control:*\r\n",
                         session->sessionID, RTSPServer->rtpIp);

  if (RTSPServer->isVideo) {
    sdpLen += snprintf(sdpDescription + sdpLen, sizeof(sdpDescription) - sdpLen,
                       "m=video 0 RTP/AVP 26\r\n"
                       "a=control:video\r\n");
  }

  const char* mediaCondition = "sendrecv"; 
  // if (haveMic && haveAmp) mediaCondition = "sendrecv"; 
  // else if (haveMic) mediaCondition = "sendonly"; 
  // else if (haveAmp) mediaCondition = "recvonly"; 
  // else mediaCondition = "inactive"; 

  if (RTSPServer->isAudio) {
    sdpLen += snprintf(sdpDescription + sdpLen, sizeof(sdpDescription) - sdpLen,
                       "m=audio 0 RTP/AVP 97\r\n"
                       "a=rtpmap:97 L16/%u/1\r\n"
                       "a=control:audio\r\n"
                       "a=%s\r\n", RTSPServer->sampleRate, mediaCondition);
  }

  if (RTSPServer->isSubtitles) {
    sdpLen += snprintf(sdpDescription + sdpLen, sizeof(sdpDescription) - sdpLen,
                       "m=text 0 RTP/AVP 98\r\n"
                       "a=rtpmap:98 t140/1000\r\n"
                       "a=control:subtitles\r\n");
  }

  char response[1024];
  int responseLen = snprintf(response, sizeof(response),
                             "RTSP/1.0 200 OK\r\nCSeq: %d\r\n%s\r\nContent-Base: rtsp://%s:554/\r\nContent-Type: application/sdp\r\nContent-Length: %d\r\n\r\n"
                             "%s",
                             session->cseq, RTSPServerdateHeader(), RTSPServer->rtpIp, sdpLen, sdpDescription);
  
  write(session->isHttp ? session->httpSock : session->sock, response, responseLen);
}

/**
 * @brief Handles the SETUP RTSP request.
 * 
 * @param request The RTSP request.
 * @param session The RTSP session->
 */
void RTSPServerhandleSetup(char* request, TRTSP_Session *session) {
  session->isMulticast = strstr(request, "multicast") != NULL;
  session->isTCP = strstr(request, "RTP/AVP/TCP") != NULL;

#ifndef OVERRIDE_RTSP_SINGLE_CLIENT_MODE
  // Track the first client's connection type
  if (!RTSPServer->firstClientConnected) {
    RTSPServer->firstClientConnected = true;
    RTSPServer->firstClientIsMulticast = session->isMulticast;
    RTSPServer->firstClientIsTCP = session->isTCP;

    // Set max clients based on the first client's connection type, accounting for HTTP tunneling
    if (session->isHttp) {
        // Keep current max clients since it was already increased for HTTP tunneling
        RTSP_LOGD(LOG_TAG, "Keeping current max clients for HTTP tunneling");
    } else {
        RTSPServersetMaxClients(RTSPServer->firstClientIsMulticast ? RTSPServer->maxRTSPClients : 1);
    }
  } else {
    // Determine if the connection should be rejected
    bool placeholder = RTSPServer->firstClientIsTCP;
    bool rejectConnection =
        (RTSPServer->firstClientIsMulticast && !session->isMulticast) ||
        (!RTSPServer->firstClientIsMulticast &&
         (session->isMulticast || session->isTCP != placeholder));

    if (rejectConnection) {
      RTSP_LOGW(LOG_TAG, "Rejecting connection because it does not match the first client's connection type");
      char response[512];
      snprintf(response, sizeof(response),
               "RTSP/1.0 461 Unsupported Transport\r\n"
               "CSeq: %d\r\n"
               "%s\r\n\r\n",
               session->cseq, RTSPServerdateHeader());
      if (write(session->sock, response, strlen(response)) < 0) {
        RTSP_LOGE(LOG_TAG, "Failed to send rejection response to client.");
      }
      return;
    }
  }
#else
  setMaxClients(RTSPServer->maxRTSPClients);
#endif

  bool setVideo = strstr(request, "video") != NULL;
  bool setAudio = strstr(request, "audio") != NULL;
  bool setSubtitles = strstr(request, "subtitles") != NULL;
  uint16_t clientPort = 0;
  uint16_t serverPort = 0;
  uint8_t rtpChannel = 0;

  // Extract client port or RTP channel based on transport method
  if (session->isTCP) {
    char* interleaveStart = strstr(request, "interleaved=");
    if (interleaveStart) {
      interleaveStart += 12;
      char* interleaveEnd = strchr(interleaveStart, '-');
      if (interleaveStart && interleaveEnd) {
        *interleaveEnd = 0;
        rtpChannel = atoi(interleaveStart);
        RTSP_LOGD(LOG_TAG, "Extracted RTP channel: %d", rtpChannel);
      } else {
        RTSP_LOGE(LOG_TAG, "Failed to find interleave end");
      }
    } else {
      RTSP_LOGE(LOG_TAG, "Failed to find interleaved=");
    }
  } else if (!session->isMulticast) {
    char* rtpPortStart = strstr(request, "client_port=");
    if (rtpPortStart) {
      rtpPortStart += 12;
      char* rtpPortEnd = strchr(rtpPortStart, '-');
      if (rtpPortStart && rtpPortEnd) {
        *rtpPortEnd = 0;
        clientPort = atoi(rtpPortStart);
        RTSP_LOGD(LOG_TAG, "Extracted client port: %d", clientPort);
      } else {
        RTSP_LOGE(LOG_TAG, "Failed to find client port end");
      }
    } else {
      RTSP_LOGE(LOG_TAG, "Failed to find client_port=");
    }
  }

  // Setup video, audio, or subtitles based on the request
  if (setVideo) {
    session->cVideoPort = clientPort;
    serverPort = RTSPServer->rtpVideoPort;
    RTSPServer->videoCh = rtpChannel;
    if (!session->isTCP) {
      if (session->isMulticast) {
        RTSPServercheckAndSetupUDP(&RTSPServer->videoMulticastSocket, true, serverPort, RTSPServer->rtpIp);
      } else {
        RTSPServercheckAndSetupUDP(&RTSPServer->videoUnicastSocket, false, serverPort, RTSPServer->rtpIp);
      }
    }
  }
  
  if (setAudio) {
    session->cAudioPort = clientPort;
    serverPort = RTSPServer->rtpAudioPort;
    RTSPServer->audioCh = rtpChannel;
    if (!session->isTCP) {
      if (session->isMulticast) {
        RTSPServercheckAndSetupUDP(&RTSPServer->audioMulticastSocket, true, serverPort, RTSPServer->rtpIp);
      } else {
        RTSPServercheckAndSetupUDP(&RTSPServer->audioUnicastSocket, false, serverPort, RTSPServer->rtpIp);
      }
    }
  }
  
  if (setSubtitles) {
    session->cSrtPort = clientPort;
    serverPort = RTSPServer->rtpSubtitlesPort;
    RTSPServer->subtitlesCh = rtpChannel;
    if (!session->isTCP) {
      if (session->isMulticast) {
        RTSPServercheckAndSetupUDP(&RTSPServer->subtitlesMulticastSocket, true, serverPort, RTSPServer->rtpIp);
      } else {
        RTSPServercheckAndSetupUDP(&RTSPServer->subtitlesUnicastSocket, false, serverPort, RTSPServer->rtpIp);
      }
    }
  }


#ifdef RTSP_VIDEO_NONBLOCK
  if (setVideo && RTSPServer->rtpVideoTaskHandle == NULL) {
    xTaskCreate(rtpVideoTaskWrapper, "rtpVideoTask", RTP_STACK_SIZE, RTSPServer, RTP_PRI, &RTSPServer->rtpVideoTaskHandle);
  }
  if (RTSPServer->rtspStreamBuffer == NULL && psramFound()) {
    RTSPServer->rtspStreamBuffer = (uint8_t*)ps_malloc(MAX_RTSP_BUFFER);
  }
#endif

  char* response = (char*)malloc(512);
  if (response == NULL) {
    RTSP_LOGE(LOG_TAG, "Failed to allocate memory");
    return;
  }

  // Formulate the response based on transport method
  if (session->isTCP) {
    snprintf(response, 512,
             "RTSP/1.0 200 OK\r\n"
             "CSeq: %d\r\n"
             "%s\r\n"
             "Transport: RTP/AVP/TCP;unicast;interleaved=%d-%d\r\n"
             "Session: %u\r\n\r\n",
             session->cseq, RTSPServerdateHeader(), rtpChannel, rtpChannel + 1, session->sessionID);
  } else if (session->isMulticast) {
    snprintf(response, 512,
             "RTSP/1.0 200 OK\r\nCSeq: %d\r\n%s\r\nTransport: RTP/AVP;multicast;destination=%s;port=%d-%d;ttl=%d\r\nSession: %u\r\n\r\n",
             session->cseq, RTSPServerdateHeader(), RTSPServer->rtpIp, serverPort, serverPort + 1, RTSPServer->rtpTTL, session->sessionID);
  } else {
    snprintf(response, 512,
             "RTSP/1.0 200 OK\r\nCSeq: %d\r\n%s\r\nTransport: RTP/AVP;unicast;destination=127.0.0.1;source=127.0.0.1;client_port=%d-%d;server_port=%d-%d\r\nSession: %u\r\n\r\n",
             session->cseq, RTSPServerdateHeader(), clientPort, clientPort + 1, serverPort, serverPort + 1, session->sessionID);
  }

  write(session->isHttp ? session->httpSock : session->sock, response, strlen(response));
  
  free(response);
  //RTSPServer->sessions[session->sessionID] = session;
}

/**
 * @brief Handles the PLAY RTSP request.
 * 
 * @param session The RTSP session->
 */
void RTSPServerhandlePlay(TRTSP_Session *session) {
  session->isPlaying = true;
//  RTSPServer->sessions[session->sessionID] = session;
  RTSPServersetIsPlaying(true);

  char response[256];
  snprintf(response, sizeof(response),
           "RTSP/1.0 200 OK\r\n"
           "CSeq: %d\r\n"
           "%s\r\n"
           "Range: npt=0.000-\r\n"
           "Session: %u\r\n"
           "RTP-Info: url=rtsp://127.0.0.1:554/\r\n\r\n",
           session->cseq,
           RTSPServerdateHeader(),
           session->sessionID);

  write(session->isHttp ? session->httpSock : session->sock, response, strlen(response));
}

/**
 * @brief Handles the PAUSE RTSP request.
 * 
 * @param session The RTSP session->
 */
void RTSPServerhandlePause(TRTSP_Session * session) {
  session->isPlaying = false;
 // RTSPServer->sessions[session->sessionID] = session;
  RTSPServerupdateIsPlayingStatus();
  char response[128];
  int len = snprintf(response, sizeof(response),
                     "RTSP/1.0 200 OK\r\nCSeq: %d\r\nSession: %u\r\n\r\n",
                     session->cseq, session->sessionID);
  
  write(session->isHttp ? session->httpSock : session->sock, response, len);
  RTSP_LOGD(LOG_TAG, "Session %u is now paused.", session->sessionID);
}

/**
 * @brief Handles the TEARDOWN RTSP request.
 * 
 * @param session The RTSP session->
 */
void RTSPServerhandleTeardown(TRTSP_Session * session) {
  session->isPlaying = false;
//  RTSPServer->sessions[session->sessionID] = session;
  RTSPServerupdateIsPlayingStatus();

  char response[128];
  int len = snprintf(response, sizeof(response),
                     "RTSP/1.0 200 OK\r\nCSeq: %d\r\nSession: %u\r\n\r\n",
                     session->cseq, session->sessionID);
  
  write(session->isHttp ? session->httpSock : session->sock, response, len);

  RTSP_LOGD(LOG_TAG, "RTSP Session %u has been torn down.", session->sessionID);
}

/**
 * @brief Handles incoming RTSP requests.
 * 
 * @param sock The socket file descriptor.
 * @param clientAddr The client address.
 * @return true if the request was handled successfully, false otherwise.
 */
bool RTSPServerhandleRTSPRequest(TRTSP_Session * session) {
  void *placeholder = malloc(RTSP_BUFFER_SIZE);
 
  char *buffer = (char *)placeholder;
  if (!buffer) {
    RTSP_LOGE(LOG_TAG, "Failed to allocate buffer with ps_malloc");
    return false;
  }

  int totalLen = 0;
  int len = 0;

  // Read data from socket until end of RTSP header or buffer limit is reached
  while ((len = recv(session->sock, buffer + totalLen, RTSP_BUFFER_SIZE - totalLen - 1, 0)) > 0) {
    totalLen += len;
    if (strstr(buffer, "\r\n\r\n")) {
      break;
    }
    if (totalLen >= RTSP_BUFFER_SIZE) { // Adjusted for null-terminator
      RTSP_LOGE(LOG_TAG, "Request too large for buffer. Total length: %d", totalLen);
      free(buffer); // Free allocated memory
      return false;
    }
  }

  if (totalLen <= 0) {
    int err = errno;
    free(buffer);
    if (err == EWOULDBLOCK || err == EAGAIN) {
      return true;
    } else if (err == ECONNRESET || err == ENOTCONN) {
      RTSP_LOGD(LOG_TAG, "Connection reset/closed - HandleTeardown");
      // Handle teardown for current session
      RTSPServerhandleTeardown(session);
      // If RTSPServer is an HTTP session, find and teardown both GET and POST sessions
      if (session->isHttp && session->sessionCookie[0] != '\0') {
          // Find the paired session
          TRTSP_Session* pairedSession = RTSPServerfindSessionByCookie(session->sessionCookie);
          if (pairedSession && pairedSession != &session) {
              RTSP_LOGD(LOG_TAG, "Found paired HTTP session, handling teardown");
              RTSPServerhandleTeardown(pairedSession);
          }
      }
      
      return false;
    } else {
      RTSP_LOGE(LOG_TAG, "Error reading from socket, error: %d", err);
      return false;
    }
  }

  // Check to see if RTCP packet and ignore for now...
  buffer[totalLen] = 0; // Null-terminate the buffer
  if (buffer[0] == '$') {
    free(buffer); // Free allocated memory
    return true; 
  }

  uint8_t firstByte = buffer[0]; 
  uint8_t version = (firstByte >> 6) & 0x03;
  if (version == 2) { 
    uint8_t payloadType = buffer[1] & 0x7F;
    if (payloadType >= 200 && payloadType <= 204) {
      free(buffer); // Free allocated memory
      return true;
    }
    free(buffer); // Free allocated memory
    return true;
  }

  // Check if the request is base64 encoded FIRST
  RTSP_LOGD(LOG_TAG, "Checking if base64 encoded");
  
  if (RTSPServerisBase64Encoded(buffer, totalLen)) {
    RTSP_LOGD(LOG_TAG, "Buffer is base64 encoded, decoding...");
    char* decodedBuffer = (char*)malloc(RTSP_BUFFER_SIZE);
    if (!decodedBuffer) {
      RTSP_LOGE(LOG_TAG, "Failed to allocate memory for decoded buffer");
      free(buffer);
      return false;
    }

    size_t decodedLen;
    if (RTSPServerdecodeBase64(buffer, totalLen, decodedBuffer, &decodedLen)) {
      RTSP_LOGD(LOG_TAG, "Decoded buffer: %s", decodedBuffer);
      free(buffer);
      buffer = decodedBuffer;
      totalLen = decodedLen;
    } else {
      RTSP_LOGE(LOG_TAG, "Failed to decode base64 buffer");
      free(decodedBuffer);
      free(buffer);
      return false;
    }
  }

  int cseq = RTSPServercaptureCSeq(buffer);
  if (cseq == -1) {
    RTSP_LOGE(LOG_TAG, "CSeq not found in request: %s", buffer);
    write(session->sock, "RTSP/1.0 400 Bad Request\r\n\r\n", 29);
    free(buffer); // Free allocated memory
    return true;
  }

  session->cseq = cseq;

  // Extract session ID using the provided function
  uint32_t sessionID = RTSPServerextractSessionID(buffer);
  /*
  if (sessionID != 0 && sessions->find(sessionID) != sessions->end()) {
    session->sessionID = sessionID;
  }
*/
  // Authentication check
  if (RTSPServer->authEnabled) {
    char* authHeader = strstr(buffer, "Authorization: Basic ");
    if (!authHeader) {
      RTSPServersendUnauthorizedResponse(session);
      free(buffer); // Free allocated memory
      return true;
    } else {
      authHeader += 21; // Move pointer to the base64 encoded credentials
      char* authEnd = strstr(authHeader, "\r\n");
      if (authEnd) {
        *authEnd = 0; // Null-terminate the base64 string
        if (strcmp(authHeader, RTSPServer->base64Credentials) != 0) {
          RTSPServersendUnauthorizedResponse(session);
          free(buffer); // Free allocated memory
          return true;
        } else {
          // Remove the Authorization header from the buffer before continuing
          memmove(authHeader - 21, authEnd + 2, strlen(authEnd + 2) + 1);
        }
      } else {
        RTSPServersendUnauthorizedResponse(session);
        free(buffer); // Free allocated memory
        return true;
      }
    }
  }

  // Handle HTTP tunneling methods first
  if (strncmp(buffer, "GET / HTTP/", 10) == 0 && strstr(buffer, "Accept: application/x-rtsp-tunnelled")) {
    RTSP_LOGD(LOG_TAG, "Handle GET HTTP Request: %s", buffer);
    
    // Increase max clients by 1 to account for HTTP tunneling
    uint8_t currentMaxClients = RTSPServergetMaxClients();
    RTSPServersetMaxClients(currentMaxClients + 1);
    RTSP_LOGD(LOG_TAG, "Increased max clients to %d for HTTP tunneling", currentMaxClients + 1);
    
    session->isHttp = true;
    char sessionCookie[MAX_COOKIE_LENGTH];
    RTSPServerextractSessionCookie(buffer, sessionCookie, sizeof(sessionCookie));
    strncpy(session->sessionCookie, sessionCookie, MAX_COOKIE_LENGTH - 1);
    session->sessionCookie[MAX_COOKIE_LENGTH - 1] = '\0';

    char response[512];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"  // Use HTTP/1.1 for better compatibility
             "Server: ESP32\r\n"
             "Connection: keep-alive\r\n"
             "%s"
             "Cache-Control: no-store\r\n"
             "Pragma: no-cache\r\n"
             "Content-Type: application/x-rtsp-tunnelled\r\n"
             "\r\n",
             RTSPServerdateHeader());
    write(session->sock, response, strlen(response));  // Use direct socket for initial HTTP response
  }
  else if (strncmp(buffer, "POST / HTTP/", 11) == 0 && strstr(buffer, "Content-Type: application/x-rtsp-tunnelled")) {
    RTSP_LOGD(LOG_TAG, "RTSP-over-HTTP Tunnel Established");
    RTSP_LOGD(LOG_TAG, "Handle POST HTTP Request: %s", buffer);
    
    // Extract cookie from POST request
    char sessionCookie[MAX_COOKIE_LENGTH];
    RTSPServerextractSessionCookie(buffer, sessionCookie, sizeof(sessionCookie));
    
    // Find corresponding GET session
    TRTSP_Session* getSession = RTSPServerfindSessionByCookie(sessionCookie);
    if (getSession) {
        // Keep POST session but use GET session's socket for responses
        session->httpSock = getSession->sock;
        session->isHttp = true;
        strncpy(session->sessionCookie, sessionCookie, MAX_COOKIE_LENGTH - 1);
        session->sessionCookie[MAX_COOKIE_LENGTH - 1] = '\0';
    } else {
        RTSP_LOGE(LOG_TAG, "No matching GET session found for cookie: %s", sessionCookie);
    }
  } else {
    // Handle regular RTSP commands
    RTSPServerhandleRTSPCommand(buffer, session);
  }

  free(buffer);
  return true;
}

void RTSPServersendUnauthorizedResponse(TRTSP_Session * session) {
  char response[256];
  snprintf(response, sizeof(response),
           "RTSP/1.0 401 Unauthorized\r\n"
           "CSeq: %d\r\n"
           "WWW-Authenticate: Basic realm=\"ESP32\"\r\n\r\n",
           session->cseq);
  
  write(session->isHttp ? session->httpSock : session->sock, response, strlen(response));
  RTSP_LOGW(LOG_TAG, "Sent 401 Unauthorized response to client.");
}

void RTSPServerhandleRTSPCommand(char* command, TRTSP_Session * session) {
  if (strncmp(command, "OPTIONS", 7) == 0) {
    RTSP_LOGD(LOG_TAG, "Handle RTSP Options");
    RTSPServerhandleOptions(command, session);
  } else if (strncmp(command, "DESCRIBE", 8) == 0) {
    RTSP_LOGD(LOG_TAG, "Handle RTSP Describe");
    RTSPServerhandleDescribe(session);
  } else if (strncmp(command, "SETUP", 5) == 0) {
    RTSP_LOGD(LOG_TAG, "Handle RTSP Setup");
    RTSPServerhandleSetup(command, session);
  } else if (strncmp(command, "PLAY", 4) == 0) {
    RTSP_LOGD(LOG_TAG, "Handle RTSP Play");
    RTSPServerhandlePlay(session);
  } else if (strncmp(command, "TEARDOWN", 8) == 0) {
    RTSP_LOGD(LOG_TAG, "Handle RTSP Teardown");
    RTSPServerhandleTeardown(session);
  } else if (strncmp(command, "PAUSE", 5) == 0) {
    RTSP_LOGD(LOG_TAG, "Handle RTSP Pause");
    RTSPServerhandlePause(session);
  } else {
    RTSP_LOGW(LOG_TAG, "Unknown RTSP method: %s", command);
  }
}

bool RTSPServerisBase64Encoded(const char* buffer, size_t length) {
    // First check for spaces - if found, not base64
    for (size_t i = 0; i < length; i++) {
        if (isspace(buffer[i])) {
            return false;
        }
    }

    // Now check if it's valid base64
    if (length % 4 != 0) {
        return false;
    }

    for (size_t i = 0; i < length; i++) {
        if (!isalnum(buffer[i]) && 
            buffer[i] != '+' && 
            buffer[i] != '/' && 
            buffer[i] != '=') {
            return false;
        }
    }

    return true;
}

void RTSPServerextractSessionCookie(const char* buffer, char* sessionCookie, size_t maxLen) {
    const char* cookieHeader = strstr(buffer, "x-sessioncookie:");
    if (cookieHeader) {
        cookieHeader += strlen("x-sessioncookie:");
        while (*cookieHeader == ' ') cookieHeader++;
        const char* end = strstr(cookieHeader, "\r\n");
        size_t len = end ? (size_t)(end - cookieHeader) : strlen(cookieHeader);
        len = len < maxLen ? len : maxLen - 1;
        strncpy(sessionCookie, cookieHeader, len);
        sessionCookie[len] = '\0';
    } else {
        sessionCookie[0] = '\0';
    }
}

TRTSP_Session* RTSPServerfindSessionByCookie(const char* cookie) {
   /* for (auto& pair : sessions) {
        if (strcmp(pair.second.sessionCookie, cookie) == 0) {
            return &pair.second;
        }
    }*/
    return NULL;
}




void RTSPServerstartSubtitlesTimer(esp_timer_cb_t userCallback) { 
  const esp_timer_create_args_t timerConfig = { 
    .callback = userCallback, // User-defined callback function 
    .arg = NULL, // Optional argument, can be set to NULL
    .dispatch_method = ESP_TIMER_TASK, // Dispatch method, set to default
    .name = "periodic_timer" ,
    .skip_unhandled_events = false // Optional, can be set to false
    }; 
    //esp_timer_create(&timerConfig, &sendSubtitlesTimer); 
    //esp_timer_start_periodic(sendSubtitlesTimer, 1000000); 
}

void RTSPServersetMaxClients(uint8_t newMaxClients) {
  if (xSemaphoreTake(RTSPServer->maxClientsMutex, portMAX_DELAY) == pdTRUE) {
    if (newMaxClients <= MAX_CLIENTS) {
      RTSPServer->maxClients = newMaxClients;
      RTSP_LOGI(LOG_TAG, "Max clients updated to: %d", RTSPServer->maxClients);
    } else {
      RTSP_LOGW(LOG_TAG, "Requested max clients (%d) exceeds the hardcoded limit (%d). Max clients set to %d.", newMaxClients, MAX_CLIENTS, MAX_CLIENTS);
      RTSPServer->maxClients = MAX_CLIENTS;
    }
    xSemaphoreGive(RTSPServer->maxClientsMutex);
  } else {
    RTSP_LOGE(LOG_TAG, "Failed to acquire maxClients mutex");
  }
}

uint8_t RTSPServergetMaxClients() {
  uint8_t clients = 0;
  if (xSemaphoreTake(RTSPServer->maxClientsMutex, portMAX_DELAY) == pdTRUE) {
    clients = RTSPServer->maxClients;
    xSemaphoreGive(RTSPServer->maxClientsMutex);
  } else {
    RTSP_LOGE(LOG_TAG, "Failed to acquire maxClients mutex");
  }
  return clients;
}

void RTSPServerincrementActiveRTSPClients() {
  if (RTSPServer->activeRTSPClients < 255) {
    RTSPServer->activeRTSPClients++;
    RTSP_LOGI(LOG_TAG, "Active RTSP clients count incremented: %d", RTSPServer->activeRTSPClients);
  } else {
    RTSP_LOGW(LOG_TAG, "Max RTSP clients reached: %d", 255);
  }
}

void RTSPServerdecrementActiveRTSPClients() {
  if (RTSPServer->activeRTSPClients > 0) {
    RTSPServer->activeRTSPClients--;
    RTSP_LOGI(LOG_TAG, "Active RTSP clients count decremented: %d", RTSPServer->activeRTSPClients);
  } else {
    RTSP_LOGW(LOG_TAG, "Min RTSP clients already: %d", 0);
  }
}

uint8_t RTSPServergetActiveRTSPClients() {
  return RTSPServer->activeRTSPClients;
}

void RTSPServerupdateIsPlayingStatus() {
  bool anyClientStreaming = false;
  /*for (const auto& sessionPair : sessions) {
    if (sessionPair.second.isPlaying) {
      anyClientStreaming = true;
      break;
    }
  }*/
  RTSPServersetIsPlaying(anyClientStreaming);
}

void RTSPServersetIsPlaying(bool playing) {
    xSemaphoreTake(RTSPServer->isPlayingMutex, portMAX_DELAY);
    RTSPServer->isPlaying = playing;
    xSemaphoreGive(RTSPServer->isPlayingMutex);
}

bool RTSPServergetIsPlaying()  {
    bool playing;
    xSemaphoreTake(RTSPServer->isPlayingMutex, portMAX_DELAY);
    playing = RTSPServer->isPlaying;
    xSemaphoreGive(RTSPServer->isPlayingMutex);
    return playing;
}

bool RTSPServerreadyToSendFrame()  {
  return RTSPServergetIsPlaying() && RTSPServer->rtpFrameSent;
}

bool RTSPServerreadyToSendAudio()  {
  return RTSPServergetIsPlaying() && RTSPServer->rtpAudioSent;
}

bool RTSPServerreadyToSendSubtitles()  {
  return RTSPServergetIsPlaying() && RTSPServer->rtpSubtitlesSent;
}

int RTSPServercaptureCSeq(char* request) {
  char* cseqStr = strstr(request, "CSeq: ");
  if (cseqStr == NULL) {
    return -1;
  }
  cseqStr += 6;
  char* endOfLine = strchr(cseqStr, '\n');
  if (endOfLine) {
    *endOfLine = 0;
  }
  int cseq = atoi(cseqStr);
  if (endOfLine) {
    *endOfLine = '\n';
  }
  return cseq;
}

uint32_t RTSPServergenerateSessionID() {
  return esp_random();
}

uint32_t RTSPServerextractSessionID(char* request) {
  char* sessionStr = strstr(request, "Session: ");
  if (sessionStr == NULL) {
    return 0;
  }
  sessionStr += 9;
  char* endOfLine = strchr(sessionStr, '\n');
  if (endOfLine) {
    *endOfLine = 0;
  }

  while (isspace(*sessionStr)) sessionStr++;
  char* end = sessionStr + strlen(sessionStr) - 1;
  while (end > sessionStr && isspace(*end)) end--;
  *(end + 1) = 0;

  uint32_t sessionID = strtoul(sessionStr, NULL, 10);

  if (endOfLine) {
    *endOfLine = '\n';
  }

  return sessionID;
}

const char* RTSPServerdateHeader() {
  static char buffer[50];
  time_t now = time(NULL);
  strftime(buffer, sizeof(buffer), "Date: %a, %d %b %Y %H:%M:%S GMT", gmtime(&now));
  return buffer;
}

bool RTSPServersetCredentials(const char* username, const char* password) {
  if (username && password && strlen(username) > 0 && strlen(password) > 0) {
    char credentials[128];
    snprintf(credentials, sizeof(credentials), "%s:%s", username, password);

    // Base64 encode the credentials
    base64_encodestate state;
    base64_init_encodestate(&state);
    int encodedLen = base64_encode_chars(credentials, strlen(credentials), &RTSPServer->base64Credentials);
    RTSPServer->base64Credentials[encodedLen] = '\0'; // Null-terminate the encoded string

    RTSPServer->authEnabled = true;
    RTSP_LOGI(LOG_TAG, "Authentication enabled with provided credentials.");
    return true; // Indicate success
  } else {
    RTSPServer->authEnabled = false;
    RTSP_LOGI(LOG_TAG, "Authentication disabled.");
    return false; // Indicate failure
  }
}

bool RTSPServerdecodeBase64(const char* input, size_t inputLen, char* output, size_t* outputLen) {
    base64_decodestate state;
    base64_init_decodestate(&state);
    int len = base64_decode_block(input, inputLen, output, &state);
    if (len >= 0) {
        *outputLen = len;
        output[len] = '\0';
        return true;
    }
    return false;
}



void RTSPServercheckAndSetupUDP(int *rtpSocket, bool isMulticast, uint16_t rtpPort, IPAddress rtpIp) {
  if (*rtpSocket == -1) {
    *rtpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (*rtpSocket < 0) {
      RTSP_LOGE(LOG_TAG, "Failed to create RTP socket");
      return;
    }
    if (!RTSPServersetNonBlocking(*rtpSocket)) {
      RTSP_LOGE(LOG_TAG, "Failed to set RTP socket to non-blocking mode.");
      close(*rtpSocket);
      return;
    }
    struct sockaddr_in rtpAddr;
    memset(&rtpAddr, 0, sizeof(rtpAddr));
    rtpAddr.sin_family = AF_INET;
    rtpAddr.sin_port = htons(rtpPort);
    if (isMulticast) {
      inet_aton(RTSPServer->rtpIp, &rtpAddr.sin_addr);
      setsockopt(*rtpSocket, IPPROTO_IP, IP_MULTICAST_TTL, &RTSPServer->rtpTTL, sizeof(RTSPServer->rtpTTL));
    } else {
      rtpAddr.sin_addr.s_addr = INADDR_ANY;
      if (bind(*rtpSocket, (struct sockaddr *)&rtpAddr, sizeof(rtpAddr)) < 0) {
        RTSP_LOGE(LOG_TAG, "Failed to bind RTP socket on port %d", rtpPort);
        return;
      }
    }
  }
}

void RTSPServersendTcpPacket(const uint8_t* packet, size_t packetSize, int sock) {
  if (xSemaphoreTake(RTSPServer->sendTcpMutex, portMAX_DELAY) == pdTRUE) {
    ssize_t sent = 0;
    while (sent < packetSize) {
      ssize_t result = send(sock, packet + sent, packetSize - sent, 0);
      if (result < 0) {
        int err = errno;
        if (err == EAGAIN || err == EWOULDBLOCK) {
          fd_set write_fds;
          FD_ZERO(&write_fds);
          FD_SET(sock, &write_fds);
          //struct timeval tv = { .tv_sec = 0, .tv_usec = 100000 }; // 100ms
          int ret = select(sock + 1, NULL, &write_fds, NULL, NULL);
          if (ret <= 0) {
            RTSP_LOGE(LOG_TAG, "Failed to send TCP packet, select timeout or error");
            break;
          }
          continue;
        } else if (err != EPIPE && err != ECONNRESET && err != ENOTCONN && err != EBADF) {
          RTSP_LOGE(LOG_TAG, "Failed to send TCP packet, errno: %d", err);
          break;
        } else {
          break;
        }
      } else {
        sent += result;
      }
    }
    xSemaphoreGive(RTSPServer->sendTcpMutex);
  } else {
    RTSP_LOGE(LOG_TAG, "Failed to acquire mutex");
  }
}

bool RTSPServersetNonBlocking(int sock) { 
  int flags = fcntl(sock, F_GETFL, 0); 
  if (flags == -1) { 
    RTSP_LOGE(LOG_TAG, "Failed to get socket flags"); 
    return false; 
  } 
  if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1) { 
    RTSP_LOGE(LOG_TAG, "Failed to set socket to non-blocking mode"); 
    return false;
  } 
  RTSP_LOGI(LOG_TAG, "Socket set to non-blocking mode");
  return true;
}



// local files
#include "Client.h"

// AIE files
#include "Gizmos.h"
#include "Input.h"
#include "GameMessages.h"

// Third Party dependencies
#include <imgui.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>

using aie::Gizmos;

Client::Client() {

}

Client::~Client() {
}

bool Client::startup() {
	
	setBackgroundColour(0.25f, 0.25f, 0.25f);

   Client::handleNetworkConnections();
   

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(glm::vec3(10), glm::vec3(0), glm::vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);

	return true;
}

void Client::shutdown() {

	Gizmos::destroy();
}

void Client::update(float deltaTime) {

	// query time since application started
	float time = getTime();

	// wipe the gizmos clean for this frame
	Gizmos::clear();

   Client::handleNetworkMessages();
	
	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void Client::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);

	Gizmos::draw(m_projectionMatrix * m_viewMatrix);
}

void Client::handleNetworkConnections()
{
   _peerInterface = RakNet::RakPeerInterface::GetInstance();
   initialiseClientConnection();
}

void Client::initialiseClientConnection()
{
   RakNet::SocketDescriptor sd;

   _peerInterface->Startup(1, &sd, 1);

   std::cout << "connecting to server at: " << IP << std::endl;

   RakNet::ConnectionAttemptResult result = _peerInterface->Connect(IP, PORT, nullptr, 0);

   if (result != RakNet::CONNECTION_ATTEMPT_STARTED)
   {
      std::cout << "Unable to start connection, Error number: " << result << std::endl;
   }

}

void Client::handleNetworkMessages()
{
   RakNet::Packet* packet;

   for (packet = _peerInterface->Receive(); packet;
             _peerInterface->DeallocatePacket(packet),
             packet = _peerInterface->Receive())
   {
      switch (packet->data[0])
      {
      case ID_REMOTE_DISCONNECTION_NOTIFICATION:
         std::cout << "Another client has disconnected.\n";
         break;
      case ID_REMOTE_CONNECTION_LOST:
         std::cout << "Another client has lost connection.\n";
         break;
      case ID_CONNECTION_REQUEST_ACCEPTED:
         std::cout << "Our connection request has been accepted.\n";
         break;
      case ID_NO_FREE_INCOMING_CONNECTIONS:
         std::cout << "The server is full.\n";
         break;
      case ID_DISCONNECTION_NOTIFICATION:
         std::cout << "We have been disconnected.\n";
         break; 
      case ID_CONNECTION_LOST:
         std::cout << "We have lost connection.\n";
         break; 
      case ID_SERVER_TEXT_MESSAGE: 
      case ID_CLIENT_CHAT_MESSAGE:
      {
         RakNet::BitStream bsIn(packet->data, packet->length, false);
         bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
         RakNet::RakString str;
         bsIn.Read(str);
         std::cout << str << std::endl;
         break;
      }
      default:
         std::cout << "Received message with unknown id: " << packet->data[0] << std::endl;
         break;
      }
   }
}




#include "legacy/tcpip.h"

#include <array>
#include <chrono>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>

namespace {

constexpr auto kTimeout = std::chrono::seconds(5);
constexpr auto kPollDelay = std::chrono::milliseconds(50);
constexpr char kPing[] = "udp_ping";
constexpr char kPong[] = "udp_pong";

bool Wait_For_Message(TcpipManagerClass& manager, std::string& message) {
	std::array<char, 256> buffer{};
	const auto start = std::chrono::steady_clock::now();
	while (std::chrono::steady_clock::now() - start < kTimeout) {
		const int received = manager.Read(buffer.data(), static_cast<int>(buffer.size()));
		if (received > 0) {
			message.assign(buffer.data(), buffer.data() + received);
			return true;
		}
		std::this_thread::sleep_for(kPollDelay);
	}
	return false;
}

int Run_Server() {
	TcpipManagerClass manager;
	manager.Set_Protocol_UDP(TRUE);
	manager.Start_Server();

	std::cout << "server: waiting for client ping..." << std::endl;
	std::string message;
	if (!Wait_For_Message(manager, message)) {
		std::cerr << "server: timed out waiting for ping" << std::endl;
		return 1;
	}

	std::cout << "server: received '" << message << "'" << std::endl;
	manager.Write(const_cast<char*>(kPong), static_cast<int>(std::strlen(kPong)));
	std::cout << "server: sent pong" << std::endl;
	return 0;
}

int Run_Client(std::string host) {
	TcpipManagerClass manager;
	manager.Set_Bind_Port(0);
	manager.Set_Protocol_UDP(TRUE);
	manager.Set_Host_Address(const_cast<char*>(host.c_str()));
	manager.Start_Client();

	manager.Write(const_cast<char*>(kPing), static_cast<int>(std::strlen(kPing)));
	std::cout << "client: sent ping" << std::endl;

	std::string message;
	if (!Wait_For_Message(manager, message)) {
		std::cerr << "client: timed out waiting for pong" << std::endl;
		return 1;
	}

	std::cout << "client: received '" << message << "'" << std::endl;
	return message == kPong ? 0 : 1;
}

}  // namespace

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "usage: " << argv[0] << " --server|--client [host]" << std::endl;
		return 1;
	}

	const std::string mode = argv[1];
	if (mode == "--server") {
		return Run_Server();
	}

	if (mode == "--client") {
		std::string host = "127.0.0.1";
		if (argc >= 3) {
			host = argv[2];
		}
		return Run_Client(host);
	}

	std::cerr << "unknown mode: " << mode << std::endl;
	return 1;
}

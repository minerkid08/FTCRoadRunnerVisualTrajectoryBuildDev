#include "Upload.hpp"
#include "NodeGrid.hpp"
#include "Save.hpp"
#include <filesystem>
#include <fstream>
#include <imgui/imgui.h>
#include <iostream>
#include <sstream>
#include <stack>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

const int INVALID_SOCKET = -1;
const int SOCKET_ERROR = -1;

int sock = socket(AF_UNIX, SOCK_STREAM, 0);

static NodeGrid* grid;

void Upload::init(NodeGrid* _grid)
{
  grid = _grid;
	int iResult = 0; 
	if (iResult != 0)
	{
		grid->err =
			("WSAStartup failed with error: " + std::to_string(iResult) + "Uploading to the robot will not work");
		return;
	}
}

static void connectToRobot()
{
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	//ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int iResult = getaddrinfo("192.168.43.1", "6969", &hints, &result);
	if (iResult != 0)
	{
		grid->err =
			("getaddrinfo failed with error: " + std::to_string(iResult) + "Uploading to the robot will not work");
		return;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (sock == INVALID_SOCKET)
		{
			grid->err =
				("socket failed with error: " + std::to_string(iResult) + "Uploading to the robot will not work");
			return;
		}
		iResult = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SO_ERROR)
		{
      close(sock);
			sock = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (sock == INVALID_SOCKET)
	{
		grid->err = ("unable to connect to robot");
		return;
	}
}

static std::string sendToRobot(const std::string& msg)
{
	int iResult = send(sock, msg.c_str(), msg.size(), 0);
	if (iResult == SO_ERROR)
	{
		grid->err = ("send failed with error: " + std::to_string(iResult));
		close(sock);
		return "err";
	}

	printf("Bytes Sent: %ld\n", iResult);
	std::string recive = "";
	char* buf = new char[513];
	while (recv(sock, buf, 512, 0))
	{
		buf[512] = '\0';
		recive += buf;
		memset(buf, 0, 512);
	}
	std::string str2;
	for (wchar_t c : recive)
	{
		if (c == '\r')
			break;
		else
			str2 += c;
	}
	return str2;
}

void Upload::closeSock()
{
	/*int iResult = shutdown(sock, SD_SEND);
	if (iResult == SO_ERROR)
	{
		grid->err = ("shutdown failed with error: " + std::to_string(iResult) + "Uploading to the robot will not work");
	}
  */
	close(sock);
}

static std::string getFile(std::string path)
{
	std::stringstream sstream;
	std::ifstream ifstream(path);
	sstream << ifstream.rdbuf();

	std::string str;
	path = path.substr(4);
	path = "paths" + path;
	std::cout << path << "\n";
	for (int i = 0; i < path.size(); i++)
	{
		if (path[i] == '\\')
		{
			path[i] = '/';
		}
	}
	str += path;
	str += '?';
	str += sstream.str();
	str += '\1';
	return str;
}

void Upload::upload(bool current)
{
  connectToRobot();
	grid->err = "";
	grid->msg = "";
	std::string s = "\t";
	if (current)
	{
		Save::save(grid);
		s += getFile(Save::getPath());
	}
	else
	{
		std::stack<std::string> stack;
		stack.push("save");
		while (!stack.empty())
		{
			std::string path = stack.top();
			stack.pop();
			for (const std::filesystem::path& path2 : std::filesystem::directory_iterator(path))
			{
				if (std::filesystem::is_directory(path2))
				{
					stack.push(path2.string());
				}
				else
				{
					s += getFile(path2.string());
				}
			}
		}
	}
	s += '\2';
	sendToRobot(s);
	grid->msg = "Uploaded";
}

void Upload::pull()
{
  connectToRobot();
	grid->err = "";
	grid->msg = "";

	std::string s = "\tGet\3paths\2";

	std::string res = sendToRobot(s);

	bool readingPath = true;
	std::string path = "";
	std::string data = "";
	for (char c : res)
	{
		if (c == '\n' && readingPath)
		{
			readingPath = false;
			continue;
		}
		if (c == '\b')
		{
			for (char& c2 : path)
				if (c2 == '/')
					c2 = '\\';
			int ind = path.find_last_of('\\');

			std::string folders = path.substr(0, ind);

			ind = path.find_first_of('\\');
			folders = "save\\" + folders.substr(ind);

			ind = path.find_first_of('\\');
			path = "save\\" + path.substr(ind);

			if (!std::filesystem::exists(folders))
				std::filesystem::create_directories(folders);
			std::ofstream stream(path);
			stream << data;
			readingPath = true;
			std::cout << "written " << path << "\n";
			std::cout << "folders " << folders << "\n";
			path = "";
			data = "";
			continue;
		}
		if (readingPath)
			path += c;
		else
			data += c;
	}
	std::cout << "done\n";
	grid->msg = "done";
}

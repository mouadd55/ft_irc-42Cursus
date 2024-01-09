/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moudrib <moudrib@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 11:59:22 by moudrib           #+#    #+#             */
/*   Updated: 2024/01/09 16:29:01 by moudrib          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <poll.h>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include "../../include/utils/utils.hpp"
#include "../../include/utils/colors.hpp"
#include "../../include/irc_server/server.hpp"

void	Server::parsePortNumberAndPassword( const std::string& s_port, const std::string& password )
{
	unsigned short		port;
	std::stringstream	portNumber(s_port);

	portNumber >> port;
	if (s_port[0] == ' ' || s_port[0] == '-' || !portNumber.eof() || portNumber.fail())
		throw std::invalid_argument(INVALID_PORT_NUMBER "\n  " VALID_PORT_NUMBER);
	this->port = port;
	if (password.length() == 0)
		throw std::invalid_argument(INVALID_PORT_NUMBER "\n  " VALID_PORT_NUMBER);
	this->serverPassword = password;
}


void	Server::sendRegistrationMessages( int clientSocket )
{
	time_t now = time(0);
	char* date_time = ctime(&now);
	std::string	date(date_time, strlen(date_time) - 1);

	my_send(clientSocket, 1, " :Welcome ", " to the IRCServer !");
	my_send(clientSocket, 2, " :Your host is ", " , running version 1.0 !");
	my_send(clientSocket, 3, " :This server was created ", date);
	my_send(clientSocket, 4, " version 1.0", "");
	my_send(clientSocket, 5, " are supported by this server", "");
}

void	Server::setupServerSocket( void )
{
	this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->serverSocket == -1)
		throw std::runtime_error(SOCKET_CREATION);
	// std::cout << BOLD "Socket created\n";
	setNonBlocking(this->serverSocket);

	sockaddr_in	serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(this->port);
	
	if (bind(this->serverSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == -1)
	{
		close(this->serverSocket);
		throw std::runtime_error(SOCKET_BINDING);
	}
	// std::cout << BOLD "successfully binding the socket\n";
	if (listen(this->serverSocket, 1) == -1)
	{
		close(this->serverSocket);
		throw std::runtime_error(LISTENING_ERROR);
	}
}
void	Server::my_send( int clientSocket, int num
	, const std::string& part1, const std::string& part2 )
{
	std::string	hostname = ":IRCserver";
	std::string	nickname = this->clientStates[clientSocket].nickname;
	std::string	numeric =	(num == 1) ? " 001 " + nickname + part1 + nickname + part2:
							(num == 2) ? " 002 " + nickname + part1 + hostname.substr(1, 10) + part2:
							(num == 3) ? " 003 " + nickname + part1 + part2:
							(num == 4) ? " 004 " + nickname + " " + hostname + part1:
							(num == 5) ? " 005 " + nickname + part1: "";
	std::string	endPart = "\r\n";
	std::string	fullMessage = hostname + numeric + endPart;
	send(clientSocket, fullMessage.c_str(), fullMessage.length(), 0);
}

void Server::send_message(const std::string& msge, int clientSocket)
{
	std::cout << msge << std::endl;
	std::string channel = removeMsgCommand(msge);
	if (channel.length() == 0)
		return ;
	std::cout << "channel: " << channel << std::endl;
	std::string message = msge.substr(msge.find(channel) + channel.length() + 1);
	if (message.length() == 0)
		return ;
	for (size_t i = 0; i < this->fds.size(); i++)
	{
		std::cout << "client name: " << this->clientStates[this->fds[i].fd].nickname << std::endl;
		if (this->clientStates[this->fds[i].fd].nickname == channel)
		{
			std::string msg = ":" + this->clientStates[clientSocket].nickname + "!" + this->clientStates[clientSocket].username + "@" + this->clientStates[clientSocket].hostname + " PRIVMSG " + channel + " :" + message + "\r\n";
			send(this->fds[i].fd, msg.c_str(), msg.length(), 0);
			return ;
		}
	}
	sendwrongUserMessage(clientSocket, channel);
	return ;
}

size_t countChanels(const std::string& channels)
{
	int chanelnum = 0;
	for (size_t i = 0; i < channels.length(); i++)
	{
		if (channels[i] == ',')
			chanelnum++;
	}
	if (channels[channels.length() - 1] != ',')
		chanelnum++;
	return chanelnum;
}
std::vector<std::string> split(std::string s, char del)
{
	std::vector<std::string> res;
	std::string tmp;
	for (size_t i = 0; i < s.length(); i++)
	{
		if (s[i] == del)
		{
			res.push_back(tmp);
			tmp = "";
		}
		else
			tmp += s[i];
	}
	if (tmp.length() != 0)
		res.push_back(tmp);
	return res;
}

void Server::handelJoinchannel(const std::string& msge, int clientSocket)
{
	std::string channelsstr = removeMsgCommand(msge);
	if (channelsstr.length() == 0)
		return ;
	std::string passwords = msge.substr(msge.find(channelsstr) + channelsstr.length() + 1);
	std::cout << "passwords: " << passwords << std::endl;
	std::vector<std::string> chanel;
	std::vector<std::string> pass;
	chanel = split(channelsstr, ',');
	if (chanel.size() != 0)
		pass = split(passwords, ',');
	for (int i = 0; i < (int)chanel.size(); i++)
	{
		std::cout<<"- " << chanel[i] << std::endl;
		if (!this->channels.count(chanel[i])) {
            std::vector<ClientState> user;
            user.push_back(this->clientStates[clientSocket]);

            if (!pass.empty() && i < static_cast<int>(pass.size())) {
                std::cout << "new channel: " << chanel[i] << " with pass " << pass[i] << std::endl;
                Channels newChannel(ADMIN, clientSocket, chanel[i], "", pass[i], "", 100, user);
                this->channels.insert(std::pair<std::string, Channels>(chanel[i], newChannel));
            } else {
                std::cout << "new channel no pass " << std::endl;
                Channels newchanel(ADMIN, clientSocket, chanel[i], "", "", "", 100, user);
                this->channels.insert(std::pair<std::string, Channels>(chanel[i], newchanel));
            }
        }
		else
		{
			if (channels[chanel[i]].getPassMode())
			{
				std::cout << "password need" << std::endl;
			}
			else{
				std::cout << "no password need" << std::endl;
				std::vector<ClientState> user;
				user.push_back(this->clientStates[clientSocket]);
				this->channels[chanel[i]].setChannelClients(clientSocket, user);	
			}
				
		}
	}
	for (int i = 0; i < (int)chanel.size(); i++)
	{
		std::cout << "has pass word "<< (bool)this->channels[chanel[i]].getPassMode() << std::endl;
	}
	std::cout << "im here" << std::endl;
	return ;
}

void Server::handleClientCommunication(size_t clientIndex)
{
	size_t 	end;
	char	buffer[BUFFER_SIZE];
	int		recvBytes = recv(this->fds[clientIndex].fd, buffer, sizeof(buffer), 0);

	if (recvBytes <= 0)
	{
		this->clientStates.erase(this->fds[clientIndex].fd); // erase client from the map
		this->clientBuffers.erase(this->fds[clientIndex].fd);
		close(this->fds[clientIndex].fd);
		this->fds.erase(this->fds.begin() + clientIndex);
		if (recvBytes == 0)
			std::cout << BOLD FG_RED "➖ Client disconnected\n" FG_DEFAULT;
		else
			perror("recv");
		return ;
	}
	this->clientBuffers[this->fds[clientIndex].fd].append(buffer, recvBytes);
	if ((end = this->clientBuffers[this->fds[clientIndex].fd].find('\n')) != std::string::npos)
	{
		std::string	completeMessage = this->clientBuffers[this->fds[clientIndex].fd].substr(0, end);
		if (completeMessage[0] == '\n')
			return ;
		std::string	command = getCommand(this->fds[clientIndex].fd, completeMessage);
		// fprintf(stderr, "|command: %s|\n", command.c_str());
		if (command.empty())
		{
			this->clientBuffers[this->fds[clientIndex].fd].clear();
			return ;
		}
		std::string parameters = getParameters(this->fds[clientIndex].fd, command, completeMessage);
		// fprintf(stderr, "|parameter: %s|\n", parameters.c_str());
		if (parameters.empty())
		{
			this->clientBuffers[this->fds[clientIndex].fd].clear();
			return ;
		}
		if (!isClientFullyAuthenticated(this->fds[clientIndex].fd))
			authenticateClient(this->fds[clientIndex].fd, command, parameters);
		else
			processAuthenticatedClientCommand(this->fds[clientIndex].fd, command, parameters);
		this->clientBuffers[this->fds[clientIndex].fd].clear();
	}
}

void	Server::connectionRegistration( int clientSocket, const std::string& command )
{
	std::string	registrationMsg = ":IRCServer 462 " + command + " :You have not registered\r\n";
	if ((command == "NICK" && !this->clientStates[clientSocket].isAuthenticated)
		|| (command != "PASS" && command != "NICK" && command != "USER"
		&& !isClientFullyAuthenticated(clientSocket)))
		send(clientSocket, registrationMsg.c_str(), registrationMsg.length(), 0);
}


void	Server::authenticateClient( int clientSocket, std::string& command, const std::string& parameters  )
{
	ClientState	state;

	if (command.length() != 0)
		connectionRegistration(clientSocket, command);
	this->clientStates.insert(std::pair<int, ClientState>(clientSocket, state));
	this->clientBuffers.insert(std::pair<int, std::string>(clientSocket, ""));
	if (!this->clientStates[clientSocket].isAuthenticated)
		handlePassCommand(clientSocket, command, parameters);
	if (!this->clientStates[clientSocket].hasNick)
		handleNickCommand(clientSocket, command, parameters);
	if (!this->clientStates[clientSocket].hasUser)
		handleUserCommand(clientSocket, command, parameters);
	if (isClientFullyAuthenticated(clientSocket))
		sendRegistrationMessages(clientSocket);
}

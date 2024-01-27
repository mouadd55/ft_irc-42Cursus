/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moudrib <moudrib@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/28 10:32:29 by moudrib           #+#    #+#             */
/*   Updated: 2024/01/27 11:57:24 by moudrib          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <sstream>
#include <iostream>
#include <sys/socket.h>
#include "../../include/utils/utils.hpp"
#include "../../include/utils/colors.hpp"
#include "../../include/utils/replies.hpp"

bool setNonBlocking(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		return false;
	return true;
}

bool	validCommands( const std::string& command )
{
	std::string	commands[12] = {	"PASS",
									"NICK",
									"USER",
									"JOIN",
									"PONG",
									"KICK",
									"MODE",
									"QUIT",
									"TOPIC",
									"INVITE",
									"NOTICE",
									"PRIVMSG"};

	for (int i = 0; i < 12; i++)	
		if (commands[i] == command)
			return true;
	return false;
}

std::string	getCommand( int clientSocket, const std::string& message )
{
	std::string	command;
	std::stringstream	input(message);

	if (message.empty())
			return "";
	getline(input, command, ' ');
	int lastChar = command.length() - 1;
	command = (command[lastChar] == '\n') ? command.substr(0, lastChar) : command;
	for (size_t i = 0; i < command.length(); i++)
		command[i] = std::toupper(command[i]);
	if (!validCommands(command) || message[0] == ' ')
	{
		std::string	unkonwn = ":IRCServer 421 " + command + " :Unknown command\r\n";
		send(clientSocket, unkonwn.c_str(), unkonwn.length(), 0);
		return "";
	}
	return command;
}

std::string	getParameters( int clientSocket, const std::string& command, const std::string& message )
{
	if (command.empty())
		return "";
	int start = command.length() + 1;
	int flag = (message.find('\r', 0) != std::string::npos) ? 1 : 0;
	std::string	parameters;
	if (message[start])
		parameters = message.substr(start, message.length() - start - flag);
	if (((message[start - 1] != ' ' || parameters.length() == 0) && command != "NOTICE"))
		return (notEnoughParametersReply(clientSocket, command), "");
	return parameters;
}
// dyawli matemse7hoch fl merge
void sendwrongCommandMessage(int clientSocket)
{
	std::string wrongCommandMsg = FG_RED "   Wrong command\n" FG_DEFAULT;
	send(clientSocket, wrongCommandMsg.c_str(), wrongCommandMsg.length(), 0);
}

void noMessageToSend(int clientSocket)
{
	std::string wrongCommandMsg = ":IRCServer 412 :No text to send\r\n";
	send(clientSocket, wrongCommandMsg.c_str(), wrongCommandMsg.length(), 0);
}

std::string removeMsgCommand(const std::string& fullMessage) {
    std::istringstream iss(fullMessage);
    std::string command, channel, message;
    iss >> command >> channel;
    std::getline(iss, message);

    message.erase(0, message.find_first_not_of(" \t\n\r\f\v"));
    message.erase(message.find_first_not_of(" \t\n\r\f\v") + 1);
        return channel;
}

std::string Joinchannelpars(const std::string& msge)
{
	std::string channel = removeMsgCommand(msge);
	if (channel.length() == 0)
		return "";
	std::string message = msge.substr(msge.find(channel) + channel.length() + 1);
	if (message.length() == 0)
		return "";
	return channel;
}

bool	validNickname( int clientSocket, const std::string& nickname )
{
	size_t i = -1, length = nickname.length();
	while (++i < length && nickname[i] == ' ');
	if (i == length)
	{
		std::string	noNicknameMsg = ":IRCServer 431 NICK :No nickname given\r\n";
		send(clientSocket, noNicknameMsg.c_str(), noNicknameMsg.length(), 0);
		return false;
	}
	int	flag = isdigit(nickname[0]) ? 1 : 0;
	for (i = 0; i < nickname.length(); i++)
	{
		if ((!isalnum(nickname[i]) && nickname[i] != '_' && nickname[i] != '\n') || flag)
		{
			std::string	noNicknameMsg = ":IRCServer 432 NICK :Erroneus nickname\r\n";
			send(clientSocket, noNicknameMsg.c_str(), noNicknameMsg.length(), 0);
			return false;
		}
	}
	return true;
}

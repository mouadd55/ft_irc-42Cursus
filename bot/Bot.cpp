/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moudrib <moudrib@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 12:03:17 by moudrib           #+#    #+#             */
/*   Updated: 2024/01/19 11:05:40 by moudrib          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

void	Bot::botAuthentication()
{
	std::string	passCommand = "PASS " + this->serverPassword + "\n";
	send(this->botSocket, passCommand.c_str(), passCommand.length(), 0);
	std::string	nickCommand = "NICK " + this->nickname + "\n";
	send(this->botSocket, nickCommand.c_str(), nickCommand.length(), 0);
	std::string	userCommand = "USER IRCBot 0 * IRCBot\n";
	send(this->botSocket, userCommand.c_str(), userCommand.length(), 0);
}

void	Bot::connectToServer( void )
{
	this->botSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->botSocket == -1)
		throw std::runtime_error(SOCKET_CREATION);

	sockaddr_in	socketAddress;
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(this->port);
	socketAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (socketAddress.sin_addr.s_addr == INADDR_NONE)
		throw std::runtime_error(INVALID_IP);
	
	if (connect(this->botSocket, reinterpret_cast<sockaddr *>(&socketAddress), sizeof(socketAddress)) == -1)
	{
		close(this->botSocket);
		throw std::runtime_error(CONNECTION_FAILED);
	}
}

void	Bot::failedAuthenticationCheck( std::string& message )
{
	if (message.length() < 15)
		return ;
	if (message.substr(0, 15).find("464") != std::string::npos)
	{
		std::cerr << BOLD FG_RED "Error: " FG_DEFAULT "Password incorrect\n";
		exit(EXIT_FAILURE);
	}
	else if (message.substr(0, 15).find("433") != std::string::npos)
	{
		std::cerr << BOLD FG_RED "Error: " FG_DEFAULT "Nickname is already in use\n";
		exit(EXIT_FAILURE);
	}
	this->isAuthenticated = true;
	std::cout << BOLD "Try:   \"PRIVMSG <bot> <someone> <command>\"\n" FG_DEFAULT;
}

void	Bot::handleCommands( const std::string& message )
{
	size_t	pos;
	int flag = message.find('\r') != std::string::npos ? 2 : 1;
	if ((pos = message.find("!")) == std::string::npos)
		return ;
	this->sender = message.substr(1, pos - 1);
	if ((pos = message.find("PRIVMSG")) == std::string::npos)
		return ;
	if ((pos = message.find(":", pos)) == std::string::npos)
		return ;
	pos++;
	std::stringstream	parameters(message.substr(pos, message.length() - pos - flag));
	getline(parameters, this->receiver, ' ');
	if (this->receiver.length() == 0)
		return ;
	pos += this->receiver.length() + 1;
	if (message.length() <= pos)
		return ;
	this->command = message.substr(pos, message.length() - pos - flag);
	std::string	toSend;
	if (this->command == "usmell")
		toSend = "PRIVMSG " + this->receiver + " You have been pinged. Take a shower you stink\r\n";
	else if (this->command == "stoptalking")
		toSend = "PRIVMSG " + this->receiver + " You have been pinged. You are too noisy\r\n";
	else
		toSend = "PRIVMSG " + this->sender + " Try \"usmell\" or \"stoptalking\"\r\n";
	send(this->botSocket, toSend.c_str(), toSend.length(), 0);
}

void Bot::respondToMessages()
{
	char buffer[BUFFER_SIZE];

	while (true)
	{
		size_t recvBytes = recv(this->botSocket, buffer, sizeof(buffer), 0);
		if (recvBytes <= 0)
			exit(EXIT_FAILURE);
		std::string	message(buffer, recvBytes);
		if (!this->isAuthenticated)
			failedAuthenticationCheck(message);
		handleCommands(message);
	}
}

bool	validNickname( const std::string& nickname )
{
	size_t i = -1, length = nickname.length();
	while (++i < length && nickname[i] == ' ');
	if (i == length)
		return false;
	for (i = 0; i < nickname.length(); i++)
		if (!isalnum(nickname[i]) && nickname[i] != '_' && nickname[i] != '\n')
			return false;
	return true;
}

void	Bot::parseArguments( const std::string& s_port, const std::string& password, const std::string& nickname )
{
	unsigned short		port;
	std::stringstream	portNumber(s_port);

	portNumber >> port;
	if (s_port[0] == ' ' || s_port[0] == '-' || !portNumber.eof() || portNumber.fail() || port < 1024)
		throw std::invalid_argument(INVALID_PORT_NUMBER "\n  " VALID_PORT_NUMBER);
	this->port = port;
	if (password.length() == 0)
		throw std::invalid_argument(INVALID_PASSWORD);
	this->serverPassword = password;
	if (!validNickname(nickname))
		throw std::invalid_argument(INVALID_NICKNAME);
	this->nickname = nickname;
	this->server = "127.0.0.1";
	this->isAuthenticated = false;
}

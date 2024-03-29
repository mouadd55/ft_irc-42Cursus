 /* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiplexing.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moudrib <moudrib@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/27 21:23:25 by moudrib           #+#    #+#             */
/*   Updated: 2024/01/16 14:24:46 by moudrib          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "../../include/utils/utils.hpp"
#include "../../include/utils/colors.hpp"
#include "../../include/irc_server/server.hpp"

void	Server::initializePollStructure( int fd )
{
	struct pollfd	newSocket;

	newSocket.fd = fd;
	newSocket.events = POLLIN;
	this->fds.push_back(newSocket);
}

void	Server::acceptNewClient()
{
	int	newSocket;
	sockaddr_in	clientAddress;
	socklen_t	clientAddressSize = sizeof(clientAddress);

	newSocket = accept(this->serverSocket, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressSize);
	if (newSocket == -1)
		return ;
	initializePollStructure(newSocket);
	this->clientStates[newSocket].hostname = inet_ntoa(clientAddress.sin_addr);
	std::cout << BOLD FG_GREEN "➕ Client connected\n" FG_DEFAULT;
}

void	Server::runServerLoop()
{
	initializePollStructure(this->serverSocket);

	while (true)
	{
		int pollResult = poll(this->fds.data(), this->fds.size(), 0);
		if (pollResult == -1)
			throw std::runtime_error(POLL_FAILURE);
		for (size_t clientIndex = 0; clientIndex < this->fds.size(); clientIndex++)
		{
			if (this->fds[clientIndex].revents & POLLIN)
			{
				if (clientIndex == 0)
					acceptNewClient();
				else
				{
					this->fd = this->fds[clientIndex].fd;
					if (handleClientCommunication(clientIndex))
						if (this->clientBuffers.find(this->fd) != this->clientBuffers.end())
							this->clientBuffers[this->fd].clear();
				}
			}
		}
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channels.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbenidar <bbenidar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 15:58:24 by bbenidar          #+#    #+#             */
/*   Updated: 2024/02/01 15:33:32 by bbenidar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <arpa/inet.h>
#include "../../include/utils/utils.hpp"
#include "../../include/utils/colors.hpp"
#include "../../include/irc_server/server.hpp"

Channels::Channels(int usermode,int clientSocket,std::string name, std::string topic, std::string password, int limit, std::vector<ClientState> user)
{
	this->channelIsPrivate = false;
	this->channelTopicModeratorOnly = false;
	this->channelName = name;
	this->channelTopic = topic;
	this->channelPassword = password;
	this->channelUserLimit = limit;
	this->channelClients[clientSocket] = user;
	this->chanMode = "";
	std::time_t currentTime = std::time(0);
	this->topicTimeSetting = std::ctime(&currentTime);
	if (usermode == ADMIN)
		this->channelModerators[clientSocket] = user;
}

Channels::~Channels() {}

void Channels::setChannelName(const std::string& name)
{
	this->channelName = name;
}

void Channels::setChannelTopic(const std::string& topic)
{
	this->channelTopic = topic;
}

void Channels::setChannelPassword(const std::string& password)
{
	this->channelPassword = password;
}

void Channels::setChannelUserLimit(int limit)
{
	this->channelUserLimit = limit;
}

void Channels::setChannelMode(const std::string& mode, bool add)
{
	if (add)
		this->chanMode += (this->chanMode.find(mode) != std::string::npos) ? "" : mode;
	else
	{
		std::string::size_type i = this->chanMode.find(mode);
		if (i != std::string::npos)
			this->chanMode.erase(i, mode.length());
	}
}

std::string Channels::getChannelMode() const
{
	return this->chanMode;
}

void Channels::setChannelClients(int clientSocket, const std::vector<ClientState>& user)
{
	this->channelClients[clientSocket] = user;
}

void Channels::setChannelModerators(int clientSocket, const std::vector<ClientState>& user, std::string seter)
{
	std::string msg = ":" + seter + " MODE " + this->channelName + " +o " + user[0].nickname + "\r\n";
	this->sendBroadcastMessage(msg, clientSocket);
	this->channelModerators[clientSocket] = user;
}

bool Channels::getPassMode() const
{
	if (this->channelPassword.length() != 0)
		return true;
	return false;
}

bool Channels::getClientisInChannel(int clientSocket) const
{
	if (this->channelClients.count(clientSocket))
		return true;
	return false;
}

std::string	Channels::getChannelPassword() const
{
	return this->channelPassword;
}

int Channels::getChannelUserLimit() const
{
	return this->channelUserLimit;
}

int Channels::getChannelClientsSize() const
{
	return this->channelClients.size();
}

bool Channels::getifClientIsBanned(int clientSocket) const
{
	if (this->channelBannedClients.count(clientSocket))
		return true;
	return false;
}

std::map<int, std::vector<ClientState> > Channels::getChannelClients() const
{
	return this->channelClients;
}

bool Channels::getifClientIsInvited(int clientSocket) const
{
	std::map<int, std::vector<ClientState> >::const_iterator it = this->channelInvitedClients.find(clientSocket);
    return it != this->channelInvitedClients.end();
}

bool Channels::getifClientIsModerator(int clientSocket) const
{
	if (this->channelModerators.count(clientSocket))
		return true;
	return false;
}

std::string Channels::getChannelName() const
{
	return this->channelName;
}

void Channels::setChannelBannedClients(int clientSocket, const std::vector<ClientState>& user)
{
	this->channelBannedClients[clientSocket] = user;
}

void Channels::setChannelInvitedClients(int clientSocket, ClientState& user)
{
	this->channelInvitedClients[clientSocket].push_back(user);
}

void Channels::KickClient(int clientSocket)
{
	this->channelClients.erase(clientSocket);
	if (this->channelInvitedClients.count(clientSocket))
		this->channelInvitedClients.erase(clientSocket);
}
void Channels::setChannelprivateMode(bool mode)
{
	this->channelIsPrivate = mode;
}

bool Channels::getifChannelIsPrivate() const
{
	return this->channelIsPrivate;
}

std::string Channels::getChannelTopic() const
{
	return this->channelTopic;
}

void Channels::removeModerator(int clientSocket)
{
	this->channelModerators.erase(clientSocket);
}

void Channels::setChannelTopicModeratorOnly(bool mode)
{
	this->channelTopicModeratorOnly = mode;
}

bool Channels::getChannelTopicModeratorOnly() const
{
	return this->channelTopicModeratorOnly;
}

void Channels::sendBroadcastMessage(const std::string& message, int clientSocket)
{
	(void)clientSocket;
	std::map<int, std::vector<ClientState> >::iterator it;
	for (it = this->channelClients.begin(); it != this->channelClients.end(); it++)
			send(it->first, message.c_str(), message.length(), 0);
}

void Channels::setTopicTime(const std::string& time)
{
	this->topicTimeSetting = time;
}

std::string Channels::getTopicTime() const
{
	return this->topicTimeSetting;
}

std::string Channels::getChannelClientInOneString() const
{
	std::string res;
	std::map<int, std::vector<ClientState> >::const_iterator it = this->channelClients.begin();
	while (it != this->channelClients.end())
	{
		if (getifClientIsModerator(it->first))
			res += "@";
		res += it->second[0].nickname;
		res += " ";
		it++;
	}
	return res;
}


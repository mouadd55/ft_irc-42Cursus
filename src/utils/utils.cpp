/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moudrib <moudrib@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/28 10:32:29 by moudrib           #+#    #+#             */
/*   Updated: 2023/12/28 12:44:00 by moudrib          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include "../../include/utils/utils.hpp"
#include "../../include/utils/colors.hpp"

void	sendWelcomeMessage(int clientSocket)
{
	std::string welcomeMsg = BOLD FG_GREEN "Welcome to the IRC Server!\n" FG_WHITE;
	send(clientSocket, welcomeMsg.c_str(), welcomeMsg.length(), 0);
}

void	sendAuthenticationInstructions(int clientSocket)
{
	std::string authInstructions = FG_LGRAY
		"  Authentication Instructions:\n"
		"  1. PASS <server_password>: Set the server password.\n"
		"  2. NICK <nickname>: Set your nickname.\n"
		"  3. USER <username> <hostname> <servername> :<realname>: Set your user details.\n"
		"  Note: All three steps are required for successful authentication.\n" FG_DEFAULT;

	send(clientSocket, authInstructions.c_str(), authInstructions.length(), 0);
}
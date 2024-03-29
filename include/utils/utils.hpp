/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbenidar <bbenidar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/28 10:33:47 by moudrib           #+#    #+#             */
/*   Updated: 2024/02/01 15:31:13 by bbenidar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

# include <vector>
# include <string>
# include <map>
#include "../../include/utils/colors.hpp"
#include "../../include/irc_server/server.hpp"
# include <iostream>

struct ClientState;
class Server;

# define INVALID_ARGUMENTS		BOLD FG_RED "Error: " FG_WHITE "invalid arguments."
# define VALID_INPUT			"Try ./ft_irc + \"Port number\" + \"Password\""

# define INVALID_PORT_NUMBER	BOLD FG_RED "Error: " FG_WHITE "invalid port number."
# define VALID_PORT_NUMBER		"The port number must be between 1024 and 65535"

# define SOCKET_CREATION		BOLD FG_RED "Error: creating socket"

# define SOCKET_BINDING			BOLD FG_RED "Error: binding socket"

# define LISTENING_ERROR		BOLD FG_RED "Error: listening for connections"

# define ACCEPT_CONNECTION		BOLD FG_RED "Error: accepting connection"

# define POLL_FAILURE			BOLD FG_RED "Error: poll() failed"

# define SETSOCKOPT				BOLD FG_RED "Error: setsockopt() failed"

bool		setNonBlocking(int fd);
void		noMessageToSend(int clientSocket);
bool		validCommands( const std::string& command );
std::vector<std::string> split(std::string s, char del);
std::string	removeMsgCommand(const std::string& fullMessage);
std::vector<std::string> splitIntoPairs(const std::string& input);
std::string	getCommand( int clientSocket, const std::string& message );
bool		validNickname( int clientSocket, const std::string& nickname );
std::string	getParameters( int clientSocket, const std::string& command, const std::string& message );
void printChanneljoin(std::map<int, ClientState>& clientStates, std::string Topic , int clientSocket, const std::string& channel ,std::string chanelClient, std::string mode);

#endif
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moudrib <moudrib@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 11:57:49 by moudrib           #+#    #+#             */
/*   Updated: 2024/01/07 16:31:23 by moudrib          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

# include <map>
# include <poll.h>
# include <vector>
# include <iostream>

# define BUFFER_SIZE 4096

struct ClientState {
	bool isAuthenticated;
	bool hasNick;
	bool hasUser;
	std::string nickname;
	std::string username;
	std::string realname;
	std::string hostname;
	std::string servername;

	ClientState() : isAuthenticated(false), hasNick(false), hasUser(false) {}
};


class Server
{

private:
	int				serverSocket;
	std::string		serverPassword;
	unsigned short	port;
	std::vector<struct pollfd>	fds;
	std::map<int, ClientState>	clientStates;	

public:

	void	passCommand( int clientSocket );
	void	changeClientNickname( int clientSocket, const std::string& parameters );

	void	runServerLoop( void );
	void	acceptNewClient( void );

	void	setupServerSocket( void );
	void	initializePollStructure( int fd );
	bool	isClientFullyAuthenticated( int clientSocket );
	void	handleClientCommunication( size_t clientIndex );
	void	parsePortNumberAndPassword( const std::string& s_port, const std::string& serverPassword );

	bool	isNicknameAvailable( const std::string& nickname );
	bool	handlePassCommand( int clientSocket, std::string& command, const std::string& parameters );
	bool	handleNickCommand( int clientSocket, std::string& command, const std::string& parameters );
	bool	handleUserCommand( int clientSocket, std::string& command, const std::string& parameters );
	void	authenticateClient( int clientSocket, std::string& command, const std::string& parameters );

	void	sendRegistrationMessages( int clientSocket );
	void	my_send( int clientSocket, int num, const std::string& part1, const std::string& part2 );
	void	connectionRegistration( int clientSocket, const std::string& command );
	void	processAuthenticatedClientCommand(int clientSocket, const std::string& command, const std::string& );

};

#endif
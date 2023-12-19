/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moudrib <moudrib@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/18 16:09:14 by moudrib           #+#    #+#             */
/*   Updated: 2023/12/19 13:03:04 by moudrib          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "../include/utils/colors.hpp"
#include "../include/irc_server/server.hpp"

int main( int argc, char **argv )
{
	Server	server;
	try
	{
		if (argc != 3)
			throw std::invalid_argument(INVALID_ARGUMENTS "\n   " VALID_INPUT);
		std::string	input = argv[1];
		server.parsePortNumber(input);
	}
	catch (const std::logic_error& e)
	{
		std::cout << e.what() << std::endl;
	}
}

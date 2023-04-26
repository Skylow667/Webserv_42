#include "../inc/Server.hpp"
#include "../inc/parsing.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

void	handle_sigint(int signum)
{
	(void)signum;
	if (Server::_running == true)
		Server::_running = false;
}

int main (int ac, char *av[])
{
	if (ac != 2) {
		std::cerr << "Error: Correct format is ./webserv [configuration file](optionnal)\n";
		return EXIT_FAILURE;
	}
	string file = av[1];
	data servers;
	try {
		pars_conf(file, servers);	
		pars_struct(servers);
	}catch (std::exception & e) {
		cout << e.what() << endl;
		return (EXIT_FAILURE);
	}
  	Server server(servers);
  	server.setSocket();
  	server.runServer();
  	return 0;
}
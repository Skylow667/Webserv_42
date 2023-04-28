#include "../../inc/parsing.hpp"
#include <dirent.h>

bool find_bracket(vector<string> & tab) {
	for (unsigned long i = 0; i < tab.size(); i++) {
		for (unsigned long j = 0; j < tab[i].size(); j++) {
			if (tab[i][j] == '{' || tab[i][j] == '}')
				return true;
		}
	}
	return false;
}

vector<string> mysplit(string & line, string delimiter) {
	string str;
	vector<string> tab;
	for (unsigned long i = 0; i < line.size(); i++) {
		while (line[i] && strchr(delimiter.c_str(), line[i]))
			i++;
		while (line[i] && !strchr(delimiter.c_str(), line[i])) {
			str += line[i];
			i++;
		}
		tab.push_back(str);
		str.clear();
	}
	// for (unsigned long i = 0; i < tab.size(); i++)
		// cout << "'" << tab[i] << "'" << " ";
	// cout << endl;
	return (tab);
}

void recursive_location(fstream & file, block_serv & servers, vector<string> options, int stage) {
	string line;
	if (!getline(file, line))
		throw (logic_error("Error: need a bracket after location block!"));
	vector<string> tab = mysplit(line, DELIMITER);
	if (tab.size() != 1 || tab[0] != "{")
		throw (logic_error("Error: need a bracket after location block!"));
	block_location location;
	servers.serv.push_back(location);
	int index = servers.serv.size() - 1;
	servers.serv[index].stage = stage;
	vector<string>::iterator it;
	while (getline(file, line)) {
		if (!line.empty()) {
			tab = mysplit(line, DELIMITER);
			it = find(options.begin(), options.end(), tab[0]);
		}
		if (line.empty())
			continue ;
		else if (tab.size() == 1 && tab[0] == "}")
			return ;
		else if (find_bracket(tab))
			throw (logic_error("Error: a bracket are a bad position in file!"));
		else if (tab.size() == 2 && tab[0] == "location") {
			servers.serv[index].path = tab[1];
			recursive_location(file, servers, options, stage + 1);
		}
		else if (it != options.end()) {
			if (tab.size() < 2)
				throw (logic_error("Error: " + tab[0] + " need an argument!"));
			else if (tab[0] == "errpage" && tab.size() != 3)
				throw (logic_error("Error: errpage need two arguments!"));
			else if (tab[0] == "autoindex" && tab.size() != 2 && tab[1] != "on" && tab[1] != "off")
				throw (logic_error("Error: directory_listing need arguments! (only on and off)"));
			else if (tab[0] == "cgi" && tab.size() != 3)
				throw (logic_error("Error: cgi need two arguments! (using: cgi [path_to_interpreter] [extension])"));
			else if (tab[0] == "limit_client_body_size" && tab.size() != 2)
				throw (logic_error("Error: body size take only one arguments!"));
			else if (tab[0] == "redirect" && tab.size() != 2)
				throw (logic_error("Error: redirect take only one arguments!"));
			for (unsigned long i = 1; i < tab.size(); i++)
				servers.serv[index].conf.insert(make_pair(tab[0], tab[i]));
		} 
		else
			throw (logic_error("Error: bad input!"));
	}
	throw (logic_error("Error: need a close bracket at the end of the server!"));
}

void pars_line(fstream & file, data & servers, vector<string> options) {
	string line;
	if (!getline(file, line))
		throw (logic_error("Error: need a bracket after server block!"));
	vector<string> tab = mysplit(line, DELIMITER);
	if (tab.size() != 1 || tab[0] != "{")
		throw (logic_error("Error: need a bracket after server block!"));
	block_serv serv;
	servers.serv.push_back(serv);
	int index = servers.serv.size() - 1;
	vector<string>::iterator it;
	while (getline(file, line)) {
		if (!line.empty()) {
			tab = mysplit(line, DELIMITER);
			it = find(options.begin(), options.end(), tab[0]);
		}
		if (line.empty())
			continue ;
		else if (tab.size() == 1 && tab[0] == "}")
			return ;
		else if (find_bracket(tab))
			throw (logic_error("Error: a bracket are a bad position in file!"));
		else if (tab[0] == "location") {
			if (tab.size() != 2)
				throw (logic_error("Error: location block need a path!"));
			recursive_location(file, servers.serv[index], options, 0);
		}
		else if (it != options.end()) {
			if (tab.size() < 2)
				throw (logic_error("Error: " + tab[0] + " need an argument!"));
			else if (tab[0] == "errpage" && tab.size() != 3)
				throw (logic_error("Error: errpage need two arguments!"));
			else if (tab[0] == "autoindex" && tab.size() != 2 && tab[1] != "on" && tab[1] != "off")
				throw (logic_error("Error: directory_listing need arguments! (only on and off)"));
			else if (tab[0] == "cgi" && tab.size() != 3)
				throw (logic_error("Error: cgi need two arguments! (using: cgi [path_to_interpreter] [extension])"));
			else if (tab[0] == "limit_client_body_size" && tab.size() != 2)
				throw (logic_error("Error: body size take only one arguments!"));
			else if (tab[0] == "redirect" && tab.size() != 2)
				throw (logic_error("Error: redirect take only one arguments!"));
			for (unsigned long i = 1; i < tab.size(); i++)
				servers.serv[index].conf.insert(make_pair(tab[0], tab[i]));
		} 
		else
			throw (logic_error("Error: bad input!"));
	}
	throw (logic_error("Error: need a close bracket at the end of the server!"));
}

void pars_conf(string & file, data & servers) {
	string opt[NB_OPT] = {"root", "index", "listen", "methods", "errpage", "server_names", "cgi", "autoindex", "redirect", "limit_client_body_size"};
	vector<string> options;
	DIR	*dir;

	for (int i = 0; i < NB_OPT; i++)
		options.push_back(opt[i]);
	fstream input;
	input.open(file.c_str(), ios::in);
	if (!input)
		throw (logic_error("Error: config file can't be open!"));
	dir = opendir(file.c_str());
	if (dir != NULL)
	{
		closedir(dir);
		throw (logic_error("Error: config file can't use a directory!"));
	}
	string line;
	while (getline(input, line)) {
		vector<string> tab = mysplit(line, DELIMITER);
		if (line.empty())
			continue ;
		else if (tab.size() == 1 && tab[0] == "server") 
			pars_line(input, servers, options);
		else
			throw (logic_error("Error: need a server block!"));
	}
}

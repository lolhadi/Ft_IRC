#include "ft_irc.hpp"

QuitCommand::QuitCommand(Server *server, bool authRequired) : Command(server, authRequired) {}

QuitCommand::~QuitCommand() {}

// quiting the server
void QuitCommand::execute(Client *client, std::vector<std::string> arguments) {
	// check for reasoning
	std::string reason = arguments.empty() ? "Leaving..." : arguments.at(0);
	reason = reason.at(0) == ':' ? reason.substr(1) : reason;
	client->write(RPL_QUIT(client->getPrefix(), reason));
	// this is the kill switch
	this->_server->delClient(client->getFD());
}

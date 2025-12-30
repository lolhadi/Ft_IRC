#include "ft_irc.hpp"

NickCommand::NickCommand(Server *server, bool auth) : Command(server, auth) {}

NickCommand::~NickCommand() {}

/*
	# e.g : when user send NICK lolhadi

	# Check if user only send NICK, no argument
		$ Send error

	# get the nickname: then serachers the whole server with that name
		$ if return a pointer (not NULL), means taken or in used

	# If every is fine set the nickname and welcome
*/
void NickCommand::execute(Client *client, std::vector<std::string> arguments)
{
	if (arguments.empty() || arguments[0].empty())
	{
		client->reply(ERR_NONICKNAMEGIVEN(client->getPrefix()));
		return;
	}

	std::string requestNick = arguments[0];

	if (_server->getClient(requestNick))
	{
		client->reply(ERR_NICKNAMEINUSE(client->getPrefix(), requestNick));
		return;
	}
	client->setNickname(requestNick);
	client->welcome();
}

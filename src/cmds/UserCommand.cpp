#include "ft_irc.hpp"

UserCommand::UserCommand(Server *server, bool auth) : Command(server, auth) {}

UserCommand::~UserCommand() {}

/*
	# Same with PASS: it check is registerd
		$ error if registed
		$ Once username is set up cannot change
*/
void UserCommand::execute(Client *client, std::vector<std::string> arguments) {

	if (client->isRegistered())
	{
		client->reply(ERR_ALREADYREGISTERED(client->getPrefix()));
		return;
	}

	/*
		# Argument size need to be 4 parts(Username. Mode , Unused, Realname)

		# Argument 0: is the username
		# Argument 3, Realname, .substr(1) remove the :
		then welcome
	*/
	if (arguments.size() < 4) {
		client->reply(ERR_NEEDMOREPARAMS(client->getPrefix(), "USER"));
		return;
	}
	client->setUsername(arguments[0]);
	client->setRealName(arguments[3].substr(1));
	client->welcome();
}

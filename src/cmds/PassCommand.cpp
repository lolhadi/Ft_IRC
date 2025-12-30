#include "ft_irc.hpp"

PassCommand::PassCommand(Server *server, bool auth) : Command(server, auth) {}

PassCommand::~PassCommand() {}

/*
	# When a user send : PASS mypassword(e.g)
*/
void PassCommand::execute(Client *client, std::vector<std::string> arguments)
{
	// Check if the current client is already registerd
		// if true, reply  ady registered
	if (client->isRegistered())
	{
		client->reply(ERR_ALREADYREGISTERED(client->getPrefix()));
		return;
	}

	// If after the PASS and no argument: error not enough parameter
	if (arguments.empty())
	{
		client->reply(ERR_NEEDMOREPARAMS(client->getPrefix(), "PASS"));
		return;
	}

	/*
		# e.g : ./ircserv 6667 mypassword
			# so mypassword will set my irc server

		# the substr (:) is just to see if the irc client send a :in the password
			# if yes move 1 character
			# if no continue

		# check the password, if mismatch return an error
	*/
	if (_server->getPassword() != arguments[0].substr(arguments[0][0] == ':' ? 1 : 0))
	{
		client->reply(ERR_PASSWDMISMATCH(client->getPrefix()));
		return;
	}

	// is all good set into true then welcome ()
	client->setCorrectPassword(true);
	client->welcome();
}

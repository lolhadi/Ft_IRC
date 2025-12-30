#include "ft_irc.hpp"

// Handle the Private Message to another user
PrivMsgCommand::PrivMsgCommand(Server *server) : Command(server) {};

PrivMsgCommand::~PrivMsgCommand() {};

// Example PRIVMSG #test : Hello world
void PrivMsgCommand::execute(Client *client, std::vector<std::string> arguments) {

	// check if the arguemnt is empty, so need enough paramater
	if (arguments.size() < 2 || arguments[0].empty() || arguments[1].empty()) {
		client->reply(ERR_NEEDMOREPARAMS(client->getNickName(), "PRIVMSG"));
		return;
	}
	// so the argumentsat 0 is #test
	std::string target = arguments.at(0);
	std::string message;

	// append what is the message with spaces
	for (std::vector<std::string>::iterator it = arguments.begin() + 1; it != arguments.end(); it++) {
		message.append(*it + " ");
	}
	// remove the : if got
	message = message.at(0) == ':' ? message.substr(1) : message;

	// this where the if the target.0 is # means it a channel
	if (target.at(0) == '#') {

		// get the channel list and iterate
		std::vector<Channel *> client_chans = client->getUserChans();
		std::vector<Channel *>:: iterator it = client_chans.begin();

		Channel *chan;
		std::string chan_name;
		// go find the correct channel
		while (it != client_chans.end())
		{
			chan = it.operator*();
			if (chan->getName() == target)
				break ;
			++it;
		}
		// if until the end did no find the channel then error
		if (it == client_chans.end())
		{
			_server->broadcast(ERR_NOTONCHANNEL(client->getNickName(), target));
			return;
		}
		// Broadcast the message
		chan->broadcast(RPL_PRIVMSG(client->getPrefix(), target, message), client);
		return;
	}
	// this Send the private message to the user
	Client *dest = _server->getClient(target);
	if (!dest)
	{	// error no user
		client->reply(ERR_NOSUCHNICK(client->getNickName(), target));
		return;
	}
	// Write the private message to the correct user
	dest->write(RPL_PRIVMSG(client->getPrefix(), target, message));
}

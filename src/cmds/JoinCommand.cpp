#include "ft_irc.hpp"

JoinCommand::JoinCommand(Server *server) : Command(server) {}

JoinCommand::~JoinCommand() {}

// format : JOIN <channel>{,<channel>} [<key>{,<key>}]
void JoinCommand::execute(Client *client, std::vector<std::string> arguments)
{
	// If the /join is enter with no param error empty
	if (arguments.empty())
	{
		client->reply(ERR_NEEDMOREPARAMS(client->getNickName(), "JOIN"));
		return;
	}

	// nama is the room name, Password is optional for private room
	std::string name = arguments[0];
	std::string password = arguments.size() > 1 ? arguments[1] : "";

	// Getting the channel from the SERVER
	Channel *channel = _server->getChannel(name);
	if (!channel) // the channel dont exist create the channel
		channel = _server->createChannel(name, password, client);

	if (channel->invitOnlyChan())
	{
		client->reply(ERR_INVITEONLYCHAN(client->getNickName(), channel->getName()));
		return ;
	}

	// check already in chan
	std::vector<Client *> clients = channel->getChanClients();
	std::vector<Client *>::iterator it;

	it = clients.begin();
	while (it != clients.end())
	{
		Client *cl = it.operator*();
		if (cl == client)
			return ;
		it++;
	}

	if (channel->getMaxUsers() > 0 && channel->getNbrClients() >= channel->getMaxUsers())
	{
		client->reply(ERR_CHANNELISFULL(client->getNickName(), name));
		return;
	}

	if (channel->getPassword() != password)
	{
		client->reply(ERR_BADCHANNELKEY(client->getNickName(), name));
		return;
	}

	if (channel->invitOnlyChan() == 1)
	{
		client->reply(ERR_INVITONLYCHAN(client->getNickName(), name));
		return;
	}
	// GO JOIN THE CHANNEL
	client->join(channel);
}

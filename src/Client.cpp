#include "ft_irc.hpp"
#include "rpl.hpp"

Client::Client(Server *server, int fd, std::string const &hostname, int port)
					: _fd(fd), _hostname(hostname), _port(port), _correct_password(false), _server(server){}
Client::~Client() {}



void	Client::write(const std::string &message) const
{
	this->_server->send(message, this->getFD());
}

std::string Client::getPrefix() const
{
	if (this->getNickName().empty())
		return ("*");
	return _nickname + (_username.empty() ? "" : "!" + _username) + (_hostname.empty() ? "" : "@" + _hostname);
}

bool	Client::isRegistered() const
{
	if (!this->getNickName().empty() && !this->getUserName().empty() && !this->getRealName().empty() && this->_correct_password)
		return (true);
	return (false);
}

/*
	# So Everytime i want to send message to a client i used this function

	# So this tell the server to send message to the client
*/
void	Client::reply(const std::string &reply) {
	this->write(":" + this->_server->getServerName() + " " + reply);
}

void	Client::join(Channel *chan)
{
	// We tell the channel: to add the client(GUY) into the list
	chan->addClient(this);
	// tell the Client: add this channel to your list of joined rooms
	_user_chans.push_back(chan);
	// so if the 1st person to join got to be the king
	if (chan->getNbrClients() == 1)
	{
		chan->setAdmin(this);
		chan->addOper(this);
	}

	/*
		** THIS LOOP CONCATENATES ALL CHANNEL NICKNAMES INTO A SINGLE SPACE-SEPARATED STRING FOR IRC
			REPLIES LIKE NAMES
		# Create an empty string

		# std::vector<std::string> nicknames = chan->getNickNames();
			# This tell the channel to give all the nickname inside your list(vector)

		# for (std::vector<std::string>::iterator it = nicknames.begin(); it != nicknames.end(); it++)
		users.append(*it + " ");
			# so iterate from the start until the end
			# then for every iteration (*it)nickname add a space after
	*/
	std::string users;
	std::vector<std::string> nicknames = chan->getNickNames();
	for (std::vector<std::string>::iterator it = nicknames.begin(); it != nicknames.end(); it++)
		users.append(*it + " ");
	// Annouce to everyone that someone joind the channel
	chan->broadcast(RPL_JOIN(getPrefix(), chan->getName()));

	reply(RPL_NOTOPIC(this->getNickName(), chan->getName()));
	reply(RPL_NAMREPLY(this->getNickName(), chan->getName(), users));
	reply(RPL_ENDOFNAMES(this->getNickName(), chan->getName()));
}

void 	Client::leave(Channel *chan, int kicked, std::string &reason)
{
	// if the user in channel is not empty means erase the user
	if (!_user_chans.empty())
		_user_chans.erase(this->_user_chans.begin() + this->_channelIndex(chan));
	if (!kicked) // check if the user is kick in the channel then kick
		chan->removeClient(this, reason);
}

/*
	# So every person that is connected into the server got it own object in memory:
		$ Their socket
		$ Nickname
		$ Username
		$ current status(which channel)
*/
void	Client::welcome()
{
	// Final gatekeeper
	if (!this->isRegistered())
		return ;

	// This line is requierd by IRC
	reply(RPL_WELCOME(this->getNickName(), this->getPrefix()));
	reply(RPL_YOURHOST(this->getNickName(), this->_server->getServerName(), "0.1"));
	reply(RPL_CREATED(this->getNickName(), this->_server->getStartTime()));
	reply(RPL_MYINFO(this->getNickName(), this->_server->getServerName(), "0.1", "aiorsw", "IObeiklmnopstv"));

	// TODO:  Can Change the Message
	reply("375 " + this->getNickName() + " :- " + this->_server->getServerName() + " Message of the day -");
	reply("372 " + this->getNickName() + " :- Welcome to our IRC server!");
	reply("376 " + this->getNickName() + " :End of MOTD command");
}

unsigned long	Client::_channelIndex(Channel *channel)
{
	unsigned long i = 0;
	std::vector<Channel *>::iterator it = this->_user_chans.begin();

	while (it != this->_user_chans.end())
	{
		if (*it == channel)
			return i;
		it++;
		i++;
	}
	return 0;
}

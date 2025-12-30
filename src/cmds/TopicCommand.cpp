#include "ft_irc.hpp"

TopicCommand::TopicCommand(Server *server) : Command(server) {}
TopicCommand::~TopicCommand() {}

void TopicCommand::execute(Client *client, std::vector<std::string> arguments)
{
    // 1. Basic Validation
    if (arguments.empty()) {
        client->reply(ERR_NEEDMOREPARAMS(client->getNickName(), "TOPIC"));
        return;
    }

    std::string channelName = arguments[0];
    Channel *chan = _server->getChannel(channelName);

    // 2. Does the channel exist?
    if (!chan) {
        client->reply(ERR_NOSUCHCHANNEL(client->getNickName(), channelName));
        return;
    }

    // 3. Are you even in the channel? (IRC rule: You usually must be inside to see/change topic)
    if (!chan->hasClient(client)) {
        client->reply(ERR_NOTONCHANNEL(client->getNickName(), channelName));
        return;
    }


    // MODE A: VIEW TOPIC (No topic provided)

    if (arguments.size() == 1)
    {
        if (chan->getTopic().empty())
            client->reply(RPL_NOTOPIC(client->getNickName(), channelName));
        else
            client->reply(RPL_TOPIC(client->getNickName(), channelName, chan->getTopic()));
        return;
    }


    // MODE B: CHANGE TOPIC (Topic provided)


    // 4. The Operator Security Check (The most important part!)
    // If the channel has "+t" mode (Topic Lock), ONLY Operators can change it.
    if (chan->isTopicProtected() && client != chan->getAdmin() && !chan->is_oper(client))
    {
        client->reply(ERR_CHANOPRIVSNEEDED(client->getNickName(), channelName));
        return;
    }

    // 5. Parse the new topic
    // Arguments might be: ["#test", ":New", "Topic", "Here"]
    // You need to join them back together.
    std::string newTopic = "";
    for (size_t i = 1; i < arguments.size(); i++)
        newTopic += arguments[i] + " ";

    // Remove the leading colon if it exists (":New" -> "New")
    if (!newTopic.empty() && newTopic[0] == ':')
        newTopic = newTopic.substr(1);

    // 6. Set and Broadcast
    chan->setTopic(newTopic);
    chan->broadcast(RPL_TOPIC(client->getPrefix(), channelName, newTopic));
}

#include "ft_irc.hpp"

/*
	* So When the server side Pass on the first word (e.g JOIN)
	* The CommandHandler pickup the word and Looks up in our Big Book
	* Then it should find JoinCommand and Goes there.

	* _command: thisis a std::map(container) think like a phonebook
		* KEY: The command name(string): JOIN
		* Value: The tool the handle it (Command object)
			# "new": used this so it sit in the memory (heap) ready to use over again
*/
CommandHandler::CommandHandler(Server *server) : _server(server)
{
	_commands["PASS"] = new PassCommand(_server, false);
	_commands["NICK"] = new NickCommand(_server, false);
	_commands["USER"] = new UserCommand(_server, false);
	_commands["QUIT"] = new QuitCommand(_server, false);
	_commands["PING"] = new PingCommand(_server);
	_commands["PONG"] = new PongCommand(_server);
	_commands["JOIN"] = new JoinCommand(_server);
	_commands["MODE"] = new ModeCommand(_server);
	_commands["PART"] = new PartCommand(_server);
	_commands["KICK"] = new KickCommand(_server);
	_commands["INVITE"] = new InvitCommand(_server);
	_commands["PRIVMSG"] = new PrivMsgCommand(_server);
	_commands["TOPIC"] = new TopicCommand(_server);
	// _commands["NOTICE"] = new NoticeCommand(_server);
	// _commands["WHO"] = new WhoCommand(_server);
	// _commands["LIST"] = new ListCommand(_server);
}

// Called for destructor
CommandHandler::~CommandHandler()
{
	for (std::map<std::string, Command *>::iterator it = _commands.begin(); it != _commands.end(); it++)
		delete it->second;
}
/*
	THE MAIN JOB (INVOKE: MIGHT CHANGE LATER)
	* This functions runs everytime a user send a message
		# Client *client: who send the message.
		# std::string &message: what did they says?
*/
void CommandHandler::invoke(Client *client, const std::string &message)
{
	//Preparing To Read
	std::stringstream ssMessage(message); // Convertin the basic string into char
	std::string syntax;	// temporary string variable to hold one line of text< getline

	// THE LOOP(HANDLING MULTIPLE COMMAND) separated by newline (\n) until nothing left
	while (std::getline(ssMessage, syntax))
	{
		/*
			# CLEANING THE TRASH (\r) -Carriage Return-
				# Usually internet and windows has a \r\n and linux use \n at the end.
				# getline usually remove the \n and left with \r
			* So this line checks the last characters a \r
				# YES: Cut if off(substr)
				# NO: Keep the string as is
		*/
		syntax = syntax.substr(0, syntax[syntax.length() - 1] == '\r' ? syntax.length() - 1 : syntax.length());

		/*
			# FINDING THE COMMAND NAME
				# syntax.find(' '): find the position of the first space
				# substr(0,..): slice the string start from 0 up to the space
				# store in the Command
		*/
		std::string name = syntax.substr(0, syntax.find(' '));

		// THE LOOKUP(TRY-CATCH)
		try
		{

			// Check inside or map container the command
				// if found: it gave the comannd object
				// if not: throw exception
			Command *command = _commands.at(name);

			/*
				* PARSING THE ARGUMENT

				# stringstream ss: will make a new stream, it will skip the
				# ss >> buf : read word by word and skips spaces.
				# then argument.push_back(buf): create a list(vector) of words
				 # e.g for "JOIN #test key" argument become "#test, key"
			*/
			std::vector<std::string> arguments; // Create an empty vector of string : to store all the argument
			std::string buf; // temporary storage while reading stringstream
			std::stringstream ss(syntax.substr(name.length(), syntax.length()));

			// read word by word (>>) the ss into buf and push to vector
			while (ss >> buf)
			{
				arguments.push_back(buf);
			}

			// SECURITY CHECK & EXECUTION
			/*
				* Most command requires to be logged. so it check authRequired is true
					and if client is registerd or not
					# if not kick with error not register with the nickname
			*/
			if (command->authRequired() && !client->isRegistered())
			{
				client->reply(ERR_NOTREGISTERED(client->getNickName()));
				return;
			}
			// IF EVERYTING IS GOOD EXECUTE THE COMMMAND
			command->execute(client, arguments);
		}

		// HANDLING UNKNOWN COMMAND
		// error of unknwon command and the client nickname with the command
		catch (const std::out_of_range &e)
		{
			// Usually IRC client send a  command "CAP"(Capabilities negotiation) auto when connecting
			// AVOID CLIENT SPAM ERROR
			if (name != "CAP")
				client->reply(ERR_UNKNOWNCOMMAND(client->getNickName(), name));
		}
	}
}

#include <chrono>
#include <iostream>
#include <thread>
#include <chrono>
#include "Client.h"
#include "Server.h"
#include "Message.h"
#include <regex>

using namespace std;

int main(int argc, char** argv)
{
  if (argc == 1)
  {
    std::cerr << "Wrong parameters. Usage server: app_name -s. Usage client: app_name IDENIFIER SUBSCRIBE_TYPE1 SUBSCRIBE_TYPE2 ... \n";
    return -1;
  }

  ConfigManager mng;

  try
  {
      mng.set_path(R"(etc/)");
      mng.load_config("config01");
  }
  catch(const std::exception& ex)
  {
      std::cerr << "Error. " + std::string(ex.what());
      return -1;
  }

  // As server
  if (argc == 2)
  {
    std::string mode(argv[1]);
    if ( mode == "-s")
    {
      try
      {
        MessageBroker::Server srv;
        srv.load_config(mng);

        std::thread server_thread([&srv]()
        {
          try
          {
            srv.start();
          }
          catch (const std::exception& ex)
          {
            std::cerr << "Error. " + std::string(ex.what());
          }
        });

        std::cin.ignore();

        srv.stop();
        server_thread.join();
      }
      catch (const std::exception& ex)
      {
        std::cerr << "Error. " + std::string(ex.what());
        return -1;
      }
    }
    else
      std::cerr << "Wrong usage. Start server: app_name -s";
    return 0;
  }
  // As client
  else
  {
    const std::string app_id(argv[1]);

    std::vector<std::string> subscribe_types;
    for (int idx = 2; idx < argc; ++idx)
    {
      subscribe_types.push_back(argv[idx]);
    }

    try
    {
      MessageBroker::Client cli;
      
      cli.load_config(mng);
      cli.connect();

      for (auto& sub_types : subscribe_types)
      {
        cli.register_handler(sub_types, [](const char* msg, const std::size_t len)
        {
          std::cout << std::string(msg, len) << endl;
        });
      }

      std::thread message_queue_thread([&cli, &app_id]()
      {
        try
        {
          cli.start_heartbeat(app_id);
          cli.start_message_processing();
          cli.disconnect();
        }
        catch (const std::exception& ex)
        {
          std::cerr << ex.what();
        }
      });

      std::regex filter(R"(\w+ *: *\w*)");

      while (true)
      {
        // Read commands
        std::string command;
        if (std::getline(std::cin, command))
        {
          if (command == "STOP")
            break;

          if (std::regex_match(command, filter))
          {
            auto offset = command.find(" ");
            while (offset != std::string::npos)
            {
              command.replace(offset, 1, "");
              offset = command.find(" ");
            }
            auto msg_type = command.substr(0, command.find(":"));
            auto msg_body = command.substr(command.find(":") + 1);

            cli.send_message(Message(msg_type, msg_body));
          }
          else
          {
            std::cerr << "Wrong message format. Must be - MessageType : [Message]";
            continue;
          }
        }
        else
        {
          break;
        }
      }
      cli.stop_heartbeat();
      cli.stop_message_processing();
      message_queue_thread.join();
      return 0;
    }
    catch (const std::exception& ex)
    {
      std::cerr << ex.what();
    }
  }
  return 0;
}

#include "Tintin_reporter.hpp"

Tintin_reporter::Tintin_reporter(void)
{
	std::cout << "Default Constructor reporter called" << std::endl;
}

void Tintin_reporter::logs(const std::string& message) {

  // Obtention de la date et de l'heure actuelles
  std::time_t now = std::time(0);
  //Formattage de la date en structure avec heure mois année... 
  std::tm* localTime = std::localtime(&now);
  
  // impression de la date formatté dans mon buffer 
  char buffer[80];
  strftime(buffer, sizeof(buffer), "[%d/%m/%Y-%H:%M:%S]", localTime);

 if (!fileExists("/var/log/matt_daemon")) {
     createLogFile("/var/log/matt_daemon");
 }

	std::cout << "TEST" << std::endl;	
}

bool Tintin_reporter::fileExists(const char* filename) {
    // creating an ifstream object named file
    ifstream file;

    // opening the file
    file.open(filename);
    if (file)
    {
        file.close()
        return (true)
    }
    else
    {
        return (false)
    }
}

Tintin_reporter::~Tintin_reporter()
{
	std::cout << "Destructor Tintin Reporter called" << std::endl;
}

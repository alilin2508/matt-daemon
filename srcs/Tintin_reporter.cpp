#include "Tintin_reporter.hpp"


Tintin_reporter::Tintin_reporter(void){
	std::cout << "Default Constructor reporter called" << std::endl;
}

void Tintin_reporter::logs(const std::string& message, const char* mode) {

  // Obtention de la date et de l'heure actuelles
  std::time_t now = std::time(0);
  //Formattage de la date en structure avec heure mois année... 
  std::tm* localTime = std::localtime(&now);
  
  // impression de la date formatté dans mon buffer 
  char buffer[80];
  strftime(buffer, sizeof(buffer), "[%d/%m/%Y-%H:%M:%S]", localTime);

	printFile(message, buffer, mode);
 
}

void Tintin_reporter::openOrCreate(const std::string& filename) {
	streamLogFile.open(filename, std::ios::out | std::ios::app);
}

void Tintin_reporter::closeStream() {
	streamLogFile.close();
}

void Tintin_reporter::printFile(const std::string& message, const char* formatTime, const char* mode) {
	std::cout << "printing in file" << std::endl;
	streamLogFile << formatTime << " [ "<< mode << " ] - Matt_daemon : " << message << std::endl;
}

Tintin_reporter::~Tintin_reporter(){
	std::cout << "Destructor Tintin Reporter called" << std::endl;
}

#include "ThermManager.h"

/* default constructor */
ThermManager::ThermManager(std::shared_ptr<SynchronisedFile> CpuFile) { 

  /* get the cpu run file shared pointer */
  this->CpuFile = CpuFile;
}

/* get the temperature */
TemperatureAcq * ThermManager::GetTemperature() {

  /* define command to read temperature from all thermistors on COM port 1 */
  const char * cmd = "digitemp -s /dev/ttyS0 -a";

  /* run the digitemp command */
  std::string output = CpuTools::CommandToStr(cmd);
  
  /* parse the output */
  ParseDigitempOutput(output);
  
  /* make the results struct */
  TemperatureAcq * temperature_result = NULL;
  
  return temperature_result;
}


/* parse the digitemp output */
float ThermManager::ParseDigitempOutput(std::string input_string) {

  std::regex num_with_two_dp("([0-9]+\\.[0-9]{2}) ([0-9]+\\.[0-9]{2})");
  std::smatch match;
  float val;

  /* search for numbers with 2 decimal places */
  std::string::const_iterator searchStart(input_string.cbegin());
  while (std::regex_search(searchStart, input_string.cend(), match, num_with_two_dp)) {
      std::cout << (searchStart == input_string.cbegin() ? "" : " " ) << match[0];
      val = std::stof(match[0]);
	searchStart += match.position() + match.length();
    }
  
    /*
  std::regex_search(input_string, match, num_with_two_dp); 
  for(auto v: match) {
    std::cout << v << std::endl;
    val = std::stof(v);
    std::cout << val << std::endl;
  }
    */
    
  return val;
}
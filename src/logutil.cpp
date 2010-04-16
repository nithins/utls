#include <logutil.h>

#include <ctime>

std::string get_cur_time()
{
  std::time_t rawtime;
  struct std::tm * timeinfo;
  char buffer [80];

  std::time ( &rawtime );
  timeinfo = std::localtime ( &rawtime );

  std::strftime (buffer,80,"%H:%M:%S",timeinfo);

  return std::string(buffer);
}

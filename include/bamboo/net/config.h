#ifndef __BAMBOO_NET_CONFIG__
#define __BAMBOO_NET_CONFIG__

#include <vector>
#include <string>
#include <cassert>
#include "../logger.h"
#include <boost/shared_ptr.hpp>

namespace bamboo {

typedef std::string InetAddr;

class Config {
public:
  int get_id() const;
  void set_id(int id);

  int get_num_of_processes() const;
  void set_num_of_processes(int num_of_processes);

  typedef std::vector<unsigned short> Ports;
  
  const Ports& get_ports() const;
  void set_ports(const Ports& ports);

  typedef std::vector<InetAddr> Addresses;

  const Addresses& get_addresses() const;
  void set_addresses(const Addresses& addresses);

  boost::shared_ptr<Logger> get_logger() const;
  void set_logger(boost::shared_ptr<Logger> logger);

private:
  int id_;
  int num_of_processes_;
  Ports ports_;
  Addresses addresses_;
  boost::shared_ptr<Logger> logger_;
};

}

#endif

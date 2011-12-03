#include <stdexcept>

#include <boost/function.hpp>
#include <boost/any.hpp>

#include <n_vector.h>

class configurable_t
{
public:

  enum eFieldType {EFT_DATA_RO,EFT_DATA_RW,EFT_ACTION};

  typedef n_vector_t<int,2> data_index_t;
  typedef boost::function<void(void)> action_callback_t;

  virtual data_index_t dim() = 0 ;

  virtual bool exchange_field(const data_index_t &,boost::any &)  = 0;

  virtual eFieldType exchange_header(const int &,boost::any &) = 0;

  static bool s_exchange_action
      (const action_callback_t &p_val,boost::any &c_val)
  {
    if(c_val.empty())
      c_val = boost::any(p_val);
    else
      throw std::logic_error("action should be called when interacted with");

    return false;
  }

  template <typename T>
      static bool s_exchange_data_rw(T &p_val,boost::any &c_val)
  {
    bool ret = false;

    if(c_val.empty())
      c_val = boost::any(p_val);
    else
    {
      ret   = (p_val != boost::any_cast<T>(c_val));
      p_val = boost::any_cast<T>(c_val);
    }
    return ret;
  }

  template <typename T>
      static bool s_exchange_data_ro(const T &p_val,boost::any &c_val)
  {
    if(c_val.empty())
      c_val = boost::any(p_val);
    else
      throw std::logic_error("read only property cannot write");

    return false;
  }
};

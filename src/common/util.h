#ifndef IB_COMMON_UTIL_H
#define IB_COMMON_UTIL_H

#include <vector>
#include <string>
#include <sstream>

namespace ib
{
namespace common
{
using namespace std;

vector<string> split(const std::string &str, const std::string &separator);

int encrypt_string(const char *string, int buflen, char*& encstring);
int decrypt_string(const char *string, int length, char*& decstring);
string EncryptPswd(string& sPassword);

string now();
string get_time();
string get_date();
//uint32_t timestamp();
string get_app_root();
wstring to_wstr(const string &str);
string to_str(const wstring &str);

//@ip 网络序
string format_ip_addr( unsigned long ip );


template <typename T>
  string NumberToString ( T Number )
  {
     ostringstream ss;
     ss << Number;
     return ss.str();
  }

template <typename T>
  T StringToNumber ( const string &Text )
  {
     istringstream ss(Text);
     T result;
     return ss >> result ? result : 0;
  }

  class splitor
  {
  public:
      splitor(const std::string &str, const std::string &separator)
          :val_list_(split(str, separator))
      {};
      string operator[](size_t index)
      {
          if (index >= val_list_.size())
          {
              return "";
          }
          else
          {
              return val_list_[index];
          }
      };
      size_t size()
      {
          return val_list_.size();
      }
  private:
      vector<string> val_list_;
  };

  string get_file_name(string path);
  const char* get_file_name(const char * path);

  void to_upper(string & str);
  void to_lower(string & str);
  bool equal_ignore_case(const string & str1, const string & str2);

  bool is_mobile_phone_num(string &phone_number);
  bool is_telephone_num(string &phone_number);
  bool is_IDD(string &phone_number);//国际长途直拨号码

  enum E_PHONE_NUMBER_TYPE {
      PHONE_NUMBER_UNKNOWN = 0,
      PHONE_NUMBER_MOBILE = 1,
      PHONE_NUMBER_TELEPHONE = 2,
      PHONE_NUMBER_400 = 4,
      PHONE_NUMBER_800 = 5,
      PHONE_NUMBER_IDD = 6 // 国际长途直拨
  };

  pair<E_PHONE_NUMBER_TYPE, bool> check_phone_num_type(string &phone_number);
}
}

namespace ib_common = ib::common;

#endif
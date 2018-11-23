#ifndef _JWT_OPENSSL_H_
#define _JWT_OPENSSL_H_

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;


class JWTEncode{
public:
	JWTEncode(std::string keyFilePath = "");
	~JWTEncode(){}

	std::string get_provider_authentication_tokens(const std::string& KeyID, const std::string& TeamID);

private:
	uint64_t  UnixTimeStamp();
	void base64UrlEncode(std::string& str );
	void url_encode(char* str);
	void jwt_sign(std::string& str);
	int openssl_sign_ecdsa(char **out, unsigned int *len, const char *str);

private:
	std::string m_skeyfilepath;
	std::string m_str_tokens;
	uint64_t last_iat;
};

#endif
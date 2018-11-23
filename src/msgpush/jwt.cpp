
#include <map>
#include <sstream>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/ecdsa.h>
#include <openssl/buffer.h>
#include <openssl/pem.h>

#include "jwt.h"
#include "public/base64.h"

JWTEncode::JWTEncode(std::string keyFilePath) 
	:m_skeyfilepath(keyFilePath),
	m_str_tokens(""), 
	last_iat(0)
{

}


std::string JWTEncode::get_provider_authentication_tokens(const std::string& KeyID, const std::string& TeamID)
{
	uint64_t iat = UnixTimeStamp();

	//token update 45 minute
	if (!m_str_tokens.empty() && (iat - last_iat)<2700)
	{
		return m_str_tokens;
	}

	last_iat = iat;

	std::string str_header, str_payload;
	std::string tokens("");
	std::ostringstream os;

	os << "{\"alg\":\"ES256\","
		<< "\"kid\":\"" << KeyID << "\"}";

	str_header = os.str();

	os.str("");
	os << "{\"iss\":\"" << TeamID << "\","
		<< "\"iat\":" <<  iat << "}";

	str_payload = os.str();

	base64UrlEncode(str_header);
	base64UrlEncode(str_payload);

	tokens = str_header + "." + str_payload;
	std::string str_sign(tokens);

	jwt_sign(str_sign);

	m_str_tokens = tokens + "." + str_sign;

	return m_str_tokens;

}


uint64_t  JWTEncode::UnixTimeStamp()
{
	ptime epoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));
	time_duration time_from_epoch = second_clock::universal_time() - epoch;

	return time_from_epoch.total_seconds();
}

void JWTEncode::base64UrlEncode(std::string& str)
{
	int nLen1 = str.length();
	int nLen2 = nLen1 * 2;

	char *szEncode = NULL;
	szEncode = new char[nLen2];
	if (!szEncode)
	{
		return;
	}
	Base64Encode(szEncode, str.c_str(), str.length());
	url_encode(szEncode);
	str = szEncode;

	delete[] szEncode;

}
void JWTEncode::url_encode(char* str)
{
	int len = strlen(str);
	int i, t;

	for (i = t = 0; i < len; i++) {
		switch (str[i]) {
		case '+':
			str[t++] = '-';
			break;
		case '/':
			str[t++] = '_';
			break;
		case '=':
			break;
		default:
			str[t++] = str[i];
		}
	}
	str[t] = '\0';
}
void JWTEncode::jwt_sign(std::string& str)
{
	char *out = NULL;
	unsigned int len = 0;

	size_t ret = openssl_sign_ecdsa(&out, &len, str.c_str());
	if (!out)
	{
		return;
	}

	char *szEncode = NULL;
	szEncode = new char[2 * len];
	if (!szEncode) return;

	Base64Encode(szEncode, out, len);
	url_encode(szEncode);

	str = szEncode;

	delete[] szEncode;
	free(out);

}

int JWTEncode::openssl_sign_ecdsa(char **out, unsigned int *len, const char *str)
{
	int src_len = strlen(str);
	unsigned char digest[32];
	unsigned int digest_len = 0;

	EC_KEY *eckey = NULL;
	BIO   *bp = NULL;
	int ret = 0;

	bp = BIO_new_file( m_skeyfilepath.c_str(), "rb");
	eckey = PEM_read_bio_ECPrivateKey(bp, NULL, NULL, NULL);
	if (eckey == NULL){
		BIO_free(bp);
		return 0;
	}

	do{
		EVP_MD_CTX mdctx;
		EVP_MD_CTX_init(&mdctx);
		EVP_DigestInit(&mdctx, EVP_sha256());
		EVP_DigestUpdate(&mdctx, (const void *)str, src_len);
		EVP_DigestFinal(&mdctx, (unsigned char*)digest, &digest_len);

		unsigned int slen = ECDSA_size(eckey);
		*out = (char*)malloc(slen);
		if (NULL == *out) break;

		ret = ECDSA_sign(0, digest, digest_len, (unsigned char*)*out, &slen, eckey);
		*len = slen;
	} while (0);

	BIO_free(bp);
	EC_KEY_free(eckey);
	return ret;
}

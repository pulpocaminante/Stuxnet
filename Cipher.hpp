#pragma once
#include <string>

static class Railfence
{
public:
    static void decipher(int key, const std::wstring& ciphertext, std::wstring& plaintext)
    {
        int i, length = ciphertext.size(), skip, line, j, k = 0;
        plaintext.clear();
        plaintext.resize(length);
        for (line = 0; line < key - 1; line++) {
            skip = 2 * (key - line - 1);
            j = 0;
            for (i = line; i < length;) {
                plaintext[i] = ciphertext[k++];
                if ((line == 0) || (j % 2 == 0)) i += skip;
                else i += 2 * (key - 1) - skip;
                j++;
            }
        }
        for (i = line; i < length; i += 2 * (key - 1)) plaintext[i] = ciphertext[k++];
    }

    static void encipher(int key, const std::wstring& ciphertext, std::wstring& plaintext)
    {
        int i, length = ciphertext.size(), skip, line, j, k = 0;
        plaintext.clear();
        plaintext.resize(length);
        for (line = 0; line < key - 1; line++) {
            skip = 2 * (key - line - 1);
            j = 0;
            for (i = line; i < length;) {
                plaintext[i] = ciphertext[k++];
                if ((line == 0) || (j % 2 == 0)) i += skip;
                else i += 2 * (key - 1) - skip;
                j++;
            }
        }
        for (i = line; i < length; i += 2 * (key - 1)) plaintext[i] = ciphertext[k++];
    }
	
	static HRESULT ReadBinaryFile(
		_In_  std::string& sFilePath,
		_Out_ unsigned char*& buffer,
		_Out_ size_t& fileSize)
	{
		// Reading size of file
		FILE* file = fopen(sFilePath.c_str(), "rb+");
        if (file == NULL) return E_FAIL;
		fseek(file, 0, SEEK_END);
		long int size = ftell(file);
		fclose(file);
		fileSize = size;

		// Reading data to array of unsigned chars
		file = fopen(sFilePath.c_str(), "rb+");
		buffer = (unsigned char*)malloc(size);
		int bytes_read = fread(buffer, sizeof(unsigned char), size, file);
		fclose(file);

        if (bytes_read > 1)
            return S_OK;
        
        return E_FAIL;
	}

	static void WriteBinaryFile(
		_In_  std::string& sFilePath,
		_In_  unsigned char* buffer,
		_In_  size_t fileSize)
	{
		FILE* file = fopen(sFilePath.c_str(), "wb+");
		fwrite(buffer, sizeof(unsigned char), fileSize, file);
		fclose(file);
	}

	static std::string base64_encode(const unsigned char* src, size_t len)
	{

		static const unsigned char base64_table[65] =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		unsigned char* out, * pos;
		const unsigned char* end, * in;

		size_t olen;

		olen = 4 * ((len + 2) / 3); /* 3-byte blocks to 4-byte */

		if (olen < len)
			return std::string(); /* integer overflow */

		std::string outStr;
		outStr.resize(olen);
		out = (unsigned char*)&outStr[0];

		end = src + len;
		in = src;
		pos = out;
		while (end - in >= 3) {
			*pos++ = base64_table[in[0] >> 2];
			*pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
			*pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
			*pos++ = base64_table[in[2] & 0x3f];
			in += 3;
		}

		if (end - in) {
			*pos++ = base64_table[in[0] >> 2];
			if (end - in == 1) {
				*pos++ = base64_table[(in[0] & 0x03) << 4];
				*pos++ = '=';
			}
			else {
				*pos++ = base64_table[((in[0] & 0x03) << 4) |
					(in[1] >> 4)];
				*pos++ = base64_table[(in[1] & 0x0f) << 2];
			}
			*pos++ = '=';
		}

		return outStr;
	}
   

    static std::string b64decode(const void* data, const size_t len)
    {
        static const int B64index[256] = { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62, 63, 62, 62, 63, 52, 53, 54, 55,
         56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,
         7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,
         0,  0,  0, 63,  0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
         41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };
        unsigned char* p = (unsigned char*)data;
        int pad = len > 0 && (len % 4 || p[len - 1] == '=');
        const size_t L = ((len + 3) / 4 - pad) * 4;
        std::string str(L / 4 * 3 + pad, '\0');

        for (size_t i = 0, j = 0; i < L; i += 4)
        {
            int n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 | B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
            str[j++] = n >> 16;
            str[j++] = n >> 8 & 0xFF;
            str[j++] = n & 0xFF;
        }
        if (pad)
        {
            int n = B64index[p[L]] << 18 | B64index[p[L + 1]] << 12;
            str[str.size() - 1] = n >> 16;

            if (len > L + 2 && p[L + 2] != '=')
            {
                n |= B64index[p[L + 2]] << 6;
                str.push_back(n >> 8 & 0xFF);
            }
        }
        return str;
    }
    
    static HRESULT FileToBase64UTF16(
		_In_ std::string filePath,
		_Out_ std::wstring& base64)
    {
		// Buffer to read file to
        unsigned char* buffer;
        size_t fileSize = 0;

		// Read file to buffer (memory allocation is handled within ReadBinaryFile)
        HRESULT hres = NULL;
        hres = ReadBinaryFile(filePath, buffer, fileSize);

        if (fileSize > 0 && SUCCEEDED(hres))
        {
            ILog("Read %d bytes from file\n", fileSize);
        }
        else
        {
            ILog("Failed to read file\n");
            return E_FAIL;
        }
		// Encode file contents to base64
        std::string encoded = base64_encode(buffer, fileSize);

        
        // Print the length
		ILog("Base64 encoded string length: %d\n", encoded.length());
		
		// Convert file contents from a UTF8 character array to a UTF16 wide character array
        int wslen = MultiByteToWideChar(CP_ACP, 0, encoded.c_str(), strlen(encoded.c_str()), 0, 0);
        BSTR bstr = SysAllocStringLen(0, wslen);

        // returns 0 if it fails, and !(>0) is S_OK
        hres = !(MultiByteToWideChar(CP_ACP, 0, encoded.c_str(), strlen(encoded.c_str()), bstr, wslen));
        // Use bstr here

		// Cast it to a wstring and deliver it by reference
        base64 = bstr;

		// Free the bstr
        SysFreeString(bstr);

        return hres;
    }

    static void Base64UTF16ToFile(
        _In_ std::wstring& base64,
        _In_ std::string& filePath)
    {
        // Convert wstring to string
		int mblen = WideCharToMultiByte(CP_ACP, 0, base64.c_str(), base64.length(), 0, 0, 0, 0);
		char* mbstr = new char[mblen + 1];
		WideCharToMultiByte(CP_ACP, 0, base64.c_str(), base64.length(), mbstr, mblen, 0, 0);

        // Contents are now in `mbstr` and we can destroy `base64`
        base64.clear();
        
        // Decode base64 to binary
        std::string decoded = b64decode(mbstr, mblen);
        
		// Convert decoded string to unsigned char array
		unsigned char* buffer = (unsigned char*)decoded.c_str();
		size_t fileSize = decoded.length();

        // Contents are now in `buffer` and we can destroy `decoded`
        decoded.clear();

        // Print the length
		ILog("Base64 decoded string length: %d\n", fileSize);
        
		// Write binary to file
		WriteBinaryFile(filePath, buffer, fileSize);
        

        return;
    }
};

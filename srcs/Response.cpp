#include "../inc/Response.hpp"
#include "../inc/Request.hpp"
#include "../inc/Server.hpp"
#include "../inc/webserv.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <ios>
#include <limits>
#include <sstream>
#include <utility>

std::string Response::_CRLF = "\r\n";

std::string Response::_404Page = "HTTP/1.1 404 Not Found\r\n"
"Content-type: text/html\r\n"
"\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Not Found</h1>\r\n"
"<p>The requested URL was not found on this server.</p>\r\n"
"</body>\r\n"
"</html>\r\n\r\n";

std::string Response::_403Page = "HTTP/1.1 403 Forbidden\r\n"
"Content-type: text/html\r\n"
"\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Forbidden</h1>\r\n"
"<p>You don't have permission to access this URL on this server.</p>\r\n"
"</body>\r\n"
"</html>\r\n\r\n";

Response::Response()
{
	_statusCode = 0;
	_contentType.first = "Content-Type: ";
	_contentLength.first = "Content-Length: ";
	_connection.first = "Connection: ";
	_connection.second = "keep_alive";
}

Response::~Response()
{
}

void Response::buildResponse(const Request& request, const std::string& filePath)
{
	if (_statusCode == 200 || _statusCode == 201) {
		switch (request.getRequestType()) {
			case GET_REQUEST:
				buildGetHeader(request.getRequestSubType());
				buildGetBody(filePath);
				break;
			case POST_REQUEST:
				buildPostHeader(request.getRequestSubType());
				break;
			case DELETE_REQUEST:
				break;
			default:
				break;
		}
		//do this in another function and probably need to rename functions
		buildCompleteResponse();
	}
	else
		buildErrorResponse();
}

void Response::buildGetHeader(int requestSubType)
{
	switch (requestSubType) {
		case TEXT:
			_contentType.second = "text/html";
			break;
		case IMAGE:
			_contentType.second = "image/*";
			break;
		case VIDEO:
			break;
		default:
			break;
	}
}

void Response::buildGetBody(const std::string& filePath)
{
	std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);
	if (file) {
		//Find file size and append it to buffer
		file.seekg(0, std::ios::end);
		_contentLength.second = file.tellg();
		file.seekg(0, std::ios::beg);

		const char* fileData = new char [_contentLength.second];
		file.read((char *)fileData, _contentLength.second);
		file.close();

		std::ostringstream fileSs;
		fileSs.write(fileData, _contentLength.second);
		_binaryData = fileSs.str();
		delete [] fileData;
	}
}

void Response::buildPostHeader(int requestSubType)
{
	//Idk ??
}

void Response::buildCompleteResponse()
{
	_completeResponse =
		"HTTP/1.1 " + itostr(_statusCode) + _CRLF 
		+ _contentType.first + _contentType.second + _CRLF 
		+ _contentLength.first + itostr(_contentLength.second) + _CRLF
		+ _connection.first + _connection.second + _CRLF
		+ _CRLF
		+ _binaryData;
}

void Response::buildErrorResponse()
{
	switch (_statusCode) {
		case 404:
			std::cerr << "Error 404: Client asked for non existing ressource\n";
			_completeResponse = _404Page;
			break;
		case 403:
			std::cerr << "Error 403: Client asked for forbidden ressource\n";
			_completeResponse = _403Page;
			break;
	}
}

void Response::setStatusCode(int statusCode)
{
	_statusCode = statusCode;
}

int Response::getStatusCode(void)
{
	return _statusCode;
}

std::string& Response::getCompleteResponse(void)
{
	return _completeResponse;
}


//Probably move this function somewhere else
std::string Response::itostr(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}
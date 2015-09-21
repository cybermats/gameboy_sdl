#pragma once

#include <exception>
#include <string>

class gb_exception : public std::exception
{
public:
	gb_exception(const std::string& message)
		: _message(message)
	{}

	virtual const char* what() const throw() override
	{
		return _message.c_str();
	}

private:
	const std::string _message;
};
	
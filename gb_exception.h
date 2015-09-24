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

	gb_exception(const gb_exception&) = default;
	gb_exception& operator=(const gb_exception&) = default;

private:
	std::string _message;
};
	
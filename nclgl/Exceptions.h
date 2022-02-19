#pragma once

#include <stdexcept>

class NotImplementedException : public std::logic_error
{
public:
    NotImplementedException() : std::logic_error("Function not yet implemented! ") { };
};

class NullPtrException : public std::logic_error
{
public:
    NullPtrException() : std::logic_error("Should not receive null pointer here! ") { };
};

class LoadErrorException : public std::logic_error
{
public:
    LoadErrorException() : std::logic_error("Loading error! ") { };
};

class TypeNotSupportedException : public std::logic_error
{
public:
    TypeNotSupportedException() : std::logic_error("Type is not supported! ") { };
};

class ParameterNotSupportedException : public std::logic_error
{
public:
    ParameterNotSupportedException() : std::logic_error("Parameter (combination) is not supported! ") { };
};
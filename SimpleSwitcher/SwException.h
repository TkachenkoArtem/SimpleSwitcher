#pragma once
#include <exception>
//#include <boost/throw_exception.hpp>
//#include <>


class SwException : public std::exception
{

};


//#define SW_TRY try{
//
//#define SW_CATCH }catch(const SwException& ex) {}
//
//
//
////#define SW_THROW_BOOL_WIN(X, F, ...) {BOOL __err = (X); if(!__err) { SwException exc(GetLastError()); LogException(exc, F, _ARG_); throw exc; }
//inline void SwThrowBOOL(BOOL res, const char* file, int line, const char* format = nullptr, ...)
//{
//	if(!res)
//	{
//		SwException exc(GetLastError());
//		// log
//		throw exc;
//	}
//}
//
//#define SW_THROW_BOOL_WIN(X) {SwThrowBOOL(X, __FILE__, __LINE__); }
//#define SW_THROW_BOOL_WIN_EX(X, F, ...) {SwThrowBOOL(X, __FILE__, __LINE__, F, __VA_ARGS__); }
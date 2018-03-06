#ifndef LIST_H
#define LIST_H

#include <BaseObject.h>
#include <string>
#include <vector>

class List : public BaseObject{
	public:
		std::vector<BaseObject *> val;
		List(std::vector<BaseObject *> val);
		virtual BaseObject * __add__(BaseObject * other);
		virtual BaseObject * __sub__(BaseObject * other);
		virtual BaseObject * __mul__(BaseObject * other);
		virtual BaseObject * __div__(BaseObject * other);
		virtual BaseObject * __pow__(BaseObject * other);
		virtual BaseObject * __mod__(BaseObject * other);

		virtual BaseObject * __copy__();
		virtual std::string __type__();
		virtual std::string __repr__();
};

#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <math.h>
#include <assert.h>

#include <Foxlang.h>

COMPARISON_OPERATOR REVERSE_COMPARISON_OPERATOR[] = {EQ,NEQ,GT,LT,GTE,LTE};

TypeObject BaseObjectType = {
	VarBaseObject_HEAD_INIT(&BaseObjectType,0)
	"BaseObject",								//typename
	sizeof (BaseObject),						//startsize
	0,											//itemsize
	0,											//destructor
	0,											//comparemethods
	0,											//numbermethods
	0,											//hash
	0,											//repr
};

TypeObject VarBaseObjectType = {
	VarBaseObject_HEAD_INIT(&VarBaseObjectType,0)
	"VarBaseObject",							//typename
	sizeof (VarBaseObject),						//startsize
	0,											//itemsize
	0,											//destructor
	0,											//comparemethods
	0,											//numbermethods
	0,											//hash
	0,											//repr
};

char * BaseObject_Repr_CHARPNT(BaseObject * self){
	BaseObject * res = BaseObject_Repr(self);
	char * str = StringObject_Str_CHARPNT(res);
	BaseObject_Dealloc(res);
	return str;
}

BaseObject * BaseObject_Repr(BaseObject * self){
	if(self == NULL){
		printf("hey");
		return NULL;
	}
	if(OBJTYPE(self)->reprfunction == NULL){
		return StringObject_Fromformat("<%s object at %p>",OBJTYPE(self)->typename,self);
	}
	BaseObject * res = OBJTYPE(self)->reprfunction(self);
	return res;
}


BaseObject * BaseObject_Compare_NEQ(BaseObject * a, BaseObject * b){
		if(OBJTYPE(a)->compare != NULL && OBJTYPE(a)->compare->NEQ != NULL){
			return OBJTYPE(a)->compare->NEQ(a,b);
		}else if(OBJTYPE(a)->compare != NULL && OBJTYPE(a)->compare->NEQ == NULL && OBJTYPE(b)->compare != NULL && OBJTYPE(b)->compare->NEQ != NULL){
			return OBJTYPE(b)->compare->NEQ(b,a);
		}else if(OBJTYPE(a)->compare == NULL && OBJTYPE(b)->compare != NULL && OBJTYPE(b)->compare->NEQ != NULL){
			return OBJTYPE(b)->compare->NEQ(b,a);
		}else if(OBJTYPE(a)->compare != NULL && OBJTYPE(a)->compare->NEQ == NULL && OBJTYPE(b)->compare != NULL && OBJTYPE(b)->compare->NEQ == NULL && (OBJTYPE(a)->compare->EQ != NULL||OBJTYPE(b)->compare->EQ != NULL)){
			return BOOLEAN_NOT(BaseObject_Compare_EQ(a,b));
		}else if(OBJTYPE(a)->compare == NULL && OBJTYPE(b)->compare != NULL && OBJTYPE(b)->compare->NEQ == NULL && OBJTYPE(b)->compare->EQ != NULL){
			return BOOLEAN_NOT(BaseObject_Compare_EQ(a,b));
		}else{
			return (BaseObject *)BooleanObject_FromBool(a != b);
		}
}


BaseObject * BaseObject_Compare_EQ(BaseObject * a, BaseObject * b){
		if(OBJTYPE(a)->compare != NULL && OBJTYPE(a)->compare->EQ != NULL){
			return OBJTYPE(a)->compare->EQ(a,b);
		}else if(OBJTYPE(a)->compare != NULL && OBJTYPE(a)->compare->EQ == NULL && OBJTYPE(b)->compare != NULL && OBJTYPE(b)->compare->EQ != NULL){
			return OBJTYPE(b)->compare->EQ(b,a);
		}else if(OBJTYPE(a)->compare == NULL && OBJTYPE(b)->compare != NULL && OBJTYPE(b)->compare->EQ != NULL){
			return OBJTYPE(b)->compare->EQ(b,a);
		}else if(OBJTYPE(a)->compare != NULL && OBJTYPE(a)->compare->EQ == NULL && OBJTYPE(b)->compare != NULL && OBJTYPE(b)->compare->EQ == NULL && (OBJTYPE(a)->compare->NEQ != NULL||OBJTYPE(b)->compare->NEQ != NULL)){
			return BOOLEAN_NOT(BaseObject_Compare_NEQ(a,b));
		}else if(OBJTYPE(a)->compare == NULL  && OBJTYPE(b)->compare != NULL && OBJTYPE(b)->compare->EQ == NULL && OBJTYPE(b)->compare->NEQ != NULL){
			return BOOLEAN_NOT(BaseObject_Compare_NEQ(a,b));
		}else{
			return (BaseObject *)BooleanObject_FromBool(a == b);
		}
}


BaseObject * BaseObject_Compare(BaseObject * a, BaseObject * b,COMPARISON_OPERATOR op){
	if(a == NULL || b == NULL){
		return False;
	}

	//try to call compare functions
	if(OBJTYPE(a)->compare == NULL && OBJTYPE(b)->compare == NULL){
		if(op == EQ){
			return (BaseObject *)BooleanObject_FromBool(a == b);
		}else if(op == NEQ){
			return (BaseObject *)BooleanObject_FromBool(a != b); 
		}else{
			return False;
		}
	}

	if(op == EQ){
		return BaseObject_Compare_EQ(a,b);
	}else if(op == NEQ){
		return BaseObject_Compare_NEQ(a,b);
	}
	//add LT/GT/LTE/GTE handlers
	else{
		return False;
	}
}


int BaseObject_Compare_BOOL(BaseObject * a, BaseObject * b,COMPARISON_OPERATOR op){
	int compared_result = -1;
	if(a == b){
		if(op == EQ){
			return 1;
		}else if(op == NEQ){
			return 0;
		}
	}else{
		BaseObject * res = BaseObject_Compare(a,b,op);
		if (res == NULL){
			compared_result = -1;
		}else if(IS_BOOLEAN(res)){
			compared_result = BOOLEAN_IS_TRUE(res);
		}else{
			compared_result = BASEOBJECT_IS_TRUE(res);
		}						
		DECREF(res);
	}
	return compared_result;
}


HASH BaseObject_Hash_HASH(BaseObject * self){
	if(OBJTYPE(self)->generate_hash == NULL){
		//exception handler
		printf("object not hashable");
		return -1;
	}else{
		return OBJTYPE(self)->generate_hash(self);
	}
}

BaseObject * BaseObject_Hash(BaseObject * self){
	return (BaseObject *)IntegerObject_Fromlong(BaseObject_Hash_HASH(self));
}

void BaseObject_PRINTFUNC(BaseObject * self){
	char * res = BaseObject_Repr_CHARPNT(self);
	printf("%s\n",res);
	free(res);
}
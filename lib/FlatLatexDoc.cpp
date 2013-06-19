
#include <iostream>
#include "FlatLatexDoc.h"
#include "tralics/tralics.h"

using namespace v8;
using namespace std;

Persistent<Function>  FlatLatexDoc::constructor;
FlatLatexDoc::FlatLatexDoc(){}
FlatLatexDoc::~FlatLatexDoc(){}

/**
 factory method to return a new object instance of FlatLatexDoc
 @args -  not being used
**/
Handle<Value> FlatLatexDoc::NewInstance(const Arguments& args)
{
    HandleScope scope;
    
    //only take in the first argument
    const unsigned argc = 1;
    Handle<Value> argv[argc] = { args[0] };
    
    //return a new js instance is equivilent to returning a instance of a constructor
    //function of this class
    //this eventually invokes  FlatLatexDoc::New  
    Local<Object> instance = FlatLatexDoc::constructor->NewInstance(argc, argv);
    return scope.Close(instance);
}

/**
 JavaScript Class Definition 
 This is invoked when module is initialized
 In this function, we setup the meta info for the javascript class which 
 is equivilent to this C++ class , including class name, constructor function, 
 and fields
 
 consider this as the actual definition code of a javascript class like 'defineClass' 
**/
void FlatLatexDoc::Init(Handle<Object> target)
{
    //this function template will produce the js function being the constructor function
    Local<FunctionTemplate> tpl = FunctionTemplate::New(FlatLatexDoc::New);
    tpl->SetClassName(v8String::NewSymbol("FlatLatexDoc"));
    
    //each function ahs a prototype template whichs gives access to defining of the prototype object of this class
    //define a function on the prototype of every js object
    tpl->PrototypeTemplate()->Set(v8String::NewSymbol("analyze"), FunctionTemplate::New(FlatLatexDoc::Analyze)->GetFunction());

    //the instance template allows definition of all fields on object instance of this class
    
    //always 1-1 relations between a js obj instantiated from this class and its corresponding C++ object 
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    
    //define fields  on every FlatLatexDoc js object 
    tpl->InstanceTemplate()->Set(v8String::NewSymbol("version"), v8String::NewSymbol("0.0.1"));
    tpl->InstanceTemplate()->Set(v8String::NewSymbol("stat"), FunctionTemplate::New(FlatLatexDoc::Stat)->GetFunction());
    
    //once the function template is defined,
    //use it to create a function instance and expose it as this class's constructor to JS 
    //then in JS, we can do:
    //   var obj = new FlatLatexDoc(...) 
    FlatLatexDoc::constructor = Persistent<Function>::New(tpl->GetFunction());
    //this is equivilent to 'exports.FlatLatexDoc = FlatLatexDoc' from within a JS module
    target->Set(v8String::NewSymbol("FlatLatexDoc"), constructor);
    
    //and we are done defining the FlatLatexDoc class!
}

/**
  JS Constructor Function Returning 
  
  JS:   var obj = new FlatLatexDoc(...)
**/
Handle<Value> FlatLatexDoc::New(const Arguments& args)
{
    HandleScope scope;
    
    //create a new C++ instance object and wraps it to 'this' variable
    //so that in JS, 'this' being used within the function scope of this object
    //actually refers back to this object !!
    FlatLatexDoc* obj = new FlatLatexDoc();
    obj->Wrap(args.This());
    
    return args.This();
}

/**
 Method defined on JS Class Prototype
**/
Handle<Value> FlatLatexDoc::Analyze(const Arguments& args)
{

    HandleScope scope;
    
    if(args.This().IsEmpty() || args.This()->InternalFieldCount()==0)
    {
        cout << "no associated object call" << endl;
        return scope.Close(Undefined());
    }

    //every time JS invockes this function, it is invoked on an object,
    //which previously has been associated with a C++ object instance of FlatLatexDoc,
    //we shall unwrap and obtain a pointer reference to this C++ object
    //the trick here is to use 'this' pointer to uncover it !
//    FlatLatexDoc* obj = ObjectWrap::Unwrap<FlatLatexDoc>(args.This());
    
    //once we have the object, now good to go with the analysis 
    
    return scope.Close(Undefined());
}


/**
 Method defined on JS Object
**/
Handle<Value> FlatLatexDoc::Stat(const Arguments& args)
{
    HandleScope scope;
    
    
//    FlatLatexDoc* obj = ObjectWrap::Unwrap<FlatLatexDoc>(args.This());
    //compute some stat and return it as a generic js object
    Local<Object> stat = Object::New();
    stat->Set(v8String::NewSymbol("size"), Number::New(0));

    return scope.Close(stat);
}


















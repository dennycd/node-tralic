#ifndef _FlatLatexDoc_h
#define _FlatLatexDoc_h

#include <node.h>

typedef v8::String v8String;
class FlatLatexDoc : public node::ObjectWrap {
public:
    static void Init(v8::Handle<v8::Object> target); //initializer 
    static v8::Handle<v8::Value> NewInstance(const v8::Arguments& args); //factory method

    //hide constructor so this class type can only be created via JS invocation
    FlatLatexDoc();
    ~FlatLatexDoc();

    
private:
    static v8::Handle<v8::Value> New(const v8::Arguments& args);
    static v8::Handle<v8::Value> Analyze(const v8::Arguments& args);
    static v8::Handle<v8::Value> Stat(const v8::Arguments& args);
    
private:
    static v8::Persistent<v8::Function> constructor; 
};

#endif

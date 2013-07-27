#include <iostream>
#include <sstream>
#include <string>
#include <node.h>
#include <v8.h>
#include <cstdlib>

#include "tralics/tralics.h"
#include "FlatLatexDoc.h"

//#define BUILDING_NODE_EXTENSION

using namespace v8;
using namespace std;

string TRALICS_OUTPUT_OPT= "-output_dir=.";
string TRALICS_CONFIG_OPT = "-confdir=./tralics";

Persistent<FlatLatexDoc> gLatexDocObj;//module level object that exist beyond the scope of a single function call from JS
static Handle<Object> curFlagObj; //temporarily hoilding references to the falt object for returning

/**
 Recursive traverse xml node starting with the root node 
 
 @root - the root xml node
 @path - current path string
 @index - root's sequential index among all siblings belonging to a same parent node (if any)
**/
static void recursive_depth_first(Xml* root, string path, unsigned int index)
{
    //cout << "looking at path " << path << endl;

    bool nameIsEmpty = (root->get_name().c_str()==NULL);

    vector<Xmlp> childs = root->get_tree();
    
    if(root->get_id().value <= 0)   //text node reached
    {
        if(nameIsEmpty)
            return;  //simply ignore it 
            
        string txt = root->get_name().c_str();
        ostringstream oss;
        oss << path << ":txt#" << index;
        path = oss.str();

        //cout << "INFO: adding txt content at path " << path << " : " << txt.c_str() << endl;
        curFlagObj->Set(v8String::NewSymbol(path.c_str()), v8String::NewSymbol(txt.c_str()));

        //for sure no offspring under a text node
        return;
    }
    else {   //internal xml node, extend the path
    
        string name;
        if(nameIsEmpty)
        {
            cout << "WARNING: an internal xml node has its name being empty string" << endl;
            name = "NULL";
        }
        else {
            //append name
            name = root->get_name().c_str();   
        }
    
        path = (path.length()==0) ? name : (path+":"+name);

        
        //append unique id
        ostringstream oss; 
        oss << path << "#" << index; //root->get_id().value;
        path = oss.str();
            
        //if shall never happen that same path string name exist
        if(curFlagObj->Has(v8String::NewSymbol(path.c_str())))
        {
            cout << "WARNING: duplicate path key detected " << path << endl;
            return;
        }

        //if reaching a math formula node, simply dump the mathml content to string
        if(name=="math")
        {
            string content;
            root->convert_to_string_safe(content);
            //cout << "INFO: adding math content at path " << path << " : " << content.c_str() << endl;
            curFlagObj->Set(v8String::NewSymbol(path.c_str()), v8String::NewSymbol(content.c_str()));
            return;
        }
    }
    
    
    unsigned int childIndex = 0;
    for(vector<Xmlp>::const_iterator me = childs.begin(); me!=childs.end(); ++me)
    {
        if(*me==NULL) 
        {
            cout << "WARNING: observing empty child xml node" << endl;
            continue;
        }
        recursive_depth_first(*me, path, childIndex);
        childIndex++;
    }
}

static void HandleParseDone(MainClass* mainCls)
{
    Stack* pStack = mainCls->the_stack;
    Xmlp pdoc = pStack->document_element();
    
    string rootPath = ""; //reset for each doc parsing
    recursive_depth_first(pdoc, rootPath, 0);
}




/**
 Static Analyze Function. This function analyzes a single latex doc on disk and returns the flattened text content 
 
 @param filename - absolute file name 
 @param callback -  function( docObj )
            * docObj is a flattend js object, indexed by each field's xml_path, with value being its text content
                    xml_path  : "std:maketitle:title"
**/
static Handle<Value> Analyze(const Arguments& args){
    HandleScope scope;
    
    if(args.Length()!=2 || !(args[0]->IsString() && args[1]->IsFunction()))
    {
        ThrowException(Exception::TypeError(v8String::NewSymbol("Invalid Arguments")));
        return scope.Close(Undefined());
    }
    
    //extrac filepath and callback
    Local<v8String> filepath = args[0]->ToString();
    Local<Function> callback = Local<Function>::Cast(args[1]);
    
    //convert from v8String -> AsciiValue -> c std string -> const char* 
    string filepathStr = *(v8String::AsciiValue(filepath));
    //cout << "filepath = " << filepathStr << endl;

    //analyze doc and assemble results
    Local<Object> flatObj = Object::New();
    curFlagObj = flatObj;

    const unsigned argumentCount = 5;
    const char* arguments[] = {"tralics", "-silent", filepathStr.c_str(), TRALICS_CONFIG_OPT.c_str(), TRALICS_OUTPUT_OPT.c_str()};
    main_function(argumentCount,arguments, HandleParseDone);


    //invoke callback 
    const unsigned argc = 1;
    Local<Value> argv[argc] = { flatObj }; //callback(flatObj)
    callback->Call(Context::GetCurrent()->Global(), argc, argv);
     
    return scope.Close(Undefined());   
}





void InitModule(Handle<Object> target){

    

    //expose module elements
    target->Set(v8::String::NewSymbol("analyze"), FunctionTemplate::New(Analyze)->GetFunction());
    
    //export C++ Class 
    FlatLatexDoc::Init(target);
    
    //Initial a Global Scope Object but not exposing it to JS
    if(gLatexDocObj.IsEmpty())
    {
        Handle<FlatLatexDoc> handle(new FlatLatexDoc()); //raw obj to handle
        gLatexDocObj = Persistent<FlatLatexDoc>::New(handle); //handle to persistent 
    }
}

NODE_MODULE(tralics_main, InitModule)

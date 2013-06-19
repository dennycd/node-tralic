#include <iostream>
#include <sstream>
#include "tralics.h"

using namespace std;


static void recursive_depth_first(Xml* root, ostringstream& oss)
{
    vector<Xmlp> childs = root->get_tree();
    
    string suffix = "";
    if(root->get_id().value <= 0)
    {
        const char* cstr = root->get_name().c_str();
        if(!cstr)
            oss << "[NULL";
        else
        {
            string tmp = cstr;
            if(tmp=="\n")
                oss << "[\\n";
            else
                oss << "[" << tmp;        
        }

            
        suffix = "]";
    }
    else {
        oss << "{" << root->get_name().c_str();
        suffix = "}";
    }
    

    for(vector<Xmlp>::const_iterator me = childs.begin(); me!=childs.end(); me++)
    {
        if(*me==NULL) 
        {
            cout << "WARNING: observing empty child xml node" << endl;
            continue;
        }
        recursive_depth_first(*me, oss);        
    }
    oss << suffix;
}

static void HandleParseDone(MainClass* mainCls)
{
    Stack* pStack = mainCls->the_stack;
    Xmlp pdoc = pStack->document_element();
    
    ostringstream oss;
    recursive_depth_first(pdoc, oss);
    cout << "RESULT: " << oss.str() << endl;
}



int main(int argc, const char * argv[])
{

    const char* arguments[] = {"tralics", 
                                "/var/askmath/documents/tralilcs_sample/txtc.tex", 
                                "-confdir=/var/askmath/config/tralics",
                                "-output_dir=/var/askmath/cache"};
                                
    main_function(4,arguments, HandleParseDone);
    

//    const char* arguments2[] = {"tralics", 
//                                "/var/askmath/documents/sample.tex", 
//                                "-confdir=/var/askmath/config/tralics",
//                                "-output_dir=/var/askmath/cache"};
//                                
//    main_function(4,arguments2, HandleParseDone);    
    
    return 0;
}


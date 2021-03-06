// Tralics, a LaTeX to XML translator.
// Copright INRIA/apics/marelle (Jose' Grimm) 2008-2011

// This software is governed by the CeCILL license under French law and
// abiding by the rules of distribution of free software.  You can  use, 
// modify and/ or redistribute the software under the terms of the CeCILL
// license as circulated by CEA, CNRS and INRIA at the following URL
// "http://www.cecill.info". 
// (See the file COPYING in the main directory for details)


// The file contains code for configurating the Raweb

#include "tralics.h"

const char* txconfig_rcsid=
  "$Id: txconfig.C,v 2.14 2011/06/28 08:15:25 grimm Exp $";

namespace config_ns {
  Buffer sec_buffer;
  string all_themes = "";
  Buffer Txbuf;
  ParamDataVector config_data;
  bool have_default_ur = false;
  string the_default_rc="";
  int ur_size=0;
  int composition_section = -1;
  bool cur_sec_no_topic= false;

}

namespace config_ns {
  void interpret_section_list(Buffer&B,bool new_syntax);
  void bad_conf(String);
  void interpret_data_list(Buffer&B,const string&);
  void interpret_theme_list(const Buffer&B);
  bool is_good_ur(const string& x);
  int next_RC_in_buffer(Buffer&B,string&sname,string&lname);
  string check_section(const string&);
  string check_spec_section(const string&s);
}

using namespace config_ns;

// --------------------------------------------------

// Configuration file option lists.
// The configuration file may contains lines of the form
// FOO_vals="/A/B/C", or FOO_vals="/A/a/B/b/C/c".
// transformed into a vector of data, named FOO, with some execptions.


// This error is fatal
void config_ns::bad_conf(String s)
{
  main_ns::log_and_tty 
    << "The configuration file for the RA is ra2007.tcf or ra.tcf\n"
    << "It must define a value for the parameter " <<  s <<  "\n" 
    << "See transcript file " << the_log.get_filename() << " for details\n"
    << "No xml file generated\n" << lg_fatal;
  exit(1);
}

// We make sure these four items always exist
ParamDataVector::ParamDataVector()
{
  data.push_back(new ParamDataList("ur"));
  data.push_back(new ParamDataList("sections"));
  data.push_back(new ParamDataList("profession"));
  data.push_back(new ParamDataList("affiliation"));
}

// We may add a special slot at the end
void ParamDataList::check_other()
{
  if(is_lower_case (name[0]) && !empty())
    push_back(ParamDataSlot("Other","Other"));
}

// There are 5 values required for the RA. In the case of themes,
// we just store a string with initial and final space, and use strstr
// for finding. The case of section is defined in txcheck.
// In the case of profession and affiliation, we add Other at the end.

void MainClass::finish_init()
{
  if(in_ra()) {
    if(year<=2003) all_themes = " 1a 1b 1c 2a 2b 3a 3b 4a 4b ";
    if(all_themes.empty()) bad_conf("theme_vals");
    if(config_data.data[0]->empty()) bad_conf("ur_vals");
    if(year>=2007) {
      if(config_data.data[3]->empty()) bad_conf("affiliation_vals");
      if(config_data.data[2]->empty()) bad_conf("profession_vals");
    }
    int n = config_data.data[1]->size();
    if(n==0) bad_conf("sections_vals");
    if(n<2) { // this is bad
      config_data.data[1]->push_back(ParamDataSlot("dummy1", "dummy1"));
      config_data.data[1]->push_back(ParamDataSlot("dummy2", "dummy2"));
      main_ns::log_and_tty << "Config file did not provide sections\n"
       << lg_fatal;
      exit(1);
    }
    if(composition_section == n) composition_section = -1;
  }
  int n = config_data.data.size();
  for(int i=2;i<n;i++) config_data.data[i]->check_other();
}

// --------------------------------------------------
// This interprets theme_vals = "foo bar"
// and all consequences

void config_ns::interpret_list(const string& a, Buffer&b)
{
  if (a=="section") interpret_section_list(b,false);
  else if(a=="fullsection") interpret_section_list(b,true); 
  else if(a=="theme") interpret_theme_list(b);
  else if(a=="") {}
  else interpret_data_list(b,a);
}

// Function called when theme_vals is seen in the config file.
void config_ns::interpret_theme_list(const Buffer&B)
{
  Txbuf << bf_reset << ' ' << B << ' ';
  Txbuf.lowercase();
  all_themes = Txbuf.to_string();
}


// This function is called when we translate a theme value.
string MainClass::check_theme(const string& s)
{
  string res = Txbuf.add_with_space(s.c_str());
  if(!strstr(all_themes.c_str(),Txbuf.c_str())) {
    err_ns::local_buf.reset();
    if(s.empty())
      err_ns::local_buf << "Empty or missing theme\n";
    else
      err_ns::local_buf << "Invalid theme " << s << "\n";
    if(all_themes.empty())
      err_ns::local_buf << "Configuration file defines nothing";
    else
      err_ns::local_buf << "Valid themes are" << all_themes;  
    err_ns::signal_error("Bad theme",0);
  }
  return res;
}

// --------------------------------------------------

// Interprets Sections ="/foo/bar*/gee". 
// In 2007, we have  NewSections ="/foo/FOO/bar/BAR/gee/GEE"
// Puts foo bar+ gee in the transcript file (we use + instead of *, meaning
// that the character was interpreted). Three RaSection objects are
// created, named foo, bar and gee. The second is marked: not for topic.
// In simplified mode, a section has a name and a number.
// Otherwise it has additional fields.
void config_ns::interpret_section_list(Buffer&B,bool new_syntax)
{
  ParamDataList* V=config_data.data[1];
  if(start_interpret(B,"//")) {
    V->reset();
    sec_buffer.reset();
    composition_section = -1;
  }
  string s="",r="";
  for(;;) {
    if(!B.slash_separated(s)) break;
    if(new_syntax) { 
      if(!B.slash_separated(r)) break;
    } else r = "";
    if(s.empty()) continue;
    bool star = false;
    if(!new_syntax) {
      int ns = s.size();
      if(ns>1 && s[ns-1]=='*') { s=string(s,0,ns-1); star = true; }
    }
    if(r.empty()) r = s;
    the_log << "Section: " << s << (star ? "+": "") << " -> " << r << "\n";
    if(s=="composition") composition_section = V->size()+1;
    sec_buffer << " " << s;
    V->push_back(ParamDataSlot(s,r,!star));
  }
}

// --------------------------------------------------

// Return the data associated to name, may create depend on creat
ParamDataList* ParamDataVector::find_list(const string& name,bool creat)
{
  int n = data.size();
  for(int i=0;i<n;i++) if (data[i]->its_me(name)) return data[i];
  if(!creat) return 0;
  ParamDataList* res = new ParamDataList(name);
  data.push_back(res);
  return res;
}

inline void ParamDataSlot::to_buffer(Buffer&B) const
{
  B << key << "=" << value <<",";
}

inline void ParamDataSlot::key_to_buffer(Buffer&B) const
{
  B << " " << key;
}

void ParamDataList::keys_to_buffer(Buffer& B) const
{
  int n = size();
  for(int i=0; i<n;i++) data[i].key_to_buffer(B);
}

// Converts the whole data struture as foo1=bar1,foo2=bar2,
string config_ns::find_keys(const string& name)
{
  ParamDataList* X = config_data.find_list(name,false);
  if(!X) return "";
  Txbuf.reset();
  int n = X->size();
  for(int i=0;i<n;i++) X->data[i].to_buffer(Txbuf);
  if(n>0) Txbuf.rrl();
  return Txbuf.to_string();
}

// Return the value of the key in a list.
string config_ns::find_one_key(const string& name,const string&key)
{
  if(name=="ur") return pers_rc(key);
  if(name=="theme") return the_main->check_theme(key);
  if(name=="fullsection") return check_section(key);
  if(name=="section") return check_spec_section(key);
  ParamDataList* X = config_data.find_list(name,false);
  if(!X) {
    the_parser.parse_error("Configuration file does not define ",
				   name,"no list");
    return "";
  }
  int n = X->size();
  for(int i=0;i<n;i++) if(X->data[i].key == key) return X->data[i].value;
  err_ns::local_buf << bf_reset
		    << "Illegal value '" << key << "' for " << name << "\n"
		    << "Use one of:";
  X->keys_to_buffer(err_ns::local_buf);
  err_ns::signal_error("illegal data",0);
  return "";
}

string config_ns::check_section(const string&s)
{
  static int cur_section=-1;
  int k = -1;
  err_ns::local_buf.reset();
  vector <ParamDataSlot>& X = config_data.data[1]->data;
  int n = X.size(); // number of sections
  if(s.empty()) k = cur_section;
  else
    for(int i=0;i<n;i++)
      if(X[i].key==s) { k = i+1; break; }
  if(k>0 && k< cur_section) {
    err_ns::local_buf << "Bad section " << s << " after " 
		      << X[cur_section-1].key << "\n"
		      << "Order of sections is" << sec_buffer;  
    err_ns::signal_error("",1);
  } else if(k==-1) {
    if(n==0) {
      the_parser.parse_error("Illegal access to fullsection list.");
      return "";
    } 
    if(!s.empty()) {
      err_ns::local_buf <<"Invalid section " << s << "\n"
			<< "Valid sections are" << sec_buffer;  
      err_ns::signal_error("",1);
      if(cur_section<0)  cur_section = 1;
    }
  } else cur_section = k;
  if(cur_section<0)  {
    the_parser.parse_error("No default section");  
    cur_section = 1;
  }
  if(cur_section == composition_section) {
    static bool first_module = true;
    if(first_module) first_module = false;
    else {
      the_parser.parse_error("Only one module accepted in composition");
      cur_section ++;
    }
  }
  // return non-empty string only if section is new
  static int prev = -1;
  if(prev == cur_section) return "";
  prev = cur_section;
  cur_sec_no_topic = X[cur_section-1].no_topic();
  return X[cur_section-1].value;
}

// Special command. We assume that cur_sec_no_topic
// is correctlty set.
string config_ns::check_spec_section(const string&s)
{
  if(cur_sec_no_topic) return "";
  else if (s.empty()) return "default";
  else return s;
}





// We add a final slash, or double slash, this makes parsing easier;
// We also remove an initial slash (This is not done if the separator is a 
// space, case where s is empty).
// An initial plus sign means: append the line to the vector, else reset.

bool config_ns::start_interpret(Buffer&B, String s)
{
  bool ret_val = false;
  B.push_back(s);
  B.reset_ptr();
  if(B.head()=='+') {
    B.advance();
    the_log << "+";
  } else ret_val = true;
  if(s[0] && B.head()=='/') B.advance();
  return ret_val;
}

// This interprets Foo="Visiteur/foo/Chercheur//Enseignant//Technique//"
// This creates four slots in a table indexed by k.
void config_ns::interpret_data_list(Buffer&B, const string& name)
{
  ParamDataList* V=config_data.find_list(name,true);
  if (start_interpret(B,"//")) V->reset();
  for(;;) {
    string r1,r2;
    if(!B.slash_separated(r1)) return;
    if(!B.slash_separated(r2)) return;
    if(r1.empty()) continue;
    if(r2.empty()) r2= r1;
    the_log << name << ": " << r1 << " -> " << r2 << "\n";
    V->push_back(ParamDataSlot(r1,r2));
  }
}

// -----------------------------------------------------------

// User says \UR{foo,bar}
// returns -1 if there no other RC in the buffer.
// returns -2 if the RC is invalid
// returns location in the table otherwise
int config_ns::next_RC_in_buffer(Buffer&B,string&sname,string&lname)
{
  vector <ParamDataSlot> & ur_list =config_data.data[0]->data;
  B.skip_sp_tab_comma();  
  if(!B.head()) return -1;
  if(strncmp(B.c_str(B.get_ptr()),"\\UR",3) ==0) {
    static bool warned=false;
    if(!warned && the_parser.get_ra_year()>2006) {
      main_ns::log_and_tty << "You should use Lille instead of \\URLille,\n";
      main_ns::log_and_tty <<  "Nancy instead of \\URNancy, etc.\n";
      warned=true;
    }
    B.advance(3);
  }
  B.set_ptr1();
  B.skip_letter();
  int k = ur_list.size();
  for(int j = 0; j<k;j++)
    if(B.contains_here(ur_list[j].key.c_str())) {
      sname =ur_list[j].key;
      lname =ur_list[j].value;
      ur_list[j].mark_used();
      return j;
    }
  return -2;
}

// This is the function used since 2007, when definining the
// Research Centers of the team;
// May return  <URRocquencourt/><URSophia/>
void config_ns::check_RC(Buffer& B,Xml*res)
{
  string tmp = the_names[np_rcval].c_str(); 
  bool need_elt = tmp[0]=='+';// Hack
  String S=0;
  if(need_elt) S = tmp.c_str()+1;
  Buffer temp2;
  string sname, lname;
  temp2.reset();
  vector<int> vals;
  int nb = 0;
  Xml* new_elt;B.reset_ptr();
  for(;;) {
    int j=next_RC_in_buffer(B,sname,lname);
    if(j==-1) break;
    if(j==-2) { nb=0; break; } 
    if(need_elt)
      new_elt = new Xml(Istring(string (S+sname)),0);
    else {
      new_elt = new Xml(np_rcval,0);
      new_elt->get_id().add_attribute(Istring("name"), Istring(sname));
    }
    res->push_back(new_elt);
    temp2 << sname << " ";
    the_default_rc = sname;
    nb++;
  }
  ur_size = nb;
  if(nb==1) have_default_ur = true;
  if(nb) {
    the_log << "Localisation " << temp2 << "\n";
    return;
  }

  err_ns::local_buf <<bf_reset;
  if(B.empty())
    err_ns::local_buf << "Empty localisation value\n";
  else
    err_ns::local_buf << "Illegal localisation value: " << B.c_str() << "\n";
  err_ns::local_buf << "Use one or more of:";
  config_data.data[0]->keys_to_buffer(err_ns::local_buf);
  err_ns::signal_error("",1);
}

// Interprets the RC argument of a pers command
// This returns the short name, said otherwise, the argument.
// Notice the space case when argument is empty, or +foo or =foo.
string config_ns::pers_rc(const string& rc)
{
  if(rc.empty()) {
    if(have_default_ur) return the_default_rc;
    if(the_main->in_ra() &&the_parser.get_ra_year()>2006) {
      // signal error, make a default
      the_parser.parse_error("No default Research Centre defined");
      the_default_rc = "unknown";
    } 
    have_default_ur = true;
    return the_default_rc;
  } else if(rc[0]=='+') {
    return  rc.c_str()+1;
  } else {
    bool spec = (rc.size()>=2 && rc[0]=='=');
    string RC = spec ? rc.c_str()+1 : rc;
    if(!is_good_ur(RC)) {
      err_ns::local_buf << bf_reset
			<< "Invalid Unit Centre " << rc << "\n"
			<< "Use one of:";
      vector <ParamDataSlot> & V = config_data.data[0]->data;
      int n = V.size();
      for(int i=0; i<n;i++) if(V[i].is_used) V[i].key_to_buffer(err_ns::local_buf);
      err_ns::signal_error("illegal data",0);
    }
    if (spec) {
      the_default_rc = RC;
      have_default_ur = true;
    }
    return RC;
  }
}

bool config_ns::is_good_ur(const string& x)
{
  vector <ParamDataSlot> & ur_list =config_data.data[0]->data;
  int n = ur_list.size();
  if(ur_size==0) {
    for(int i=0;i<n; i++)
      ur_list[i].mark_used();
    ur_size = n;
  }
  for(int i=0;i<n; i++)
    if(ur_list[i].matches(x)) return true;
  return false;   
}

// --------------------------------------------------


// If S is, say `Cog A', this puts ` cog ' in the buffer, returns `cog'.
string Buffer::add_with_space(String s)
{
  int i=0;
  while(s[i]==' ') ++i;
  reset();
  push_back_space();
  while(s[i] && s[i] != ' ') { push_back(s[i]); ++i; }
  lowercase();
  string res = to_string(1);
  push_back_space();
  return res;
}


// --------------------------------------------------

// This creates the file foo_.bbl and initiates the bibtex translation.



// In the case of "foo bar gee", puts foo, bar and gee in the vector.
// Initial + means append, otherwise replace.

void Buffer::interpret_aux(vector <Istring>& bib, vector <Istring>& bib2)
{
  if(config_ns::start_interpret(*this,"")) {bib.resize(0); bib2.resize(0);}
  for(;;) {
    bool keep= true;
    skip_sp_tab();
    if(!head()) break;
    int a = ptr;
    if (head()=='-') { keep = false; if(!head()) break; a++; the_log << "--"; }
    while(head() && !is_space(head())) 
      advance(); 
    ptr1 = a;
    string k = substring(); 
    if (keep) bib.push_back(Istring(k)); else bib2.push_back(Istring(k));
    the_log << k <<" ";
  }
  the_log << "\n";
}

void Buffer::interpret_bibtex_list()
{
  the_log << "bibtex_fields: ";
  vector <Istring>& bib =  the_main->get_bibtex_fields();
  vector <Istring>& bib1 =  the_main->get_bibtex_fields_s();
  interpret_aux(bib, bib1);
}

void Buffer::interpret_bibtex_extension_list()
{
  the_log << "bibtex_extensions: ";
  vector <Istring>& bib =  the_main->get_bibtex_extensions();
  vector <Istring>& bib2 =  the_main->get_bibtex_extensions_s();
  interpret_aux(bib, bib2);
}

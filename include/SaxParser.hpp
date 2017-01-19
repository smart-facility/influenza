#ifndef __SAXPARSER_H
#define __SAXPARSER_H

#include <libxml++/libxml++.h>
#include <vector>

#include <repast_hpc/AgentId.h>
#include <repast_hpc/SharedContext.h>
#include "Individual.hpp"
#include "Activity.hpp"
#include "Data.hpp"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "repast_hpc/Random.h"

class Model;

class VBSaxParser : public xmlpp::SaxParser {

private:
  int _proc;
  Model& _model;
  
public:
  VBSaxParser(int aProc, Model& aModel);
  virtual ~VBSaxParser();

protected:
  //overrides:
  virtual void on_start_document();
  virtual void on_end_document();
  virtual void on_start_element(const Glib::ustring &name,
                                const AttributeList &properties);
  virtual void on_end_element(const Glib::ustring &name);
  virtual void on_characters(const Glib::ustring &characters);
  virtual void on_comment(const Glib::ustring &text);
  virtual void on_warning(const Glib::ustring &text);
  virtual void on_error(const Glib::ustring &text);
  virtual void on_fatal_error(const Glib::ustring &text);

  Individual* on_individual(const AttributeList &properties);
  void on_activity(const AttributeList& properties, Individual* ind);
  
};


#endif //__SAXPARSER_H


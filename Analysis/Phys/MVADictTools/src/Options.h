#ifndef OPTIONS_MVACLASSIFIER_H 
#define OPTIONS_MVACLASSIFIER_H 1

/***
 * Options class to pass options from map<string, string> from python
 * Then can perform a check
 *
 * @author Sam Hall
 * @date 2013-07-29
 */

class Options {
 private:
  //============================================================================
  // pass the options parser
  bool m_pass;
  // Options map
  std::map<std::string, std::string> m_opts;
  // Options map of allowed options
  std::map<std::string, std::string> m_allowed;
  //============================================================================

 public:
  //============================================================================
  // constructor and destructor
  Options(const std::map<std::string, std::string>& opts)
    : m_pass(true)
  {
    m_opts = opts;
    return;
  }
  ~Options() {;}
  //============================================================================

  // add an option with no default ==> Required
  template<class T>
  void add(std::string name,
      std::string about,
      T& input,
      MsgStream& info)
  {
    m_allowed[name] = about;;
    if (m_opts.count(name)) {
      try {
        input = boost::lexical_cast<T>(m_opts[name]);
      } catch (boost::bad_lexical_cast&) {
        info << "ERROR casting option \"" << name
             << "\" as " << m_opts[name] << endmsg; 
        m_pass &= false;
        return;
      }
    } else {
      info << "ERROR: Option \"" << name
           << "\"";
      if (name != about) {
        info << " (" << about << ")"; 
      }
      info << " is undefined (and required)" << endmsg;
      m_pass &= false;
      return;
    }
    info << name << " : " << input << endmsg;
    m_opts.erase(name);
    m_pass &= true;
    return;
  }
  
  // add non-required option
  template<class T>
  void add(std::string name,
      std::string about,
      T& input,
      MsgStream& info,
      const T& init)
  {
    m_allowed[name] = about;;
    if (m_opts.count(name)) {
      try {
        input = boost::lexical_cast<T>(m_opts[name]);
      } catch (boost::bad_lexical_cast&) {
        info << "ERROR casting option \"" << name
             << "\" as " << m_opts[name] << endmsg; 
        m_pass &= false;
        return;
      }
    } else {
      input = init;
      m_pass &= true;
      return;
    }
    info << name << " : " << input << endmsg;
    m_opts.erase(name);
    m_pass &= true;
    return;
  }
  
  // perform checks
  bool check(MsgStream& info) {
    if (m_opts.size() > 0) {
      std::map<std::string, std::string>::iterator it;
      info << "ERROR Unkonown options:" << endmsg;
      it = m_opts.begin();
      for (; it!=m_opts.end(); ++it) {
        info << "  " << it->first << " : " << it->second << endmsg;
      }
      info << "The allowed options are:" << endmsg;
      it = m_allowed.begin();
      for (; it!=m_allowed.end(); ++it) {
        info << "  " << it->first << " : " << it->second << endmsg;
      }
      m_pass &= false;
    }
    return m_pass;
  }
  //============================================================================
};


#endif

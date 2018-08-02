#ifndef CMT_CMDIF_H
#define CMT_CMDIF_H

public:
std::string handleCommand (const std::vector<std::string>& args)
{
  std::vector<std::string> _args = args;
  std::string ret = "";

  if (args[0] == "command") {
    _args.erase(_args.begin());
    ret = this->own_handle_command(_args);
    if (ret != "") {
      ret += "\n";
    }
#if 0 // CommandCB
    // call call back funcion only when return message does not include error keyword
    if (ret.find("is invalid") == std::string::npos    // defined command
     && ret.find("Error") == std::string::npos) {      // correct argument
      std::string cb_msg = this->CommandCB(args);
      if (cb_msg.find("error") != std::string::npos) { // correct usage
        ret += cb_msg + "\n";
      }
    }
#endif // CommandCB
  }
  else if (_args[0] == "reg") {
    ret = this->regif_handle_command(_args);
    if (ret != "") {
      ret += "\n";
    }
  }
  else if (_args[0] == "tgt") {
    ret = this->tgt_handle_command(_args);
    if (ret != "") {
      ret += "\n";
    }
  }

  // send a command to all targets
  else {
    bool cmd_found = false;
    std::string baseid_message;

    baseid_message = this->own_handle_command(_args);
    if (baseid_message.find("is invalid", 0) == std::string::npos) { // defined command
      cmd_found = true;
      if (baseid_message != "") {
        ret += baseid_message + "\n";
      }
#if 0 // CommandCB
      // call call back funcion only when return message does not include error keyword
    if (ret.find("is invalid") == std::string::npos      // defined command
     && ret.find("Error") == std::string::npos) {        // correct argument
        std::string cb_msg = this->CommandCB(args);
        if (cb_msg.find("error") != std::string::npos) { // correct usage
          ret += cb_msg + "\n";
        }
      }
#endif // CommandCB
    }

    _args.insert(_args.begin(), "reg");
    baseid_message = this->regif_handle_command(_args);
    if (baseid_message.find("is invalid", 0) == std::string::npos) {
      cmd_found = true;
      if (baseid_message != "") {
        ret += baseid_message;
      }
    }
    _args.erase(_args.begin());

    _args.insert(_args.begin(), "tgt");
    baseid_message = this->tgt_handle_command(_args);
    if (baseid_message.find("is invalid", 0) == std::string::npos) {
      cmd_found = true;
      if (baseid_message != "") {
        ret += baseid_message;
      }
    }
    _args.erase(_args.begin());

    if (!cmd_found) {
      ret = "Error (" + mInstName + ") command name \"" + _args[0] + "\" is invalid.\n";
    }
  }
  
  // Insert input command
#if 0 // EnableInsertInput
  if (mEnableInsertInput) {
    std::string header_str = "(" + mInstName + + ":" + mCmdId + ") [ ";
    for (unsigned int i=0 ; i<args.size() ; i++) {
      header_str += (std::string)args[i] + " ";
    }
    header_str += "]";
    ret = header_str + "\n" + ret;
  }
#endif // EnableInsertInput

#if 0 // EnableDumpResult
  if (mEnableDumpResult && mCmdId == "self") {
    printf("%s", ret.c_str());
  }
#endif // EnableDumpResult
  return ret;
}

#if 0 // HWBreak
void setCommandHandler(vpcl::commandHandler * handler) {
  if (handler == NULL) return;
  cmd_handler = handler;
}
void hw_break(std::string break_name) {
  // if specified break point is enable, have commandHandler read a script file
  if (mBreakTable.find(break_name) != mBreakTable.end()) {
    cmd_handler->handleCommand();
  }
}
#endif // HWBreak

private:
std::string own_handle_command(std::vector<std::string>& args)
{
  std::string ret = "";
  std::string err_msg = "Error ("   + mInstName + ") ";   // error message

  std::string specified_cmd = " (";
  std::vector<std::string>::iterator cmd_it;
  for (cmd_it=args.begin() ; cmd_it!=args.end() ; cmd_it++) {
    specified_cmd += " " + *cmd_it;
  }
  specified_cmd += " ) : Type " + mInstName + " help";

  if ((int)args.size() <= 0) {
      ret = err_msg + "command name \"\" is invalid.";
      return ret;
  }

  if (args[0] == "help") {
    ret += "    --- command ---\n";
    ret += "    help                                      Show direction\n";
#if 0 // HWBreak
    ret += "    go                                        (Re)Start the simulation\n";
    ret += "    HWBreak <break-name>                      Enable specified break point (Defailt: none)\n";
#endif // HWBreak
#if 1 // MessageLevel
    ret += "    MessageLevel   <fatal|error|warning|info> Select debug message level (Default: fatal,error)\n";
#endif // MessageLevel
#if 0 // EnableInsertInput
    ret += "    EnableInsertInput <true/false>            Display typed command (Default: false)\n";
#endif // EnableInsertInput
#if 0 // DumpProfile
    ret += "    DumpProfile [file_name] [time_unit]       Dump profile information (Default: file_name=stdout, time_unit=SC_NS)\n";
#endif // DumpProfile
#if 0 // ClearProfile
    ret += "    ClearProfile                              Reset profile internal information\n";
#endif // ClearProfile
#if 0 // AssertReset
    ret += "    AssertReset <start_time> <period>         Assert and deassert reset signal to a target model\n";
#endif // AssertReset
#if 0 // EnableDumpResult
    ret += "    EnableDumpResult <status>                 Dump a command return string into standard output too\n";
#endif // EnableDumpResult
    ret += "    freq_CLK <frequency>                      Frequency of PCLK (unit:Hz). Default value is 100000 Hz ( Default:100000  )\n";
    ret += "    DumpInterrupt <enable>                    Display interrupt information. False if not to dump interrupt information. True if dump interrupt information. ( Default:\"false\"  )\n";
    ret += "    update_registers                          Copy the register value\n";

  }
#if 1 // MessageLevel
  else if (args[0] == "MessageLevel") {
    // read mode
    if ((int)args.size() == 1) {
      std::map<std::string, bool>::iterator it;
      for (it = mMessageLevel.begin(); it != mMessageLevel.end(); it++) {
        if (it->second) {
          ret += " " + it->first;
        }
      }
    }
    // write mode
    else if ((int)args.size() == 2) {
      // update to new setting
      mMessageLevel.clear();

      std::vector<std::string> arg_vec = str2vec(args[1], '|');
      std::vector<std::string>::iterator it;
      for (it = arg_vec.begin(); it != arg_vec.end(); it++) {
        mMessageLevel[*it] = true;
      }
    }
    else {
      ret = err_msg + "wrong number of arguments" + specified_cmd;
    }
  }
#endif // MessageLevel
#if 0 // HWBreak
  else if (args[0] == "go") {
    if ((int)args.size() == 1) {
      cmd_handler->setContinue();
    }
    else {
      ret = err_msg + "wrong number of arguments" + specified_cmd;
    }
  }
  else if (args[0] == "HWBreak") {
    // read mode
    if ((int)args.size() == 1) {
      std::map<std::string, bool>::iterator it;
      for (it = mBreakTable.begin(); it != mBreakTable.end(); it++) {
        if (it->second) {
          ret += " " + it->first;
        }
      }
    }
    // write mode
    else if ((int)args.size() == 2) {
      // update to new setting
      mBreakTable.clear();
      std::vector<std::string> arg_vec = str2vec(args[1], '|');
      std::vector<std::string>::iterator it;
      for (it = arg_vec.begin(); it != arg_vec.end(); it++) {
        mBreakTable[*it] = true;
      }
    }
    else {
      ret = err_msg + "wrong number of arguments" + specified_cmd;
    }
  }
#endif // HWBreak
#if 0 // EnableInsertInput
  else if (args[0] == "EnableInsertInput") {
    // read mode
    if ((int)args.size() == 1) {
      if (mEnableInsertInput) {
        ret = "true";
      }
      else if (!mEnableInsertInput) {
        ret = "false";
      }
    }
    // write mode
    else if ((int)args.size() == 2) {
      if (args[1] == "true") {
        mEnableInsertInput = true;
      }
      else if (args[1] == "false") {
        mEnableInsertInput = false;
      }
      else {
        ret = err_msg + "wrong argument" + specified_cmd;
      }
    }
    else {
      ret = err_msg + "wrong number of arguments" + specified_cmd;
    }
  }
#endif // EnableInsertInput
#if 0 // DumpProfile
  else if (args[0] == "DumpProfile") {
    if ((int)args.size() >= 4) {
      ret = err_msg + "wrong number of arguments" + specified_cmd;
      return ret;
    }
  
    sc_time_unit time_unit = SC_NS;
    if ((int)args.size() >= 3) {
           if (args[2] == "SC_SEC") time_unit = SC_SEC;
      else if (args[2] == "SC_MS" ) time_unit = SC_MS;
      else if (args[2] == "SC_US" ) time_unit = SC_US;
      else if (args[2] == "SC_NS" ) time_unit = SC_NS;
      else if (args[2] == "SC_PS" ) time_unit = SC_PS;
      else if (args[2] == "SC_FS" ) time_unit = SC_FS;
      else {
        ret = err_msg + "wrong argument" + specified_cmd;
        return ret;
      }
    }
  
    FILE *fp = stdout;
    if ((int)args.size() >= 2 && args[1] != "stdout") {
#ifdef __RE_SEQURITY_H
      if (fopen_s( &fp, args[1].c_str(), "a" ) != 0 )
#else
      fp = fopen(args[1].c_str(), "a");
      if (fp == NULL)
#endif
      {
        ret = "Cannot dump profile to " + (std::string)args[1];
        return ret;
      }
    }
  
    DumpProfile(fp, time_unit); // not check return value
    if (fp != stdout && fp != NULL) {
      fclose(fp);
    }
  }
#endif // DumpProfile
#if 0 // ClearProfile
  else if (args[0] == "ClearProfile") {
    if ((int)args.size() == 1) {
      ClearProfile();
    }
    else {
      ret = err_msg + "wrong number of arguments" + specified_cmd;
    }
  }
#endif // ClearProfile
#if 0 // AssertReset
  else if (args[0] == "AssertReset") {
    if ((int)args.size() == 3) {
      double start_time = 0, period = 0;
      bool IsTranslateCorrect = true;
      std::istringstream str_start_time(args[1]);
      std::istringstream str_period    (args[2]);

      str_start_time >> start_time;
      if (((unsigned int)str_start_time.rdstate() & (unsigned int)std::istringstream::failbit) != 0) {
        IsTranslateCorrect = false;
      }
      
      str_period >> period;
      if (((unsigned int)str_period.rdstate() & (unsigned int)std::istringstream::failbit) != 0) {
        IsTranslateCorrect = false;
      }

      if (start_time < 0 || period <= 0) {
        IsTranslateCorrect = false;
      }

      if (IsTranslateCorrect) {
        AssertReset(start_time, period);
      }
      else {
        ret = err_msg + "wrong argument: " + args[1] + " " + args[2]
            + specified_cmd;
      }
    }
    else {
      ret = err_msg + "wrong number of arguments" + specified_cmd;
    }
  }
#endif // AssertReset
#if 0 // EnableDumpResult
  else if (args[0] == "EnableDumpResult") {
    // read mode
    if (0) {;}
    else if ((int)args.size() == 1) {
      if (mEnableDumpResult) {
        ret = "true";
      }
      else {
        ret = "false";
      }
    }
    // write mode
    else if ((int)args.size() == 2) {
      if (args[1] == "true") {
        mEnableDumpResult = true;
      }
      else if (args[1] == "false") {
        mEnableDumpResult = false;
      }
      else {
        ret = err_msg + "wrong argument: " + (std::string)args[1] + specified_cmd;
      }
    }
    else {
      ret = err_msg + "wrong number of arguments" + specified_cmd;
    }
  }
#endif // EnableDumpResult
  else  {
    ret = user_def_command(args);
  }

  return ret;
}

std::string user_def_command(std::vector<std::string>& args)
{
  std::string ret = "";
  std::string err_msg = "Error ("   + mInstName + ") ";   // error message

  std::string specified_cmd = " (";
  std::vector<std::string>::iterator cmd_it;
  for (cmd_it=args.begin() ; cmd_it!=args.end() ; cmd_it++) {
    specified_cmd += " " + *cmd_it;
  }
  specified_cmd += " ) : Type " + mInstName + " help";

  if (0) {}
  else if (args[0] == "freq_CLK") {
    // read mode
    if (0) {;}
    else if ((int)args.size() == 1) {
      std::ostringstream stream;
      stream << freq_CLK;
      ret = stream.str();
    }
    // write mode
    else if ((int)args.size() == 2) {
      if (0) {
      }
      else {
        int write_val = 0;
        if (!str2num(args[1], write_val)) {
          ret += err_msg + "wrong argument: " + (std::string)args[1] + specified_cmd;
        } else {
          freq_CLK = write_val;
        }
      }
    }
    else {
      ret = err_msg + "wrong number of arguments" + specified_cmd;
    }
  }
  else if (args[0] == "DumpInterrupt") {
    // read mode
    if (0) {;}
    else if ((int)args.size() == 1) {
      ret = DumpInterrupt;
    }
    // write mode
    else if ((int)args.size() == 2) {
      DumpInterrupt = (std::string)args[1];
    }
    else {
      ret = err_msg + "wrong number of arguments" + specified_cmd;
    }
  }
  else if (args[0] == "update_registers") {
    if ((int)args.size() > 0 && (int)args.size() <= 1) {
      update_registers();
    }
    else {
      ret = err_msg + "wrong number of arguments" + specified_cmd;
    }
  }

  else {
    ret = err_msg + "command name \"command " + (std::string)args[0] + "\" is invalid.";
  }

  return ret;
}

#if 1 // reg
std::string regif_handle_command(std::vector<std::string>& args)
{
  std::string ret = "";
  std::string err_msg = "Error ("   + mInstName + ") ";   // error message

  std::string specified_cmd = " (";
  std::vector<std::string>::iterator cmd_it;
  for (cmd_it=args.begin() ; cmd_it!=args.end() ; cmd_it++) {
    specified_cmd += " " + *cmd_it;
  }
  specified_cmd += " ) : Type " + mInstName + " help";

  // dump register list
  if ((int)args.size() == 1) {
    vpcl::re_register *reg_p = first_reg_object();
    while (reg_p != NULL) {
      printf("%s\n", reg_p->name().c_str());
      reg_p = next_reg_object();
    }
  }
  else {
    bool reg_found = false;
    vpcl::re_register *reg_p = first_reg_object();
    std::vector<std::string> args_updated;
    std::copy( args.begin(), args.end(), back_inserter( args_updated ) );
    while (reg_p != NULL) {
      if (strmatch(args[1].c_str(), reg_p->name().c_str())) {
        reg_found = true;
        args_updated[1] = reg_p->name();
        ret += this->reg_handle_command(args_updated) + "\n";
      }
      reg_p = next_reg_object();
    }
    if (!reg_found) {
      ret = this->reg_handle_command(args_updated);
    }
  }

  return ret;
}
#endif

#if 0 // port
#define PORT_READ_WRITE(port_name, port_val, port_type, print_func, str2_func)  \
        if (port_obj != NULL) {                                                 \
          if ((int)args.size() == 1) {                                          \
            find_success = true;                                                \
            print_func(#port_name, port_val);                                   \
          }                                                                     \
          else if ((int)args.size() == 2) {                                     \
            port_type write_val = 0;                                            \
            if (str2_func(args[1], write_val)) {                                \
              find_success = true;                                              \
              mVal_##port_name = write_val;                                     \
            }                                                                   \
            else {                                                              \
              ret = err_msg + args.at(1) + " is wrong argument" + specified_cmd;\
            }                                                                   \
          }                                                                     \
        }                                                                       \

#define PORT_READ(port_name, port_val, print_func)                              \
        if (port_obj != NULL) {                                                 \
          if ((int)args.size() == 1) {                                          \
            find_success = true;                                                \
            print_func(#port_name, port_val);                                   \
          }                                                                     \
        }

void print_double(std::string port_name, double port_val) {
    printf("%-10s %s = %f\n", mInstName.c_str(), port_name.c_str(), port_val); 
}

template<typename T>
void print_uint(std::string port_name, T port_val) {
    printf("%-10s %s = %d\n", mInstName.c_str(), port_name.c_str(), port_val); 
}

template<typename T>
void print_uint64(std::string port_name, T port_val) {
    printf("%-10s %s = %llu\n", mInstName.c_str(), port_name.c_str(), port_val); 
}

std::string port_handle_command(std::vector<std::string> args)
{
  std::string ret = "";
  std::string err_msg = "Error ("   + mInstName + ") ";   // error message

  std::string specified_cmd = " (";
  std::vector<std::string>::iterator cmd_it;
  for (cmd_it=args.begin() ; cmd_it!=args.end() ; cmd_it++) {
    specified_cmd += " " + *cmd_it;
  }
  specified_cmd += " ) : Type " + mInstName + " help";

  // dump all port information
  if ((int)args.size() == 0) {
    std::vector<sc_object *> child_obj = get_child_objects();
    std::vector<sc_object *>::iterator it;

    for (it=child_obj.begin() ; it!=child_obj.end() ; it++) {
      std::string target = (*it)->name();
      if (target.find(mInstName) != std::string::npos
       && (!strcmp((*it)->kind(), "sc_in") || !strcmp((*it)->kind(), "sc_out"))) {
        printf("%-10s <sc_out> %s\n", mInstName.c_str(), (*it)->basename());
      }
    }
  }

  // Dump help message
  else if (args[0] == "help") {
    ret += "    --- port ---\n";
    ret += "    <port> <value>                     Set value to <port> output port\n";
  }

  // port write / read
  else if ((int)args.size() == 1 || (int)args.size() == 2) {
    std::vector<sc_object *> child_obj = get_child_objects();
    std::vector<sc_object *>::iterator it;
    bool find_success = false;

    for (it=child_obj.begin() ; it!=child_obj.end() ; it++) {
      std::string obj_name = (*it)->name();
      std::string arg_name = mInstName + "." + (std::string)args[0];
      if (strmatch(arg_name.c_str(), obj_name.c_str())) {

      }
    }

    if (!find_success) {
      ret = err_msg + "is invalid command name";
    }
  }
  else {
    ret = err_msg + "The argument number of " + args.at(0) + " is invalid" + specified_cmd;
  }

  return ret;
}

#undef PORT_READ_WRITE
#undef PORT_READ
#endif // port

bool str2num(std::string str, bool &num)
{
  std::istringstream is(str);
  int tmp;

  if (str.substr(0, 2) == "0x") {
    is >> std::hex >> tmp;
  }
  else if (str.substr(0, 1) == "0") {
    is >> std::oct >> tmp;
  } else {
    is >> std::dec >> tmp;
  }

  if (is.fail() || !is.eof()) {
    if (str == "true") {
      num = true;
    } else if (str == "false") {
      num = false;
    } else {
      return false;
    }
  } else {
    if (tmp == 1) {
      num = true;
    } else if (tmp == 0) {
      num = false;
    } else {
      return false;
    }
  }
  return true;
}

bool str2num(std::string str, char &num)
{
  std::istringstream is(str);
  int tmp;

  if (str.substr(0, 2) == "0x") {
    is >> std::hex >> tmp;
  }
  else if (str.substr(0, 1) == "0") {
    is >> std::oct >> tmp;
  }
  else {
    is >> std::dec >> tmp;
  }

  if (is.fail() || !is.eof()) {
    return false;
  }
  else {
    num = (char)tmp;
    return true;
  }
}

bool str2num(std::string str, unsigned char &num)
{
  std::istringstream is(str);
  unsigned int tmp;

  if (str.substr(0, 2) == "0x") {
    is >> std::hex >> tmp;
  }
  else if (str.substr(0, 1) == "0") {
    is >> std::oct >> tmp;
  }
  else {
    is >> std::dec >> tmp;
  }

  if (is.fail() || !is.eof()) {
    return false;
  }
  else {
    num = (unsigned char)tmp;
    return true;
  }
}

template<typename T>
bool str2num(std::string str, T &num)
{
  std::istringstream is(str);
  T tmp;

  if (str.substr(0, 2) == "0x") {
    is >> std::hex >> tmp;
  }
  else if (str.substr(0, 1) == "0") {
    is >> std::oct >> tmp;
  }
  else {
    is >> std::dec >> tmp;
  }

  if (is.fail() || !is.eof()) {
    return false;
  }
  else {
    num = tmp;
    return true;
  }
}

bool str2dbl(std::string str, double &num)
{
  std::istringstream is(str);
  double tmp;

  is >> tmp;

  if (is.fail() || !is.eof()) {
    return false;
  }
  else {
    num = tmp;
    return true;
  }
}

void CommandInit(std::string name = "") {
  // initialize internal variables
  mInstName = name;
  mLineNum = 0;
  mFileName = "";
  mCmdId = "self";

#if 0 // EnableDumpResult
  mEnableDumpResult = false;
#endif // EnableDumpResult

  // initialize pre-defined command
#if 0 // EnableInsertInput
  mEnableInsertInput = false;
#endif // EnableInsertInput
#if 1 // MessageLevel
  mMessageLevel.insert(std::pair<std::string, bool>("fatal",   true ));
  mMessageLevel.insert(std::pair<std::string, bool>("error",   true ));
  mMessageLevel.insert(std::pair<std::string, bool>("warning", false));
  mMessageLevel.insert(std::pair<std::string, bool>("info",    false));
#endif // MessageLevel
#if 0 // HWBreak
  mBreakTable["none"] = true;
  cmd_handler = NULL;
#endif // HWBreak

  // initialize user define command
  freq_CLK = 100000;
  DumpInterrupt = "false";


}

std::vector<std::string> str2vec(std::string str, const char sep)
{
  std::vector<std::string> buf;
  int index = 0;

  for (int i=0 ; i<(int)str.size() ; i++) {
    if (str[i] == sep) {
      if (i > index) {
        buf.push_back(str.substr((unsigned int)index, (unsigned int)(i-index)));
      }
      index = i+1;
    }
  }
  if (index < (int)str.size()) {
    buf.push_back(str.substr((unsigned int)index));
  }
  return buf;
}

void _re_printf(std::string group, const char *message, ...) {
  // message group check
#ifndef REGIF_SC_REPORT
#if 1 // MessageLevel
  if (!mMessageLevel[group]) return;
#endif // MessageLevel
#endif//REGIF_SC_REPORT

  if (message == NULL) return;

  // print header
  #ifdef SYSTEMC_H
  std::stringstream t_stream;
  std::string cur_time;

  double tu_value = 0;
  std::string tu_name = "";
  if (sc_get_time_resolution() < sc_time(1, SC_PS)) {
    tu_value = 1000000000000000LL;
    tu_name = " fs";
  }
  else if (sc_get_time_resolution() < sc_time(1, SC_NS)) {
    tu_value = 1000000000000LL;
    tu_name = " ps";
  }
  else if (sc_get_time_resolution() < sc_time(1, SC_US)) {
    tu_value = 1000000000;
    tu_name = " ns";
  }
  else if (sc_get_time_resolution() < sc_time(1, SC_MS)) {
    tu_value = 1000000;
    tu_name = " us";
  }
  else if (sc_get_time_resolution() < sc_time(1, SC_SEC)) {
    tu_value = 1000;
    tu_name = " ms";
  }
  else {
    tu_value = 1;
    tu_name = " s";
  }
  t_stream << sc_time_stamp().value() * (uint64) (sc_get_time_resolution().to_seconds() * tu_value) << tu_name;

  cur_time = t_stream.str();
  #else
  std::string cur_time = "";
  #endif

#ifndef REGIF_SC_REPORT
  #ifdef REGIF_MSG_COLOR
  // color setting
  //
  // severity color       color Linux       Windows
  // fatal    RED         RED   \033[0;31m  FOREGROUND_RED
  // error    RED         BLUE  \033[0;34m  FOREGROUND_BLUE
  // warning  RED
  // info     BLUE
  #ifdef _WIN32
  HANDLE hStdout;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (group == "fatal" || group == "error" || group == "warning") {
    SetConsoleTextAttribute(hStdout, FOREGROUND_RED);
  }
  else if (group == "info" ){
    SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE);
  }
  #else
  if (group == "fatal" || group == "error" || group == "warning") {
    printf("\033[0;31m");
  }
  else if (group == "info" ){
    printf("\033[0;34m");
  }
  #endif
  #endif
  printf("%8s [%20s] (%10s) ", group.c_str(), cur_time.c_str(), mInstName.c_str());
#endif//REGIF_SC_REPORT

  // print body
  va_list argptr;
  va_start(argptr, message);
  if (argptr == NULL) return;
#ifdef REGIF_SC_REPORT
  char str[1024];
  sprintf(str, "[%20s] ", cur_time.c_str());
  vsprintf(str+23, message, argptr);
  if (group == "fatal") {
    SC_REPORT_FATAL(mInstName.c_str(), str);
  }
  else if (group == "error") {
    SC_REPORT_ERROR(mInstName.c_str(), str);
  }
  else if (group == "warning") {
    SC_REPORT_WARNING(mInstName.c_str(), str);
  }
  else if (group == "info") {
    SC_REPORT_INFO(mInstName.c_str(), str);
  }
#else //REGIF_SC_REPORT
  vprintf(message, argptr);
#endif//REGIF_SC_REPORT

#ifndef REGIF_SC_REPORT
  #ifdef DumpFileNameLineNum
  // print footer
  if (group == "fatal" || group == "error") {
    printf(" [%s:%d]\n", mFileName.c_str(), mLineNum);
  }
  // legacy code
  //else {
  //  printf("\n");
  //}
  #endif//DumpFileNameLineNum
#endif//REGIF_SC_REPORT
  va_end(argptr);

#ifndef REGIF_SC_REPORT
  #ifdef REGIF_MSG_COLOR
  #ifdef _WIN32
  SetConsoleTextAttribute(hStdout, csbi.wAttributes);
  #else
  printf("\033[m");
  #endif
  #endif
#endif//REGIF_SC_REPORT

  fflush(stdout);
  fflush(stderr);

  if (group == "fatal") {
    exit(1);
  }
}

void get_fileline(std::string filename, int line_number) {
#ifdef _WIN32
  filename = filename.substr(filename.rfind("\\")+1);
#else
  filename = filename.substr(filename.rfind("/")+1);
#endif
  mFileName = filename;
  mLineNum  = line_number;
}

#ifndef re_printf
#define re_printf get_fileline(__FILE__, __LINE__); _re_printf
#endif//re_printf

bool strmatch(const char *ptn, const char *str) {
  bool ret;

  if (ptn == NULL || str == NULL) return false;

  switch( *ptn ) {
  case '\0':
    ret = '\0' == *str;
    break;
  case '*' :
    ret = strmatch( &ptn[1], str ) || (('\0' != *str) && strmatch( ptn, &str[1] ));
    break;
  case '?' :
    ret = ('\0' != *str) && strmatch( &ptn[1], &str[1] );
    break;
  default  :
    ret = ((unsigned char)*ptn == (unsigned char)*str) && strmatch( &ptn[1], &str[1] );
    break;
  }
  return ret;
}


// declare handleCommand relative members
#if 0 // EnableInsertInput
bool mEnableInsertInput;
#endif // EnableInsertInput
#if 1 // MessageLevel
std::map<std::string, bool> mMessageLevel;
#endif // MessageLevel
#if 0 // HWBreak
std::map<std::string, bool> mBreakTable;
vpcl::commandHandler* cmd_handler;
#endif // HWBreak

std::string mInstName;
std::string mFileName;
std::string mCmdId;
int mLineNum;
#if 0 // EnableDumpResult
bool mEnableDumpResult;
#endif // EnableDumpResult

int freq_CLK;
std::string DumpInterrupt;

#endif//CMT_CMDIF_H

#include "control_port.h"

void Ccontrol_port::Initialize(void)
{
  mPCLKupdate = false;
  mPRESETNupdate = false;

  pclk.initialize(0x0);
  preset_n.initialize(0x0);
  //irq.initialize(0x0);
}

bool Ccontrol_port::tgt_wr(unsigned int addr, unsigned char *p_data, unsigned int size, vpcl::tlm_if_extension *p_ext)
{
  bool result = true;
  bool update = false;
  unsigned int tmp_addr = addr;

  #ifdef IS_MODELED_ENDIAN_BIG
  tmp_addr = (/8 - size) ^ addr;
  #endif
  if (p_data == NULL)
  {
    result = false;
    printf("[%s] Error: Pointer data is NULL!\n", this->name());
  }
  else
  {
    if ((size == 1) || (size == 2) || (size == 4))
    {
      unsigned int data = 0x0;
      for (unsigned int idx = 0; idx < size; idx++)
      {
       unsigned int bshift = idx * 8;
        data = data | ((unsigned int)(p_data[idx]) << bshift);
      }
      std::vector<std::string> cmd;
      //Write register basing on address
      switch (tmp_addr & 0xFF)
      {
        case emADDR_PCLK0:
          (*REG_PCLK0) = data;
          mPCLKupdate = true;
          break;

        case emADDR_PCLK1:
          (*REG_PCLK1) = data;
          mPCLKupdate = true;
          break;

        case emADDR_PRESETN:
          (*REG_PRESETN) = data;
          mPRESETNupdate = true;
          break;

        case emADDR_RESET:
          (*REG_RESET) = data;
          break;

        case emADDR_LATENCY:
          (*REG_LATENCY) = data;
          break;
        
        case emADDR_HDLECOMM:
          if (data == 0x1)//FREQ_PCLK_SET
          {
            sc_dt::uint64 pclk0 = (*REG_PCLK0).read();
            sc_dt::uint64 pclk1 = (*REG_PCLK1).read();
            sc_dt::uint64 pclk = (pclk1 << 32) | pclk0;
            char tmp[255];
            sprintf(tmp,"%f",(double)pclk);
            cmd.push_back("SetCLKfreq");
            cmd.push_back("pclk");
            cmd.push_back(tmp);
            cmt->handleCommand(cmd);        
          }
          else if (data == 0x2)//FREQ_PCLK_GET
          {
            cmd.push_back("tgt");
            cmd.push_back("get_param");
            cmd.push_back("m_bus_clk");
            std::string str = cmt->handleCommand(cmd);
          }
          else if (data == 0x4)//ASSERTRESET
          {
            char start_str[255], period_str[255];
            unsigned int start = (unsigned int)(*REG_RESET)["START"];
            unsigned int period = (unsigned int)(*REG_RESET)["period"];
            sprintf(start_str,"%d",start);
            sprintf(period_str, "%d", period);
            cmd.push_back("AssertReset");
            cmd.push_back(start_str);
            cmd.push_back(period_str);
            cmt->handleCommand(cmd);
          } 
          else if (data == 0x8) 
          {
            cmd.push_back("DumpStatInfo");
            cmt->handleCommand(cmd);
          }
          else if (data == 0x10)
          {
            char latency_str[255];
            unsigned int latency = (*REG_LATENCY).read();
            sprintf(latency_str, "%d", latency);
            cmd.push_back(latency_str);
            cmt->handleCommand(cmd);
          }
          break;

      case emADDR_CTRL:
         if (data == 0x0)
         {
           return result;
         }
      }

    }
  }
}

bool Ccontrol_port::tgt_rd(unsigned int addr, unsigned char *p_data, unsigned int size, vpcl::tlm_if_extension *p_ext)
{
    bool result = true;
    unsigned int tmp_addr = addr;
    #ifdef IS_MODELED_ENDIAN_BIG
    tmp_addr = (/8 - size) ^ addr;
    #endif
    if(p_data == NULL)
    {
        result = false;
        printf("[%s] Error: Pointer data is Null!\n",this->name());
    } 
    else
    {                                        
        if((size == 1) || (size == 2) || (size == 4 ))
        { 
            unsigned int data = 0x0;
            //read register basing on address
            switch (tmp_addr & 0xFF) {
                case emADDR_PCLK0:
                     data = (*REG_PCLK0).read();
                     printf("info [%s] (%s) Read PCLK0 = 0x%X.\n", sc_time_stamp().to_string().c_str(), this->name(), data);
                     break;

                case emADDR_PCLK1:
                     data = (*REG_PCLK1).read();
                     printf("info [%s] (%s) Read PCLK1 = 0x%X.\n", sc_time_stamp().to_string().c_str(), this->name(), data);      
  
                case emADDR_PRESETN:
                     data = (*REG_PRESETN).read();
                     printf("info [%s] (%s) Read preset_n = 0x%X.\n", sc_time_stamp().to_string().c_str(), this->name(), data);
                     break;

                case emADDR_CTRL:
                     data = (*REG_CTRL).read();
                     printf("info [%s] (%s) Read CTRL = 0x%X.\n", sc_time_stamp().to_string().c_str(), this->name(), data);
                     break;

                case emADDR_RESET:
                     data = (*REG_RESET).read();
                     printf("info [%s] (%s) Read RESET = 0x%X.\n", sc_time_stamp().to_string().c_str(), this->name(), data);
                     break;

                case emADDR_LATENCY:
                     data = (*REG_LATENCY).read();
                     printf("info [%s] (%s) Read LATENCY = 0x%X.\n", sc_time_stamp().to_string().c_str(), this->name(), data);
                     break;

                case emADDR_HDLECOMM:
                     data = (*REG_HDLECOMM).read();
                     printf("info [%s] (%s) Read HDLECOMM = 0x%X.\n", sc_time_stamp().to_string().c_str(), this->name(), data);
                     break;

                default: 
                     result = false;
                     printf("[%s] Error: The address %08X of register is wrong\n",this->name(), tmp_addr);
                     break;
            }

            if (result = true) 
	    {
                for(unsigned int index=0; index < size; index++)
	        {
                    if(p_data != NULL)					
                    {
                         p_data[index] = (unsigned char) (data>>(index*8))&0xFF;
                    }
                }
            }
        } 
	else 
	{
            result = false;
            printf("[%s] Error: The size of register is wrong\n",this->name());
        }
    }
    return result;
}

bool Ccontrol_port::tgt_wr_dbg(unsigned int addr, unsigned char *p_data, unsigned int size, vpcl::tlm_if_extension *p_ext)
{
   return (tgt_wr (addr, p_data, size, p_ext));
} 

bool Ccontrol_port::tgt_rd_dbg(unsigned int addr, unsigned char *p_data, unsigned int size, vpcl::tlm_if_extension *p_ext)
{
   return (tgt_rd (addr, p_data, size, p_ext));
}

void Ccontrol_port::PortRead(void)
{
   printf("info [%s] (%s) Value of irq      : %d\n", sc_time_stamp().to_string().c_str(), this->name(), (unsigned int)irq.read()); 
}

void Ccontrol_port::irqMethod(void)
{
    bool read_val = irq.read();
    (*REG_IRQ) = (unsigned int) read_val;
    DumpPortInfo("irq", 0, (*REG_IRQ), false);
    mirqPreValue = read_val;
}

void Ccontrol_port::cb_REG_IRQ_IRQ(void)
{
    (*REG_IRQ)["IRQ"] = irq.read();
}


void Ccontrol_port::cb_REG_CTRL_CTRL(void)
{
    if ((*REG_CTRL)["CTRL"] == 1)
    {
        // Write Port
        (*REG_CTRL)["CTRL"] = 0;
    } 
    else 
    {
        // Read Port
        PortRead();
    }
}

void Ccontrol_port::DumpPortInfo(std::string port_name, unsigned int index_value, unsigned int value, bool index_enable)
{
    if (index_enable) 
    {
        printf("info [%s] (%s) Port %s[%d] is changed value to 0x%04X.\n", sc_time_stamp().to_string().c_str(), this->name(), port_name.c_str(), index_value, value);
    } 
    else 
    {
        printf("info [%s] (%s) Port %s is changed value to 0x%04X.\n", sc_time_stamp().to_string().c_str(), this->name(), port_name.c_str(), value);
    }
}

void Ccontrol_port::set_cmt_pointer(Ccmt *p_cmt)
{
  if (p_cmt != NULL)
  {
    this->p_cmt = p_cmt;
  }
}
//=======================================================
#ifndef __CONTROL_PORT_H__
#define __CONTROL_PORT_H__

#include "systemc.h"
#include "cmt.h"
#include "re_register.h"
#include "tlm_tgt_if.h"

using namespace vpcl;

class Ccontrol_port: public sc_module
                   , public vpcl::tlm_tgt_if<32>
                   , public vpcl::reg_super
{
public:
	enum ctrl_port_reg_addr
        {
          emADDR_PCLK0    = 0x00,
          emADDR_PCLK1    = 0x04,
          emADDR_PRESETN  = 0x08,
         // emADDR_BREAK    = 0x0C,
          emADDR_CTRL     = 0x0C,
          emADDR_HDLECOMM = 0x10,
          emADDR_RESET    = 0x14,
          emADDR_LATENCY  = 0x18,
          //emADDR_DATASIZE = 0x1C,      
	  emADDR_IRQ      = 0x1C,
        };

	Ccmt *cmt;
        sc_out<sc_dt::uint64> pclk;
        sc_out<bool> preset_n;
        sc_in<sc_uint<4> > irq;

        void set_cmt_pointer(Ccmt *p_cmt);
      

private:
       re_register *REG_PCLK0;
       re_register *REG_PCLK1;
       re_register *REG_PRESETN;
      // re_register *REG_BREAK;
       re_register *REG_CTRL;
       re_register *REG_HDLECOMM;
       re_register *REG_RESET;
       re_register *REG_LATENCY;
       //re_register *REG_DATASIZE;
       re_register *REG_IRQ;

       bool mPCLKupdate;
       bool mPRESETNupdate;
       //bool mBREAKupdate;
       sc_uint<4> mirqPreValue;
	  

public: 
       SC_HAS_PROCESS (Ccontrol_port);
       Ccontrol_port(sc_module_name name): sc_module(name)
                  , vpcl::tlm_tgt_if<32>(name)
                  , vpcl::reg_super()
                  , pclk("pclk")
                  , preset_n("preset_n")
                  , irq("irq")
      {
          Initialize();

          REG_PCLK0    = new re_register(emADDR_PCLK0,   this, "REG_PCLK0"    , name);
          REG_PCLK1    = new re_register(emADDR_PCLK1,   this, "REG_PCLK1"    , name);
          REG_PRESETN  = new re_register(emADDR_PRESETN, this, "REG_PRESTN"   , name);
	  //REG_BREAK    = new re_register(emADDR_BREAK,   this, "REG_BREAK"    , name);
          REG_CTRL     = new re_register(emADDR_CTRL,    this, "REG_CTRL"     , name);
          REG_HDLECOMM = new re_register(emADDR_HDLECOMM,this, "REG_HDLECOMM" , name);
  	  REG_RESET    = new re_register(emADDR_RESET,   this, "REG_RESET"    , name);
          REG_LATENCY  = new re_register(emADDR_LATENCY, this, "REG_LATENCY"  , name);
          //REG_DATASIZE = new re_register(emADDR_DATASIZE,this, "REG_DATASIZE" , name);
	  REG_IRQ      = new re_register(emADDR_IRQ     ,this, "REG_IRQ"      , name);

	 (*REG_PCLK0   )(31, 0, "PCLK0"        , 0x0, ACC_RW);
         (*REG_PCLK1   )(31, 0, "PCLK1"        , 0x0, ACC_RW);
         (*REG_PRESETN) (0,  0, "PRESETN"      , 0x0, ACC_RW);
         //(*REG_BREAK) (31, 0, "PCLK0", 0x0, ACC_RW);
         (*REG_CTRL    )(0,  0, "CTRL"         , 0x0, ACC_RD);
         (*REG_HDLECOMM)(0,  0, "FREQ_PCLK_SET", 0x0, ACC_RW); 
	 (*REG_HDLECOMM)(1,  1, "FREQ_PCLK_GET", 0x0, ACC_RW);
	 (*REG_HDLECOMM)(2,  2, "ASSERTRESET"  , 0x0, ACC_RW);
	 (*REG_HDLECOMM)(3,  3, "DUMPSTATINFO" , 0x0, ACC_RW);
         (*REG_RESET   )(15, 0, "START"        , 0x0, ACC_RW);
         (*REG_RESET   )(31, 16, "PERIOD"      , 0x0, ACC_RW);
         (*REG_LATENCY )(31, 0,  "LATENCY"     , 0x0, ACC_RW);
         //(*REG_DATASIZE)(4,  0,  "DATASIZE"    , 0x8, ACC_RW);
  	 (*REG_IRQ     )(3,  0,  "IRQ"          ,0x8, ACC_RW);

         
      }

      ~Ccontrol_port()
      {
        delete REG_PCLK0;
        delete REG_PCLK1;
        delete REG_PRESETN;
        //delete REG_BREAK;
        delete REG_CTRL;
        delete REG_RESET;
        delete REG_LATENCY;
        delete REG_IRQ;
        delete REG_HDLECOMM;
      }
      
      std::string handleCommand(const std::vector<std::string> &args)
      { 
        return "";
      }

private:
     Ccmt *p_cmt;
     bool tgt_wr(unsigned int addr, unsigned char *p_data, unsigned int size, vpcl::tlm_if_extension *p_ext);
     bool tgt_rd(unsigned int addr, unsigned char *p_data, unsigned int size, vpcl::tlm_if_extension *p_ext);
     bool tgt_wr_dbg(unsigned int addr, unsigned char *p_data, unsigned int size, vpcl::tlm_if_extension *p_ext);
     bool tgt_rd_dbg(unsigned int addr, unsigned char *p_data, unsigned int size, vpcl::tlm_if_extension *p_ext);
    // void tgt_acc(tlm::tlm_generic_payload &trans, sc_time &t);
     void Initialize(void);
     void PortRead(void);
     void DumpPortInfo(std::string port_name, unsigned int index_value, unsigned int value, bool index_enable);
     void irqMethod(void);
     void cb_REG_IRQ_IRQ(void);
     void cb_REG_CTRL_CTRL(void);
};

#endif //__CONTROL_PORT__H
//=========================================================================
#ifndef _ADOPTER_H_
#define _ADOPTER_H_

#include <cstdio>
#include "systemc.h"
#include "cpu.h"
#include "tlm_ini_if.h"
using namespace vpcl;

class Ccmt_adopter: public sc_module
                 , public vpcl::tlm_ini_if<32>
{
public:
    enum eDebug_command
    {
      emCmdRead,
      emCmdWrite,
      emCmdIgnore
    };
 
    enum eCMTAdopterINTFactor
    {
      emirq0 = 0xA00,
      emirq1 = 0xA20,
      emirq2 = 0xA40,
      emirq3 = 0xA60
    };
   //interrupt port
     sc_in<sc_uint<4> > irq;
     sc_in<bool> preset_n;
  // sc_in <bool > irq0;
  // sc_in <bool > irq1;
  // sc_in <bool > irq2;
  // sc_in <bool > irq3;
  // sc_in <bool > preset_n;

   SC_HAS_PROCESS (Ccmt_adopter);
   Ccmt_adopter(sc_module_name name): sc_module(name)
                                    , vpcl::tlm_ini_if<32>(name,0)
   {
     p_cpu = NULL;

     SC_THREAD(AccDebug_Thread);
     dont_initialize();
     sensitive << AccDebugEvent;

     SC_THREAD(irq0_Thread)
     dont_initialize();
     sensitive << irq;
 
     SC_THREAD(irq1_Thread);
     dont_initialize();
     sensitive << irq;

     SC_THREAD(irq2_Thread);
     dont_initialize();
     sensitive << irq;

     SC_THREAD(irq3_Thread)
     dont_initialize();
     sensitive << irq;
     

     //SC_THREAD(INT_Thread);
     //dont_initialize();
   }
   
   ~Ccmt_adopter()
    {
    }

    
    std::string handleCommand(const std::vector<std::string> &args)
    {
      if ((args.size() == 1) && (args[0] == "start_dbg_trans"))
      {
        AccDebugEvent.notify(10, SC_NS);
      }
      return "";
    }
   
    void set_cpu_pointer (Ccpu *cpu)
    {
      p_cpu = cpu;
    }

private:
    Ccpu *p_cpu;
    sc_event AccDebugEvent;
 
    void AccDebug_Thread()
    {
    } 
    void issue_trans()
    {
    }

    void irq0_Thread()
    {
      while(true)
      {
        if (((irq->read() && 0x1) != 0x0) && (preset_n->read() == 1))
        {
          wait(15, SC_NS);
          int irq0 = irq->read();
          printf ("[%s] [%s] INFO: irq0 is changed into 0x%08X!\n",this->name(), sc_time_stamp().to_string().c_str(), irq0);
          p_cpu->CpuSetInterrupt(15,emirq0);
        }
        wait();
      }
    }

    void irq1_Thread()
    {
      while(true)
      {
        if (((irq->read() && 0x2) == true) && (preset_n->read() == 1))
        {
          wait(15, SC_NS);
          int irq1 = irq->read();
          printf ("[%s] [%s] INFO: irq1 is changed into 0x%08X!\n",this->name(), sc_time_stamp().to_string().c_str(), irq1);
          p_cpu->CpuSetInterrupt(15,emirq1);
        }
        wait();
      }
   }
 
   void irq2_Thread()
    {
      while(true)
      {
        if (((irq->read() && 0x4) == true) && (preset_n->read() == 1))
        {
          wait(15, SC_NS);
          int irq3 = irq->read();
          printf ("[%s] [%s] INFO: irq2 is changed into 0x%08X!\n",this->name(), sc_time_stamp().to_string().c_str(), irq3);
          p_cpu->CpuSetInterrupt(15,emirq2);
        }
        wait();
      }
   }

   void irq3_Thread()
    {
      while(true)
      {
        if (((irq->read() && 0x8) == true) && (preset_n->read() == 1))
        {
          wait(15, SC_NS);
          int irq3 = irq->read();
          printf ("[%s] [%s] INFO: irq3 is changed into 0x%08X!\n",this->name(), sc_time_stamp().to_string().c_str(), irq3);
          p_cpu->CpuSetInterrupt(15,emirq3);
        }
        wait();
      }
   }

    void issue_dbg_trans(const char* description, unsigned long data, unsigned long expected_value, unsigned int addr, unsigned int size, unsigned int cmd = emCmdWrite)
    {
      dump_message ("info","%s %d byte\n", description, size);
      unsigned char p_data[8] = {0};
      memcpy(p_data, &data, 4);
      tlm::tlm_command command;
      tlm::tlm_generic_payload *trans = acquire();
      if (cmd == emCmdWrite)
      {
        command = tlm::TLM_WRITE_COMMAND;
      }
      else if (cmd == emCmdRead)
      {
        command = tlm::TLM_READ_COMMAND;
      }
      else
      {
        command = tlm::TLM_IGNORE_COMMAND;
      }
      trans->set_command(command);
      trans->set_address(addr);
      trans->set_data_ptr(p_data);
      trans->set_data_length(size);
      trans->set_streaming_width(size);
      trans->set_byte_enable_ptr(NULL);
      if (command == tlm::TLM_WRITE_COMMAND)
      {
        ini_acc(*trans);
        wait(5,SC_NS);

        trans = acquire();
        trans->set_command(command);
        trans->set_address(addr);
        trans->set_data_ptr(p_data);
        trans->set_data_length(size);
        trans->set_streaming_width(size);
        trans->set_byte_enable_ptr(NULL);
        //
        ini_acc_dbg(*trans);
        wait(5,SC_NS);
  
        unsigned char p_rd_data[8] = {0};
        command = tlm::TLM_READ_COMMAND;
        trans = acquire();
        trans->set_command(command);
        trans->set_address(addr);
        trans->set_data_ptr(p_rd_data);
        trans->set_data_length(size);
        trans->set_streaming_width(size);
        trans->set_byte_enable_ptr(NULL);

        ini_acc(*trans);
        wait(5, SC_NS);

        unsigned char p_rd_data_dbg[8] = {0};
        command = tlm::TLM_READ_COMMAND;
        trans = acquire();
        trans->set_command(command);
        trans->set_address(addr);
        trans->set_data_ptr(p_rd_data_dbg);
        trans->set_data_length(size);
        trans->set_streaming_width(size);
        trans->set_byte_enable_ptr(NULL);

        ini_acc_dbg(*trans);
        //wait(5, SC_NS);
        memcpy(&data, p_rd_data_dbg, 4);
      }
      else
      {
        ini_acc(*trans);
        trans = acquire();
        trans->set_command(command);
        trans->set_address(addr);
        trans->set_data_ptr(p_data);
        trans->set_data_length(size);
        trans->set_streaming_width(size);
        trans->set_byte_enable_ptr(NULL);
        ini_acc_dbg(*trans);     
      }
      if (data != expected_value)
      {
        dump_message("info","Debug Transaction Read 0x%X = %1X FAIL!\n", addr, data);
      }
      else
      {
        dump_message("info","Debug Transaction Read 0x%X = %1X PASS!\n", addr, data);
      }
 }

   void dump_message(std::string group, const char *message, ...) 
   {
     #ifdef SYSTEMC_H
     std::stringstream t_stream;
     std::string cur_time;
     double tu_value = 0;
     std::string tu_name = "";
     if (sc_get_time_resolution() < sc_time(1, SC_PS))
     {
       tu_value = 1000000000000000LL;
       tu_name = " fs";
     }
     else if (sc_get_time_resolution() < sc_time(1, SC_NS))
     {
       tu_value = 1000000000000LL;
       tu_name = " ps";
     }
     else if (sc_get_time_resolution() < sc_time(1, SC_US))
     {
       tu_value = 1000000000;
       tu_name = " fs";
     }

     else if (sc_get_time_resolution() < sc_time(1, SC_MS))
     {
       tu_value = 1000000;
       tu_name = " us";
     }
     else if (sc_get_time_resolution() < sc_time(1, SC_SEC))
     {
       tu_value = 1000;
       tu_name = " ms";
     }
     else
     {
       tu_value = 1;
       tu_name = " s";
     }
     //string stream to concenate strings together and not print to screen
     t_stream << sc_time_stamp().value() * (uint64) (sc_get_time_resolution().to_seconds() * tu_value) << tu_name;
     
     cur_time = t_stream.str();//str to get/set content string for stringstream
     #else 
     std::string cur_time = "";
     #endif
     printf("%8s [%20s] (%10s)", group.c_str(), cur_time.c_str(), this->name());;//c_str() return pointer to an array contains a null-terminated sequence of characters
     
     //print body
     va_list argptr;
     va_start(argptr, message);
     if (argptr == NULL) 
     {
       return;
     }
     vprintf(message, argptr);
     va_end(argptr);
     fflush(stdout);
     fflush(stderr);
   }

};
#endif
 


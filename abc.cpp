#include "cmt.h"

Ccmt::Ccmt(sc_module_name name):
            Ccmt_regif((std::string)name, 32)
          , Cgeneral_timer(name)
          , vpcl::tlm_tgt_if<32, tlm::tlm_base_protocol_types, 1>(name)
          , irq("irq")
           
{

    // Initialize output ports
    irq.initialize(0x0);

    // Initialize internal variables
    mfreq_CLK = 100000;//max 2^32
    mIRQVal = 0;

    mCMTReset = false;
    mCmdReset = false;
    mIsCmdResetStatus = false;
    mPCLKFreq = 0;
    mPCLKPeriod = 0;

    //Initialize clock
    Initialize_clock();
    // mIRQ3Val = 0;

    SC_METHOD(irqChannelMethod);
    dont_initialize(); 
    sensitive << irq;
  
    SC_METHOD(ResetMethod)
    dont_initialize();
    sensitive << preset_n;

    SC_METHOD(PCLKPeriodMethod)
    dont_initialize();
    sensitive << pclk;

   
    #ifdef CWR_SYSTEMC
    //handle command
    SCML_COMMAND_PROCESSOR(handleCommand);
    SCML_ADD_COMMAND("tgt", 1, 8, "tgt 0 <cmd> <arg0> <arg1> <arg2> ...", "prefix of tlm target if command");
    SCML_ADD_COMMAND("command", 1, 3, "command <param> <arg> <1/->", "prefix of setting module commands/parameters");
    SCML_ADD_COMMAND("reg", 1, 8, "reg <cmd> <arg0> <arg1> <arg2> ...", "prefix of register command");
    #endif 

}


Ccmt::~Ccmt()
{
  std::cout << "Run CMT sucessfully" << std::endl;
}

void Ccmt::Initialize_clock()
{
  std::cout << "***************************" << std::endl;
  std::cout << "Initialize clock" << std::endl;
  this->Cgeneral_timer::setClkCountPeriod(1/mfreq_CLK);
  mCMTReset = false;
  mCmdReset = false;
  mIsCmdResetStatus = false;
}
//transaction read and write 
void Ccmt::tgt_acc(tlm::tlm_generic_payload &trans, sc_time &t)
{
    tlm::tlm_command command;
    sc_dt::uint64 address = 0;
    unsigned int data_length = 0;
    unsigned char *data_ptr = NULL;
    bool status = this->tgt_get_gp_attribute(command, address, data_ptr, data_length, trans, false);

    sc_assert(data_ptr != NULL);
    if (command == tlm::TLM_READ_COMMAND)//command is read command
    {
      status = reg_rd((unsigned int)address, data_ptr, data_length);
    } 
    else if (command == tlm::TLM_WRITE_COMMAND)//write command
    {
      status = reg_wr((unsigned int)address, data_ptr, data_length);
    } 
    else
    {
        // Be necessarily TLM_IGNORE_COMMAND
      status = true;//igofre
    }


    if (status) 
    {
      trans.set_response_status(tlm::TLM_OK_RESPONSE);
    } 
    else 
    {
      trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    }
}

//used for debug read and write funtion 
unsigned int Ccmt::tgt_acc_dbg (tlm::tlm_generic_payload &trans)
{
    tlm::tlm_command command;
    sc_dt::uint64 address = 0;
    unsigned int data_length = 0;
    unsigned char *data_ptr = NULL;
    bool status = this->tgt_get_gp_attribute(command, address, data_ptr, data_length, trans, true);

    sc_assert(data_ptr != NULL);
    if (command == tlm::TLM_READ_COMMAND) //read command
    {
      status = reg_rd_dbg((unsigned int)address, data_ptr, data_length);
    } 
    else if (command == tlm::TLM_WRITE_COMMAND)//write command
    {
      status = reg_wr_dbg((unsigned int)address, data_ptr, data_length);
    } 
    else 
    {
      status = true;//igore commanad
    }
	
    if (status) 
    {
      trans.set_response_status(tlm::TLM_OK_RESPONSE);
      return data_length;
    } 
    else 
    {
      trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
      return 0;
    }
}


//**************************** METHOD *************************
void Ccmt::irqChannelMethod()
{
   // DumpInterruptMsg();
   std::cout << "Interurpt assert " << std::endl;
}

void Ccmt::PCLKPeriodMethod(void)
{
  if !(mCMTReset || mCmdReset)
  {
    SetCLKfreq("pckl", (double)pclk.read());
  }
}

//***********************************************************
void Ccmt::cmpMatchOutputHandling(bool value)
{
  (*CMCNT) = 0x00;
}


//====================== Interrupt Handling ==============================
void Ccmt::interruptHandling()
{
  if ((unsigned int)(*CMCR)["CMIE"] == emInterrupt_enable) 
  {
    mIRQVal = !mIRQVal;
    this->irq.write(mIRQVal);
  }
}

//====================== Start CLOCK ====================================
void Ccmt::cb_CMSTR_STR0(RegCBstr str)
{
    std::cout << "*************" << std::endl;
    std::cout << "Start counter" << std::endl;
    std::cout << "*************" << std::endl;

    if ((unsigned int)(*CMSTR)["STR0"] != (unsigned int)(str.pre_data & 0x1))
    {
        std::cout << "cb_CMSTR_STR0" << std::endl;
        if ((*CMSTR)["STR0"] == 1)
        {
          std::cout << " Bit STR0 equal 1" << std::endl;
          this->Cgeneral_timer::setSTR(true);
        } 
        else
        {
            // stop counter
          this->Cgeneral_timer::setSTR(false);
        }
    } 
}

//===================== Start CLOCK STR1 =============================
void Ccmt::cb_CMSTR_STR1(RegCBstr str)
{
}

//===================== set counter value ============================	
void Ccmt::cb_CMCNT_CMCNT(RegCBstr str)
{
    std::cout << "cb_CMCNT_CMCNT" << std::endl;
    if (str.is_wr) 
    {
      if (*CMCNT < *CMCMSE) 
      {
        if ((*CMCNT != (unsigned int)str.pre_data))
         {
            // stop counter
            this->Cgeneral_timer::setSTR(false);
            // Set counter value of channel 0
            this->Cgeneral_timer::setCNT((unsigned int)*CMCNT); 
            this->Cgeneral_timer::setSTR(true);
         }
       }
       else
       {
            re_printf("warning","Value of CM0CNT register must be smaller than value of CM0CMSE register.\n");
        }
    } 
    else
    {
        std::cout << "Register CMCNT callback read" << std::endl;
        (*CMCNT) =  this->Cgeneral_timer::getCNT();
        std::cout << "value of CMCNT: " << *CMCNT << std::endl;
    }
}


//========================= set constant value ====================
void Ccmt::cb_CMCMSE_CMCMSE(RegCBstr str)
{
    if ((unsigned int)(*CMCMSE) != (unsigned int)str.pre_data) 
    {
       if ((*CMSTR)["STR0"] == 0) //stop counter
       {
          std::cout << "Set compare match value of channel 0" << std::endl;
          this->Cgeneral_timer::setCOR((unsigned int)(*CMCMSE)); 
       } 
       else  
       {
          re_printf("warning","Cannot change CM0CMSE register while counter is operating.\n");
       }
    } 
}


//============================= CKS ==================================================
void Ccmt::cb_CMCR_CKS(RegCBstr str)
{
    unsigned int tmp_clock;
    if ((unsigned int)(*CMCR)["CKS"] != ((unsigned int)(str.pre_data & 0x3)))
    {
        if ((*CMSTR)["STR0"] == 0) 
        {
            // Set CKS 
          switch ((*CMCR)["CKS"])
          {
            case 0x0:
              tmp_clock = (unsigned int)(*CMCR)["CKS"]/8;
            break;

            case 0x1:
              tmp_clock = (unsigned int)(*CMCR)["CKS"]/32;
            break;

            case 0x2:
              tmp_clock = (unsigned int)(*CMCR)["CKS"]/128;
            break;

            case 0x4:
              tmp_clock = (unsigned int)(*CMCR)["CKS"]/512;
            break;

            default:
              tmp_clock = (unsigned int)(*CMCR)["CKS"];
            break;        
          }
          this->Cgeneral_timer::setCKS(tmp_clock);
        } 
        else
        {
            re_printf("warning","Cannot change CKS bit while counter is operating.\n");
        }
    } 
}


void Ccmt::DumpInterruptMsg()
{
    if (DumpInterrupt == "true") 
    {
      re_printf ("info","Interrupt is asserted.\n");
    }
}

void Ccmt::update_registers()
{
  #ifdef CWR_SYSTEMC
    cwmem.put((unsigned int)*CMSTR, 0x00);
   // cwmem.put((unsigned int)*CM23STR, 0x10);
    cwmem.put((unsigned int)*CMCR  , 0x02);
   // cwmem.put((unsigned int)*CM1CR  , 0x08);
   // cwmem.put((unsigned int)*CM2CR  , 0x12);
   // cwmem.put((unsigned int)*CM3CR  , 0x18);
  
    cwmem.put(this->Cgeneral_timer::getCNT, 0x04);
    cwmem.put(this->Cgeneral_timer::getCNT, 0x04);
  #endif
}

//========================== GET TIME RESOLUTION ====================
double Ccmt::GetTimeResolution(void)
{
  double tu_value = 0;
  if (sc_get_time_resolution() < sc_time(1, SC_PS))
  {
    tu_value = 1000000000000000LL;
  }
  else if (sc_get_time_resolution() < sc_time(1,SC_NS))
  {
    tu_value = 1000000000000LL;
  }
  else if (sc_get_time_resolution() < sc_time(1,SC_US))
  {
    tu_value = 1000000000;
  }
  
 else if (sc_get_time_resolution() < sc_time(1,SC_MS))
  {
    tu_value = 1000000;
  }
   else if (sc_get_time_resolution() < sc_time(1,SC_SEC))
  {
    tu_value = 1000;
  }
   else 
  {
    tu_value = 1;
  }
  
}

void Ccmt::EnableReset (const bool is_active)
{
  //if (is_active)
  Ccmt_regif::EnableReset(is_active);
  if (mCMTReset)
  {
    this->tgt_enable_reset(is_active);
  }
}
void Ccmt::ResetMethod(void)
{
  bool is_active = (preset_n.read() == vpcl::emResetActive) ? true : false;
  
  if (is_active && mIsCmdResetStatus)
  {
    mCmdResetEvent.cancel();
    mCancelCmdResetEvent.cancel();
    mIsCmdResetStatus = false;
    mCmdReset = false;   
  }

  if (is_active)
  {
    re_printf("info","Reset signal is asserted.\n");
    mCMTReset = is_active;
    this->EnableReset(is_active);
  }
  else
  {
    re_printf("info","Reset signal is negated.\n");
    mCMTReset = is_active;
    this->EnableReset(is_active);
  }
}
//========================== LATENCY TLM ============================
void Ccmt:: SetLatency_TLM(const double pclk_period, const bool is_constructor)
{
  vpcl::tlm_if_tgt_parameter tgt_param = vpcl::tlm_tgt_if<32>::tgt_get_param();
  if (is_constructor)
  {
    tgt_param.fw_req_phase = tlm::END_REQ;
  }
  sc_time new_clock (pclk_period, SC_NS);
  tgt_param.bus_clk = new_clock;
  tgt_param.rd_latency = tgt_param.rd_req_latency + 2 * tgt_param.bus_clk;
  tgt_param.wr_latency = tgt_param.wr_req_latency + 2 * tgt_param.bus_clk;
  vpcl::tlm_tgt_if<32>::tgt_set_param(&tgt_param);
}
//========================== Command CB ==============================
void Ccmt:: SetCLKfreq (std::string clk_name, double clk_freq)
{
  if (clk_name == "pclk")
  {
    mPCLKFreq = 0;
    mPCLKPeriod = 0;
 
   if (clk_freq <= 0)
   {
     re_pirntf("error","Clock frequency is invalid.\n");
   }
   else
   {
     mPCLKFreq = clk_freq;
     mPCLKPeriod = (double)(emNanoSecond / clk_freq);
     double time_unit = (double)(emNanoSecond / GetTimeResolution());
     if (time_unit > mPCLKPeriod)
     {
       re_printf("warning","The pclk period is less than 1 unit time of system.\n");
     }
     re_printf("info", "PCLK is set with a frequency as %lld Hz.\n", (unsigned long long)mPCLKFreq);
     SetLatency_TLM(mPCLKPeriod, false);
   }
  }
  else
  {
    re_printf("error","Clock name is invalid.\n");
  }
}

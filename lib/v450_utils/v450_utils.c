// -*- c++ -*-

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or (at
// your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Dean W. Anneser
// Company: RTLinux Solutions LLC for Highland Technology, Inc.
// Date: Sun Aug 08 11:37:38 2021

#include <stdio.h>
#include <string.h>
#include <unistd.h>

// prototypes

#include "deg_conv_proto.h"
#include "ht_utils_proto.h"
#include "misc_math_utils_proto.h"
#include "v450_utils_proto.h"

// structure defs

#include "v450_regs_sdef.h"

static int read_loop_resistance_macro_performed = 0;


// v450_config_rtd_type(pb, 'a', V450_RTD_TYPE_100_OHM_PT)		# set RTD a to 100 ohm PT

int v450_config_rtd_type(V450_REGS *pb, char rtd_id, int type)
{
  switch(rtd_id)
    {
    case 'a':
      switch(type)
        {
        case '0':
          pb->rtda = V450_RTD_TYPE_UNUSED;
          break;
        case 1:
          pb->rtda = V450_RTD_TYPE_100_OHM_PT; // set to 100 ohm platinum
          break;
        case 2:
          pb->rtda = V450_RTD_TYPE_1000_OHM_PT; // set to 1000 ohm platinum
          break;
        default:
          return(-1);
        }
      break;

    case 'b':
      switch(type)
        {
        case '0':
          pb->rtdb = V450_RTD_TYPE_UNUSED;
          break;
        case 1:
          pb->rtdb = V450_RTD_TYPE_100_OHM_PT; // set to 100 ohm platinum
          break;
        case 2:
          pb->rtdb = V450_RTD_TYPE_1000_OHM_PT; // set to 1000 ohm platinum
          break;
        default:
          return(-1);
        }
      break;

    case 'c':
      switch(type)
        {
        case '0':
          pb->rtdc = V450_RTD_TYPE_UNUSED;
          break;
        case 1:
          pb->rtdc = V450_RTD_TYPE_100_OHM_PT; // set to 100 ohm platinum
          break;
        case 2:
          pb->rtdc = V450_RTD_TYPE_1000_OHM_PT; // set to 1000 ohm platinum
          break;
        default:
          return(-1);
        }
      break;

    case 'd':
      switch(type)
        {
        case '0':
          pb->rtdd = V450_RTD_TYPE_UNUSED;
          break;
        case 1:
          pb->rtdd = V450_RTD_TYPE_100_OHM_PT; // set to 100 ohm platinum
          break;
        case 2:
          pb->rtdd = V450_RTD_TYPE_1000_OHM_PT; // set to 1000 ohm platinum
          break;
        default:
          return(-1);
        }
      break;

    default:
      return(-1);
    }
  return(0);
}


// v450_read_rtd_type(pb, 'b', &type)		# read RTD b type (a-d) and load into parameter type (0 = unused, 1 = 100 ohm PT, or 2 = 1000 ohm PT)

int v450_read_rtd_type(V450_REGS *pb, char rtd_id, int *ptype)
{
  switch(rtd_id)
    {
    case 'a':
      *ptype = (int)pb->rtda;
      break;

    case 'b':
      *ptype = (int)pb->rtdb;
      break;

    case 'c':
      *ptype = (int)pb->rtdc;
      break;

    case 'd':
      *ptype = (int)pb->rtdd;
      break;

    default:
      return(-1);
    }

  return(0);
}


// v450_read_rtd_temp(pb, 'r', 'F', &temp)	# read reference (internal) temp (a-d,r,1,2), convert to Fahrenheit, and load into parameter temp

int v450_read_rtd_temp(V450_REGS *pb, char rtd_id, char deg_spec, float *ptemp)
{
  float deg;
  float degC;

  switch(rtd_id)
    {
    case 'a':
      degC = (float)pb->tempa / 16.0;
      break;

    case 'b':
      degC = (float)pb->tempb / 16.0;
      break;

    case 'c':
      degC = (float)pb->tempc / 16.0;
      break;

    case 'd':
      degC = (float)pb->tempd / 16.0;
      break;

    case 'r':
      degC = (float)pb->tempr / 16.0;
      break;

    case '1':
      degC = (float)pb->fake1 / 16.0;
      break;

    case '2':
      degC = (float)pb->fake2 / 16.0;
      break;

    default:
      return(-1);
    }

  switch(deg_spec)
    {
    case 'C':
      deg = degC;
      break;
    case 'F':
      deg = degC_to_degF(degC);
      break;
    case 'K':
      deg = degC_to_degK(degC);
      break;
    case 'R':
      deg = degC_to_degR(degC);
      break;
    default:
      return(-1);
    }
  
  *ptemp = deg;
  return(0);
}


// v450_read_rtd_ohms(pb, 't', &ohms)		# read internal test resistor resistance in ohms, and load into parameter ohms

int v450_read_rtd_ohms(V450_REGS *pb, char rtd_id, float *pohms)
{
  switch(rtd_id)
    {
    case 'a':
      *pohms = (float)ht_read_interlocked(&pb->rahi) / 65536.0;
      break;

    case 'b':
      *pohms = (float)ht_read_interlocked(&pb->rbhi) / 65536.0;
      break;

    case 'c':
      *pohms = (float)ht_read_interlocked(&pb->rchi) / 65536.0;
      break;

    case 'd':
      *pohms = (float)ht_read_interlocked(&pb->rdhi) / 65536.0;
      break;

    case 't':
      *pohms = (float)ht_read_interlocked(&pb->trhi) / 65536.0;
      break;
      
    default:
      return(-1);
    }

  return(0);
}


// v450_write_fake_rtd_temp(pb, '1', 'C', 100.0)	# write 100 degC into RTD fake 1

int v450_write_fake_rtd_temp(V450_REGS *pb, char rtd_id, char deg_spec, float temp)
{
  float degC;
  
  switch(deg_spec)
    {
    case 'C':
      degC = temp;
      break;
    case 'F':
      degC = degF_to_degC(temp);
      break;
    case 'K':
      degC = degK_to_degC(temp);
      break;
    case 'R':
      degC = degR_to_degC(temp);
      break;
    default:
      return(-1);
    }
  
  switch(rtd_id)
    {
    case '1':
      pb->fake1 = degC * 16.0;
      break;
    case '2':
      pb->fake2 = degC * 16.0;
      break;
    default:
      return(-1);
    }
  return(0);
}


int v450_read_reg(V450_REGS *pb, unsigned short offset, unsigned short *reg_value)
{
  *reg_value = *(unsigned short *)((void *)pb + (offset & 0xfffe));
  return(0);
}


int v450_read_interlock_reg(V450_REGS *pb, unsigned short offset, unsigned int *reg_value)
{
  *reg_value = ht_read_interlocked((unsigned short *)((void *)pb + (offset & 0xfffc)));
  return(0);
}


// v450_config_analog_chnl(pb, chnl_num, rf_code, rn_code, ocd_enable_flag)

int v450_config_analog_chnl(V450_REGS *pb, unsigned int chnl_num, unsigned int rf_code, unsigned int rn_code, unsigned int ocd_flag)
{
  unsigned short chnl_ctl = 0;

  chnl_ctl = (((unsigned short)rn_code << V450_RN_CODE_SHIFT) | ((unsigned short)rf_code << V450_RF_CODE_SHIFT));
  if (rn_code <= V450_RN_CODE_500_MV)
    chnl_ctl |= ((unsigned short)ocd_flag << V450_OCD_SHIFT);
  pb->ccb[chnl_num].ctl = chnl_ctl;

  return(0);
}


// v450_config_tc_chnl(pb, chnl_num, rf_code, rs_code, rn_code, ocd_enable_flag, dft_enable_flag, loop_resistance_enable_flag)

int v450_config_tc_chnl(V450_REGS *pb, unsigned int chnl_num, unsigned int rf_code, unsigned int rs_code, unsigned int rn_code, unsigned int ocd_flag, unsigned int dft_code, unsigned int loop_resistance_enable_flag)
{
  unsigned short chnl_ctl = 0;
  static unsigned short digital_filter[V450_CHNLS_PER_BOARD];
  unsigned short param0 = 0;

  chnl_ctl = (((unsigned short)rn_code << V450_RN_CODE_SHIFT) | ((unsigned short)rs_code << V450_RS_CODE_SHIFT) | ((unsigned short)rf_code << V450_RF_CODE_SHIFT));
  pb->ccb[chnl_num].ctl = chnl_ctl;
  
  if (dft_code != V450_DF_NONE)
    {
      if ((char)pb->romrev < 'E')
        return(-1);
      
      if (v450_execute_macro(pb, V450_MACRO_GET_DFILT, 0, digital_filter) == -1)
        return(-1);

      digital_filter[chnl_num] = dft_code;

      if (v450_execute_macro(pb, V450_MACRO_SET_DFILT, 0, digital_filter) == -1)
        return(-1);
    }

  if (loop_resistance_enable_flag != 0)
    {
      param0 = 1 << chnl_num;
      if (v450_execute_macro(pb, V450_MACRO_MEA_TC_LOOP_RES, param0, 0) == -1)
        return(-1);
    }

  return(0);
}


// v450_read_loop_resistance(pb, chnl_num, &loop_resistance)

int v450_read_loop_resistance(V450_REGS *pb, int chnl_num, float *pohms_data)
{
  if (read_loop_resistance_macro_performed == 0)
    return(-1);
  *pohms_data = (float)(pb->ccb[chnl_num].res) / 4.0;
  return(0);
}


// v450_read_analog_chnl(pb, chnl_num, &volts_data, &counts)	# read volts and optional raw counts

int v450_read_analog_chnl(V450_REGS *pb, int chnl_num, float *pvolts_data, int *pcounts)
{
  int counts;
  unsigned short rn_code;
  float vmin, vmax;
  float volts;

  counts = (int)ht_read_interlocked((volatile unsigned short *)&pb->counts[chnl_num].dh);
  counts /= 256;
  
  if (pcounts != (int *)0)
    *pcounts = counts;

  if (v450_chnl_to_rn_code(pb, chnl_num, &rn_code) == -1)
    return(-1);

  if (v450_rn_code_to_range(rn_code, &vmin, &vmax) == -1)
    return(-1);

  volts = (float)ratio((double)counts, V450_COUNTS_MIN_24, V450_COUNTS_MAX_24, vmin, vmax);
  *pvolts_data = volts;
  return(0);
}


// v450_read_tc_chnl(pb, chnl_num, deg_spec, &temp))

int v450_read_tc_chnl(V450_REGS *pb, int chnl_num, char deg_spec, float *ptemp)
{
  int counts;
  unsigned short rn_code;
  float degC;
  float tmin, tmax;

  counts = (int)pb->counts[chnl_num].dh;
  if (counts == 0x8000)
    return(-1);

  degC = (float)counts / 16.0;

  if (v450_chnl_to_rn_code(pb, chnl_num, &rn_code) == -1)
    return(-1);

  if (v450_rn_code_to_range(rn_code, &tmin, &tmax) == -1)
    return(-1);

  degC = (float)clamp((double)degC, (double)tmin, (double)tmax);

  switch(deg_spec)
    {
    case 'C':
      *ptemp = degC;
      break;
    case 'F':
      *ptemp = degC_to_degF(degC);
      break;
    case 'K':
      *ptemp = degC_to_degK(degC);
      break;
    case 'R':
      *ptemp = degC_to_degR(degC);
      break;
    default:
      return(-1);
    }
  
  return(0);
}


// execute macros

int v450_execute_macro(V450_REGS *pb, unsigned short macro, unsigned short param0, unsigned short *arg)
{
  int i;
  int rtc;
  
  switch(macro)
    {
    case V450_MACRO_NOOP:
      return(0);

    case V450_MACRO_SETALL_TYPE_J:	// set all channels to Type J thermocouple at 16.7 Hz, using the onboard reference junction
      return(ht_write_macro((unsigned short *)&pb->macro, V450_MACRO_SETALL_TYPE_J, 0, 0, 0));	// should complete in 1 msec

    case V450_MACRO_SETALL_TYPE_K:	// set all channels to Type K thermocouple at 16.7 Hz, using the onboard reference junction
      return(ht_write_macro((unsigned short *)&pb->macro, V450_MACRO_SETALL_TYPE_K, 0, 0, 0));	// should complete in 1 msec

    case V450_MACRO_SETALL_TYPE_E:	// set all channels to Type E thermocouple at 16.7 Hz, using the onboard reference junction
      return(ht_write_macro((unsigned short *)&pb->macro, V450_MACRO_SETALL_TYPE_E, 0, 0, 0));	// should complete in 1 msec

    case V450_MACRO_SETALL_TYPE_T:	// set all channels to Type T thermocouple at 16.7 Hz, using the onboard reference junction
      return(ht_write_macro((unsigned short *)&pb->macro, V450_MACRO_SETALL_TYPE_T, 0, 0, 0)); 	// should complete in 1 msec

    case V450_MACRO_SETALL_12_5_V:	// set all channels to +/- 12.5V at 16.7 Hz
      return(ht_write_macro((unsigned short *)&pb->macro, V450_MACRO_SETALL_12_5_V, 0, 0, 0));	// should complete in 1 msec

    case V450_MACRO_SETALL_80_MV:		// set all channels to +/- 80V at 16.7 Hz
      return(ht_write_macro((unsigned short *)&pb->macro, V450_MACRO_SETALL_80_MV, 0, 0, 0));	// should complete in 1 msec

    case V450_MACRO_SETALL_25_MV:		// set all channels to +/- 25V at 16.7 Hz
      return(ht_write_macro((unsigned short *)&pb->macro, V450_MACRO_SETALL_25_MV, 0, 0, 0));	// should complete in 1 msec

    case V450_MACRO_SET_DFILT:		// set digital filtering
      if (arg == (unsigned short *)0)
        return(-1);
      if ((char)pb->romrev < 'E')
        return(-1);
      for (i = 0; i < V450_CHNLS_PER_BOARD; i++)
        {
          switch(arg[i])
            {
            case V450_DF_NONE:		// digital filter disabled
            case V450_DF_1_HZ_BW:       // 1 Hz 16-pole Butterworth
            case V450_DF_2_HZ_BW:	// 2 Hz 16-pole Butterworth
            case V450_DF_5_HZ_BW:	// 5 Hz 16-pole Butterworth
            case V450_DF_10_HZ_BW:	// 10 Hz 16-pole Butterworth
            case V450_DF_20_HZ_BW:	// 20 Hz 16-pole Butterworth
            case V450_DF_50_HZ_BW:	// 50 Hz 16-pole Butterworth
            case V450_DF_100_HZ_BW:	// 100 Hz 16-pole Butterworth
            case V450_DF_1_HZ_BS:	// 1 Hz 16-pole Bessel
            case V450_DF_2_HZ_BS:	// 2 Hz 16-pole Bessel
            case V450_DF_5_HZ_BS:	// 5 Hz 16-pole Bessel
            case V450_DF_10_HZ_BS:	// 10 Hz 16-pole Bessel
            case V450_DF_20_HZ_BS:	// 20 Hz 16-pole Bessel
            case V450_DF_50_HZ_BS:      // 50 Hz 16-pole Bessel
            case V450_DF_100_HZ_BS:	// 100 Hz 16-pole Bessel
              pb->ext.filt.reg[i] = arg[i];
              break;
            default:
              return(-1);
            }
        }
      return(ht_write_macro((unsigned short *)&pb->macro, V450_MACRO_SET_DFILT, 0, 0, 0));	// should complete in 10 msec

    case V450_MACRO_GET_DFILT:		// get digital filtering setup
      if (arg == (unsigned short *)0)
        return(-1);
      if ((char)pb->romrev < 'E')
        return(-1);
      if (ht_write_macro((unsigned short *)&pb->macro, V450_MACRO_GET_DFILT, 0, 0, 0) == -1)	// should complete in 1 msec
        return(-1);
      for (i = 0; i < V450_CHNLS_PER_BOARD; i++)
        arg[i] = pb->ext.filt.reg[i];
      return(0);

    case V450_MACRO_GET_AFILT:		// get analog filtering types
      if (arg == (unsigned short *)0)
        return(-1);
      if (param0 == 0)
        return(-1);
      pb->param0 = param0;
      if (ht_write_macro((unsigned short *)&pb->macro, V450_MACRO_GET_AFILT, 0, 0, 0) == -1)	// should complete in 300 msec
        return(-1);
      for (i = 0; i < V450_CHNLS_PER_BOARD; i++)
        arg[i] = pb->ext.filt.reg[i];
      return(0);

    case V450_MACRO_SYNC:		// sync all channels -- PARAM0 bitmask specifies channels to sync
      return(ht_write_macro((unsigned short *)&pb->macro, V450_MACRO_SYNC, &param0, 1, 0));	// should complete in 1 msec

    case V450_MACRO_MEA_TC_LOOP_RES:	// measure thermocouple loop resistance -- PARAM0 bitmask specifies channels to measure
      read_loop_resistance_macro_performed = 1;
      return(ht_write_macro((unsigned short *)&pb->macro, V450_MACRO_MEA_TC_LOOP_RES, &param0, 1, 0));	// should complete in 3 secs

    case V450_MACRO_HARD_REBOOT:	// hard reboot -- reloads PFGSs, restarts code, disappears from bus for 4 seconds
      rtc = ht_write_macro((unsigned short *)&pb->macro, V450_MACRO_HARD_REBOOT, 0, 0, 1);	// should complete in 5 secs
      sleep(5);
      return(rtc);

    case V450_MACRO_SOFT_REBOOT:	// soft reboot -- remains on bus
      rtc = ht_write_macro((unsigned short *)&pb->macro, V450_MACRO_SOFT_REBOOT, 0, 0, 1);	// should complete in 1 sec
      sleep(2);
      return(rtc);

    case V450_MACRO_FULL_TEST:		// full test sequence -- tests all channels, as well as RTD anc temperature sensor subsystems
      return(ht_write_macro((unsigned short *)&pb->macro, V450_MACRO_FULL_TEST, 0, 0, 0));	// should complete in 80 secs

    case V450_MACRO_SINGLE_CHNL_TEST:	// single channel test sequence -- number 0-15 in PARAM0
      return(ht_write_macro((unsigned short *)&pb->macro, V450_MACRO_SINGLE_CHNL_TEST, &param0, 1, 0));	// should complete in 50 secs

    case V450_MACRO_MULT_CHNL_TEST:	// multiple channel test sequence -- bitmask in PARAM0
      return(ht_write_macro((unsigned short *)&pb->macro, V450_MACRO_MULT_CHNL_TEST, &param0, 1, 0));	// should complete in 80 secs

    default:
      return(-1);
    }
}


int v450_chnl_to_rn_code(V450_REGS *pb, int chnl, unsigned short *prn_code)
{
  *prn_code = (pb->ccb[chnl].ctl & V450_RN_CODE_MASK) >> V450_RN_CODE_SHIFT;
  return(0);
}


int v450_rn_code_to_range(int rn_code, float *pmin, float *pmax)
{
  if ((pmin == (float *)0) || (pmax == (float *)0))
    return(-1);

  switch(rn_code)
    {
    case V450_RN_CODE_OFF:
      *pmin = 0.0;
      *pmax = 0.0;
      break;
    case V450_RN_CODE_25_MV:
      *pmin = -0.025;
      *pmax =  0.025;
      break;
    case V450_RN_CODE_50_MV:
      *pmin = -0.050;
      *pmax =  0.050;
      break;
    case V450_RN_CODE_80_MV:
      *pmin = -0.080;
      *pmax =  0.080;
      break;
    case V450_RN_CODE_125_MV:
      *pmin = -0.125;
      *pmax =  0.125;
      break;
    case V450_RN_CODE_250_MV:
      *pmin = -0.25;
      *pmax =  0.25;
      break;
    case V450_RN_CODE_500_MV:
      *pmin = -0.5;
      *pmax =  0.5;
      break;
    case V450_RN_CODE_1_25_V:
      *pmin = -1.25;
      *pmax =  1.25;
      break;
    case V450_RN_CODE_2_5_V:
      *pmin = -2.5;
      *pmax =  2.5;
      break;
    case V450_RN_CODE_5_V:
      *pmin = -5.0;
      *pmax =  5.0;
      break;
    case V450_RN_CODE_12_5_V:
      *pmin = -12.5;
      *pmax =  12.5;
      break;
    case V450_RN_CODE_25_V:
      *pmin = -25.0;
      *pmax =  25.0;
      break;
    case V450_RN_CODE_50_V:
      *pmin = -50.0;
      *pmax =  50.0;
      break;
    case V450_RN_CODE_125_V:
      *pmin = -125.0;
      *pmax =  125.0;
      break;
    case V450_RN_CODE_250_V:
      *pmin = -250.0;
      *pmax =  250.0;
      break;
    case V450_RN_CODE_TYPE_J:		// -210 to 1200 degC
      *pmin = -210.0;
      *pmax = 1200.0;
      break;
    case V450_RN_CODE_TYPE_K:		// -270 to 1372 degC
      *pmin = -270.0;
      *pmax = 1372.0;
      break;
    case V450_RN_CODE_TYPE_E:		// -270 to 1000 degC
      *pmin = -270.0;
      *pmax = 1000.0;
      break;
    case V450_RN_CODE_TYPE_T:		// -270 to 400 degC
      *pmin = -270.0;
      *pmax = 400.0;
      break;
    case V450_RN_CODE_TYPE_R:		// -50 to 1768 degC
      *pmin = -50.0;
      *pmax = 1768.0;
      break;
    case V450_RN_CODE_TYPE_S:		// -50 to 1768 degC
      *pmin = -50.0;
      *pmax = 1768.0;
      break;
    case V450_RN_CODE_TYPE_B:		// 0 to 1820 degC
      *pmin = 0.0;
      *pmax = 1820.0;
      break;
    case V450_RN_CODE_TYPE_N:		// -270 to 1300 degC
      *pmin = -270.0;
      *pmax = 1300.0;
      break;
    default:
      return(-1);
    }
  return(0);
}


unsigned short v450_read_vximfr(V450_REGS *pb)
{
  return(pb->vximfr);
}


unsigned short v450_read_vxitype(V450_REGS *pb)
{
  return(pb->vxitype);
}


unsigned short v450_read_serial(V450_REGS *pb)
{
  return(pb->serial);
}


unsigned short v450_read_romid(V450_REGS *pb)
{
  return(pb->romid);
}


unsigned short v450_read_romrev(V450_REGS *pb)
{
  return(pb->romrev);
}


unsigned short v450_read_dfilt(V450_REGS *pb) // digital filter active flag
{
  return(pb->dfilt);
}


unsigned short v450_read_cflags(V450_REGS *pb)
{
  return(pb->cflags);
}


unsigned short v450_read_rflags(V450_REGS *pb)
{
  return(pb->rflags);
}


unsigned short v450_read_relays(V450_REGS *pb)
{
  return(pb->relays);
}


unsigned short v450_write_relays(V450_REGS *pb, unsigned short relays)
{
  pb->relays = relays;
  return(0);
}


unsigned short v450_read_uled(V450_REGS *pb)
{
  return(pb->uled);
}


unsigned short v450_write_uled(V450_REGS *pb, unsigned short uled)
{
  pb->uled = uled;
  return(0);
}


unsigned short v450_read_mode(V450_REGS *pb)
{
  return(pb->mode);
}


unsigned short v450_write_mode(V450_REGS *pb, unsigned short mode)
{
  pb->mode = mode;
  return(0);
}


unsigned short v450_read_calid(V450_REGS *pb)
{
  return(pb->calid);
}


unsigned short v450_read_biss(V450_REGS *pb)
{
  return(pb->biss);
}


unsigned short v450_read_ycal(V450_REGS *pb)
{
  return(pb->ycal);
}


unsigned short v450_read_dcal(V450_REGS *pb)
{
  return(pb->dcal);
}


unsigned short v450_read_bistdac(V450_REGS *pb)
{
  return(pb->bistdac);
}


unsigned short v450_write_bistdac(V450_REGS *pb, unsigned short range_code) // range_code: -15 to +15
{
  pb->bistdac = range_code;
  return(0);
}
    

unsigned short v450_read_bistblk(V450_REGS *pb, int blknum, unsigned short *bistblk) // bist block needs to point to an array of 6 unsigned short
{
  int i;
  unsigned short *pbistblk = (unsigned short *)&pb->ext.filt.reg[blknum];
  unsigned short *puserbistblk = bistblk;

  for (i = 0; i < 6; i++)
    *puserbistblk++ = *pbistblk++;

  return(0);
}


unsigned short v450_read_biststage(V450_REGS *pb)
{
  return(pb->ext.bist.stage);
}


unsigned short v450_read_bistnumerrors(V450_REGS *pb)
{
  return(pb->ext.bist.numerrors);
}


char *v450_get_cal_date(V450_REGS *pb)
{
  static char date_string[16];
  int year, month, day;

  memset((void *)date_string, 0, 16);
  year = (int)v450_read_ycal(pb);
  month = (int)v450_read_dcal(pb) >> 8;
  day = (int)v450_read_dcal(pb) & 0xff;
  sprintf(date_string, "%d/%d/%d", month, day, year);
  return(date_string);
}

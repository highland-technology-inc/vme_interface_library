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
// Date: Thu Aug 19 15:58:57 2021

#include <sys/types.h>
#include <sys/stat.h>

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// prototypes

#include "ht_utils_proto.h"		// prototypes for ht_read_interlocked(), ht_write_interlocked(), and ht_write_macro()
#include "misc_math_utils_proto.h"
#include "v545_utils_proto.h"     

// structure defs

#include "v545_regs_sdef.h"

#define SIGNED_MAX_DEG 180.0*65535.0/65536.0
#define SIGNED_MIN_DEG -180.0
#define UNSIGNED_MAX_DEG 360.0*65535.0/65536.0
#define UNSIGNED_MIN_DEG 0.0

#define DEBUG  1                // printf errors while executing library
#define EXCITATION_CHECK  0	// set to 0 to skip excitation status check -- for development only (using V548-1 transformers for synchro/resolver, instead of V548-6)


unsigned short v545_read_vximfr(V545_REGS *pb)
{
  return(pb->vximfr);
}


unsigned short v545_read_vxitype(V545_REGS *pb)
{
  return(pb->vxitype);
}


unsigned short v545_read_serial(V545_REGS *pb)
{
  return(pb->serial);
}


unsigned short v545_read_romid(V545_REGS *pb)
{
  return(pb->romid);
}


unsigned short v545_read_romrev(V545_REGS *pb)
{
  return(pb->romrev);
}


unsigned short v545_read_dash(V545_REGS *pb)
{
  return(pb->dash);
}


unsigned short v545_read_state(V545_REGS *pb)
{
  return(pb->state);
}


unsigned short v545_read_uled(V545_REGS *pb)
{
  return(pb->uled);
}


unsigned short v545_write_uled(V545_REGS *pb, unsigned short uled)
{
  pb->uled = uled;
  return(0);
}


unsigned short v545_read_biss(V545_REGS *pb)
{
  return(pb->biss);
}


unsigned short v545_read_calid(V545_REGS *pb)
{
  return(pb->calid);
}


unsigned short v545_read_ycal(V545_REGS *pb)
{
  return(pb->ycal);
}


unsigned short v545_read_dcal(V545_REGS *pb)
{
  return(pb->dcal);
}


unsigned short v545_read_swin(V545_REGS *pb) // J3 D9 input state
{
  return(pb->swin);
}


unsigned short v545_write_swout(V545_REGS *pb, unsigned short swout_mask)
{
  pb->swout = swout_mask;
  return(0);
}


int v545_read_reg(V545_REGS *pb, unsigned short offset, unsigned short *reg_value)
{
  *reg_value = *(unsigned short *)((void *)pb + (offset & 0xfffe));
  return(0);
}


// int v545_read_interlock_reg(V545_REGS *pb, unsigned short offset, unsigned int *reg_value)
// {
//   *reg_value = ht_read_interlocked((unsigned short *)((void *)pb + (offset & 0xfffc)));
//   return(0);
// }


char *v545_get_cal_date(V545_REGS *pb)
{
  static char date_string[16];
  int year, month, day;

  memset((void *)date_string, 0, 16);
  year = (int)v545_read_ycal(pb);
  month = (int)v545_read_dcal(pb) >> 8;
  day = (int)v545_read_dcal(pb) & 0xff;
  sprintf(date_string, "%d/%d/%d", month, day, year);
  return(date_string);
}


// write 3 word DDS block to user location pointed to by dds_block

int v545_read_dds_block(V545_REGS *pb, int dds_chnl, unsigned short *pdds_block)
{
  if ((dds_chnl < 0) || (dds_chnl >= V545_DDS_CHNLS_PER_BOARD))
    return(-1);

  *pdds_block++ = pb->dds[dds_chnl].dfr; // load frequency
  *pdds_block++ = pb->dds[dds_chnl].dph; // load phase
  *pdds_block++ = pb->dds[dds_chnl].dam; // load amplitude
  return(0);
}


// write 7 word CCB block to user location pointed to by ccb_block

int v545_read_ccb_block(V545_REGS *pb, int coil, unsigned short *pccb_block)
{
  if ((coil < 0) || (coil >= V545_CCB_CHNLS_PER_BOARD))
    return(-1);

  *pccb_block++ = pb->ccb[coil].ctl.reg;
  *pccb_block++ = pb->ccb[coil].sts.reg;
  *pccb_block++ = pb->ccb[coil].amp;
  *pccb_block++ = pb->ccb[coil].dly;
  *pccb_block++ = pb->ccb[coil].rms;
  *pccb_block++ = pb->ccb[coil].psd;
  *pccb_block++ = pb->ccb[coil].frq;
  return(0);
}


// get transformer type for coil

char *v545_read_xfmr_type(V545_REGS *pb, int coil)
{
  V545_CCB_REGS *pccb = (V545_CCB_REGS *)&pb->ccb[coil];
  static char *xfmr_type_str[] = {"none", "V548-1", "V546-1", "none", "none", "none", "V548-6"};
  unsigned short __attribute__((unused)) xfmr_type;

  xfmr_type = pccb->sts.bits.xfmr_type;
  return(xfmr_type_str[pccb->sts.bits.xfmr_type]);
}


// print flash information

char *v545_sprintf_checksum_flash(V545_REGS *pb)
{
  char *flash_state[] = {"Image not present", "Checksum ok", "Checksum fail", "Undefined"};
  unsigned short result[2] = {0, 0};
  static char msg[128];

  memset((void *)msg, 0, sizeof(msg));

  if (v545_execute_macro(pb, V545_MACRO_FLASH_CHECKSUM, 0, result) == -1)
    return((char *)-1);

  strcpy(msg, "Factory FPGA: ");
  strcat(msg, flash_state[((pb->param0 >> 8) & 0x3)]);
  strcat(msg, "; ");

  strcat(msg, "Factory Firmware: ");
  strcat(msg, flash_state[(pb->param0 & 0x3)]);
  strcat(msg, "; ");

  strcat(msg, "Upgrade FPGA: ");
  strcat(msg, flash_state[((pb->param1 >> 8) & 0x3)]);
  strcat(msg, "; ");

  strcat(msg, "Upgrade Firmware: ");
  strcat(msg, flash_state[(pb->param1 & 0x3)]);
  strcat(msg, "; ");

  return(msg);
}


// update flash with new flash file

int v545_load_flash(V545_REGS *pb, char *flash_filename, int *percent_complete)
{
  int i;
  unsigned short buffer[V545_REGFILE_WORD_SIZE];
  struct stat sb;               // sb.st_size == total filesize in bytes
  int num_bytes_read;
  unsigned short num_page = 0;
  FILE *fp;

  if (stat(flash_filename, &sb) == -1)
    {
#if DEBUG == 1
      printf("ERROR %s: flash file does not exist: %s\n", __FUNCTION__, flash_filename);
#endif
      return(-1);
    }

  if ((fp = fopen(flash_filename, "r")) == (FILE *)0)
    {
#if DEBUG == 1
      printf("ERROR %s: cannot open %s for reading\n", __FUNCTION__, flash_filename);
#endif
      return(-1);
    }

  if (v545_execute_macro(pb, V545_MACRO_FLASH_UNLOCK, 0, 0) == -1)
    {
#if DEBUG == 1
      printf("ERROR %s: execute flash unlock macro\n", __FUNCTION__);
#endif
      return(-1);
    }

 read_next_buffer:

  memset((void *)buffer, 0, sizeof(buffer));
  if ((num_bytes_read = fread((void *)buffer, 1, sizeof(buffer), fp)) == 0)
    goto end;

  for (i = 0; i < V545_REGFILE_WORD_SIZE; i++)
    pb->regfile.raw_regs[i] = buffer[i];

  if (percent_complete != (int *)0)
    *percent_complete = (int)((((float)num_page * (float)(sizeof(buffer))) / (float)sb.st_size) * 100.0);

  if (v545_execute_macro(pb, V545_MACRO_FLASH_WRITE, &num_page, 0) == -1)
    {
#if DEBUG == 1
      printf("ERROR %s: execute flash write macro for num_page = %d failure\n", __FUNCTION__, num_page);
#endif
      return(-1);
    }

  num_page += 1;
  goto read_next_buffer;

 end:

  if (v545_execute_macro(pb, V545_MACRO_RESET, 0, 0) == -1)
    {
#if DEBUG == 1
      printf("ERROR %s: execute macro reset failure\n", __FUNCTION__);
#endif
      return(-1);
    }

  if (percent_complete != (int *)0)
    *percent_complete = 100;

  fclose(fp);
  return(0);
}


// verify flash with new flash file

int v545_verify_flash(V545_REGS *pb, char *flash_filename, int *percent_complete)
{
  int i;
  unsigned short buffer[V545_REGFILE_WORD_SIZE];
  struct stat sb;               // sb.st_size == total filesize in bytes
  int num_bytes_read;
  unsigned short num_page = 0;
  FILE *fp;

  if (stat(flash_filename, &sb) == -1)
    {
#if DEBUG == 1
      printf("ERROR %s: flash file does not exist: %s\n", __FUNCTION__, flash_filename);
#endif
      return(-1);
    }

  if ((fp = fopen(flash_filename, "r")) == (FILE *)0)
    {
#if DEBUG == 1
      printf("ERROR %s: cannot open %s for reading\n", __FUNCTION__, flash_filename);
#endif
      return(-1);
    }

 read_next_buffer:

  memset((void *)buffer, 0, sizeof(buffer));
  if ((num_bytes_read = fread((void *)buffer, 1, sizeof(buffer), fp)) == 0)
    goto end;

  if (v545_execute_macro(pb, V545_MACRO_READ_FLASH, &num_page, 0) == -1)
    {
#if DEBUG == 1
      printf("ERROR %s: execute flash read macro for num_page = %d failure\n", __FUNCTION__, num_page);
#endif
      return(-1);
    }

  for (i = 0; i < V545_REGFILE_WORD_SIZE; i++)
    {
      if (pb->regfile.raw_regs[i] != buffer[i])
        {
#if DEBUG == 1
          printf("ERROR %s: flash differs from memory at word = %d, page = %d\n", __FUNCTION__,  i, num_page);
#endif
          return(-1);
        }
    }

  if (percent_complete != (int *)0)
    *percent_complete = (int)((((float)num_page * (float)(sizeof(buffer))) / (float)sb.st_size) * 100.0);

  num_page += 1;
  goto read_next_buffer;

 end:

  if (percent_complete != (int *)0)
    *percent_complete = 100;

  fclose(fp);
  return(0);
}


// execute macros

int v545_execute_macro(V545_REGS *pb, unsigned short macro, unsigned short *params, unsigned short *arg)
{
  int i;
  unsigned short *parg = arg;

  switch(macro)
    {
    case V545_MACRO_NOOP:
      return(0);

    case V545_MACRO_RESET:
      if (ht_write_macro((unsigned short *)&pb->macro, macro, 0, 0, 1) == -1)		// should complete in 5 secs
        return(-1);
      sleep(6);
      return(0);
      
    case V545_MACRO_FLASH_UNLOCK:
      return(ht_write_macro((unsigned short *)&pb->macro, macro, 0, 0, 0));
      
    case V545_MACRO_FLASH_ERASE:
      if (ht_write_macro((unsigned short *)&pb->macro, macro, 0, 0, 0) == -1)		// should complete in 5 secs
        return(-1);
      while (pb->param0 < 31)
        {
          if (arg != (unsigned short *)0)
            *arg = (unsigned short)(((float)(pb->param0 - 15) / 16.0) * 100.0);		// write percent complete into arg
          usleep(500000);       							// sleep 0.5
        }
      *arg = (unsigned short)(15.0 / 16.0 * 100.0);
      return(0);
      
    case V545_MACRO_FLASH_WRITE:
      for (i = V545_FB_CHNLS_PER_BOARD; i < V545_FB_CHNLS_PER_BOARD + V545_OB_CHNLS_PER_BOARD; i++) // check for active override block
        if (pb->fb[i].fbregs.fun != 0)
          return(-1);
      return(ht_write_macro((unsigned short *)&pb->macro, macro, params, 1, 0));
      
    case V545_MACRO_FLASH_CHECKSUM:
      if (ht_write_macro((unsigned short *)&pb->macro, macro, 0, 0, 0) == -1)
        return(-1);
      *parg++ = pb->param0;
      *parg = pb->param1;
      return(0);
      
    case V545_MACRO_RESET_PSD:
      return(ht_write_macro((unsigned short *)&pb->macro, macro, params, 2, 0));
      
    case V545_MACRO_READ_FLASH:
      for (i = V545_FB_CHNLS_PER_BOARD; i < V545_FB_CHNLS_PER_BOARD + V545_OB_CHNLS_PER_BOARD; i++) // check for active override block
        if (pb->fb[i].fbregs.fun != 0)
          return(-1);
      return(ht_write_macro((unsigned short *)&pb->macro, macro, params, 1, 0));

    case V545_MACRO_READ_POWER_SUPPLY:
      if (arg == (unsigned short *)0)
        return(-1);
      if (ht_write_macro((unsigned short *)&pb->macro, macro, 0, 0, 0) == -1)
        return(-1);
      for (i = 0; i < 8; i++)
        *parg++ = pb->regfile.raw_regs[i];
      return(0);

    case V545_MACRO_RESET_DDS:
      return(ht_write_macro((unsigned short *)&pb->macro, macro, params, 1, 0));
     
    default:
      return(-1);
    }
}


// configure synchro input channel

int v545_config_synchro_input(V545_REGS *pb, int fb_chnl, int EX_coil, int A_coil, int B_coil, int C_coil, int signed_flag)
{
  V545_SYNCHRO_RESOLVER_INPUT_REGS *pfb = &pb->fb[fb_chnl].sri;
  
  if (v545_is_a_valid_function_block_chnl(fb_chnl) == 0)
    return(-1);

  // check for valid coil ranges

  if ((EX_coil < 0) || (EX_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: EX_coil out of range: %d\n", __FUNCTION__, EX_coil);
#endif
      return(-1);
    }
  
  if ((A_coil < 0) || (A_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: A_coil out of range: %d\n", __FUNCTION__, A_coil);
#endif
      return(-1);
    }
  
  if ((B_coil < 0) || (B_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: B_coil out of range: %d\n", __FUNCTION__, B_coil);
#endif
      return(-1);
    }
  
  if ((C_coil < 0) || (C_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: C_coil out of range: %d\n", __FUNCTION__, C_coil);
#endif
      return(-1);
    }
  
  pfb->fun = 0;
  pfb->opr = 0;

  pfb->flg.reg = 0;
  pfb->flg.bits.type = V545_SYNCHRO_RESOLVER_SET_SYNCHRO;
  pfb->flg.bits.cutoff_freq_code = 0;

  pfb->cp01.reg = 0;
  pfb->cp01.synchro.EX_coil = (unsigned short)EX_coil;
  pfb->cp01.synchro.A_coil = (unsigned short)A_coil;

  pfb->cp23.reg = 0;
  pfb->cp23.synchro.B_coil = (unsigned short)B_coil;
  pfb->cp23.synchro.C_coil = (unsigned short)C_coil;
  
  if (signed_flag)
    {
      pfb->ap.s = 0;
      pfb->fp.s = 0;
    }
  else
    {
      pfb->ap.us = 0;
      pfb->fp.us = 0;
    }
  
  pfb->fv = 0;

  if (v545_write_function_code(pb, fb_chnl, V545_FUNC_CODE_SYNCHRO_RESOLVER_INPUT) == -1)
    {
#if DEBUG == 1
      printf("ERROR %s: v545_write_function_code returned error\n", __FUNCTION__);
#endif
      return(-1);
    }
  
  sleep(1);
  return(0);
}


// configure resolver input channel

int v545_config_resolver_input(V545_REGS *pb, int fb_chnl, int EX_coil, int X_coil, int Y_coil, int signed_flag)
{
  V545_SYNCHRO_RESOLVER_INPUT_REGS *pfb = &pb->fb[fb_chnl].sri;

  if (v545_is_a_valid_function_block_chnl(fb_chnl) == 0)
    return(-1);

  // check for valid coil ranges

  if ((EX_coil < 0) || (EX_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: EX_coil out of range: %d\n", __FUNCTION__, EX_coil);
#endif
      return(-1);
    }      
  
  if ((X_coil < 0) || (X_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: X_coil out of range: %d\n", __FUNCTION__, X_coil);
#endif
      return(-1);
    }
  
  if ((Y_coil < 0) || (Y_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: Y_coil out of range: %d\n", __FUNCTION__, Y_coil);
#endif
      return(-1);
    }
  
  pfb->fun = 0;
  pfb->opr = 0;

  pfb->flg.reg = 0;
  pfb->flg.bits.type = V545_SYNCHRO_RESOLVER_SET_RESOLVER;
  pfb->flg.bits.cutoff_freq_code = 0;

  pfb->cp01.reg = 0;
  pfb->cp01.resolver.EX_coil = (unsigned short)EX_coil;

  pfb->cp23.reg = 0;
  pfb->cp23.resolver.Y_coil = (unsigned short)Y_coil;
  pfb->cp23.resolver.X_coil = (unsigned short)X_coil;
  
  if (signed_flag)
    {
      pfb->ap.s = 0;
      pfb->fp.s = 0;
    }
  else
    {
      pfb->ap.us = 0;
      pfb->fp.us = 0;
    }

  pfb->fv = 0;

  if (v545_write_function_code(pb, fb_chnl, V545_FUNC_CODE_SYNCHRO_RESOLVER_INPUT) == -1)
    {
#if DEBUG == 1
      printf("ERROR %s: v545_write_function_code returned error\n", __FUNCTION__);
#endif
      return(-1);
    }
  
  sleep(1);
  return(0);
}


// configure synchro output channel

int v545_config_synchro_output(V545_REGS *pb, int fb_chnl, int dds_chnl, int dds_freq, int EX_coil, int A_coil, int B_coil, int C_coil, int A_inv_flag, int B_inv_flag, int C_inv_flag, float min_pos, float max_pos, float init_pos, int sbits, int signed_flag)
{
  V545_SYNCHRO_RESOLVER_OUTPUT_REGS *pfb = &pb->fb[fb_chnl].sro;
  
  if (v545_is_a_valid_function_block_chnl(fb_chnl) == 0)
    return(-1);

  // check for valid coil ranges

  if ((EX_coil < 0) || (EX_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: EX_coil out of range: %d\n", __FUNCTION__, EX_coil);
#endif
      return(-1);
    }
  
  if ((A_coil < 0) || (A_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: A_coil out of range: %d\n", __FUNCTION__, A_coil);
#endif
      return(-1);
    }
  
  if ((B_coil < 0) || (B_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: B_coil out of range: %d\n", __FUNCTION__, B_coil);
#endif
      return(-1);
    }
  
  if ((C_coil < 0) || (C_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: C_coil out of range: %d\n", __FUNCTION__, C_coil);
#endif
      return(-1);
    }
  
  // check sbits are 0, 1, or 2

  switch(sbits)
    {
    case V545_TIME_INT_1MS:
    case V545_TIME_INT_10MS:
    case V545_TIME_INT_100MS:
      break;
    default:
#if DEBUG == 1
      printf("ERROR %s: sbits must range between 0-2\n", __FUNCTION__);
#endif
      return(-1);
    }

  // check for min/max/init position validity

  if (min_pos >= max_pos)
    {
#if DEBUG == 1
      printf("ERROR %s: min_position %f must be numerically less than max_position %f\n", __FUNCTION__, min_pos, max_pos);
#endif
      return(-1);
    }

  // dds_chnl = 0-6, 7 is fixed at 400 Hz

  if (dds_freq > 0)             // if dds_freq > 0, use internal excitation
    {
      if (v545_config_dds(pb, dds_chnl, dds_freq) == -1)
        return(-1);

      if (v545_config_ccb(pb, EX_coil, dds_chnl + V545_CCB_CHNLS_PER_BOARD) == -1)
        return(-1);
    }
  else                          // if dds_freq == 0, use external excitation
    {
      if (v545_config_ccb(pb, EX_coil, dds_chnl) == -1)
        return(-1);
    }

  pfb->fun = 0;
  pfb->opr = 0;
  usleep(2000);                 // wait for 2 msec; from manual "Wait at least one millisecond."
  pfb->flg.reg = 0;
  pfb->flg.bits.type = V545_SYNCHRO_RESOLVER_SET_SYNCHRO;
  pfb->flg.bits.sbits = (unsigned short)sbits;
  pfb->ovr.reg = 0;

  pfb->cp01.reg = 0;
  pfb->cp01.synchro.EX_coil = (unsigned short)EX_coil;
  pfb->cp01.synchro.A_coil = (unsigned short)A_coil;
  pfb->cp01.synchro.A_inv_flag = (unsigned short)A_inv_flag;

  pfb->cp23.reg = 0;
  pfb->cp23.synchro.B_coil = (unsigned short)B_coil;
  pfb->cp23.synchro.B_inv_flag = (unsigned short)B_inv_flag;
  pfb->cp23.synchro.C_coil = (unsigned short)C_coil;
  pfb->cp23.synchro.C_inv_flag = (unsigned short)C_inv_flag;

  pfb->cp45 = 0;
  pfb->cp67 = 0;
  pfb->srk = 0xffff;            // set max value for amplitude scalar
  pfb->srp = 0;                 // set phase shift to 0
  pfb->tv = 0x7fff;             // set for max velocity
  
  // set hard stop registers

  if (signed_flag)
    {
      pfb->hs1.s = (short)clamp(ratio((double)min_pos, -180.0L, 180.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
      pfb->hs2.s = (short)clamp(ratio((double)max_pos, -180.0L, 180.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
    }
  else
    {
      pfb->hs1.us = (unsigned short)clamp(ratio((double)min_pos, 0.0L, 360.0L, 0.0L, 65536.0L), 0.0L, 65535.0L);
      pfb->hs2.us = (unsigned short)clamp(ratio((double)max_pos, 0.0L, 360.0L, 0.0L, 65536.0L), 0.0L, 65535.0L);
    }

  // zero out BRK (broken coil) registers

  pfb->coil_error.synchro.brk_a = 0;
  pfb->coil_error.synchro.brk_b = 0;
  pfb->coil_error.synchro.brk_c = 0;

  if (v545_write_function_code(pb, fb_chnl, V545_FUNC_CODE_SYNCHRO_RESOLVER_OUTPUT) == -1)
    {
#if DEBUG == 1
      printf("ERROR %s: v545_write_function_code returned error\n", __FUNCTION__);
#endif
      return(-1);
    }

  // set initial position

  init_pos = (float)clamp((double)init_pos, (double)min_pos, (double)max_pos);

  if (signed_flag)
    pfb->tp.s = (short)clamp(ratio((double)init_pos, -180.0L, 180.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
  else
    pfb->tp.us = (unsigned short)clamp(ratio((double)init_pos, 0.0L, 360.0L, 0.0L, 65536.0L), 0.0L, 65535.0L);

  pfb->opr = V545_SYNCHRO_RESOLVER_OPR_MOVE_HARD_STOP;
  sleep(1);
  return(0);
}


// configure resolver output channel

int v545_config_resolver_output(V545_REGS *pb, int fb_chnl, int dds_chnl, int dds_freq, int EX_coil, int X_coil, int Y_coil, int X_inv_flag, int Y_inv_flag, float min_pos, float max_pos, float init_pos, int sbits, int signed_flag)
{
  V545_SYNCHRO_RESOLVER_OUTPUT_REGS *pfb = &pb->fb[fb_chnl].sro;
  
  if (v545_is_a_valid_function_block_chnl(fb_chnl) == 0)
    return(-1);

  // check for valid coil ranges

  if ((EX_coil < 0) || (EX_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: EX_coil out of range: %d\n", __FUNCTION__, EX_coil);
#endif
      return(-1);
    }
  
  if ((X_coil < 0) || (X_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: X_coil out of range: %d\n", __FUNCTION__, X_coil);
#endif
      return(-1);
    }
  
  if ((Y_coil < 0) || (Y_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: Y_coil out of range: %d\n", __FUNCTION__, Y_coil);
#endif
      return(-1);
    }
  
  // check sbits are 0, 1, or 2

  switch(sbits)
    {
    case V545_TIME_INT_1MS:
    case V545_TIME_INT_10MS:
    case V545_TIME_INT_100MS:
      break;
    default:
#if DEBUG == 1
      printf("ERROR %s: sbits must range between 0-2\n", __FUNCTION__);
#endif
      return(-1);
    }

  // check for min/max/init position validity

  if (min_pos >= max_pos)
    {
#if DEBUG == 1
      printf("ERROR %s: min_position %f must be numerically less than max_position %f\n", __FUNCTION__, min_pos, max_pos);
#endif
      return(-1);
    }

  // dds_chnl = 0-6, 7 is fixed at 400 Hz

  if (dds_freq > 0)             // if dds_freq > 0, use interal excitation
    {
      if (v545_config_dds(pb, dds_chnl, dds_freq) == -1)
        return(-1);

      if (v545_config_ccb(pb, EX_coil, dds_chnl + V545_CCB_CHNLS_PER_BOARD) == -1)
        return(-1);
    }
  else                          // if dds_freq == 0, use external excitation
    {
      if (v545_config_ccb(pb, EX_coil, dds_chnl) == -1)
        return(-1);
    }

  pfb->fun = 0;
  pfb->opr = 0;
  usleep(2000);                 // wait for 2 msec; from manual "Wait at least one millisecond."
  pfb->flg.reg = 0;
  pfb->flg.bits.type = V545_SYNCHRO_RESOLVER_SET_RESOLVER;
  pfb->flg.bits.sbits = (unsigned short)sbits;
  pfb->ovr.reg = 0;

  pfb->cp01.reg = 0;
  pfb->cp01.resolver.EX_coil = (unsigned short)EX_coil;

  pfb->cp23.reg = 0;
  pfb->cp23.resolver.X_coil = (unsigned short)X_coil;
  pfb->cp23.resolver.X_inv_flag = (unsigned short)X_inv_flag;
  pfb->cp23.resolver.Y_coil = (unsigned short)Y_coil;
  pfb->cp23.resolver.Y_inv_flag = (unsigned short)Y_inv_flag;

  pfb->cp45 = 0;
  pfb->cp67 = 0;
  pfb->srk = 0xffff;            // set max value for amplitude scalar
  pfb->srp = 0;                 // set phase shift to 0
  pfb->tv = 0x7fff;             // set for max velocity
  
  // set hard stop registers

  if (signed_flag)
    {
      pfb->hs1.s = (short)clamp(ratio((double)min_pos, -180.0L, 180.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
      pfb->hs2.s = (short)clamp(ratio((double)max_pos, -180.0L, 180.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
    }
  else
    {
      pfb->hs1.us = (unsigned short)clamp(ratio((double)min_pos, 0.0L, 360.0L, 0.0L, 65536.0L), 0.0L, 65535.0L);
      pfb->hs2.us = (unsigned short)clamp(ratio((double)max_pos, 0.0L, 360.0L, 0.0L, 65536.0L), 0.0L, 65535.0L);
    }

  // zero out BRK (broken coil) registers

  pfb->coil_error.resolver.brk_y = 0;
  pfb->coil_error.resolver.brk_x = 0;

  if (v545_write_function_code(pb, fb_chnl, V545_FUNC_CODE_SYNCHRO_RESOLVER_OUTPUT) == -1)
    {
#if DEBUG == 1
      printf("ERROR %s: v545_write_function_code returned error\n", __FUNCTION__);
#endif
      return(-1);
    }

  // set initial position

  init_pos = (float)clamp((double)init_pos, (double)min_pos, (double)max_pos);

  if (signed_flag)
    pfb->tp.s = (short)clamp(ratio((double)init_pos, -180.0L, 180.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
  else
    pfb->tp.us = (unsigned short)clamp(ratio((double)init_pos, 0.0L, 360.0L, 0.0L, 65536.0L), 0.0L, 65535.0L);

  pfb->opr = V545_SYNCHRO_RESOLVER_OPR_MOVE_HARD_STOP;
  sleep(1);
  return(0);
}


// configure lvdt/rvdt input channel
//
// defaults:
//
// SRP set phase shift (delay) to 0

int v545_config_lvdt_rvdt_input(V545_REGS *pb, int fb_chnl, int EX_coil, int A_coil, int B_coil, int open_wiring_flag)
{
  V545_LVDT_RVDT_INPUT_REGS *pfb = &pb->fb[fb_chnl].lri;

  if (v545_is_a_valid_function_block_chnl(fb_chnl) == 0)
    return(-1);

  // check for valid coil ranges

  if ((EX_coil < 0) || (EX_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: EX_coil out of range: %d\n", __FUNCTION__, EX_coil);
#endif
      return(-1);
    }
  
  if ((A_coil < 0) || (A_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: A_coil out of range: %d\n", __FUNCTION__, A_coil);
#endif
      return(-1);
    }
  
  if (open_wiring_flag)
    {
      if ((B_coil < 0) || (B_coil >= V545_CCB_CHNLS_PER_BOARD))
        {
#if DEBUG == 1
          printf("ERROR %s: B_coil out of range: %d\n", __FUNCTION__, B_coil);
#endif
          return(-1);
        }
    }
  
  pfb->fun = 0;
  pfb->opr = 0;
  pfb->flg.bits.type = 0;
  pfb->flg.bits.filter_speed = 0;

  if (open_wiring_flag == 0)
    pfb->flg.bits.type = V545_LVDT_RVDT_RATIOMETRIC;
  else
    pfb->flg.bits.type = V545_LVDT_RVDT_OPEN_WIRING;

  pfb->cp01.bits.EX_coil = (unsigned short)EX_coil;
  pfb->cp23.bits.A_coil = A_coil;

  if (open_wiring_flag == 0)
    pfb->cp23.bits.B_coil = B_coil;

  pfb->cp45 = 0;
  pfb->cp67 = 0;
  pfb->srp = 0;                 // set phase shift to 0

  // pfb->counts.signed_flag = signed_flag;

  if (v545_write_function_code(pb, fb_chnl, V545_FUNC_CODE_LVDT_RVDT_INPUT) == -1)
    {
#if DEBUG == 1
      printf("ERROR %s: v545_write_function_code returned error\n", __FUNCTION__);
#endif
      return(-1);
    }

  sleep(1);
  return(0);
}


// configure lvdt/rvdt output channel
//
// defaults:
//
// SRK register set to max value (0xffff) for amplitude scalar register
// SRP set phase shift (delay) to 0
// TV set target velocity to max (0xffff)
// FLG.X2 set to no 2X for multiplying by 2 output of A_coil and B_coil (simulate stepping up of secondary voltage)
// FLG.S set slew operation to fastest time interval of 1 msec

int v545_config_lvdt_rvdt_output(V545_REGS *pb, int fb_chnl, int dds_chnl, int dds_freq, int EX_coil, int A_coil, int B_coil, int A_inv_flag, int B_inv_flag, int open_wiring_flag, float min_pos, float max_pos, float init_pos, int sbits)
{
  V545_LVDT_RVDT_OUTPUT_REGS *pfb = &pb->fb[fb_chnl].lro;
  short counts;
  volatile unsigned short __attribute__((unused))status;

  if (v545_is_a_valid_function_block_chnl(fb_chnl) == 0)
    return(-1);

  // check for valid coil ranges

  if ((EX_coil < 0) || (EX_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: EX_coil out of range: %d\n", __FUNCTION__, EX_coil);
#endif
      return(-1);
    }
  
  if ((A_coil < 0) || (A_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: A_coil out of range: %d\n", __FUNCTION__, A_coil);
#endif
      return(-1);
    }
  
  if (open_wiring_flag)
    {
      if ((B_coil < 0) || (B_coil >= V545_CCB_CHNLS_PER_BOARD))
        {
#if DEBUG == 1
          printf("ERROR %s: B_coil out of range: %d\n", __FUNCTION__, B_coil);
#endif
          return(-1);
        }
    }
  
  // check sbits are 0, 1, or 2

  switch(sbits)
    {
    case V545_TIME_INT_1MS:
    case V545_TIME_INT_10MS:
    case V545_TIME_INT_100MS:
      break;
    default:
      return(-1);
    }

  // dds_chnl = 0-6, 7 is fixed at 400 Hz

  if (dds_freq > 0)             // if dds_freq > 0, use interal excitation
    {
      if (v545_config_dds(pb, dds_chnl, dds_freq) == -1)
        return(-1);

      if (v545_config_ccb(pb, EX_coil, dds_chnl + V545_CCB_CHNLS_PER_BOARD) == -1)
        return(-1);
    }
  else                          // if dds_freq == 0, use external excitation
    {
      if (v545_config_ccb(pb, EX_coil, dds_chnl) == -1)
        return(-1);
    }

  pfb->fun = 0;
  pfb->opr = 0;
  pfb->flg.bits.type = 0;
  pfb->flg.bits.x2 = 0;
  pfb->flg.bits.sbits = 0;
  
  if (open_wiring_flag == 0)
    pfb->flg.bits.type = V545_LVDT_RVDT_RATIOMETRIC;
  else
    pfb->flg.bits.type = V545_LVDT_RVDT_OPEN_WIRING;

  pfb->flg.bits.x2 = V545_LVDT_RVDT_NO_2X;
  pfb->flg.bits.sbits = (unsigned short)sbits;

  // check for valid coil 0-23

  pfb->cp01.bits.EX_coil = (unsigned short)EX_coil;
  pfb->cp23.bits.A_coil = (unsigned short)A_coil;
  pfb->cp23.bits.A_inv_flag = (unsigned short)A_inv_flag;

  if (open_wiring_flag == 0)
    {
      pfb->cp23.bits.B_coil = (unsigned short)B_coil;
      pfb->cp23.bits.B_inv_flag = (unsigned short)B_inv_flag;
    }
  
  pfb->cp45 = 0;
  pfb->cp67 = 0;
  pfb->srk = 0xffff;            // set max value for amplitude scalar
  pfb->srp = 0;                 // set phase shift to 0
  pfb->tv = 0x7fff;             // set max target velocity

  if (v545_write_function_code(pb, fb_chnl, V545_FUNC_CODE_LVDT_RVDT_OUTPUT) == -1)
    {
#if DEBUG == 1
      printf("ERROR %s: v545_write_function_code returned error\n", __FUNCTION__);
#endif
      return(-1);
    }
  
  // lvdt/rvdt counts are always signed

  init_pos = (float)clamp((double)init_pos, (double)min_pos, (double)max_pos);
  counts = (short)clamp(ratio((double)init_pos, (double)min_pos, (double)max_pos, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
  pfb->td = counts;
  pfb->opr = V545_LVDT_RVDT_OPR_MOVE;
  sleep(1);
  return(0);
}


// read synchro/resolver position

int v545_read_synchro_resolver(V545_REGS *pb, int fb_chnl, int min_counts, int max_counts, float min_pos, float max_pos, float *current_pos, float *measured_secondary_voltage, int signed_flag)
{
  V545_SYNCHRO_RESOLVER_INPUT_REGS *pfb = &pb->fb[fb_chnl].sri;
  short __attribute__((unused)) ap;
    
  // int signed_flag = 0;

  if (v545_is_a_valid_function_block_chnl(fb_chnl) == 0)
    return(-1);

  // if ((min_pos < 0) || (max_pos < 0))
  //   signed_flag = 1;

  if (current_pos == (float *)0)
    return(-1);

  ap = pfb->ap.s;

  if (signed_flag)
    *current_pos = (float)clamp(ratio((double)pfb->ap.s, (double)min_counts, (double)max_counts, (double)min_pos, (double)max_pos), (double)min_pos, (double)max_pos);
  else
    *current_pos = (float)clamp(ratio((double)pfb->ap.us, (double)min_counts, (double)max_counts, (double)min_pos, (double)max_pos), (double)min_pos, (double)max_pos);

  // a pointer for the measured secondary volation could be optionally 0

  if (measured_secondary_voltage != (float *)0)
    *measured_secondary_voltage = (float)pfb->msv / 1000.0;

  return(0);
}


// write synchro position

int v545_write_synchro_resolver(V545_REGS *pb, int fb_chnl, float min_pos, float max_pos, float current_pos, int signed_flag)
{
  V545_SYNCHRO_RESOLVER_OUTPUT_REGS *pfb = &pb->fb[fb_chnl].sro;

  if (v545_is_a_valid_function_block_chnl(fb_chnl) == 0)
    return(-1);

  current_pos = (float)clamp((double)current_pos, (double)min_pos, (double)max_pos);

  if (signed_flag)
    pfb->tp.s = (short)clamp(ratio((double)current_pos, -180.0L, 180.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
  else
    pfb->tp.us = (unsigned short)clamp(ratio((double)current_pos, 0.0L, 360.0L, 0.0L, 65536.0L), 0.0L, 65535.0L);

  return(0);
}


// read lvdt/rvdt input value

int v545_read_lvdt_rvdt(V545_REGS *pb, int fb_chnl, int min_counts, int max_counts, float min_pos, float max_pos, float *current_pos, float *measured_secondary_voltage)
{
  V545_LVDT_RVDT_INPUT_REGS *pfb = &pb->fb[fb_chnl].lri;
  short actual_displacement;
  
  if (v545_is_a_valid_function_block_chnl(fb_chnl) == 0)
    return(-1);

  if (current_pos == (float *)0)
    return(-1);
  
  actual_displacement = pfb->ad; // lvdt/rvdt position counts are signed
  *current_pos = (float)ratio((double)actual_displacement, (double)min_counts, (double)max_counts, (double)min_pos, (double)max_pos);

  if (measured_secondary_voltage != (float *)0)
    *measured_secondary_voltage = (float)(pfb->msv) / 1000.0;

  // printf("actual_displacement = %#hx, min_counts = %#hx, max_counts = %#hx, min_pos = %f, max_pos = %f, current_pos = %f\n", actual_displacement, min_counts, max_counts, min_pos, max_pos, *current_pos);

  return(0);
}


// update lvdt/rvdt output value

int v545_write_lvdt_rvdt(V545_REGS *pb, int fb_chnl, float min_pos, float max_pos, float current_pos)
{
  V545_LVDT_RVDT_OUTPUT_REGS *pfb = &pb->fb[fb_chnl].lro;
  short counts;

  if (v545_is_a_valid_function_block_chnl(fb_chnl) == 0)
    return(-1);

  // lvdt/rvdt position counts are always signed

  current_pos = (float)clamp((double)current_pos, (double)min_pos, (double)max_pos);
  counts = (short)clamp(ratio((double)current_pos, (double)min_pos, (double)max_pos, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
  pfb->td = counts;
  return(0);
}


// move synchro/resolver to min and max position and read counts

int v545_read_synchro_resolver_position_counts(V545_REGS *pb, int fb_chnl, int *counts, int signed_flag)
{
  V545_SYNCHRO_RESOLVER_INPUT_REGS *pfb = &pb->fb[fb_chnl].sri;
 
  if (v545_is_a_valid_function_block_chnl(fb_chnl) == 0)
    return(-1);

  if (counts == (int *)0)
    return(-1);

  if (signed_flag)
    *counts = (int)pfb->ap.s;
  else 
    *counts = (int)pfb->ap.us;

  return(0);
}


// move lvdt/rvdt to min and max position and read counts

int v545_read_lvdt_rvdt_position_counts(V545_REGS *pb, int fb_chnl, int *counts)
{
  V545_LVDT_RVDT_INPUT_REGS *pfb = &pb->fb[fb_chnl].lri;
 
  if (v545_is_a_valid_function_block_chnl(fb_chnl) == 0)
    return(-1);

  // lvdt/rvdt counts are always signed

  if (counts != (int *)0)
    *counts = (int)pfb->ad;
  else
    return(-1);
  return(0);
}


// clear function block

int v545_clear_function_block(V545_REGS *pb, int fb_chnl)
{
  V545_FUNC_BLOCK_REGS *pfb_fbregs = &pb->fb[fb_chnl].fbregs;
  V545_SYNCHRO_RESOLVER_INPUT_REGS *pfb_sri = &pb->fb[fb_chnl].sri;
  V545_SYNCHRO_RESOLVER_OUTPUT_REGS *pfb_sro = &pb->fb[fb_chnl].sro;
  V545_LVDT_RVDT_INPUT_REGS *pfb_lri = &pb->fb[fb_chnl].lri;
  V545_LVDT_RVDT_OUTPUT_REGS *pfb_lro = &pb->fb[fb_chnl].lro;

  if (v545_is_a_valid_function_block_chnl(fb_chnl) == 0)
    return(-1);

  switch(pfb_fbregs->fun)
    {
    case V545_FUNC_CODE_LVDT_RVDT_INPUT:
      pfb_lri->fun = 0;
      pfb_lri->opr = 0;
      pfb_lri->flg.reg = 0;
      pfb_lri->cp01.reg = 0;
      pfb_lri->cp23.reg = 0;
      pfb_lri->srp = 0;
      break;

    case V545_FUNC_CODE_LVDT_RVDT_OUTPUT:
      pfb_lro->fun = 0;
      pfb_lro->opr = 0;
      pfb_lro->flg.reg = 0;
      pfb_lro->cp01.reg = 0;
      pfb_lro->cp23.reg = 0;
      pfb_lro->srk = 0;
      pfb_lro->srp = 0;
      pfb_lro->td = 0;
      pfb_lro->tv = 0;
      pfb_lro->A_coil_brk.active = 0;
      pfb_lro->A_coil_brk.pt01percent = 0;
      pfb_lro->B_coil_brk.active = 0;
      pfb_lro->B_coil_brk.pt01percent = 0;
      break;
      
    case V545_FUNC_CODE_SYNCHRO_RESOLVER_INPUT:
      pfb_sri->fun = V545_FUNC_CODE_NONE;
      pfb_sri->opr = 0;
      pfb_sri->flg.reg = 0;
      pfb_sri->cp01.reg = 0;
      pfb_sri->cp23.reg = 0;
      pfb_sri->srp = 0;
      break;
      
    case V545_FUNC_CODE_SYNCHRO_RESOLVER_OUTPUT:
      pfb_sro->fun = V545_FUNC_CODE_NONE;
      pfb_sro->opr = 0;
      pfb_sro->flg.reg = 0;
      pfb_sro->cp01.reg = 0;
      pfb_sro->cp23.reg = 0;
      pfb_sro->srk = 0;
      pfb_sro->srp = 0;
      pfb_sro->hs1.s = 0;
      pfb_sro->hs2.s = 0;
      pfb_sro->tp.s = 0;
      pfb_sro->tv = 0;
      pfb_sro->coil_error.synchro.brk_a = 0;
      pfb_sro->coil_error.synchro.brk_b = 0;
      pfb_sro->coil_error.synchro.brk_c = 0;
      break;

    default:
      return(-1);
    }

  return(0);
}


// clear function blocks

int v545_clear_function_blocks(V545_REGS *pb)
{
  int i;

  for (i = 0; i < V545_FB_CHNLS_PER_BOARD; i++)
    if (v545_clear_function_block(pb, i) == -1)
      return(-1);

  return(0);
}


// clear override

int v545_clear_active_override(V545_REGS *pb, int ob_chnl)
{
  V545_OVERRIDE_BLOCK_REGS *pob = &pb->fb[ob_chnl].ob;

  if (v545_is_a_valid_override_block_chnl(ob_chnl) == 0)
    return(-1);

  pob->flg.bits.tg = 0;
  pob->opr = 1;
  return(0);
}  


// clear override block

int v545_clear_override_block(V545_REGS *pb, int ob_chnl)
{
  V545_OVERRIDE_BLOCK_REGS *pob = &pb->fb[ob_chnl].ob;
  int i, j;

  if (v545_is_a_valid_override_block_chnl(ob_chnl) == 0)
    return(-1);

  pob->fun = 0;                 // clear FUN register
  pob->opr = 0;                 // clear OPR register
  pob->flg.reg = 0;             // clear TG, INV, LEN, and A3/A2/A1/A0 bits
  pob->dog = 0;                 // clear watchdog countdown register
  pob->trg = 0;			// clear software trigger register

  for (i = 0; i < V545_FB_CHNLS_PER_BOARD; i++)
    {
      if (pob->ovr[i].reg != 0)
        {
          j = pob->ovr[i].bits.regfile_index / 2; // read the regfile index
          pb->regfile.entry[j].pos.us = pb->regfile.entry[j].vel.us = 0;
          pob->ovr[i].reg = 0;
        }
    }
  
  return(0);
}


// clear override blocks

int v545_clear_override_blocks(V545_REGS *pb)
{
  int i;

  for (i = V545_FB_CHNLS_PER_BOARD; i < V545_FB_CHNLS_PER_BOARD + V545_OB_CHNLS_PER_BOARD; i++)
    if (v545_clear_override_block(pb, i) == -1)
      return(-1);

  return(0);
}


// clear regfile block

int v545_clear_regfile(V545_REGS *pb)
{
  int i;

  for (i = 0; i < V545_REGFILE_WORD_SIZE; i++)
    pb->regfile.raw_regs[i] = 0;

  return(0);
}


// config override switch

int v545_config_override_switch(V545_REGS *pb, int ob_chnl, int swin_mask)
{
  V545_OVERRIDE_BLOCK_REGS *pob = &pb->fb[ob_chnl].ob;

  if (v545_is_a_valid_override_block_chnl(ob_chnl) == 0)
    return(-1);

  if (swin_mask != 0)
    {
      pob->flg.bits.swin_mask = swin_mask; // switch input mask: 0x1 = sw 0, 0x2 = sw 1, 0x4 = sw 2, 0x8 = sw 3
      pob->flg.bits.len = 1;	// by default, set latch enabled
      pob->flg.bits.inv = 1;    // by default, trip override if switch is "closed" (vs. open)
    }

  pob->fun = V545_OVERRIDE_FUNC_CODE_SWITCH;
  return(0);
}


// config override watchdog

int v545_config_override_watchdog(V545_REGS *pb, int ob_chnl, float watchdog_time)
{
  V545_OVERRIDE_BLOCK_REGS *pob = &pb->fb[ob_chnl].ob;

  if (v545_is_a_valid_override_block_chnl(ob_chnl) == 0)
    return(-1);

  pob->dog = (unsigned short)clamp((double)(watchdog_time / 0.004), (double)0.0L, 65535.0L);
  pob->flg.bits.len = 1;        // set latch enable
  pob->fun = V545_OVERRIDE_FUNC_CODE_WATCHDOG;
  return(0);
}


// config s/w trigger override

int v545_config_override_swtrigger(V545_REGS *pb, int ob_chnl)
{
  V545_OVERRIDE_BLOCK_REGS *pob = &pb->fb[ob_chnl].ob;

  if (v545_is_a_valid_override_block_chnl(ob_chnl) == 0)
    return(-1);

  pob->flg.bits.len = 1;        // set latch enable
  pob->flg.bits.tg = 1;
  pob->fun = V545_OVERRIDE_FUNC_CODE_SWITCH;
  return(0);
}


// config s/w trigger override

int v545_set_override_swtrigger(V545_REGS *pb, int ob_chnl)
{
  V545_OVERRIDE_BLOCK_REGS *pob = &pb->fb[ob_chnl].ob;

  if (v545_is_a_valid_override_block_chnl(ob_chnl) == 0)
    return(-1);

  pob->trg = 1;
  return(0);
}


// linear velocity counts are calculated as follows:
//
// for S value = 00 (0x0)
//
//   65536 counts * 0.001 sec               inches 
//  --------------------------  *  velocity ------  = C counts
//  (max_pos - min_pos) inches               sec    
//
// for S value = 01 (0x1)
//
//   65536 counts * 0.01 sec                inches 
//  --------------------------  *  velocity ------  = C counts
//  (max_pos - min_pos) inches               sec    
//
// for S value = 10 (0x2)
//
//   65536 counts * 0.1 sec                 inches 
//  --------------------------  *  velocity ------  = C counts
//  (max_pos - min_pos) inches               sec    
//

// rotational velocity counts are calculated as follows:
//
// for S value = 00 (0x0)
//
//  180 deg          1 count      1000 updates
//  ---------    *   -------   *  ------------ = 5.493164 deg/sec
//  32768 counts     update          1 sec
// 
// for S value = 01 (0x1)
//
//  180 deg          1 count      100 updates
//  ---------    *   -------   *  ------------ = 0.5493164 deg/sec
//  32768 counts     update          1 sec
// 
// for S value = 10 (0x2)
//
//  180 deg          1 count      10 updates
//  ---------    *   -------   *  ------------ = 0.05493164 deg/sec
//  32768 counts     update          1 sec


// set override position and velocity

int v545_config_override_position_velocity(V545_REGS *pb, int ob_chnl, int fb_chnl, float target_position, float target_velocity, int min_counts, int max_counts, float min_pos, float max_pos, int signed_flag)
{
  V545_OVERRIDE_BLOCK_REGS *pob = &pb->fb[ob_chnl].ob;
  V545_FUNC_BLOCK_REGS *pfb_regs = &pb->fb[fb_chnl].fbregs;
  V545_LVDT_RVDT_OUTPUT_REGS *pfb_lro = &pb->fb[fb_chnl].lro;
  V545_SYNCHRO_RESOLVER_OUTPUT_REGS *pfb_sro = &pb->fb[fb_chnl].sro;

  short __attribute__((unused)) override_position_counts;
  short __attribute__((unused)) override_velocity_counts;

  unsigned short function_code;
  float time_delta = 0.0;
  float degs_per_sec;
  int regfile_index;

  if (v545_is_a_valid_override_block_chnl(ob_chnl) == 0)
    return(-1);

  if (v545_is_a_valid_function_block_chnl(fb_chnl) == 0)
    return(-1);

  // get function code for fb_chnl

  function_code = pfb_regs->fun;

  if (v545_override_get_regfile_index(pb, &regfile_index) == -1)
    return(-1);

#if DEBUG == 1
  printf("INFO %s: regfile_index = %d\n", __FUNCTION__, regfile_index);
#endif

  target_position = (float)clamp((double)target_position, (double)min_pos, (double)max_pos);

  switch(function_code)
    {
    case V545_FUNC_CODE_LVDT_RVDT_OUTPUT:
      switch(pfb_lro->flg.bits.sbits)
        {
        case 0:
          time_delta = 0.001;
          break;
        case 1:
          time_delta = 0.01;
          break;
        case 2:
          time_delta = 0.1;
          break;
        default:
          return(-1);
        }

      override_position_counts = pb->regfile.entry[regfile_index].pos.s = (short)clamp(ratio((double)target_position, (double)min_pos, (double)max_pos, (double)min_counts, (double)max_counts), -32768.0L, 32767.0L);
      override_velocity_counts = pb->regfile.entry[regfile_index].vel.s = (short)(((65536.0 * time_delta) / (max_pos - min_pos)) * target_velocity);

#if DEBUG == 1
      printf("max_counts = %d (%#x), min_counts = %d (%#x), max_pos = %f, min_pos = %f\n", max_counts, max_counts, min_counts, min_counts, max_pos, min_pos);
      printf("override_position_counts = %hd (%#hx), override_velocity_counts = %hd (%#hx)\n", override_position_counts, override_position_counts, override_velocity_counts, override_velocity_counts);
#endif

      break;

    case V545_FUNC_CODE_SYNCHRO_RESOLVER_OUTPUT:
      switch(pfb_sro->flg.bits.sbits)
        {
        case 0:
          degs_per_sec = 180.0 / 32768.0 * 1000.0; //  180 deg / 32768 counts * 1000 updates/sec
          break;
        case 1:
          degs_per_sec = 180.0 / 32768.0 * 100.0;  //  180 deg / 32768 counts * 100 updates/sec
          break;
        case 2:
          degs_per_sec = 180.0 / 32768.0 * 10.0;   //  180 deg / 32768 counts * 10 updates/sec
          break;
        default:
          return(-1);
        }

      if (signed_flag)
        {
          override_position_counts = pb->regfile.entry[regfile_index].pos.s = (short)clamp(((double)target_position * 32768.0L / 180.0L), -32768.0L, 32767.0L);
          override_velocity_counts = pb->regfile.entry[regfile_index].vel.s = (short)fmax(clamp(((double)target_velocity / degs_per_sec), -32768.0L, 32767.0L), 1L);
        }
      else
        {
          override_position_counts = pb->regfile.entry[regfile_index].pos.us = (unsigned short)clamp(((double)target_position * 65536.0L / 360.0L), 0.0L, 65535.0L);
          override_velocity_counts = pb->regfile.entry[regfile_index].vel.us = (unsigned short)fmax(clamp(((double)target_velocity / degs_per_sec), 0.0L, 65535.0L), 1L);
        }

      break;

    default:
      return(-1);
    }

  pob->ovr[fb_chnl].reg = (regfile_index * 2) | (ob_chnl << 12);

  return(0);
}


// get debug info

// int v545_get_debug_info(V545_REGS *pb)
// {
//   printf("sizeof V545_FUNCTION_CODE_REGS = 0x%lx\n", sizeof(V545_FUNC_BLOCK_REGS));
//   printf("sizeof V545_SYNCHRO_RESOLVER_INPUT_REGS = 0x%lx\n", sizeof(V545_SYNCHRO_RESOLVER_INPUT_REGS));
//   printf("sizeof V545_SYNCHRO_RESOLVER_OUTPUT_REGS = 0x%lx\n", sizeof(V545_SYNCHRO_RESOLVER_OUTPUT_REGS));
//   printf("sizeof V545_LVDT_RVDT_INPUT_REGS = 0x%lx\n", sizeof(V545_LVDT_RVDT_INPUT_REGS));
//   printf("sizeof V545_LVDT_RVDT_OUTPUT_REGS = 0x%lx\n", sizeof(V545_LVDT_RVDT_OUTPUT_REGS));
//   return(0);
// }



// --------------------- NOTE: THE FOLLOWING ROUTINES ARE ONLY CALLED INTERNALLY ---------------------



#define V545_OPR_IDLE			0x0000
#define V545_OPR_INIT			0x0001

#define V545_STATUS_INIT_COMPLETE	0x0001
#define V545_STATUS_OVERRIDE_ACTIVE	0x0100
#define V545_STATUS_CONFIG_ERROR	0x1000
#define V545_STATUS_EXCITATION_ERROR	0x2000		// set if excitation voltage is too low -- less than 1 VRMS using 1:1 transformers
#define V545_STATUS_SIGNAL_ERROR	0x4000		// only used in synchro/resolver lvdt/rvdt acquisition -- set if measured secondary voltage is less than 100mV or if there is a ADC clipping error

#define V545_FB_TYPE_NONE	0
#define V545_FB_TYPE_IO		1
#define V545_FB_TYPE_OVERRIDE	2

int v545_write_function_code(V545_REGS *pb, int fb_chnl, unsigned short function_code)
{
  V545_FUNC_BLOCK_REGS *pfb = &pb->fb[fb_chnl].fbregs;
  int fb_type = V545_FB_TYPE_NONE;
  useconds_t usec = 0;
  useconds_t usec_incr = 100000; // sleep for 0.1 sec
  useconds_t usec_max = 5000000; // max of 5 sec

  unsigned short status;
  
  // determine if function block is IO or OVERRIDE type

  if (v545_is_a_valid_function_block_chnl(fb_chnl))
    fb_type = V545_FB_TYPE_IO;
  else if (v545_is_a_valid_override_block_chnl(fb_chnl))
    fb_type = V545_FB_TYPE_OVERRIDE;
  else
    {
#if DEBUG == 1
      printf("ERROR %s: fb_chnl out of range: %d\n", __FUNCTION__, fb_chnl);
#endif
      return(-1);
    }

  // check for valid function code based on function block type

  switch(fb_type)
    {
    case V545_FB_TYPE_IO:
      switch(function_code)
        {
        case V545_FUNC_CODE_LVDT_RVDT_INPUT:
        case V545_FUNC_CODE_LVDT_RVDT_OUTPUT:
        case V545_FUNC_CODE_SYNCHRO_RESOLVER_INPUT:
        case V545_FUNC_CODE_SYNCHRO_RESOLVER_OUTPUT:
          break;
        default:
#if DEBUG == 1
          printf("ERROR %s: illegal function code: %d for function block type IO\n", __FUNCTION__, function_code);
#endif
          return(-1);
        }
      break;

    case V545_FB_TYPE_OVERRIDE:
      switch(function_code)
        {
        case V545_OVERRIDE_FUNC_CODE_WATCHDOG:
        case V545_OVERRIDE_FUNC_CODE_SWITCH:
          break;
        default:
#if DEBUG == 1
          printf("ERROR %s: illegal function code: %d for function block type OVERRIDE\n", __FUNCTION__, function_code);
#endif
          return(-1);
        }

    default:
#if DEBUG == 1
      printf("ERROR %s: illegal function code: %d\n", __FUNCTION__, function_code);
#endif
      return(-1);
    }

  pfb->fun = 0;
  pfb->opr = 0;
  pfb->fun = function_code;
  pfb->opr = V545_OPR_INIT;	// initialize
  
  // wait for 5000 msec for OPR register to set to 0

  while ((pfb->opr != V545_OPR_IDLE) && (usec < usec_max))
    {
      usleep(usec_incr);    // sleep for 10 msec
      usec += usec_incr;    // increment msec counter
    }

  if (usec >= usec_max)
    {
#if DEBUG == 1
      printf("ERROR %s: OPR register != IDLE after %d secs\n", __FUNCTION__, usec_max / 1000000);
#endif
      return(-1);
    }
  
  // check for valid status

  status = pfb->sts;

  if ((status & V545_STATUS_CONFIG_ERROR) != 0)	// configuration error
    {
#if DEBUG == 1
      printf("ERROR %s: config error bit set in status register\n", __FUNCTION__);
#endif
      return(-1);
    }

#if EXCITATION_CHECK == 1
  if ((status & V545_STATUS_EXCITATION_ERROR) != 0)	// excitation error
    {
#if DEBUG == 1
      printf("ERROR %s: excitation error bit set in status register\n", __FUNCTION__);
#endif
      return(-1);
    }
#endif

  if ((function_code == V545_FUNC_CODE_SYNCHRO_RESOLVER_INPUT) || (function_code ==  V545_FUNC_CODE_LVDT_RVDT_INPUT)) // check for signal errors on input
    if (status & V545_STATUS_SIGNAL_ERROR)
      {
#if DEBUG == 1
      printf("ERROR %s: signal error bit set in status register\n", __FUNCTION__);
#endif
        return(-1);
      }

  if (status == V545_STATUS_INIT_COMPLETE) // good return
    return(0);

#if DEBUG == 1
  printf("ERROR %s: unknown error, status register = %#hx\n", __FUNCTION__, status);
#endif
  return(-1);                   // assume bad return
}


// configure DDS frequency synthesizer registers
// max freq = 20kHz (counts = 40000)
// max amplitude = 10.24v
// chnl range = 0-6, chnl 7 fixed at 400Hz

int v545_config_dds(V545_REGS *pb, int dds_chnl, int dds_freq)
{
  V545_DDS_REGS *pdds = (V545_DDS_REGS *)&pb->dds[dds_chnl];
  
  if (dds_freq == 0)
    return(0);

  if ((dds_chnl < 0) || (dds_chnl >= V545_DDS_CHNLS_PER_BOARD - 1))
    {
#if DEBUG == 1
      printf("ERROR %s: dds channel number out of range: %d\n", __FUNCTION__, dds_chnl);
#endif
      return(-1);
    }      

  if ((dds_freq < 0) || (dds_freq > 20000))
    {
#if DEBUG == 1
      printf("ERROR %s: dds frequency out of range: %d\n", __FUNCTION__, dds_freq);
#endif
      return(-1);
    }

  pdds->dfr = 2 * (unsigned short)dds_freq;
  pdds->dph = 0;                // assume 0 phase lag
  pdds->dam = 0xffff;           // max amplitude
  return(0);
}


// configure channel control block
// ccb_chnl = 0-23
// dds_target_coil = 0-23 for input, or 24-31 for output
//
// defaults:
//
// DLY = 0 for no phase shift
// AMP = 0xffff for max amplitude

int v545_config_ccb(V545_REGS *pb, int EX_coil, int dds_target_coil)
{
  V545_CCB_REGS *pccb = (V545_CCB_REGS *)&pb->ccb[EX_coil];

  if ((EX_coil < 0) || (EX_coil >= V545_CCB_CHNLS_PER_BOARD))
    {
#if DEBUG == 1
      printf("ERROR %s: excitation coil number out of range: %d\n", __FUNCTION__, EX_coil);
#endif
      return(-1);
    }

  if ((dds_target_coil < V545_CCB_CHNLS_PER_BOARD) || (dds_target_coil >= (V545_CCB_CHNLS_PER_BOARD + V545_DDS_CHNLS_PER_BOARD)))
    {
#if DEBUG == 1
      printf("ERROR %s: dds channel number out of range: %d\n", __FUNCTION__, dds_target_coil);
#endif
      return(-1);
    }

  if (dds_target_coil >= V545_CCB_CHNLS_PER_BOARD)
    pccb->ctl.bits.ssr = 1;			// excitation is output from a DDS channel
  else
    pccb->ctl.bits.ssr = 0;			// excitation is input from an external source

  pccb->ctl.bits.dl = 0;			// no delay
  pccb->ctl.bits.fbits = 0;			// phase sensitive detector average over 1 cycle
  pccb->ctl.bits.x2 = 0;			// disable 2x gain
  pccb->ctl.bits.mbits = dds_target_coil;	// identify input coil for excitation

  pccb->dly = 0;		// no delay for phase shift
  pccb->amp = 0x7fff;		// max amplitude

  return(0);
}


// check for valid IO function block

int v545_is_a_valid_function_block_chnl(int fb_chnl)
{
  if ((fb_chnl >= 0) && (fb_chnl < V545_FB_CHNLS_PER_BOARD))
    return(1);
  else
    {
#if DEBUG == 1
      printf("ERROR %s: fb_chnl out of range: %d\n", __FUNCTION__, fb_chnl);
#endif
      return(0);
    }
}


// check for valid OVERRIDE function block

int v545_is_a_valid_override_block_chnl(int ob_chnl)
{
  if ((ob_chnl >= V545_FB_CHNLS_PER_BOARD) && (ob_chnl < V545_FB_CHNLS_PER_BOARD + V545_OB_CHNLS_PER_BOARD))
    return(1);
  else
    {
#if DEBUG == 1
      printf("ERROR %s: fb_chnl out of range: %d\n", __FUNCTION__, ob_chnl);
#endif
      return(0);
    }
}


// get regfile (0x80 offset) index for pair of registers (0-31)

int v545_override_get_regfile_index(V545_REGS *pb, int *regfile_index)
{
  int i;

  if (regfile_index == (int *)0)
    return(-1);

  for (i = 0; i < V545_REGFILE_WORD_SIZE / 2; i++)
    {
      if ((pb->regfile.entry[i].pos.us == 0) && (pb->regfile.entry[i].vel.us == 0))
        {
          *regfile_index = i;
          break;
        }
    }

  return(0);
}


// get function block type

int v545_get_fb_type(V545_REGS *pb, int fb_chnl)
{
  V545_FUNC_BLOCK_REGS *pfb_regs = &pb->fb[fb_chnl].fbregs;
  V545_SYNCHRO_RESOLVER_INPUT_REGS *pfb_sri = &pb->fb[fb_chnl].sri;	// synchro/resolver input
  V545_SYNCHRO_RESOLVER_OUTPUT_REGS *pfb_sro = &pb->fb[fb_chnl].sro;	// synchro/resolver output
  V545_LVDT_RVDT_INPUT_REGS *pfb_lri = &pb->fb[fb_chnl].lri;		// lvdt/rvdt input
  V545_LVDT_RVDT_OUTPUT_REGS *pfb_lro = &pb->fb[fb_chnl].lro;		// lvdt/rvdt output

  if (v545_is_a_valid_function_block_chnl(fb_chnl) == 0)
    return(-1);

  switch(pfb_regs->fun)
    {
    case V545_FUNC_CODE_SYNCHRO_RESOLVER_INPUT:
      if (pfb_sri->flg.bits.type == V545_SYNCHRO_RESOLVER_SET_SYNCHRO)
        return(V545_ITYPE_SYNCHRO_INPUT);
      else if (pfb_sri->flg.bits.type == V545_SYNCHRO_RESOLVER_SET_RESOLVER)
        return(V545_ITYPE_RESOLVER_INPUT);
      else
        return(-1);

    case V545_FUNC_CODE_SYNCHRO_RESOLVER_OUTPUT:
      if (pfb_sro->flg.bits.type == V545_SYNCHRO_RESOLVER_SET_SYNCHRO)
        return(V545_ITYPE_SYNCHRO_OUTPUT);
      else if (pfb_sro->flg.bits.type == V545_SYNCHRO_RESOLVER_SET_RESOLVER)
        return(V545_ITYPE_RESOLVER_OUTPUT);
      else
        return(-1);

    case V545_FUNC_CODE_LVDT_RVDT_INPUT:
      if (pfb_lri->flg.bits.type == V545_LVDT_RVDT_RATIOMETRIC)
        return(V545_ITYPE_LVDT_RVDT_RATIOMETRIC_INPUT);
      else if (pfb_lri->flg.bits.type == V545_LVDT_RVDT_OPEN_WIRING)
        return(V545_ITYPE_LVDT_RVDT_OPEN_WIRING_INPUT);
      else
        return(-1);

    case V545_FUNC_CODE_LVDT_RVDT_OUTPUT:
      if (pfb_lro->flg.bits.type == V545_LVDT_RVDT_RATIOMETRIC)
        return(V545_ITYPE_LVDT_RVDT_RATIOMETRIC_OUTPUT);
      else if (pfb_lro->flg.bits.type == V545_LVDT_RVDT_OPEN_WIRING)
        return(V545_ITYPE_LVDT_RVDT_OPEN_WIRING_OUTPUT);
      else
        return(-1);

    default:
      return(-1);
    }
}



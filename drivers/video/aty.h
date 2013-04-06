/*
 * Exported procedures for the ATI/mach64 display driver on PowerMacs.
 *
 * Copyright (C) 1997 Michael AK Tesch
 *  written with much help from Jon Howell
 *
 * Updated for 3D RAGE PRO by Geert Uytterhoeven
 *	
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

/*
 * most of the rest of this file comes from ATI sample code
 */
#ifndef REGMACH64_H
#define REGMACH64_H

/* NON-GUI MEMORY MAPPED Registers - expressed in BYTE offsets */

#define CRTC_H_TOTAL_DISP	0x0000	/* Dword offset 0_00 */
#define CRTC_H_SYNC_STRT_WID	0x0004	/* Dword offset 0_01 */
#define CRTC_H_SYNC_STRT	0x0004
#define CRTC_H_SYNC_DLY		0x0005
#define CRTC_H_SYNC_WID		0x0006

#define CRTC_V_TOTAL_DISP	0x0008	/* Dword offset 0_02 */
#define CRTC_V_TOTAL		0x0008
#define CRTC_V_DISP		0x000A
#define CRTC_V_SYNC_STRT_WID	0x000C	/* Dword offset 0_03 */
#define CRTC_V_SYNC_STRT	0x000C
#define CRTC_V_SYNC_WID		0x000E

#define CRTC_VLINE_CRNT_VLINE	0x0010	/* Dword offset 0_04 */
#define CRTC_OFF_PITCH		0x0014	/* Dword offset 0_05 */
#define CRTC_OFFSET		0x0014
#define CRTC_PITCH		0x0016

#define CRTC_INT_CNTL		0x0018	/* Dword offset 0_06 */
#define CRTC_GEN_CNTL		0x001C	/* Dword offset 0_07 */
#define CRTC_PIX_WIDTH		0x001D
#define CRTC_FIFO		0x001E
#define CRTC_EXT_DISP		0x001F

#define DSP_CONFIG		0x0020	/* Dword offset 0_08 */
#define DSP_ON_OFF		0x0024	/* Dword offset 0_09 */
#define TIMER_CONFIG		0x0028	/* Dword offset 0_0A */
#define MEM_BUF_CNTL		0x002C	/* Dword offset 0_0B */
#define MEM_ADDR_CONFIG		0x0034	/* Dword offset 0_0D */

#define CRT_TRAP		0x0038	/* Dword offset 0_0E */

#define I2C_CNTL_0		0x003C	/* Dword offset 0_0F */

#define OVR_CLR			0x0040	/* Dword offset 0_10 */
#define OVR_WID_LEFT_RIGHT	0x0044	/* Dword offset 0_11 */
#define OVR_WID_TOP_BOTTOM	0x0048	/* Dword offset 0_12 */

#define VGA_DSP_CONFIG		0x004C	/* Dword offset 0_13 */
#define VGA_DSP_ON_OFF		0x0050	/* Dword offset 0_14 */

#define CUR_CLR0		0x0060	/* Dword offset 0_18 */
#define CUR_CLR1		0x0064	/* Dword offset 0_19 */
#define CUR_OFFSET		0x0068	/* Dword offset 0_1A */
#define CUR_HORZ_VERT_POSN	0x006C	/* Dword offset 0_1B */
#define CUR_HORZ_VERT_OFF	0x0070	/* Dword offset 0_1C */

#define GP_IO			0x0078	/* Dword offset 0_1E */

#define HW_DEBUG		0x007C	/* Dword offset 0_1F */

#define SCRATCH_REG0		0x0080	/* Dword offset 0_20 */
#define SCRATCH_REG1		0x0084	/* Dword offset 0_21 */

#define CLOCK_CNTL		0x0090	/* Dword offset 0_24 */
#define CLOCK_SEL_CNTL		0x0090	/* Dword offset 0_24 */

#define CONFIG_STAT1		0x0094	/* Dword offset 0_25 */
#define CONFIG_STAT2		0x0098	/* Dword offset 0_26 */

#define BUS_CNTL		0x00A0	/* Dword offset 0_28 */

#define EXT_MEM_CNTL		0x00AC	/* Dword offset 0_2B */
#define MEM_CNTL		0x00B0	/* Dword offset 0_2C */

#define MEM_VGA_WP_SEL		0x00B4	/* Dword offset 0_2D */
#define MEM_VGA_RP_SEL		0x00B8	/* Dword offset 0_2E */

#define DAC_REGS		0x00C0	/* Dword offset 0_30 */
#define DAC_W_INDEX		0x00C0	/* Dword offset 0_30 */
#define DAC_DATA		0x00C1	/* Dword offset 0_30 */
#define DAC_MASK		0x00C2	/* Dword offset 0_30 */
#define DAC_R_INDEX		0x00C3	/* Dword offset 0_30 */
#define DAC_CNTL		0x00C4	/* Dword offset 0_31 */

#define EXT_DAC_REGS		0x00C8	/* Dword offset 0_32 */

#define GEN_TEST_CNTL		0x00D0	/* Dword offset 0_34 */

#define CUSTOM_MACRO_CNTL	0x00D4	/* Dword offset 0_35 */

#define CONFIG_CNTL		0x00DC	/* Dword offset 0_37 (CT, ET, VT) */
#define CONFIG_CHIP_ID		0x00E0	/* Dword offset 0_38 */
#define CONFIG_STAT0		0x00E4	/* Dword offset 0_39 */
#define CRC_SIG			0x00E8	/* Dword offset 0_3A */


/* GUI MEMORY MAPPED Registers */

#define DST_OFF_PITCH		0x0100	/* Dword offset 0_40 */
#define DST_X			0x0104	/* Dword offset 0_41 */
#define DST_Y			0x0108	/* Dword offset 0_42 */
#define DST_Y_X			0x010C	/* Dword offset 0_43 */
#define DST_WIDTH		0x0110	/* Dword offset 0_44 */
#define DST_HEIGHT		0x0114	/* Dword offset 0_45 */
#define DST_HEIGHT_WIDTH	0x0118	/* Dword offset 0_46 */
#define DST_X_WIDTH		0x011C	/* Dword offset 0_47 */
#define DST_BRES_LNTH		0x0120	/* Dword offset 0_48 */
#define DST_BRES_ERR		0x0124	/* Dword offset 0_49 */
#define DST_BRES_INC		0x0128	/* Dword offset 0_4A */
#define DST_BRES_DEC		0x012C	/* Dword offset 0_4B */
#define DST_CNTL		0x0130	/* Dword offset 0_4C */
#define DST_Y_X__ALIAS__	0x0134	/* Dword offset 0_4D */
#define TRAIL_BRES_ERR		0x0138	/* Dword offset 0_4E */
#define TRAIL_BRES_INC		0x013C	/* Dword offset 0_4F */
#define TRAIL_BRES_DEC		0x0140	/* Dword offset 0_50 */
#define LEAD_BRES_LNTH		0x0144	/* Dword offset 0_51 */
#define Z_OFF_PITCH		0x0148	/* Dword offset 0_52 */
#define Z_CNTL			0x014C	/* Dword offset 0_53 */
#define ALPHA_TST_CNTL		0x0150	/* Dword offset 0_54 */
#define SECONDARY_STW_EXP	0x0158	/* Dword offset 0_56 */
#define SECONDARY_S_X_INC	0x015C	/* Dword offset 0_57 */
#define SECONDARY_S_Y_INC	0x0160	/* Dword offset 0_58 */
#define SECONDARY_S_START	0x0164	/* Dword offset 0_59 */
#define SECONDARY_W_X_INC	0x0168	/* Dword offset 0_5A */
#define SECONDARY_W_Y_INC	0x016C	/* Dword offset 0_5B */
#define SECONDARY_W_START	0x0170	/* Dword offset 0_5C */
#define SECONDARY_T_X_INC	0x0174	/* Dword offset 0_5D */
#define SECONDARY_T_Y_INC	0x0178	/* Dword offset 0_5E */
#define SECONDARY_T_START	0x017C	/* Dword offset 0_5F */

#define SRC_OFF_PITCH		0x0180	/* Dword offset 0_60 */
#define SRC_X			0x0184	/* Dword offset 0_61 */
#define SRC_Y			0x0188	/* Dword offset 0_62 */
#define SRC_Y_X			0x018C	/* Dword offset 0_63 */
#define SRC_WIDTH1		0x0190	/* Dword offset 0_64 */
#define SRC_HEIGHT1		0x0194	/* Dword offset 0_65 */
#define SRC_HEIGHT1_WIDTH1	0x0198	/* Dword offset 0_66 */
#define SRC_X_START		0x019C	/* Dword offset 0_67 */
#define SRC_Y_START		0x01A0	/* Dword offset 0_68 */
#define SRC_Y_X_START		0x01A4	/* Dword offset 0_69 */
#define SRC_WIDTH2		0x01A8	/* Dword offset 0_6A */
#define SRC_HEIGHT2		0x01AC	/* Dword offset 0_6B */
#define SRC_HEIGHT2_WIDTH2	0x01B0	/* Dword offset 0_6C */
#define SRC_CNTL		0x01B4	/* Dword offset 0_6D */

#define SCALE_OFF		0x01C0	/* Dword offset 0_70 */
#define SECONDARY_SCALE_OFF	0x01C4	/* Dword offset 0_71 */

#define TEX_0_OFF		0x01C0	/* Dword offset 0_70 */
#define TEX_1_OFF		0x01C4	/* Dword offset 0_71 */
#define TEX_2_OFF		0x01C8	/* Dword offset 0_72 */
#define TEX_3_OFF		0x01CC	/* Dword offset 0_73 */
#define TEX_4_OFF		0x01D0	/* Dword offset 0_74 */
#define TEX_5_OFF		0x01D4	/* Dword offset 0_75 */
#define TEX_6_OFF		0x01D8	/* Dword offset 0_76 */
#define TEX_7_OFF		0x01DC	/* Dword offset 0_77 */

#define SCALE_WIDTH		0x01DC	/* Dword offset 0_77 */
#define SCALE_HEIGHT		0x01E0	/* Dword offset 0_78 */

#define TEX_8_OFF		0x01E0	/* Dword offset 0_78 */
#define TEX_9_OFF		0x01E4	/* Dword offset 0_79 */
#define TEX_10_OFF		0x01E8	/* Dword offset 0_7A */
#define S_Y_INC			0x01EC	/* Dword offset 0_7B */

#define SCALE_PITCH		0x01EC	/* Dword offset 0_7B */
#define SCALE_X_INC		0x01F0	/* Dword offset 0_7C */

#define RED_X_INC		0x01F0	/* Dword offset 0_7C */
#define GREEN_X_INC		0x01F4	/* Dword offset 0_7D */

#define SCALE_Y_INC		0x01F4	/* Dword offset 0_7D */
#define SCALE_VACC		0x01F8	/* Dword offset 0_7E */
#define SCALE_3D_CNTL		0x01FC	/* Dword offset 0_7F */

#define HOST_DATA0		0x0200	/* Dword offset 0_80 */
#define HOST_DATA1		0x0204	/* Dword offset 0_81 */
#define HOST_DATA2		0x0208	/* Dword offset 0_82 */
#define HOST_DATA3		0x020C	/* Dword offset 0_83 */
#define HOST_DATA4		0x0210	/* Dword offset 0_84 */
#define HOST_DATA5		0x0214	/* Dword offset 0_85 */
#define HOST_DATA6		0x0218	/* Dword offset 0_86 */
#define HOST_DATA7		0x021C	/* Dword offset 0_87 */
#define HOST_DATA8		0x0220	/* Dword offset 0_88 */
#define HOST_DATA9		0x0224	/* Dword offset 0_89 */
#define HOST_DATAA		0x0228	/* Dword offset 0_8A */
#define HOST_DATAB		0x022C	/* Dword offset 0_8B */
#define HOST_DATAC		0x0230	/* Dword offset 0_8C */
#define HOST_DATAD		0x0234	/* Dword offset 0_8D */
#define HOST_DATAE		0x0238	/* Dword offset 0_8E */
#define HOST_DATAF		0x023C	/* Dword offset 0_8F */
#define HOST_CNTL		0x0240	/* Dword offset 0_90 */

#define BM_HOSTDATA		0x0244	/* Dword offset 0_91 */
#define BM_ADDR			0x0248	/* Dword offset 0_92 */
#define BM_DATA			0x0248	/* Dword offset 0_92 */
#define BM_GUI_TABLE_CMD	0x024C	/* Dword offset 0_93 */

#define PAT_REG0		0x0280	/* Dword offset 0_A0 */
#define PAT_REG1		0x0284	/* Dword offset 0_A1 */
#define PAT_CNTL		0x0288	/* Dword offset 0_A2 */

#define SC_LEFT			0x02A0	/* Dword offset 0_A8 */
#define SC_RIGHT		0x02A4	/* Dword offset 0_A9 */
#define SC_LEFT_RIGHT		0x02A8	/* Dword offset 0_AA */
#define SC_TOP			0x02AC	/* Dword offset 0_AB */
#define SC_BOTTOM		0x02B0	/* Dword offset 0_AC */
#define SC_TOP_BOTTOM		0x02B4	/* Dword offset 0_AD */

#define DP_BKGD_CLR		0x02C0	/* Dword offset 0_B0 */
#define DP_FOG_CLR		0x02C4	/* Dword offset 0_B1 */
#define DP_FRGD_CLR		0x02C4	/* Dword offset 0_B1 */
#define DP_WRITE_MSK		0x02C8	/* Dword offset 0_B2 */
#define DP_CHAIN_MSK		0x02CC	/* Dword offset 0_B3 */
#define DP_PIX_WIDTH		0x02D0	/* Dword offset 0_B4 */
#define DP_MIX			0x02D4	/* Dword offset 0_B5 */
#define DP_SRC			0x02D8	/* Dword offset 0_B6 */
#define DP_FRGD_CLR_MIX		0x02DC	/* Dword offset 0_B7 */
#define DP_FRGD_BLGD_CLR	0x02E0	/* Dword offset 0_B8 */

#define DST_X_Y			0x02E8	/* Dword offset 0_BA */
#define DST_WIDTH_HEIGHT	0x02EC	/* Dword offset 0_BB */
#define USR_DST_PICTH		0x02F0	/* Dword offset 0_BC */
#define DP_SET_GUI_ENGINE2	0x02F8	/* Dword offset 0_BE */
#define DP_SET_GUI_ENGINE	0x02FC	/* Dword offset 0_BF */

#define CLR_CMP_CLR		0x0300	/* Dword offset 0_C0 */
#define CLR_CMP_MSK		0x0304	/* Dword offset 0_C1 */
#define CLR_CMP_CNTL		0x0308	/* Dword offset 0_C2 */

#define FIFO_STAT		0x0310	/* Dword offset 0_C4 */

#define CONTEXT_MASK		0x0320	/* Dword offset 0_C8 */
#define CONTEXT_LOAD_CNTL	0x032C	/* Dword offset 0_CB */

#define GUI_TRAJ_CNTL		0x0330	/* Dword offset 0_CC */
#define GUI_STAT		0x0338	/* Dword offset 0_CE */

#define TEX_PALETTE_INDEX	0x0340	/* Dword offset 0_D0 */
#define STW_EXP			0x0344	/* Dword offset 0_D1 */
#define LOG_MAX_INC		0x0348	/* Dword offset 0_D2 */
#define S_X_INC			0x034C	/* Dword offset 0_D3 */
#define S_Y_INC__ALIAS__	0x0350	/* Dword offset 0_D4 */

#define SCALE_PITCH__ALIAS__	0x0350	/* Dword offset 0_D4 */

#define S_START			0x0354	/* Dword offset 0_D5 */
#define W_X_INC			0x0358	/* Dword offset 0_D6 */
#define W_Y_INC			0x035C	/* Dword offset 0_D7 */
#define W_START			0x0360	/* Dword offset 0_D8 */
#define T_X_INC			0x0364	/* Dword offset 0_D9 */
#define T_Y_INC			0x0368	/* Dword offset 0_DA */

#define SECONDARY_SCALE_PITCH	0x0368	/* Dword offset 0_DA */

#define T_START			0x036C	/* Dword offset 0_DB */
#define TEX_SIZE_PITCH		0x0370	/* Dword offset 0_DC */
#define TEX_CNTL		0x0374	/* Dword offset 0_DD */
#define SECONDARY_TEX_OFFSET	0x0378	/* Dword offset 0_DE */
#define TEX_PALETTE		0x037C	/* Dword offset 0_DF */

#define SCALE_PITCH_BOTH	0x0380	/* Dword offset 0_E0 */
#define SECONDARY_SCALE_OFF_ACC	0x0384	/* Dword offset 0_E1 */
#define SCALE_OFF_ACC		0x0388	/* Dword offset 0_E2 */
#define SCALE_DST_Y_X		0x038C	/* Dword offset 0_E3 */

#define COMPOSITE_SHADOW_ID	0x0398	/* Dword offset 0_E6 */

#define SECONDARY_SCALE_X_INC	0x039C	/* Dword offset 0_E7 */

#define SPECULAR_RED_X_INC	0x039C	/* Dword offset 0_E7 */
#define SPECULAR_RED_Y_INC	0x03A0	/* Dword offset 0_E8 */
#define SPECULAR_RED_START	0x03A4	/* Dword offset 0_E9 */

#define SECONDARY_SCALE_HACC	0x03A4	/* Dword offset 0_E9 */

#define SPECULAR_GREEN_X_INC	0x03A8	/* Dword offset 0_EA */
#define SPECULAR_GREEN_Y_INC	0x03AC	/* Dword offset 0_EB */
#define SPECULAR_GREEN_START	0x03B0	/* Dword offset 0_EC */
#define SPECULAR_BLUE_X_INC	0x03B4	/* Dword offset 0_ED */
#define SPECULAR_BLUE_Y_INC	0x03B8	/* Dword offset 0_EE */
#define SPECULAR_BLUE_START	0x03BC	/* Dword offset 0_EF */

#define SCALE_X_INC__ALIAS__	0x03C0	/* Dword offset 0_F0 */

#define RED_X_INC__ALIAS__	0x03C0	/* Dword offset 0_F0 */
#define RED_Y_INC		0x03C4	/* Dword offset 0_F1 */
#define RED_START		0x03C8	/* Dword offset 0_F2 */

#define SCALE_HACC		0x03C8	/* Dword offset 0_F2 */
#define SCALE_Y_INC__ALIAS__	0x03CC	/* Dword offset 0_F3 */

#define GREEN_X_INC__ALIAS__	0x03CC	/* Dword offset 0_F3 */
#define GREEN_Y_INC		0x03D0	/* Dword offset 0_F4 */

#define SECONDARY_SCALE_Y_INC	0x03D0	/* Dword offset 0_F4 */
#define SECONDARY_SCALE_VACC	0x03D4	/* Dword offset 0_F5 */

#define GREEN_START		0x03D4	/* Dword offset 0_F5 */
#define BLUE_X_INC		0x03D8	/* Dword offset 0_F6 */
#define BLUE_Y_INC		0x03DC	/* Dword offset 0_F7 */
#define BLUE_START		0x03E0	/* Dword offset 0_F8 */
#define Z_X_INC			0x03E4	/* Dword offset 0_F9 */
#define Z_Y_INC			0x03E8	/* Dword offset 0_FA */
#define Z_START			0x03EC	/* Dword offset 0_FB */
#define ALPHA_X_INC		0x03F0	/* Dword offset 0_FC */
#define FOG_X_INC		0x03F0	/* Dword offset 0_FC */
#define ALPHA_Y_INC		0x03F4	/* Dword offset 0_FD */
#define FOG_Y_INC		0x03F4	/* Dword offset 0_FD */
#define ALPHA_START		0x03F8	/* Dword offset 0_FE */
#define FOG_START		0x03F8	/* Dword offset 0_FE */

#define OVERLAY_Y_X_START		0x0400	/* Dword offset 1_00 */
#define OVERLAY_Y_X_END			0x0404	/* Dword offset 1_01 */
#define OVERLAY_VIDEO_KEY_CLR		0x0408	/* Dword offset 1_02 */
#define OVERLAY_VIDEO_KEY_MSK		0x040C	/* Dword offset 1_03 */
#define OVERLAY_GRAPHICS_KEY_CLR	0x0410	/* Dword offset 1_04 */
#define OVERLAY_GRAPHICS_KEY_MSK	0x0414	/* Dword offset 1_05 */
#define OVERLAY_KEY_CNTL		0x0418	/* Dword offset 1_06 */

#define OVERLAY_SCALE_INC	0x0420	/* Dword offset 1_08 */
#define OVERLAY_SCALE_CNTL	0x0424	/* Dword offset 1_09 */
#define SCALER_HEIGHT_WIDTH	0x0428	/* Dword offset 1_0A */
#define SCALER_TEST		0x042C	/* Dword offset 1_0B */
#define SCALER_BUF0_OFFSET	0x0434	/* Dword offset 1_0D */
#define SCALER_BUF1_OFFSET	0x0438	/* Dword offset 1_0E */
#define SCALE_BUF_PITCH		0x043C	/* Dword offset 1_0F */

#define CAPTURE_START_END	0x0440	/* Dword offset 1_10 */
#define CAPTURE_X_WIDTH		0x0444	/* Dword offset 1_11 */
#define VIDEO_FORMAT		0x0448	/* Dword offset 1_12 */
#define VBI_START_END		0x044C	/* Dword offset 1_13 */
#define CAPTURE_CONFIG		0x0450	/* Dword offset 1_14 */
#define TRIG_CNTL		0x0454	/* Dword offset 1_15 */

#define OVERLAY_EXCLUSIVE_HORZ	0x0458	/* Dword offset 1_16 */
#define OVERLAY_EXCLUSIVE_VERT	0x045C	/* Dword offset 1_17 */

#define VAL_WIDTH		0x0460	/* Dword offset 1_18 */
#define CAPTURE_DEBUG		0x0464	/* Dword offset 1_19 */
#define VIDEO_SYNC_TEST		0x0468	/* Dword offset 1_1A */

#define SNAPSHOT_VH_COUNTS	0x0470	/* Dword offset 1_1C */
#define SNAPSHOT_F_COUNT	0x0474	/* Dword offset 1_1D */
#define N_VIF_COUNT		0x0478	/* Dword offset 1_1E */
#define SNAPSHOT_VIF_COUNT	0x047C	/* Dword offset 1_1F */

#define CAPTURE_BUF0_OFFSET	0x0480	/* Dword offset 1_20 */
#define CAPTURE_BUF1_OFFSET	0x0484	/* Dword offset 1_21 */
#define CAPTURE_BUF_PITCH	0x0488	/* Dword offset 1_22 */

#define MPP_CONFIG		0x04C0	/* Dword offset 1_30 */
#define MPP_STROBE_SEQ		0x04C4	/* Dword offset 1_31 */
#define MPP_ADDR		0x04C8	/* Dword offset 1_32 */
#define MPP_DATA		0x04CC	/* Dword offset 1_33 */
#define TVO_CNTL		0x0500	/* Dword offset 1_40 */

#define CRT_HORZ_VERT_LOAD	0x0544	/* Dword offset 1_51 */

#define AGP_BASE		0x0548	/* Dword offset 1_52 */
#define AGP_CNTL		0x054C	/* Dword offset 1_53 */

#define SCALER_COLOUR_CNTL	0x0550	/* Dword offset 1_54 */
#define SCALER_H_COEFF0		0x0554	/* Dword offset 1_55 */
#define SCALER_H_COEFF1		0x0558	/* Dword offset 1_56 */
#define SCALER_H_COEFF2		0x055C	/* Dword offset 1_57 */
#define SCALER_H_COEFF3		0x0560	/* Dword offset 1_58 */
#define SCALER_H_COEFF4		0x0564	/* Dword offset 1_59 */

#define GUI_CNTL		0x0578	/* Dword offset 1_5E */

#define BM_FRAME_BUF_OFFSET	0x0580	/* Dword offset 1_60 */
#define BM_SYSTEM_MEM_ADDR	0x0584	/* Dword offset 1_61 */
#define BM_COMMAND		0x0588	/* Dword offset 1_62 */
#define BM_STATUS		0x058C	/* Dword offset 1_63 */
#define BM_GUI_TABLE		0x05B8	/* Dword offset 1_6E */
#define BM_SYSTEM_TABLE		0x05BC	/* Dword offset 1_6F */

#define SCALER_BUF0_OFFSET_U	0x05D4	/* Dword offset 1_75 */
#define SCALER_BUF0_OFFSET_V	0x05D8	/* Dword offset 1_76 */
#define SCALER_BUF1_OFFSET_U	0x05DC	/* Dword offset 1_77 */
#define SCALER_BUF1_OFFSET_V	0x05E0	/* Dword offset 1_78 */

#define VERTEX_1_S		0x0640	/* Dword offset 1_90 */
#define VERTEX_1_T		0x0644	/* Dword offset 1_91 */
#define VERTEX_1_W		0x0648	/* Dword offset 1_92 */
#define VERTEX_1_SPEC_ARGB	0x064C	/* Dword offset 1_93 */
#define VERTEX_1_Z		0x0650	/* Dword offset 1_94 */
#define VERTEX_1_ARGB		0x0654	/* Dword offset 1_95 */
#define VERTEX_1_X_Y		0x0658	/* Dword offset 1_96 */
#define ONE_OVER_AREA		0x065C	/* Dword offset 1_97 */
#define VERTEX_2_S		0x0660	/* Dword offset 1_98 */
#define VERTEX_2_T		0x0664	/* Dword offset 1_99 */
#define VERTEX_2_W		0x0668	/* Dword offset 1_9A */
#define VERTEX_2_SPEC_ARGB	0x066C	/* Dword offset 1_9B */
#define VERTEX_2_Z		0x0670	/* Dword offset 1_9C */
#define VERTEX_2_ARGB		0x0674	/* Dword offset 1_9D */
#define VERTEX_2_X_Y		0x0678	/* Dword offset 1_9E */
#define ONE_OVER_AREA		0x065C	/* Dword offset 1_9F */
#define VERTEX_3_S		0x0680	/* Dword offset 1_A0 */
#define VERTEX_3_T		0x0684	/* Dword offset 1_A1 */
#define VERTEX_3_W		0x0688	/* Dword offset 1_A2 */
#define VERTEX_3_SPEC_ARGB	0x068C	/* Dword offset 1_A3 */
#define VERTEX_3_Z		0x0690	/* Dword offset 1_A4 */
#define VERTEX_3_ARGB		0x0694	/* Dword offset 1_A5 */
#define VERTEX_3_X_Y		0x0698	/* Dword offset 1_A6 */
#define ONE_OVER_AREA		0x065C	/* Dword offset 1_A7 */
#define VERTEX_1_S		0x0640	/* Dword offset 1_AB */
#define VERTEX_1_T		0x0644	/* Dword offset 1_AC */
#define VERTEX_1_W		0x0648	/* Dword offset 1_AD */
#define VERTEX_2_S		0x0660	/* Dword offset 1_AE */
#define VERTEX_2_T		0x0664	/* Dword offset 1_AF */
#define VERTEX_2_W		0x0668	/* Dword offset 1_B0 */
#define VERTEX_3_SECONDARY_S	0x06C0	/* Dword offset 1_B0 */
#define VERTEX_3_S		0x0680	/* Dword offset 1_B1 */
#define VERTEX_3_SECONDARY_T	0x06C4	/* Dword offset 1_B1 */
#define VERTEX_3_T		0x0684	/* Dword offset 1_B2 */
#define VERTEX_3_SECONDARY_W	0x06C8	/* Dword offset 1_B2 */
#define VERTEX_3_W		0x0688	/* Dword offset 1_B3 */
#define VERTEX_1_SPEC_ARGB	0x064C	/* Dword offset 1_B4 */
#define VERTEX_2_SPEC_ARGB	0x066C	/* Dword offset 1_B5 */
#define VERTEX_3_SPEC_ARGB	0x068C	/* Dword offset 1_B6 */
#define VERTEX_1_Z		0x0650	/* Dword offset 1_B7 */
#define VERTEX_2_Z		0x0670	/* Dword offset 1_B8 */
#define VERTEX_3_Z		0x0690	/* Dword offset 1_B9 */
#define VERTEX_1_ARGB		0x0654	/* Dword offset 1_BA */
#define VERTEX_2_ARGB		0x0674	/* Dword offset 1_BB */
#define VERTEX_3_ARGB		0x0694	/* Dword offset 1_BC */
#define VERTEX_1_X_Y		0x0658	/* Dword offset 1_BD */
#define VERTEX_2_X_Y		0x0678	/* Dword offset 1_BE */
#define VERTEX_3_X_Y		0x0698	/* Dword offset 1_BF */
#define ONE_OVER_AREA_UC	0x0700	/* Dword offset 1_C0 */
#define SETUP_CNTL		0x0704	/* Dword offset 1_C1 */
#define VERTEX_1_SECONDARY_S	0x0728	/* Dword offset 1_CA */
#define VERTEX_1_SECONDARY_T	0x072C	/* Dword offset 1_CB */
#define VERTEX_1_SECONDARY_W	0x0730	/* Dword offset 1_CC */
#define VERTEX_2_SECONDARY_S	0x0734	/* Dword offset 1_CD */
#define VERTEX_2_SECONDARY_T	0x0738	/* Dword offset 1_CE */
#define VERTEX_2_SECONDARY_W	0x073C	/* Dword offset 1_CF */


/* CRTC control values (mostly CRTC_GEN_CNTL) */

#define CRTC_H_SYNC_NEG		0x00200000
#define CRTC_V_SYNC_NEG		0x00200000

#define CRTC_DBL_SCAN_EN	0x00000001
#define CRTC_INTERLACE_EN	0x00000002
#define CRTC_HSYNC_DIS		0x00000004
#define CRTC_VSYNC_DIS		0x00000008
#define CRTC_CSYNC_EN		0x00000010
#define CRTC_PIX_BY_2_EN	0x00000020	/* unused on RAGE */
#define CRTC_DISPLAY_DIS	0x00000040
#define CRTC_VGA_XOVERSCAN	0x00000040

#define CRTC_PIX_WIDTH_MASK	0x00000700
#define CRTC_PIX_WIDTH_4BPP	0x00000100
#define CRTC_PIX_WIDTH_8BPP	0x00000200
#define CRTC_PIX_WIDTH_15BPP	0x00000300
#define CRTC_PIX_WIDTH_16BPP	0x00000400
#define CRTC_PIX_WIDTH_24BPP	0x00000500
#define CRTC_PIX_WIDTH_32BPP	0x00000600

#define CRTC_BYTE_PIX_ORDER	0x00000800
#define CRTC_PIX_ORDER_MSN_LSN	0x00000000
#define CRTC_PIX_ORDER_LSN_MSN	0x00000800

#define CRTC_FIFO_LWM		0x000f0000

#define VGA_128KAP_PAGING	0x00100000
#define VFC_SYNC_TRISTATE	0x00200000
#define CRTC_LOCK_REGS		0x00400000
#define CRTC_SYNC_TRISTATE	0x00800000

#define CRTC_EXT_DISP_EN	0x01000000
#define CRTC_ENABLE		0x02000000
#define CRTC_DISP_REQ_ENB	0x04000000
#define VGA_ATI_LINEAR		0x08000000
#define CRTC_VSYNC_FALL_EDGE	0x10000000
#define VGA_TEXT_132		0x20000000
#define VGA_XCRT_CNT_EN		0x40000000
#define VGA_CUR_B_TEST		0x80000000

#define CRTC_CRNT_VLINE		0x07f00000
#define CRTC_VBLANK		0x00000001


/* DAC control values */

#define DAC_EXT_SEL_RS2		0x01
#define DAC_EXT_SEL_RS3		0x02
#define DAC_8BIT_EN		0x00000100
#define DAC_PIX_DLY_MASK	0x00000600
#define DAC_PIX_DLY_0NS		0x00000000
#define DAC_PIX_DLY_2NS		0x00000200
#define DAC_PIX_DLY_4NS		0x00000400
#define DAC_BLANK_ADJ_MASK	0x00001800
#define DAC_BLANK_ADJ_0		0x00000000
#define DAC_BLANK_ADJ_1		0x00000800
#define DAC_BLANK_ADJ_2		0x00001000


/* Mix control values */

#define MIX_NOT_DST		0x0000
#define MIX_0			0x0001
#define MIX_1			0x0002
#define MIX_DST			0x0003
#define MIX_NOT_SRC		0x0004
#define MIX_XOR			0x0005
#define MIX_XNOR		0x0006
#define MIX_SRC			0x0007
#define MIX_NAND		0x0008
#define MIX_NOT_SRC_OR_DST	0x0009
#define MIX_SRC_OR_NOT_DST	0x000a
#define MIX_OR			0x000b
#define MIX_AND			0x000c
#define MIX_SRC_AND_NOT_DST	0x000d
#define MIX_NOT_SRC_AND_DST	0x000e
#define MIX_NOR			0x000f

/* Maximum engine dimensions */
#define ENGINE_MIN_X		0
#define ENGINE_MIN_Y		0
#define ENGINE_MAX_X		4095
#define ENGINE_MAX_Y		16383

/* Mach64 engine bit constants - these are typically ORed together */

/* BUS_CNTL register constants */
#define BUS_FIFO_ERR_ACK	0x00200000
#define BUS_HOST_ERR_ACK	0x00800000

/* GEN_TEST_CNTL register constants */
#define GEN_OVR_OUTPUT_EN	0x20
#define HWCURSOR_ENABLE		0x80
#define GUI_ENGINE_ENABLE	0x100
#define BLOCK_WRITE_ENABLE	0x200

/* DSP_CONFIG register constants */
#define DSP_XCLKS_PER_QW	0x00003fff
#define DSP_LOOP_LATENCY	0x000f0000
#define DSP_PRECISION		0x00700000

/* DSP_ON_OFF register constants */
#define DSP_OFF			0x000007ff
#define DSP_ON			0x07ff0000

/* CLOCK_CNTL register constants */
#define CLOCK_SEL		0x0f
#define CLOCK_DIV		0x30
#define CLOCK_DIV1		0x00
#define CLOCK_DIV2		0x10
#define CLOCK_DIV4		0x20
#define CLOCK_STROBE		0x40
#define PLL_WR_EN		0x02

/* PLL registers */
#define PLL_MACRO_CNTL		0x01
#define PLL_REF_DIV		0x02
#define PLL_GEN_CNTL		0x03
#define MCLK_FB_DIV		0x04
#define PLL_VCLK_CNTL		0x05
#define VCLK_POST_DIV		0x06
#define VCLK0_FB_DIV		0x07
#define VCLK1_FB_DIV		0x08
#define VCLK2_FB_DIV		0x09
#define VCLK3_FB_DIV		0x0A
#define PLL_XCLK_CNTL		0x0B
#define PLL_TEST_CTRL		0x0E
#define PLL_TEST_COUNT		0x0F

/* Fields in PLL registers */
#define PLL_PC_GAIN		0x07
#define PLL_VC_GAIN		0x18
#define PLL_DUTY_CYC		0xE0
#define PLL_OVERRIDE		0x01
#define PLL_MCLK_RST		0x02
#define OSC_EN			0x04
#define EXT_CLK_EN		0x08
#define MCLK_SRC_SEL		0x70
#define EXT_CLK_CNTL		0x80
#define VCLK_SRC_SEL		0x03
#define PLL_VCLK_RST		0x04
#define VCLK_INVERT		0x08
#define VCLK0_POST		0x03
#define VCLK1_POST		0x0C
#define VCLK2_POST		0x30
#define VCLK3_POST		0xC0

/* CONFIG_CNTL register constants */
#define APERTURE_4M_ENABLE	1
#define APERTURE_8M_ENABLE	2
#define VGA_APERTURE_ENABLE	4

/* CONFIG_STAT0 register constants (GX, CX) */
#define CFG_BUS_TYPE		0x00000007
#define CFG_MEM_TYPE		0x00000038
#define CFG_INIT_DAC_TYPE	0x00000e00

/* CONFIG_STAT0 register constants (CT, ET, VT) */
#define CFG_MEM_TYPE_xT		0x00000007

#define ISA			0
#define EISA			1
#define LOCAL_BUS		6
#define PCI			7

/* Memory types for GX, CX */
#define DRAMx4			0
#define VRAMx16			1
#define VRAMx16ssr		2
#define DRAMx16			3
#define GraphicsDRAMx16		4
#define EnhancedVRAMx16		5
#define EnhancedVRAMx16ssr	6

/* Memory types for CT, ET, VT, GT */
#define DRAM			1
#define EDO			2
#define PSEUDO_EDO		3
#define SDRAM			4
#define SGRAM			5
#define WRAM			6

#define DAC_INTERNAL		0x00
#define DAC_IBMRGB514		0x01
#define DAC_ATI68875		0x02
#define DAC_TVP3026_A		0x72
#define DAC_BT476		0x03
#define DAC_BT481		0x04
#define DAC_ATT20C491		0x14
#define DAC_SC15026		0x24
#define DAC_MU9C1880		0x34
#define DAC_IMSG174		0x44
#define DAC_ATI68860_B		0x05
#define DAC_ATI68860_C		0x15
#define DAC_TVP3026_B		0x75
#define DAC_STG1700		0x06
#define DAC_ATT498		0x16
#define DAC_STG1702		0x07
#define DAC_SC15021		0x17
#define DAC_ATT21C498		0x27
#define DAC_STG1703		0x37
#define DAC_CH8398		0x47
#define DAC_ATT20C408		0x57

#define CLK_ATI18818_0		0
#define CLK_ATI18818_1		1
#define CLK_STG1703		2
#define CLK_CH8398		3
#define CLK_INTERNAL		4
#define CLK_ATT20C408		5
#define CLK_IBMRGB514		6

/* MEM_CNTL register constants */
#define MEM_SIZE_ALIAS		0x00000007
#define MEM_SIZE_512K		0x00000000
#define MEM_SIZE_1M		0x00000001
#define MEM_SIZE_2M		0x00000002
#define MEM_SIZE_4M		0x00000003
#define MEM_SIZE_6M		0x00000004
#define MEM_SIZE_8M		0x00000005
#define MEM_SIZE_ALIAS_GTB	0x0000000F
#define MEM_SIZE_2M_GTB		0x00000003
#define MEM_SIZE_4M_GTB		0x00000007
#define MEM_SIZE_6M_GTB		0x00000009
#define MEM_SIZE_8M_GTB		0x0000000B
#define MEM_BNDRY		0x00030000
#define MEM_BNDRY_0K		0x00000000
#define MEM_BNDRY_256K		0x00010000
#define MEM_BNDRY_512K		0x00020000
#define MEM_BNDRY_1M		0x00030000
#define MEM_BNDRY_EN		0x00040000

/* ATI PCI constants */
#define PCI_ATI_VENDOR_ID	0x1002
#define PCI_MACH64_GX		0x4758
#define PCI_MACH64_CX		0x4358
#define PCI_MACH64_CT		0x4354
#define PCI_MACH64_ET		0x4554
#define PCI_MACH64_VT		0x5654
#define PCI_MACH64_GT		0x4754

/* CONFIG_CHIP_ID register constants */
#define CFG_CHIP_TYPE		0x0000FFFF
#define CFG_CHIP_CLASS		0x00FF0000
#define CFG_CHIP_REV		0xFF000000
#define CFG_CHIP_VERSION	0x07000000
#define CFG_CHIP_FOUNDRY	0x38000000
#define CFG_CHIP_REVISION	0xC0000000

/* Chip IDs read from CONFIG_CHIP_ID */
#define MACH64_GX_ID		0xD7
#define MACH64_CX_ID		0x57
#define MACH64_CT_ID		0x4354
#define MACH64_ET_ID		0x4554
#define MACH64_VT_ID		0x5654
#define MACH64_GT_ID		0x4754

/* Mach64 chip types */
#define MACH64_UNKNOWN		0
#define MACH64_GX		1
#define MACH64_CX		2
#define MACH64_CT		3
#define MACH64_ET		4
#define MACH64_VT		5
#define MACH64_GT		6

/* DST_CNTL register constants */
#define DST_X_RIGHT_TO_LEFT	0
#define DST_X_LEFT_TO_RIGHT	1
#define DST_Y_BOTTOM_TO_TOP	0
#define DST_Y_TOP_TO_BOTTOM	2
#define DST_X_MAJOR		0
#define DST_Y_MAJOR		4
#define DST_X_TILE		8
#define DST_Y_TILE		0x10
#define DST_LAST_PEL		0x20
#define DST_POLYGON_ENABLE	0x40
#define DST_24_ROTATION_ENABLE	0x80

/* SRC_CNTL register constants */
#define SRC_PATTERN_ENABLE		1
#define SRC_ROTATION_ENABLE		2
#define SRC_LINEAR_ENABLE		4
#define SRC_BYTE_ALIGN			8
#define SRC_LINE_X_RIGHT_TO_LEFT	0
#define SRC_LINE_X_LEFT_TO_RIGHT	0x10

/* HOST_CNTL register constants */
#define HOST_BYTE_ALIGN		1

/* GUI_TRAJ_CNTL register constants */
#define PAT_MONO_8x8_ENABLE	0x01000000
#define PAT_CLR_4x2_ENABLE	0x02000000
#define PAT_CLR_8x1_ENABLE	0x04000000

/* DP_CHAIN_MASK register constants */
#define DP_CHAIN_4BPP		0x8888
#define DP_CHAIN_7BPP		0xD2D2
#define DP_CHAIN_8BPP		0x8080
#define DP_CHAIN_8BPP_RGB	0x9292
#define DP_CHAIN_15BPP		0x4210
#define DP_CHAIN_16BPP		0x8410
#define DP_CHAIN_24BPP		0x8080
#define DP_CHAIN_32BPP		0x8080

/* DP_PIX_WIDTH register constants */
#define DST_1BPP		0
#define DST_4BPP		1
#define DST_8BPP		2
#define DST_15BPP		3
#define DST_16BPP		4
#define DST_32BPP		6
#define SRC_1BPP		0
#define SRC_4BPP		0x100
#define SRC_8BPP		0x200
#define SRC_15BPP		0x300
#define SRC_16BPP		0x400
#define SRC_32BPP		0x600
#define HOST_1BPP		0
#define HOST_4BPP		0x10000
#define HOST_8BPP		0x20000
#define HOST_15BPP		0x30000
#define HOST_16BPP		0x40000
#define HOST_32BPP		0x60000
#define BYTE_ORDER_MSB_TO_LSB	0
#define BYTE_ORDER_LSB_TO_MSB	0x1000000

/* DP_MIX register constants */
#define BKGD_MIX_NOT_D			0
#define BKGD_MIX_ZERO			1
#define BKGD_MIX_ONE			2
#define BKGD_MIX_D			3
#define BKGD_MIX_NOT_S			4
#define BKGD_MIX_D_XOR_S		5
#define BKGD_MIX_NOT_D_XOR_S		6
#define BKGD_MIX_S			7
#define BKGD_MIX_NOT_D_OR_NOT_S		8
#define BKGD_MIX_D_OR_NOT_S		9
#define BKGD_MIX_NOT_D_OR_S		10
#define BKGD_MIX_D_OR_S			11
#define BKGD_MIX_D_AND_S		12
#define BKGD_MIX_NOT_D_AND_S		13
#define BKGD_MIX_D_AND_NOT_S		14
#define BKGD_MIX_NOT_D_AND_NOT_S	15
#define BKGD_MIX_D_PLUS_S_DIV2		0x17
#define FRGD_MIX_NOT_D			0
#define FRGD_MIX_ZERO			0x10000
#define FRGD_MIX_ONE			0x20000
#define FRGD_MIX_D			0x30000
#define FRGD_MIX_NOT_S			0x40000
#define FRGD_MIX_D_XOR_S		0x50000
#define FRGD_MIX_NOT_D_XOR_S		0x60000
#define FRGD_MIX_S			0x70000
#define FRGD_MIX_NOT_D_OR_NOT_S		0x80000
#define FRGD_MIX_D_OR_NOT_S		0x90000
#define FRGD_MIX_NOT_D_OR_S		0xa0000
#define FRGD_MIX_D_OR_S			0xb0000
#define FRGD_MIX_D_AND_S		0xc0000
#define FRGD_MIX_NOT_D_AND_S		0xd0000
#define FRGD_MIX_D_AND_NOT_S		0xe0000
#define FRGD_MIX_NOT_D_AND_NOT_S	0xf0000
#define FRGD_MIX_D_PLUS_S_DIV2		0x170000

/* DP_SRC register constants */
#define BKGD_SRC_BKGD_CLR	0
#define BKGD_SRC_FRGD_CLR	1
#define BKGD_SRC_HOST		2
#define BKGD_SRC_BLIT		3
#define BKGD_SRC_PATTERN	4
#define FRGD_SRC_BKGD_CLR	0
#define FRGD_SRC_FRGD_CLR	0x100
#define FRGD_SRC_HOST		0x200
#define FRGD_SRC_BLIT		0x300
#define FRGD_SRC_PATTERN	0x400
#define MONO_SRC_ONE		0
#define MONO_SRC_PATTERN	0x10000
#define MONO_SRC_HOST		0x20000
#define MONO_SRC_BLIT		0x30000

/* CLR_CMP_CNTL register constants */
#define COMPARE_FALSE		0
#define COMPARE_TRUE		1
#define COMPARE_NOT_EQUAL	4
#define COMPARE_EQUAL		5
#define COMPARE_DESTINATION	0
#define COMPARE_SOURCE		0x1000000

/* FIFO_STAT register constants */
#define FIFO_ERR		0x80000000

/* CONTEXT_LOAD_CNTL constants */
#define CONTEXT_NO_LOAD			0
#define CONTEXT_LOAD			0x10000
#define CONTEXT_LOAD_AND_DO_FILL	0x20000
#define CONTEXT_LOAD_AND_DO_LINE	0x30000
#define CONTEXT_EXECUTE			0
#define CONTEXT_CMD_DISABLE		0x80000000

/* GUI_STAT register constants */
#define ENGINE_IDLE		0
#define ENGINE_BUSY		1
#define SCISSOR_LEFT_FLAG	0x10
#define SCISSOR_RIGHT_FLAG	0x20
#define SCISSOR_TOP_FLAG	0x40
#define SCISSOR_BOTTOM_FLAG	0x80

/* ATI VGA Extended Regsiters */
#define sioATIEXT		0x1ce
#define bioATIEXT		0x3ce

#define ATI2E			0xae
#define ATI32			0xb2
#define ATI36			0xb6

/* VGA Graphics Controller Registers */
#define VGAGRA			0x3ce
#define GRA06			0x06

/* VGA Seququencer Registers */
#define VGASEQ			0x3c4
#define SEQ02			0x02
#define SEQ04			0x04

#define MACH64_MAX_X		ENGINE_MAX_X
#define MACH64_MAX_Y		ENGINE_MAX_Y

#define INC_X			0x0020
#define INC_Y			0x0080

#define RGB16_555		0x0000
#define RGB16_565		0x0040
#define RGB16_655		0x0080
#define RGB16_664		0x00c0

#define POLY_TEXT_TYPE		0x0001
#define IMAGE_TEXT_TYPE		0x0002
#define TEXT_TYPE_8_BIT		0x0004
#define TEXT_TYPE_16_BIT	0x0008
#define POLY_TEXT_TYPE_8	(POLY_TEXT_TYPE | TEXT_TYPE_8_BIT)
#define IMAGE_TEXT_TYPE_8	(IMAGE_TEXT_TYPE | TEXT_TYPE_8_BIT)
#define POLY_TEXT_TYPE_16	(POLY_TEXT_TYPE | TEXT_TYPE_16_BIT)
#define IMAGE_TEXT_TYPE_16	(IMAGE_TEXT_TYPE | TEXT_TYPE_16_BIT)

#define MACH64_NUM_CLOCKS	16
#define MACH64_NUM_FREQS	50

/* Wait until "v" queue entries are free */
#define aty_WaitQueue(v)    { while ((aty_ld_le32(FIFO_STAT) & 0xffff) > \
			 ((unsigned short)(0x8000 >> (v)))); }

/* Wait until GP is idle and queue is empty */
#define aty_WaitIdleEmpty() { aty_WaitQueue(16); \
			  while ((aty_ld_le32(GUI_STAT) & 1) != 0); }

#define SKIP_2(_v) ((((_v)<<1)&0xfff8)|((_v)&0x3)|(((_v)&0x80)>>5))

#define MACH64_BIT_BLT(_srcx, _srcy, _dstx, _dsty, _w, _h, _dir) \
{ \
    aty_WaitQueue(5); \
    aty_st_le32(SRC_Y_X, (((_srcx) << 16) | ((_srcy) & 0x0000ffff))); \
    aty_st_le32(SRC_WIDTH1, (_w)); \
    aty_st_le32(DST_CNTL, (_dir)); \
    aty_st_le32(DST_Y_X, (((_dstx) << 16) | ((_dsty) & 0x0000ffff))); \
    aty_st_le32(DST_HEIGHT_WIDTH, (((_w) << 16) | ((_h) & 0x0000ffff))); \
}
#endif /* REGMACH64_H */

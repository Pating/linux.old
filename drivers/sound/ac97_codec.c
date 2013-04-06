/*
 * ac97_codec.c: Generic AC97 mixer module
 *
 * Derived from ac97 mixer in maestro and trident driver.
 *
 * Copyright 2000 Silicon Integrated System Corporation
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  History
 *	Jan 14 2000 Ollie Lho <ollie@sis.com.tw> 
 *	Isloated from trident.c to support multiple ac97 codec
 */
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>

#include "ac97_codec.h"

static int ac97_read_mixer(struct ac97_codec *codec, int oss_channel);
static void ac97_write_mixer(struct ac97_codec *codec, int oss_channel, 
			     unsigned int left, unsigned int right);
static void ac97_set_mixer(struct ac97_codec *codec, unsigned int oss_mixer, unsigned int val );
static int ac97_recmask_io(struct ac97_codec *codec, int rw, int mask);
static int ac97_mixer_ioctl(struct ac97_codec *codec, unsigned int cmd, unsigned long arg);

static struct {
	unsigned int id;
	char *name;
	int  (*init)  (struct ac97_codec *codec);
} snd_ac97_codec_ids[] = {
	{0x414B4D00, "Asahi Kasei AK4540"     , NULL},
	{0x41445340, "Analog Devices AD1881"  , NULL},
	{0x43525900, "Cirrus Logic CS4297"    , NULL},
	{0x43525913, "Cirrus Logic CS4297A"   , NULL},
	{0x43525931, "Cirrus Logic CS4299"    , NULL},
	{0x4e534331, "National Semiconductor LM4549", NULL},
	{0x83847600, "SigmaTel STAC????"      , NULL},
	{0x83847604, "SigmaTel STAC9701/3/4/5", NULL},
	{0x83847605, "SigmaTel STAC9704"      , NULL},
	{0x83847608, "SigmaTel STAC9708"      , NULL},
	{0x83847609, "SigmaTel STAC9721/23"   , NULL},
	{0x00000000, NULL, NULL}
};


/* this table has default mixer values for all OSS mixers. */
static struct mixer_defaults {
	int mixer;
	unsigned int value;
} mixer_defaults[SOUND_MIXER_NRDEVICES] = {
	/* all values 0 -> 100 in bytes */
	{SOUND_MIXER_VOLUME,	0x3232},
	{SOUND_MIXER_BASS,	0x3232},
	{SOUND_MIXER_TREBLE,	0x3232},
	{SOUND_MIXER_PCM,	0x3232},
	{SOUND_MIXER_SPEAKER,	0x3232},
	{SOUND_MIXER_LINE,	0x3232},
	{SOUND_MIXER_MIC,	0x3232},
	{SOUND_MIXER_CD,	0x3232},
	{SOUND_MIXER_ALTPCM,	0x3232},
	{SOUND_MIXER_IGAIN,	0x3232},
	{SOUND_MIXER_LINE1,	0x3232},
	{SOUND_MIXER_PHONEIN,	0x3232},
	{SOUND_MIXER_PHONEOUT,	0x3232},
	{SOUND_MIXER_VIDEO,	0x3232},
	{-1,0}
};

/* table to scale scale from OSS mixer value to AC97 mixer register value */	
static struct ac97_mixer_hw {
	unsigned char offset;
	int scale;
} ac97_hw[SOUND_MIXER_NRDEVICES]= {
	[SOUND_MIXER_VOLUME]	=	{AC97_MASTER_VOL_STEREO,63},
	[SOUND_MIXER_BASS]	=	{AC97_MASTER_TONE,	15},
	[SOUND_MIXER_TREBLE]	=	{AC97_MASTER_TONE, 	15},
	[SOUND_MIXER_PCM]	=	{AC97_PCMOUT_VOL,	31},
	[SOUND_MIXER_SPEAKER]	=	{AC97_PCBEEP_VOL,  	15},
	[SOUND_MIXER_LINE]	=	{AC97_LINEIN_VOL,	31},
	[SOUND_MIXER_MIC]	=	{AC97_MIC_VOL,		31},
	[SOUND_MIXER_CD]	=	{AC97_CD_VOL,		31},
	[SOUND_MIXER_ALTPCM]	=	{AC97_HEADPHONE_VOL,	63},
	[SOUND_MIXER_IGAIN]	=	{AC97_RECORD_GAIN,	31},
	[SOUND_MIXER_LINE1]	=	{AC97_AUX_VOL,		31},
	[SOUND_MIXER_PHONEIN]	= 	{AC97_PHONE_VOL,	15},
	[SOUND_MIXER_PHONEOUT]	= 	{AC97_MASTER_VOL_MONO,	63},
	[SOUND_MIXER_VIDEO]	=	{AC97_VIDEO_VOL,	31},
};

/* the following tables allow us to go from OSS <-> ac97 quickly. */
enum ac97_recsettings {
	AC97_REC_MIC=0,
	AC97_REC_CD,
	AC97_REC_VIDEO,
	AC97_REC_AUX,
	AC97_REC_LINE,
	AC97_REC_STEREO, /* combination of all enabled outputs..  */
	AC97_REC_MONO,	      /*.. or the mono equivalent */
	AC97_REC_PHONE	      
};

static unsigned int ac97_rm2oss[] = {
	[AC97_REC_MIC] 	 = SOUND_MIXER_MIC, 
	[AC97_REC_CD] 	 = SOUND_MIXER_CD, 
	[AC97_REC_VIDEO] = SOUND_MIXER_VIDEO, 
	[AC97_REC_AUX] 	 = SOUND_MIXER_LINE1, 
	[AC97_REC_LINE]  = SOUND_MIXER_LINE, 
	[AC97_REC_PHONE] = SOUND_MIXER_PHONEIN
};

/* indexed by bit position */
static unsigned int ac97_oss_rm[] = {
	[SOUND_MIXER_MIC] 	= AC97_REC_MIC,
	[SOUND_MIXER_CD] 	= AC97_REC_CD,
	[SOUND_MIXER_VIDEO] 	= AC97_REC_VIDEO,
	[SOUND_MIXER_LINE1] 	= AC97_REC_AUX,
	[SOUND_MIXER_LINE] 	= AC97_REC_LINE,
	[SOUND_MIXER_PHONEIN] 	= AC97_REC_PHONE
};

/* reads the given OSS mixer from the ac97 the caller must have insured that the ac97 knows
   about that given mixer, and should be holding a spinlock for the card */
static int ac97_read_mixer(struct ac97_codec *codec, int oss_channel) 
{
	u16 val;
	int ret = 0;
	struct ac97_mixer_hw *mh = &ac97_hw[oss_channel];

	val = codec->codec_read(codec , mh->offset);

	if (AC97_STEREO_MASK & (1 << oss_channel)) {
		/* nice stereo mixers .. */
		int left,right;

		left = (val >> 8)  & 0x7f;
		right = val  & 0x7f;

		if (oss_channel == SOUND_MIXER_IGAIN) {
			right = (right * 100) / mh->scale;
			left = (left * 100) / mh->scale;
		} else {
			right = 100 - ((right * 100) / mh->scale);
			left = 100 - ((left * 100) / mh->scale);
		}

		ret = left | (right << 8);
	} else if (oss_channel == SOUND_MIXER_SPEAKER) {
		ret = 100 - ((((val & 0x1e)>>1) * 100) / mh->scale);
	} else if (oss_channel == SOUND_MIXER_MIC) {
		ret = 100 - (((val & 0x1f) * 100) / mh->scale);
		/*  the low bit is optional in the tone sliders and masking
		    it lets us avoid the 0xf 'bypass'.. */
	} else if (oss_channel == SOUND_MIXER_BASS) {
		ret = 100 - ((((val >> 8) & 0xe) * 100) / mh->scale);
	} else if (oss_channel == SOUND_MIXER_TREBLE) {
		ret = 100 - (((val & 0xe) * 100) / mh->scale);
	}

#ifdef DEBUG
	printk("ac97_codec: read OSS mixer %2d (ac97 register 0x%02x), "
	       "0x%04x -> 0x%04x\n", oss_channel, mh->offset, val, ret);
#endif

	return ret;
}

/* write the OSS encoded volume to the given OSS encoded mixer, again caller's job to
   make sure all is well in arg land, call with spinlock held */
static void ac97_write_mixer(struct ac97_codec *codec, int oss_channel,
		      unsigned int left, unsigned int right)
{
	u16 val = 0;
	struct ac97_mixer_hw *mh = &ac97_hw[oss_channel];

#ifdef DEBUG
	printk("ac97_codec: wrote OSS mixer %2d (%s ac97 register 0x%02x), "
	       "left vol:%2d, right vol:%2d:",
	       oss_channel, codec->id ? "Secondary" : "Primary",
	       mh->offset, left, right);
#endif

	if (AC97_STEREO_MASK & (1 << oss_channel)) {
		/* stereo mixers */
		if (oss_channel == SOUND_MIXER_IGAIN) {
			right = (right * mh->scale) / 100;
			left = (left * mh->scale) / 100;
		} else {
			right = ((100 - right) * mh->scale) / 100;
			left = ((100 - left) * mh->scale) / 100;
		}
		val = (left << 8) | right;
	} else if (oss_channel == SOUND_MIXER_SPEAKER) {
		val = (((100 - left) * mh->scale) / 100) << 1;
	} else if (oss_channel == SOUND_MIXER_MIC) {
		val = codec->codec_read(codec , mh->offset) & ~0x801f;
		val |= (((100 - left) * mh->scale) / 100);
		/*  the low bit is optional in the tone sliders and masking
		    it lets us avoid the 0xf 'bypass'.. */
	} else if (oss_channel == SOUND_MIXER_BASS) {
		val = codec->codec_read(codec , mh->offset) & ~0x0f00;
		val |= ((((100 - left) * mh->scale) / 100) << 8) & 0x0e00;
	} else if (oss_channel == SOUND_MIXER_TREBLE)	 {
		val = codec->codec_read(codec , mh->offset) & ~0x000f;
		val |= (((100 - left) * mh->scale) / 100) & 0x000e;
	}

#ifdef DEBUG
	printk(" 0x%04x", val);
#endif
	codec->codec_write(codec, mh->offset, val);

#ifdef DEBUG
	val = codec->codec_read(codec, mh->offset);
	printk(" -> 0x%04x\n", val);
#endif
}

/* a thin wrapper for write_mixer */
static void ac97_set_mixer(struct ac97_codec *codec, unsigned int oss_mixer, unsigned int val ) 
{
	unsigned int left,right;

	/* cleanse input a little */
	right = ((val >> 8)  & 0xff) ;
	left = (val  & 0xff) ;

	if (right > 100) right = 100;
	if (left > 100) left = 100;

	codec->mixer_state[oss_mixer] = (right << 8) | left;
	codec->write_mixer(codec, oss_mixer, left, right);
}

/* read or write the recmask, the ac97 can really have left and right recording
   inputs independantly set, but OSS doesn't seem to want us to express that to
   the user. the caller guarantees that we have a supported bit set, and they
   must be holding the card's spinlock */
static int ac97_recmask_io(struct ac97_codec *codec, int rw, int mask) 
{
	unsigned int val;

	if (rw) {
		/* read it from the card */
		val = codec->codec_read(codec, 0x1a) & 0x7;
		return ac97_rm2oss[val];
	}

	/* else, write the first set in the mask as the
	   output */	

	val = ffs(mask); 
	val = ac97_oss_rm[val-1];
	val |= val << 8;  /* set both channels */

#ifdef DEBUG
	printk("ac97_codec: setting ac97 recmask to 0x%x\n", val);
#endif

	codec->codec_write(codec, 0x1a, val);

	return 0;
};

static int ac97_mixer_ioctl(struct ac97_codec *codec, unsigned int cmd, unsigned long arg)
{
	int i, val = 0;

	if (cmd == SOUND_MIXER_INFO) {
		mixer_info info;
		strncpy(info.id, codec->name, sizeof(info.id));
		strncpy(info.name, codec->name, sizeof(info.name));
		info.modify_counter = codec->modcnt;
		if (copy_to_user((void *)arg, &info, sizeof(info)))
			return -EFAULT;
		return 0;
	}
	if (cmd == SOUND_OLD_MIXER_INFO) {
		_old_mixer_info info;
		strncpy(info.id, codec->name, sizeof(info.id));
		strncpy(info.name, codec->name, sizeof(info.name));
		if (copy_to_user((void *)arg, &info, sizeof(info)))
			return -EFAULT;
		return 0;
	}

	if (_IOC_TYPE(cmd) != 'M' || _IOC_SIZE(cmd) != sizeof(int))
		return -EINVAL;

	if (cmd == OSS_GETVERSION)
		return put_user(SOUND_VERSION, (int *)arg);

	if (_IOC_DIR(cmd) == _IOC_READ) {
		switch (_IOC_NR(cmd)) {
		case SOUND_MIXER_RECSRC: /* give them the current record source */
			if (!codec->recmask_io) {
				val = 0;
			} else {
				val = codec->recmask_io(codec, 1, 0);
			}
			break;

		case SOUND_MIXER_DEVMASK: /* give them the supported mixers */
			val = codec->supported_mixers;
			break;

		case SOUND_MIXER_RECMASK: /* Arg contains a bit for each supported recording source */
			val = codec->record_sources;
			break;

		case SOUND_MIXER_STEREODEVS: /* Mixer channels supporting stereo */
			val = codec->stereo_mixers;
			break;

		case SOUND_MIXER_CAPS:
			val = SOUND_CAP_EXCL_INPUT;
			break;

		default: /* read a specific mixer */
			i = _IOC_NR(cmd);

			if (!supported_mixer(codec, i)) 
				return -EINVAL;

			/* do we ever want to touch the hardware? */
			/* val = codec->read_mixer(card,i); */
			val = codec->mixer_state[i];
			break;
		}
		return put_user(val,(int *)arg);
	}

	if (_IOC_DIR(cmd) == (_IOC_WRITE|_IOC_READ)) {
		codec->modcnt++;
		get_user_ret(val, (int *)arg, -EFAULT);

		switch (_IOC_NR(cmd)) {
		case SOUND_MIXER_RECSRC: /* Arg contains a bit for each recording source */
			if (!codec->recmask_io) return -EINVAL;
			if (!(val &= codec->record_sources)) return -EINVAL;

			codec->recmask_io(codec, 0, val);

			return 0;
		default: /* write a specific mixer */
			i = _IOC_NR(cmd);

			if (!supported_mixer(codec, i)) 
				return -EINVAL;

			ac97_set_mixer(codec, i, val);

			return 0;
		}
	}
	return -EINVAL;
}

int ac97_probe_codec(struct ac97_codec *codec)
{
	u16 id1, id2, cap;
	int i;

	/* probing AC97 codec, AC97 2.0 says that bit 15 of register 0x00 (reset) should 
	   be read zero. Probing of AC97 in this way is not reliable, it is not even SAFE !! */
	codec->codec_write(codec, AC97_RESET, 0L);
	if ((cap = codec->codec_read(codec, AC97_RESET)) & 0x8000)
		return 0;
	
	id1 = codec->codec_read(codec, AC97_VENDOR_ID1);
	id2 = codec->codec_read(codec, AC97_VENDOR_ID2);
	for (i = 0; i < sizeof (snd_ac97_codec_ids); i++) {
		if (snd_ac97_codec_ids[i].id == ((id1 << 16) | id2)) {
			codec->name = snd_ac97_codec_ids[i].name;
			codec->codec_init = snd_ac97_codec_ids[i].init;
			break;
		}
	}
	if (codec->name == NULL)
		codec->name = "Unknown";
	printk(KERN_INFO "ac97_codec: ac97 vendor id1: 0x%04x, id2: 0x%04x (%s)\n",
	       id1, id2, codec->name);
	printk(KERN_INFO "ac97_codec: capability: 0x%04x\n", cap);

	/* mixer masks */
	codec->supported_mixers = AC97_SUPPORTED_MASK;
	codec->stereo_mixers = AC97_STEREO_MASK;
	codec->record_sources = AC97_RECORD_MASK;
	if (!(cap & 0x04))
		codec->supported_mixers &= ~(SOUND_MASK_BASS|SOUND_MASK_TREBLE);

	/* generic OSS to AC97 wrapper */
	codec->read_mixer = ac97_read_mixer;
	codec->write_mixer = ac97_write_mixer;
	codec->recmask_io = ac97_recmask_io;
	codec->mixer_ioctl = ac97_mixer_ioctl;

	/* initialize volume level */
	codec->codec_write(codec, AC97_MASTER_VOL_STEREO, 0L);
	codec->codec_write(codec, AC97_PCMOUT_VOL, 0L);

	/* codec specific initialization for 4-6 channel output */
	if (codec->id != 0 && codec->codec_init != NULL) {
		codec->codec_init(codec);
	}

	/* initilize mixer channel volumes */
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++) {
		struct mixer_defaults *md = &mixer_defaults[i];
		if (md->mixer == -1) 
			break;
		if (!supported_mixer(codec, md->mixer)) 
			continue;
		ac97_set_mixer(codec, md->mixer, md->value);
	}

	return 1;
}

static int sigmatel_init(struct ac97_codec * codec)
{
	codec->codec_write(codec, AC97_SURROUND_MASTER, 0L);
	/* initialize SigmaTel STAC9721/23 */
	codec->codec_write(codec, 0x74, 0x01);
	return 1;
}

EXPORT_SYMBOL(ac97_probe_codec);

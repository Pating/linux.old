/*
 *  Routines for control of the CS8427 via i2c bus
 *  IEC958 (S/PDIF) receiver & transmitter by Cirrus Logic
 *  Copyright (c) by Jaroslav Kysela <perex@suse.cz>
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#include <sound/driver.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/control.h>
#include <sound/pcm.h>
#include <sound/cs8427.h>
#include <sound/asoundef.h>

MODULE_AUTHOR("Jaroslav Kysela <perex@suse.cz>");
MODULE_DESCRIPTION("IEC958 (S/PDIF) receiver & transmitter by Cirrus Logic");
MODULE_LICENSE("GPL");

#define chip_t snd_i2c_device_t

#define CS8427_ADDR			(0x20>>1) /* fixed address */

typedef struct {
	snd_pcm_substream_t *substream;
	char hw_status[24];		/* hardware status */
	char def_status[24];		/* default status */
	char pcm_status[24];		/* PCM private status */
	char hw_udata[32];
	snd_kcontrol_t *pcm_ctl;
} cs8427_stream_t;

typedef struct {
	unsigned char regmap[0x14];	/* map of first 1 + 13 registers */
	cs8427_stream_t playback;
	cs8427_stream_t capture;
} cs8427_t;

static unsigned char swapbits(unsigned char val)
{
	int bit;
	unsigned char res = 0;
	for (bit = 0; bit < 8; bit++) {
		res |= val & 1;
		res <<= 1;
		val >>= 1;
	}
	return res;
}

int snd_cs8427_detect(snd_i2c_bus_t *bus, unsigned char addr)
{
	int res;

	snd_i2c_lock(bus);
	res = snd_i2c_probeaddr(bus, CS8427_ADDR | (addr & 7));
	snd_i2c_unlock(bus);
	return res;
}

static int snd_cs8427_reg_write(snd_i2c_device_t *device, unsigned char reg, unsigned char val)
{
	int err;
	unsigned char buf[2];

	buf[0] = reg & 0x7f;
	buf[1] = val;
	if ((err = snd_i2c_sendbytes(device, buf, 2)) != 2) {
		snd_printk("unable to send bytes 0x%02x:0x%02x to CS8427 (%i)\n", buf[0], buf[1], err);
		return err < 0 ? err : -EREMOTE;
	}
	return 0;
}

static int snd_cs8427_reg_read(snd_i2c_device_t *device, unsigned char reg)
{
	int err;
	unsigned char buf;

	if ((err = snd_i2c_sendbytes(device, &reg, 1)) != 1) {
		snd_printk("unable to send register 0x%x byte to CS8427\n", reg);
		return err < 0 ? err : -EREMOTE;
	}
	if ((err = snd_i2c_readbytes(device, &buf, 1)) != 1) {
		snd_printk("unable to read register 0x%x byte from CS8427\n", reg);
		return err < 0 ? err : -EREMOTE;
	}
	return buf;
}

static int snd_cs8427_select_corudata(snd_i2c_device_t *device, int udata)
{
	cs8427_t *chip = snd_magic_cast(cs8427_t, device->private_data, return -ENXIO);
	int err;

	udata = udata ? CS8427_BSEL : 0;
	if (udata != (chip->regmap[CS8427_REG_CSDATABUF] & udata)) {
		chip->regmap[CS8427_REG_CSDATABUF] &= ~CS8427_BSEL;
		chip->regmap[CS8427_REG_CSDATABUF] |= udata;
		err = snd_cs8427_reg_write(device, CS8427_REG_CSDATABUF, chip->regmap[CS8427_REG_CSDATABUF]);
		if (err < 0)
			return err;
	}
	return 0;
}

static int snd_cs8427_send_corudata(snd_i2c_device_t *device,
				    int udata,
				    unsigned char *ndata,
				    int count)
{
	cs8427_t *chip = snd_magic_cast(cs8427_t, device->private_data, return -ENXIO);
	char *hw_data = udata ? chip->playback.hw_udata : chip->playback.hw_status;
	char data[32];
	int err, idx;

	if (!memcmp(hw_data, ndata, count))
		return 0;
	if ((err = snd_cs8427_select_corudata(device, udata)) < 0)
		return err;
	memcpy(hw_data, data, count);
	if (udata) {
		memset(data, 0, sizeof(data));
		if (memcmp(hw_data, data, 32) == 0) {
			chip->regmap[CS8427_REG_UDATABUF] &= ~CS8427_UBMMASK;
			chip->regmap[CS8427_REG_UDATABUF] |= CS8427_UBMZEROS | CS8427_EFTUI;
			if ((err = snd_cs8427_reg_write(device, CS8427_REG_UDATABUF, chip->regmap[CS8427_REG_UDATABUF])) < 0)
				return err;
			return 0;
		}
	}
	data[0] = CS8427_REG_AUTOINC | CS8427_REG_CORU_DATABUF;
	for (idx = 0; idx < count; idx++)
		data[idx + 1] = swapbits(ndata[idx]);
	if (snd_i2c_sendbytes(device, data, count) != count)
		return -EREMOTE;
	return 1;
}

static void snd_cs8427_free(snd_i2c_device_t *device)
{
	if (device->private_data)
		snd_magic_kfree(device->private_data);
}

int snd_cs8427_create(snd_i2c_bus_t *bus,
		      unsigned char addr,
		      snd_i2c_device_t **r_cs8427)
{
	static unsigned char initvals1[] = {
	  CS8427_REG_CONTROL1 | CS8427_REG_AUTOINC,
	  /* CS8427_REG_CLOCKSOURCE: RMCK to OMCK, no validity, disable mutes, TCBL=output */
	  CS8427_SWCLK,
	  /* CS8427_REG_CONTROL2: hold last valid audio sample, RMCK=256*Fs, normal stereo operation */
	  0x00,
	  /* CS8427_REG_DATAFLOW: output drivers normal operation, Tx<=serial, Rx=>serial */
	  CS8427_TXDSERIAL | CS8427_SPDAES3RECEIVER,
	  /* CS8427_REG_CLOCKSOURCE: Run off, CMCK=256*Fs, output time base = OMCK, input time base =
	     covered input clock, recovered input clock source is Envy24 */
	  CS8427_INC,
	  /* CS8427_REG_SERIALINPUT: Serial audio input port data format = I2S, 24-bit, 64*Fsi */
	  CS8427_SIDEL | CS8427_SILRPOL,
	  /* CS8427_REG_SERIALOUTPUT: Serial audio output port data format = I2S, 24-bit, 64*Fsi */
	  CS8427_SODEL | CS8427_SOLRPOL,
	};
	static unsigned char initvals2[] = {
	  CS8427_REG_RECVERRMASK | CS8427_REG_AUTOINC,
	  /* CS8427_REG_RECVERRMASK: unmask the input PLL clock, V, confidence, biphase, parity status bits */
	  CS8427_UNLOCK | CS8427_V | CS8427_CONF | CS8427_BIP | CS8427_PAR,
	  /* CS8427_REG_CSDATABUF:
	     Registers 32-55 window to CS buffer
	     Inhibit D->E transfers from overwriting first 5 bytes of CS data.
	     Inhibit D->E transfers (all) of CS data.
	     Allow E->F transfer of CS data.
	     One byte mode; both A/B channels get same written CB data.
	     A channel info is output to chip's EMPH* pin. */
	  CS8427_CBMR | CS8427_DETCI,
	  /* CS8427_REG_UDATABUF:
	     Use internal buffer to transmit User (U) data.
	     Chip's U pin is an output.
	     Transmit all O's for user data.
	     Inhibit D->E transfers.
	     Inhibit E->F transfers. */
	  CS8427_UD | CS8427_EFTUI | CS8427_DETUI,
	};
	int err;
	cs8427_t *chip;
	snd_i2c_device_t *device;
	unsigned char buf[32 + 1];

	if ((err = snd_i2c_device_create(bus, "CS8427", CS8427_ADDR | (addr & 7), &device)) < 0)
		return err;
	chip = device->private_data = snd_magic_kcalloc(cs8427_t, 0, GFP_KERNEL);
	if (chip == NULL) {
	      	snd_i2c_device_free(device);
		return -ENOMEM;
	}
	device->private_free = snd_cs8427_free;
	
	snd_i2c_lock(bus);
	if ((err = snd_cs8427_reg_read(device, CS8427_REG_ID_AND_VER)) != CS8427_VER8427A) {
		snd_i2c_unlock(bus);
		snd_printk("unable to find CS8427 signature (expected 0x%x, read 0x%x), initialization is not completed\n", CS8427_VER8427A, err);
		return -EFAULT;
	}
	/* turn off run bit while making changes to configuration */
	if ((err = snd_cs8427_reg_write(device, CS8427_REG_CLOCKSOURCE, 0x00)) < 0)
		goto __fail;
	/* send initial values */
	memcpy(chip->regmap + (initvals1[0] & 0x7f), initvals1 + 1, 6);
	if ((err = snd_i2c_sendbytes(device, initvals1, 7)) != 7) {
		err = err < 0 ? err : -EREMOTE;
		goto __fail;
	}
	/* Turn off CS8427 interrupt stuff that is not used in hardware */
	memset(buf, 0, 8);
	/* from address 9 to 16 */
	buf[0] = 9;	/* register */
	if ((err = snd_i2c_sendbytes(device, buf, 8)) != 8)
		goto __fail;
	/* send transfer initialization sequence */
	memcpy(chip->regmap + (initvals2[0] & 0x7f), initvals2 + 1, 3);
	if ((err = snd_i2c_sendbytes(device, initvals2, 4)) != 4) {
		err = err < 0 ? err : -EREMOTE;
		goto __fail;
	}
	/* write default channel status bytes */
	buf[0] = CS8427_REG_AUTOINC | CS8427_REG_CORU_DATABUF;
	buf[1] = swapbits((unsigned char)(SNDRV_PCM_DEFAULT_CON_SPDIF >> 0));
	buf[2] = swapbits((unsigned char)(SNDRV_PCM_DEFAULT_CON_SPDIF >> 8));
	buf[3] = swapbits((unsigned char)(SNDRV_PCM_DEFAULT_CON_SPDIF >> 16));
	buf[4] = swapbits((unsigned char)(SNDRV_PCM_DEFAULT_CON_SPDIF >> 24));
	memset(buf + 5, 0, sizeof(buf)-5);
	memcpy(chip->playback.def_status, buf + 1, 24);
	memcpy(chip->playback.pcm_status, buf + 1, 24);
	if ((err = snd_i2c_sendbytes(device, buf, 33)) != 33)
		goto __fail;
	/* turn on run bit and rock'n'roll */
	chip->regmap[CS8427_REG_CLOCKSOURCE] = initvals1[4] | CS8427_RUN;
	if ((err = snd_cs8427_reg_write(device, CS8427_REG_CLOCKSOURCE, chip->regmap[CS8427_REG_CLOCKSOURCE])) < 0)
		goto __fail;

#if 0	// it's nice for read tests
	{
	char buf[128];
	buf[0] = 0x81;
	snd_i2c_sendbytes(device, buf, 1);
	snd_i2c_readbytes(device, buf, 127);
	}
#endif
	
	snd_i2c_unlock(bus);
	if (r_cs8427)
		*r_cs8427 = device;
	return 0;

      __fail:
      	snd_i2c_unlock(bus);
      	snd_i2c_device_free(device);
      	return err < 0 ? err : -EREMOTE;
}

static int snd_cs8427_in_status_info(snd_kcontrol_t *kcontrol,
				     snd_ctl_elem_info_t *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 255;
	return 0;
}

static int snd_cs8427_in_status_get(snd_kcontrol_t *kcontrol,
				    snd_ctl_elem_value_t *ucontrol)
{
	snd_i2c_device_t *device = snd_kcontrol_chip(kcontrol);
	int data;

	snd_i2c_lock(device->bus);
	data = snd_cs8427_reg_read(device, 15);
	snd_i2c_unlock(device->bus);
	if (data < 0)
		return data;
	ucontrol->value.integer.value[0] = data;
	return 0;
}

static int snd_cs8427_spdif_info(snd_kcontrol_t *kcontrol, snd_ctl_elem_info_t * uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_IEC958;
	uinfo->count = 1;
	return 0;
}

static int snd_cs8427_spdif_get(snd_kcontrol_t * kcontrol,
				snd_ctl_elem_value_t * ucontrol)
{
	snd_i2c_device_t *device = snd_kcontrol_chip(kcontrol);
	cs8427_t *chip = snd_magic_cast(cs8427_t, device->private_data, return -ENXIO);
	
	snd_i2c_lock(device->bus);
	memcpy(ucontrol->value.iec958.status, chip->playback.def_status, 23);
	snd_i2c_unlock(device->bus);
	return 0;
}

static int snd_cs8427_spdif_put(snd_kcontrol_t * kcontrol,
				snd_ctl_elem_value_t * ucontrol)
{
	snd_i2c_device_t *device = snd_kcontrol_chip(kcontrol);
	cs8427_t *chip = snd_magic_cast(cs8427_t, device->private_data, return -ENXIO);
	unsigned char *status = kcontrol->private_value ? chip->playback.pcm_status : chip->playback.def_status;
	int err, change;

	snd_i2c_lock(device->bus);
	change = memcmp(ucontrol->value.iec958.status, status, 23) != 0;
	memcpy(status, ucontrol->value.iec958.status, 23);
	if (change && (kcontrol->private_value ? chip->playback.substream != NULL : chip->playback.substream == NULL)) {
		err = snd_cs8427_send_corudata(device, 0, status, 23);
		if (err < 0)
			change = err;
	}
	snd_i2c_unlock(device->bus);
	return change;
}

static int snd_cs8427_spdif_mask_info(snd_kcontrol_t *kcontrol, snd_ctl_elem_info_t * uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_IEC958;
	uinfo->count = 1;
	return 0;
}

static int snd_cs8427_spdif_mask_get(snd_kcontrol_t * kcontrol,
				      snd_ctl_elem_value_t * ucontrol)
{
	memset(ucontrol->value.iec958.status, 0xff, 23);
	return 0;
}

#define CONTROLS (sizeof(snd_cs8427_iec958_controls)/sizeof(snd_kcontrol_new_t))

static snd_kcontrol_new_t snd_cs8427_iec958_controls[] = {
{
	iface: SNDRV_CTL_ELEM_IFACE_PCM,
	info: snd_cs8427_in_status_info,
	name: "IEC958 CS8427 Input Status",
	access: SNDRV_CTL_ELEM_ACCESS_READ | SNDRV_CTL_ELEM_ACCESS_VOLATILE,
	get: snd_cs8427_in_status_get,
},
{
	access:		SNDRV_CTL_ELEM_ACCESS_READ,
	iface:		SNDRV_CTL_ELEM_IFACE_PCM,
	name:           SNDRV_CTL_NAME_IEC958("",PLAYBACK,MASK),
	info:		snd_cs8427_spdif_mask_info,
	get:		snd_cs8427_spdif_mask_get,
},
{
	iface:		SNDRV_CTL_ELEM_IFACE_PCM,
	name:           SNDRV_CTL_NAME_IEC958("",PLAYBACK,DEFAULT),
	info:		snd_cs8427_spdif_info,
	get:		snd_cs8427_spdif_get,
	put:		snd_cs8427_spdif_put,
	private_value:	0
},
{
	access:		SNDRV_CTL_ELEM_ACCESS_READWRITE | SNDRV_CTL_ELEM_ACCESS_INACTIVE,
	iface:		SNDRV_CTL_ELEM_IFACE_PCM,
	name:           SNDRV_CTL_NAME_IEC958("",PLAYBACK,PCM_STREAM),
	info:		snd_cs8427_spdif_info,
	get:		snd_cs8427_spdif_get,
	put:		snd_cs8427_spdif_put,
	private_value:	1
}};

int snd_cs8427_iec958_build(snd_i2c_device_t *cs8427,
			    snd_pcm_substream_t *play_substream,
			    snd_pcm_substream_t *cap_substream)
{
	cs8427_t *chip = snd_magic_cast(cs8427_t, cs8427->private_data, return -ENXIO);
	snd_kcontrol_t *kctl;
	int idx, err;

	snd_assert(play_substream && cap_substream, return -EINVAL);
	for (idx = 0; idx < CONTROLS; idx++) {
		kctl = snd_ctl_new1(&snd_cs8427_iec958_controls[idx], cs8427);
		if (kctl == NULL)
			return -ENOMEM;
		kctl->id.device = play_substream->pcm->device;
		kctl->id.subdevice = play_substream->number;
		err = snd_ctl_add(cs8427->bus->card, kctl);
		if (err < 0)
			return err;
		if (!strcmp(kctl->id.name, SNDRV_CTL_NAME_IEC958("",PLAYBACK,PCM_STREAM)))
			chip->playback.pcm_ctl = kctl;
	}

	snd_assert(chip->playback.pcm_ctl, return -EIO);
	return 0;
}

int snd_cs8427_iec958_active(snd_i2c_device_t *cs8427, int active)
{
	cs8427_t *chip;

	snd_assert(cs8427, return -ENXIO);
	chip = snd_magic_cast(cs8427_t, cs8427->private_data, return -ENXIO);
	if (active)
		memcpy(chip->playback.pcm_status, chip->playback.def_status, 24);
	chip->playback.pcm_ctl->access &= ~SNDRV_CTL_ELEM_ACCESS_INACTIVE;
	snd_ctl_notify(cs8427->bus->card, SNDRV_CTL_EVENT_MASK_VALUE |
					  SNDRV_CTL_EVENT_MASK_INFO, &chip->playback.pcm_ctl->id);
	return 0;
}

int snd_cs8427_iec958_pcm(snd_i2c_device_t *cs8427, unsigned int rate)
{
	cs8427_t *chip;
	char *status;
	int err;

	snd_assert(cs8427, return -ENXIO);
	chip = snd_magic_cast(cs8427_t, cs8427->private_data, return -ENXIO);
	status = chip->playback.pcm_status;
	snd_i2c_lock(cs8427->bus);
	if (status[0] & IEC958_AES0_PROFESSIONAL) {
		status[0] &= ~IEC958_AES0_PRO_FS;
		switch (rate) {
		case 32000: status[0] |= IEC958_AES0_PRO_FS_32000; break;
		case 44100: status[0] |= IEC958_AES0_PRO_FS_44100; break;
		case 48000: status[0] |= IEC958_AES0_PRO_FS_48000; break;
		default: status[0] |= IEC958_AES0_PRO_FS_NOTID; break;
		}
	} else {
		status[3] &= ~IEC958_AES3_CON_FS;
		switch (rate) {
		case 32000: status[3] |= IEC958_AES3_CON_FS_32000; break;
		case 44100: status[3] |= IEC958_AES3_CON_FS_44100; break;
		case 48000: status[3] |= IEC958_AES3_CON_FS_48000; break;
		}
	}
	err = snd_cs8427_send_corudata(cs8427, 0, status, 23);
	if (err > 0)
		snd_ctl_notify(cs8427->bus->card,
			       SNDRV_CTL_EVENT_MASK_VALUE,
			       &chip->playback.pcm_ctl->id);
	snd_i2c_unlock(cs8427->bus);
	return err < 0 ? err : 0;
}

EXPORT_SYMBOL(snd_cs8427_detect);
EXPORT_SYMBOL(snd_cs8427_create);
EXPORT_SYMBOL(snd_cs8427_iec958_build);
EXPORT_SYMBOL(snd_cs8427_iec958_active);
EXPORT_SYMBOL(snd_cs8427_iec958_pcm);
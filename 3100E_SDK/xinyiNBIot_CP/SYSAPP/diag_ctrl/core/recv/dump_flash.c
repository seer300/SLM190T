/*
 * dump_flash.c
 *
 *  Created on: 2022年5月18日
 *      Author: jiangzj
 */

#include "xy_memmap.h"
#include "dump_flash.h"
#include "flash_adapt.h"
#include "qspi_flash.h"
#include "xy_flash.h"
#include "mpu_protect.h"
#include "ipc_msg.h"
#include "hw_memmap.h"

extern void flash_vendor_delay(unsigned long uldelay);
#define UPROUND(raw, base)    ((((raw)+(base)-1)/(base))*(base))
#define MAGIC_NUM         0x12345678
#define IMAGE_NUM         ( 12 )
#define MAX_FILE_NAME     ( 32 )
#define BB_REG_LEN     	  ( 0x300 )     //BB_REG_BASE:0x4001b000

// External Flash First 256 bytes
typedef struct {
	uint32_t flash_addr;
	uint32_t ram_addr;
	uint32_t len;
	char file_name[MAX_FILE_NAME];
}image_info_t;

typedef struct {
	// Header Info
	uint32_t magic_num;
	uint32_t image_num;
	uint32_t assert_core;
	// Image Info
	image_info_t image_info[IMAGE_NUM];
} Flash_Header;

typedef struct {
	unsigned int image_id[3];
	unsigned int load_addr;
	unsigned int exec_addr;
	unsigned int len_byte;
	unsigned int image_content_crc;
	unsigned int image_info_crc;
} Flash_Image_Def;
// External Flash First 256 bytes
typedef struct {
	// Header Info
	unsigned int image_num;
	unsigned int image_info_addr_base;
	unsigned int user_code;
	unsigned int nv_addr_base;
	unsigned int nv_len_byte;
	unsigned int exchange_addr_base;
	unsigned int exchange_len_byte;
	unsigned int header_info_crc;

	// Image Info
    Flash_Image_Def image_info[7];
} Flash_Header_Def;

#if 0
void Wait_Flash_stable(void)
{
	while(DMAChannelTransferRemainCNT(FLASH_DMA_CHANNEL));
	DMAIntClear(FLASH_DMA_CHANNEL);

    qspi_rbuf_wait_idle();
    qspi_wbuf_wait_idle();
	qspi_wait_idle();

//	FLASH_ExitXIPMode(&xinyi_flash);
//	FLASH_WaitIdle();

	if(g_flash_status.isDone == flash_PES_busy)
	{
		FLASH_PE_Resume();
		flash_vendor_delay(20);

		while((FLASH_GetStatusReg2() & 0x80) == 0x80);  //wait for SUS==0

		flash_vendor_delay(300);

		FLASH_WaitIdle();
	}
}

void write_flash_for_dump(uint32_t addr, uint8_t *data,uint32_t size)
{
	if((HWREGB(AONPRCM_BASE + 0x01) & 0x01) ) //CP Remap
	{
		if(addr >= FLASH_CP_REMAP_BASE && addr < OTA_FLASH_BASE())
		{
			while(1);
		}
	}
	else
	{
		if(addr >= FLASH_CP_BASE && addr < OTA_FLASH_BASE())
		{
			while(1);
		}
	}

	flash_erase(addr, size);
	flash_write_no_erase(addr, data, size);
}

uint32_t dump_get_partition_info(uint32_t *addr, uint32_t *len)
{
	//Flash_Header_Def  flash_header;

	*addr = OTA_FLASH_BASE();

	*len = DUMP_INFO_BASE_ADDR - *addr + DUMP_INFO_BASE_LEN;
	*addr = DUMP_INFO_BASE_ADDR;

	return *addr;

}
#endif

__FLASH_FUNC void dump_flash_to_file(void)
{
	Flash_Header dump_flash;
	image_info_t* image_info;

	// 等待正常流程的log数据发送完成
	osCoreEnterCritical();
	if(DMAChannelTransferRemainCNT(DMA_CHANNEL_1) != 0)
	{
		DMAChannelWaitIdle(DMA_CHANNEL_1);
	}
	osCoreExitCritical();

	flash_read(DUMP_INFO_BASE_ADDR, (uint8_t *)&dump_flash, sizeof(dump_flash));

	extern void diag_dma_to_io(void);
	diag_dma_to_io();
	
	if(dump_flash.magic_num == MAGIC_NUM)
	{
		for(unsigned int i = 0; i < dump_flash.image_num; i++)
		{
			image_info = &dump_flash.image_info[i];
			diag_dump_mem_info(image_info->file_name, image_info->flash_addr, image_info->len);
		}
	}
	
	diag_port_wait_send_done();
		
	extern void diag_io_to_dma(void);
	diag_io_to_dma();
}

#if 0
void dump_to_flash(void)
{
	uint32_t addr, base_addr, len;
	Flash_Header dump_flash = {.magic_num = MAGIC_NUM, .image_num = 0, .assert_core = g_if_cp_dump};
	image_info_t* image_info;

	if(g_if_cp_dump == 1)
	{
//OPENCPU模式，CP死机，不通知AP
		Dump_Notice();
	}

	if(g_factory_nv->softap_fac_nv.dump_mem_into_flash == 1)
	{
		//如果模组模式，AP或者CP死机，都是CP保存dump信息到flash，cp死机，通知AP卡死；ap死机，通知CP进行dump流程；
		if(Is_OpenCpu_Ver())
		{
			//opencpu形式，release模式下，CP死机，则CP不执行dump_to_flash流程,等到AP发现并执行，AP发现CP异常，AP自己保存dump信息到flash中；AP死机，与模组一致
			if(g_factory_nv->softap_fac_nv.off_debug == 1 && g_if_cp_dump)
				return;
		}
	
		Wait_Flash_stable();

		base_addr = dump_get_partition_info(&addr, &len);

		if(len > (SRAM_LENGTH + AP_SRAM_LENGTH + SHARE_MEM_LENGTH + BAK_MEM_LENGTH + UPROUND(BB_REG_LEN, FLASH_SECTOR_LENGTH) + DUMP_INFO_BASE_LEN))
		{
			image_info = &dump_flash.image_info[dump_flash.image_num++];
			image_info->len = TCM_AP_SIZE;
			image_info->flash_addr = (addr -= image_info->len);
			image_info->ram_addr = RAM0_DMA_BASE; //cp看ap的地址
			if(g_if_cp_dump)
			{
				strcpy(image_info->file_name, "ap_ram_01000000");
			}
			else
			{
				strcpy(image_info->file_name, "ap_assert_ram_01000000");
			}

			image_info = &dump_flash.image_info[dump_flash.image_num++];
			image_info->len = SRAM_LENGTH;
			image_info->flash_addr = (addr -= image_info->len);
			if((HWREGB(AONPRCM_BASE + 0x01) & 0x01) ) //CP Remap
			{
				image_info->ram_addr = TCM_CP_REMAP_BASE;
			}
			else
			{
				image_info->ram_addr = TCM_CP_BASE;
			}
			if(g_if_cp_dump)
			{
				if(HWREG(AONPRCM_BASE + 0x01) & 0x01)
				{
					strcpy(image_info->file_name, "cp_assert_ram_10000000");
				}
				else
				{
					strcpy(image_info->file_name, "cp_assert_ram_00000000");
				}
			}
			else
			{
				if(HWREG(AONPRCM_BASE + 0x01) & 0x01)
				{
					strcpy(image_info->file_name, "cp_ram_10000000");
				}
				else
				{
					strcpy(image_info->file_name, "cp_ram_00000000");
				}
			}

			image_info = &dump_flash.image_info[dump_flash.image_num++];
			image_info->len = BAK_MEM_LENGTH + SHARE_MEM_LENGTH;
			image_info->flash_addr = (addr -= image_info->len);
			image_info->ram_addr = SHARE_MEM_BASE;
			strcpy(image_info->file_name, "shared_ram_60000000");

			image_info = &dump_flash.image_info[dump_flash.image_num++];
			image_info->len = BB_REG_LEN;
			image_info->flash_addr = (addr -= UPROUND(BB_REG_LEN, FLASH_SECTOR_LENGTH));
			image_info->ram_addr = BB_REG_BASE;
			strcpy(image_info->file_name, "bb_reg_0x4001b000");
		}

		for(uint32_t i = 0; i < dump_flash.image_num; i++)
		{
			image_info = &dump_flash.image_info[i];
			write_flash_for_dump(image_info->flash_addr, (uint8_t *)image_info->ram_addr, image_info->len);
		}

		write_flash_for_dump(base_addr, (uint8_t *)&dump_flash, sizeof(dump_flash));

	}
}
#endif

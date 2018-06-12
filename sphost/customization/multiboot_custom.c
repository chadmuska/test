#if MULTI_BOOT_STAGE

#include "common.h"
#include "api/multiboot_api.h"

/**
 * \brief custom callbacks of multiboot at different stages
 * \param stage, only the specific stages defined by host project makefile
 * have callbacks
 * \note for fine executable clean linkage, it is strongly advised to
 * call external atomic items defined in different objects (in custom archives)
 * for different stages and keep this file minimum
 */

void
multibootCustomCb(
		UINT32 stage
		)
{
	switch (stage) {

#  if USE_MULTI_BOOT_STAGE_PWR_GPIO
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_PWR_GPIO
	case MULTI_BOOT_STAGE_PWR_GPIO:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_PWR_GPIO*/
#  endif /*USE_MULTI_BOOT_STAGE_PWR_GPIO*/


#  if USE_MULTI_BOOT_STAGE_VFS
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_VFS
	case MULTI_BOOT_STAGE_VFS:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_VFS*/
#  endif /*USE_MULTI_BOOT_STAGE_VFS*/


#  if USE_MULTI_BOOT_STAGE_DEV_START
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_START
	case MULTI_BOOT_STAGE_DEV_START:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_START*/
#  endif /*USE_MULTI_BOOT_STAGE_DEV_START*/


#  if USE_MULTI_BOOT_STAGE_DEV_RTC
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_RTC
	case MULTI_BOOT_STAGE_DEV_RTC:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_RTC*/
#  endif /*USE_MULTI_BOOT_STAGE_DEV_RTC*/


#  if USE_MULTI_BOOT_STAGE_DEV_TEST
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_TEST
	case MULTI_BOOT_STAGE_DEV_TEST:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_TEST*/
#  endif /*USE_MULTI_BOOT_STAGE_DEV_TEST*/


#  if USE_MULTI_BOOT_STAGE_DISK_RSV
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISK_RSV
	case MULTI_BOOT_STAGE_DISK_RSV:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISK_RSV*/
#  endif /*USE_MULTI_BOOT_STAGE_DISK_RSV*/


#  if USE_MULTI_BOOT_STAGE_DISP
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISP
	case MULTI_BOOT_STAGE_DISP:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISP*/
#  endif /*USE_MULTI_BOOT_STAGE_DISP*/


#  if USE_MULTI_BOOT_STAGE_SENSOR
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_SENSOR
	case MULTI_BOOT_STAGE_SENSOR:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_SENSOR*/
#  endif /*USE_MULTI_BOOT_STAGE_SENSOR*/


#  if USE_MULTI_BOOT_STAGE_DEV_SECOND
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_SECOND
	case MULTI_BOOT_STAGE_DEV_SECOND:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_SECOND*/
#  endif /*USE_MULTI_BOOT_STAGE_DEV_SECOND*/


#  if USE_MULTI_BOOT_STAGE_DISK_MEDIA
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISK_MEDIA
	case MULTI_BOOT_STAGE_DISK_MEDIA:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISK_MEDIA*/
#  endif /*USE_MULTI_BOOT_STAGE_DISK_MEDIA*/


#  if USE_MULTI_BOOT_STAGE_DEV_AUDIO
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_AUDIO
	case MULTI_BOOT_STAGE_DEV_AUDIO:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_AUDIO*/
#  endif /*USE_MULTI_BOOT_STAGE_DEV_AUDIO*/


#  if USE_MULTI_BOOT_STAGE_DEV_USB
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_USB
	case MULTI_BOOT_STAGE_DEV_USB:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_USB*/
#  endif /*USE_MULTI_BOOT_STAGE_DEV_USB*/


#  if USE_MULTI_BOOT_STAGE_MEDIA
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_MEDIA
	case MULTI_BOOT_STAGE_MEDIA:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_MEDIA*/
#  endif /*USE_MULTI_BOOT_STAGE_MEDIA*/


#  if USE_MULTI_BOOT_STAGE_MODESW
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_MODESW
	case MULTI_BOOT_STAGE_MODESW:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_MODESW*/
#  endif /*USE_MULTI_BOOT_STAGE_MODESW*/


#  if USE_MULTI_BOOT_STAGE_SYS_RES
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_SYS_RES
	case MULTI_BOOT_STAGE_SYS_RES:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_SYS_RES*/
#  endif /*USE_MULTI_BOOT_STAGE_SYS_RES*/


#  if USE_MULTI_BOOT_STAGE_APP_START
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_APP_START
	case MULTI_BOOT_STAGE_APP_START:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_APP_START*/
#  endif /*USE_MULTI_BOOT_STAGE_APP_START*/


#  if USE_MULTI_BOOT_STAGE_CMD
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_CMD
	case MULTI_BOOT_STAGE_CMD:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_CMD*/
#  endif /*USE_MULTI_BOOT_STAGE_CMD*/


#  if USE_MULTI_BOOT_STAGE_HOST
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_HOST
	case MULTI_BOOT_STAGE_HOST:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_HOST*/
#  endif /*USE_MULTI_BOOT_STAGE_HOST*/


#  if USE_MULTI_BOOT_STAGE_APP_DELAY
#    if MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_APP_DELAY
	case MULTI_BOOT_STAGE_APP_DELAY:
		/* multiboot custom callback, advised to call external
		 * items defined in separated objects (in archives) */
		break;
#    endif /*MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_APP_DELAY*/
#  endif /*USE_MULTI_BOOT_STAGE_APP_DELAY*/

	}
}

#endif /*MULTI_BOOT_STAGE*/

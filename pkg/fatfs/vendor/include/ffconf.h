/*---------------------------------------------------------------------------/
/  FatFs - Configuration file
/---------------------------------------------------------------------------*/

#define FFCONF_DEF	86606	/* Revision ID */

/*---------------------------------------------------------------------------/
/ Function Configurations
/---------------------------------------------------------------------------*/

#ifndef FATFS_FFCONF_OPT_FS_READONLY
#define FF_FS_READONLY 0
#else
#define FF_FS_READONLY FATFS_FFCONF_OPT_FS_READONLY
#endif/* This option switches read-only configuration. (0:Read/Write or 1:Read-only)
/  Read-only configuration removes writing API functions, f_write(), f_sync(),
/  f_unlink(), f_mkdir(), f_chmod(), f_rename(), f_truncate(), f_getfree()
/  and optional writing functions as well. */


#ifndef FATFS_FFCONF_OPT_FS_MINIMIZE
#define FF_FS_MINIMIZE 0
#else
#define FF_FS_MINIMIZE FATFS_FFCONF_OPT_FS_MINIMIZE
#endif/* This option defines minimization level to remove some basic API functions.
/
/   0: Basic functions are fully enabled.
/   1: f_stat(), f_getfree(), f_unlink(), f_mkdir(), f_truncate() and f_rename()
/      are removed.
/   2: f_opendir(), f_readdir() and f_closedir() are removed in addition to 1.
/   3: f_lseek() function is removed in addition to 2. */


#ifndef FATFS_FFCONF_OPT_USE_STRFUNC
#define FF_USE_STRFUNC 0
#else
#define FF_USE_STRFUNC FATFS_FFCONF_OPT_USE_STRFUNC
#endif/* This option switches string functions, f_gets(), f_putc(), f_puts() and f_printf().
/
/  0: Disable string functions.
/  1: Enable without LF-CRLF conversion.
/  2: Enable with LF-CRLF conversion. */


#ifndef FATFS_FFCONF_OPT_USE_FIND
#define FF_USE_FIND	 0
#else
#define FF_USE_FIND	 FATFS_FFCONF_OPT_USE_FIND
#endif/* This option switches filtered directory read functions, f_findfirst() and
/  f_findnext(). (0:Disable, 1:Enable 2:Enable with matching altname[] too) */


#ifndef FATFS_FFCONF_OPT_USE_MKFS
#define FF_USE_MKFS	 1
#else
#define FF_USE_MKFS	 FATFS_FFCONF_OPT_USE_MKFS
#endif/* This option switches f_mkfs() function. (0:Disable or 1:Enable) */


#ifndef FATFS_FFCONF_OPT_USE_FASTSEEK
#define FF_USE_FASTSEEK 0
#else
#define FF_USE_FASTSEEK FATFS_FFCONF_OPT_USE_FASTSEEK
#endif/* This option switches fast seek function. (0:Disable or 1:Enable) */


#ifndef FATFS_FFCONF_OPT_USE_EXPAND
#define FF_USE_EXPAND 0
#else
#define FF_USE_EXPAND FATFS_FFCONF_OPT_USE_EXPAND
#endif/* This option switches f_expand function. (0:Disable or 1:Enable) */


#ifndef FATFS_FFCONF_OPT_USE_CHMOD
#define FF_USE_CHMOD 1
#else
#define FF_USE_CHMOD FATFS_FFCONF_OPT_USE_CHMOD
#endif/* This option switches attribute manipulation functions, f_chmod() and f_utime().
/  (0:Disable or 1:Enable) Also FF_FS_READONLY needs to be 0 to enable this option. */


#ifndef FATFS_FFCONF_OPT_USE_LABEL
#define FF_USE_LABEL 1
#else
#define FF_USE_LABEL FATFS_FFCONF_OPT_USE_LABEL
#endif/* This option switches volume label functions, f_getlabel() and f_setlabel().
/  (0:Disable or 1:Enable) */


#ifndef FATFS_FFCONF_OPT_USE_FORWARD
#define FF_USE_FORWARD 0
#else
#define FF_USE_FORWARD FATFS_FFCONF_OPT_USE_FORWARD
#endif/* This option switches f_forward() function. (0:Disable or 1:Enable) */


/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/---------------------------------------------------------------------------*/

#ifndef FATFS_FFCONF_OPT_CODE_PAGE
#define FF_CODE_PAGE 437
#else
#define FF_CODE_PAGE FATFS_FFCONF_OPT_CODE_PAGE
#endif/* This option specifies the OEM code page to be used on the target system.
/  Incorrect code page setting can cause a file open failure.
/
/   437 - U.S.
/   720 - Arabic
/   737 - Greek
/   771 - KBL
/   775 - Baltic
/   850 - Latin 1
/   852 - Latin 2
/   855 - Cyrillic
/   857 - Turkish
/   860 - Portuguese
/   861 - Icelandic
/   862 - Hebrew
/   863 - Canadian French
/   864 - Arabic
/   865 - Nordic
/   866 - Russian
/   869 - Greek 2
/   932 - Japanese (DBCS)
/   936 - Simplified Chinese (DBCS)
/   949 - Korean (DBCS)
/   950 - Traditional Chinese (DBCS)
/     0 - Include all code pages above and configured by f_setcp()
*/


#ifndef FATFS_FFCONF_OPT_USE_LFN
#define FF_USE_LFN	 0
#else
#define FF_USE_LFN	 FATFS_FFCONF_OPT_USE_LFN
#endif
#ifndef FATFS_FFCONF_OPT_MAX_LFN
#define FF_MAX_LFN	 255
#else
#define FF_MAX_LFN	 FATFS_FFCONF_OPT_MAX_LFN
#endif/* The FF_USE_LFN switches the support for LFN (long file name).
/
/   0: Disable LFN. FF_MAX_LFN has no effect.
/   1: Enable LFN with static working buffer on the BSS. Always NOT thread-safe.
/   2: Enable LFN with dynamic working buffer on the STACK.
/   3: Enable LFN with dynamic working buffer on the HEAP.
/
/  To enable the LFN, ffunicode.c needs to be added to the project. The LFN function
/  requiers certain internal working buffer occupies (FF_MAX_LFN + 1) * 2 bytes and
/  additional (FF_MAX_LFN + 44) / 15 * 32 bytes when exFAT is enabled.
/  The FF_MAX_LFN defines size of the working buffer in UTF-16 code unit and it can
/  be in range of 12 to 255. It is recommended to be set 255 to fully support LFN
/  specification.
/  When use stack for the working buffer, take care on stack overflow. When use heap
/  memory for the working buffer, memory management functions, ff_memalloc() and
/  ff_memfree() in ffsystem.c, need to be added to the project. */


#ifndef FATFS_FFCONF_OPT_LFN_UNICODE
#define FF_LFN_UNICODE 0
#else
#define FF_LFN_UNICODE FATFS_FFCONF_OPT_LFN_UNICODE
#endif/* This option switches the character encoding on the API when LFN is enabled.
/
/   0: ANSI/OEM in current CP (TCHAR = char)
/   1: Unicode in UTF-16 (TCHAR = WCHAR)
/   2: Unicode in UTF-8 (TCHAR = char)
/   3: Unicode in UTF-32 (TCHAR = DWORD)
/
/  Also behavior of string I/O functions will be affected by this option.
/  When LFN is not enabled, this option has no effect. */


#ifndef FATFS_FFCONF_OPT_LFN_BUF
#define FF_LFN_BUF	 255
#else
#define FF_LFN_BUF	 FATFS_FFCONF_OPT_LFN_BUF
#endif
#ifndef FATFS_FFCONF_OPT_SFN_BUF
#define FF_SFN_BUF	 12
#else
#define FF_SFN_BUF	 FATFS_FFCONF_OPT_SFN_BUF
#endif/* This set of options defines size of file name members in the FILINFO structure
/  which is used to read out directory items. These values should be suffcient for
/  the file names to read. The maximum possible length of the read file name depends
/  on character encoding. When LFN is not enabled, these options have no effect. */


#ifndef FATFS_FFCONF_OPT_STRF_ENCODE
#define FF_STRF_ENCODE 3
#else
#define FF_STRF_ENCODE FATFS_FFCONF_OPT_STRF_ENCODE
#endif/* When FF_LFN_UNICODE >= 1 with LFN enabled, string I/O functions, f_gets(),
/  f_putc(), f_puts and f_printf() convert the character encoding in it.
/  This option selects assumption of character encoding ON THE FILE to be
/  read/written via those functions.
/
/   0: ANSI/OEM in current CP
/   1: Unicode in UTF-16LE
/   2: Unicode in UTF-16BE
/   3: Unicode in UTF-8
*/


#ifndef FATFS_FFCONF_OPT_FS_RPATH
#define FF_FS_RPATH	 0
#else
#define FF_FS_RPATH	 FATFS_FFCONF_OPT_FS_RPATH
#endif/* This option configures support for relative path.
/
/   0: Disable relative path and remove related functions.
/   1: Enable relative path. f_chdir() and f_chdrive() are available.
/   2: f_getcwd() function is available in addition to 1.
*/


/*---------------------------------------------------------------------------/
/ Drive/Volume Configurations
/---------------------------------------------------------------------------*/

#ifndef FATFS_FFCONF_OPT_VOLUMES
#define FF_VOLUMES	 1
#else
#define FF_VOLUMES	 FATFS_FFCONF_OPT_VOLUMES
#endif/* Number of volumes (logical drives) to be used. (1-10) */


#ifndef FATFS_FFCONF_OPT_STR_VOLUME_ID
#define FF_STR_VOLUME_ID 0
#else
#define FF_STR_VOLUME_ID FATFS_FFCONF_OPT_STR_VOLUME_ID
#endif
#ifndef FATFS_FFCONF_OPT_VOLUME_STRS
#define FF_VOLUME_STRS	 "RAM","NAND","CF","SD","SD2","USB","USB2","USB3"
#else
#define FF_VOLUME_STRS	 FATFS_FFCONF_OPT_VOLUME_STRS
#endif/* FF_STR_VOLUME_ID switches string support for volume ID.
/  When FF_STR_VOLUME_ID is set to 1, also pre-defined strings can be used as drive
/  number in the path name. FF_VOLUME_STRS defines the drive ID strings for each
/  logical drives. Number of items must be equal to FF_VOLUMES. Valid characters for
/  the drive ID strings are: A-Z and 0-9. */


#ifndef FATFS_FFCONF_OPT_MULTI_PARTITION
#define FF_MULTI_PARTITION 0
#else
#define FF_MULTI_PARTITION FATFS_FFCONF_OPT_MULTI_PARTITION
#endif/* This option switches support for multiple volumes on the physical drive.
/  By default (0), each logical drive number is bound to the same physical drive
/  number and only an FAT volume found on the physical drive will be mounted.
/  When this function is enabled (1), each logical drive number can be bound to
/  arbitrary physical drive and partition listed in the VolToPart[]. Also f_fdisk()
/  funciton will be available. */


#ifndef FATFS_FFCONF_OPT_MIN_SS
#define FF_MIN_SS	 512
#else
#define FF_MIN_SS	 FATFS_FFCONF_OPT_MIN_SS
#endif
#ifndef FATFS_FFCONF_OPT_MAX_SS
#define FF_MAX_SS	 512
#else
#define FF_MAX_SS	 FATFS_FFCONF_OPT_MAX_SS
#endif/* This set of options configures the range of sector size to be supported. (512,
/  1024, 2048 or 4096) Always set both 512 for most systems, generic memory card and
/  harddisk. But a larger value may be required for on-board flash memory and some
/  type of optical media. When FF_MAX_SS is larger than FF_MIN_SS, FatFs is configured
/  for variable sector size mode and disk_ioctl() function needs to implement
/  GET_SECTOR_SIZE command. */


#define FF_LBA64		0
/* This option switches support for 64-bit LBA. (0:Disable or 1:Enable)
/  To enable the 64-bit LBA, also exFAT needs to be enabled. (FF_FS_EXFAT == 1) */


#define FF_MIN_GPT		0x100000000
/* Minimum number of sectors to switch GPT format to create partition in f_mkfs and
/  f_fdisk function. 0x100000000 max. This option has no effect when FF_LBA64 == 0. */


#ifndef FATFS_FFCONF_OPT_USE_TRIM
#define FF_USE_TRIM	 0
#else
#define FF_USE_TRIM	 FATFS_FFCONF_OPT_USE_TRIM
#endif/* This option switches support for ATA-TRIM. (0:Disable or 1:Enable)
/  To enable Trim function, also CTRL_TRIM command should be implemented to the
/  disk_ioctl() function. */


#ifndef FATFS_FFCONF_OPT_FS_NOFSINFO
#define FF_FS_NOFSINFO 0
#else
#define FF_FS_NOFSINFO FATFS_FFCONF_OPT_FS_NOFSINFO
#endif/* If you need to know correct free space on the FAT32 volume, set bit 0 of this
/  option, and f_getfree() function at first time after volume mount will force
/  a full FAT scan. Bit 1 controls the use of last allocated cluster number.
/
/  bit0=0: Use free cluster count in the FSINFO if available.
/  bit0=1: Do not trust free cluster count in the FSINFO.
/  bit1=0: Use last allocated cluster number in the FSINFO if available.
/  bit1=1: Do not trust last allocated cluster number in the FSINFO.
*/



/*---------------------------------------------------------------------------/
/ System Configurations
/---------------------------------------------------------------------------*/

#ifndef FATFS_FFCONF_OPT_FS_TINY
#define FF_FS_TINY	 1
#else
#define FF_FS_TINY	 FATFS_FFCONF_OPT_FS_TINY
#endif/* This option switches tiny buffer configuration. (0:Normal or 1:Tiny)
/  At the tiny configuration, size of file object (FIL) is shrinked FF_MAX_SS bytes.
/  Instead of private sector buffer eliminated from the file object, common sector
/  buffer in the filesystem object (FATFS) is used for the file data transfer. */


#ifndef FATFS_FFCONF_OPT_FS_EXFAT
#define FF_FS_EXFAT	 0
#else
#define FF_FS_EXFAT	 FATFS_FFCONF_OPT_FS_EXFAT
#endif/* This option switches support for exFAT filesystem. (0:Disable or 1:Enable)
/  When enable exFAT, also LFN needs to be enabled.
/  Note that enabling exFAT discards ANSI C (C89) compatibility. */


#ifndef FATFS_FFCONF_OPT_FS_NORTC
#define FF_FS_NORTC	 0
#else
#define FF_FS_NORTC	 FATFS_FFCONF_OPT_FS_NORTC
#endif
#ifndef FATFS_FFCONF_OPT_NORTC_MON
#define FF_NORTC_MON 1
#else
#define FF_NORTC_MON FATFS_FFCONF_OPT_NORTC_MON
#endif
#ifndef FATFS_FFCONF_OPT_NORTC_MDAY
#define FF_NORTC_MDAY 1
#else
#define FF_NORTC_MDAY FATFS_FFCONF_OPT_NORTC_MDAY
#endif
#ifndef FATFS_FFCONF_OPT_NORTC_YEAR
#define FF_NORTC_YEAR 2020
#else
#define FF_NORTC_YEAR FATFS_FFCONF_OPT_NORTC_YEAR
#endif/* The option FF_FS_NORTC switches timestamp functiton. If the system does not have
/  any RTC function or valid timestamp is not needed, set FF_FS_NORTC = 1 to disable
/  the timestamp function. All objects modified by FatFs will have a fixed timestamp
/  defined by FF_NORTC_MON, FF_NORTC_MDAY and FF_NORTC_YEAR in local time.
/  To enable timestamp function (FF_FS_NORTC = 0), get_fattime() function need to be
/  added to the project to read current time form real-time clock. FF_NORTC_MON,
/  FF_NORTC_MDAY and FF_NORTC_YEAR have no effect.
/  These options have no effect at read-only configuration (FF_FS_READONLY = 1). */


#ifndef FATFS_FFCONF_OPT_FS_LOCK
#define FF_FS_LOCK	 0
#else
#define FF_FS_LOCK	 FATFS_FFCONF_OPT_FS_LOCK
#endif/* The option FF_FS_LOCK switches file lock function to control duplicated file open
/  and illegal operation to open objects. This option must be 0 when FF_FS_READONLY
/  is 1.
/
/  0:  Disable file lock function. To avoid volume corruption, application program
/      should avoid illegal open, remove and rename to the open objects.
/  >0: Enable file lock function. The value defines how many files/sub-directories
/      can be opened simultaneously under file lock control. Note that the file
/      lock control is independent of re-entrancy. */


#ifndef FATFS_FFCONF_OPT_FS_REENTRANT
#define FF_FS_REENTRANT 0
#else
#define FF_FS_REENTRANT FATFS_FFCONF_OPT_FS_REENTRANT
#endif
#ifndef FATFS_FFCONF_OPT_FS_TIMEOUT
#define FF_FS_TIMEOUT 1000
#else
#define FF_FS_TIMEOUT FATFS_FFCONF_OPT_FS_TIMEOUT
#endif
#ifndef FATFS_FFCONF_OPT_SYNC_t
#define FF_SYNC_t	 HANDLE
#else
#define FF_SYNC_t	 FATFS_FFCONF_OPT_SYNC_t
#endif/* The option FF_FS_REENTRANT switches the re-entrancy (thread safe) of the FatFs
/  module itself. Note that regardless of this option, file access to different
/  volume is always re-entrant and volume control functions, f_mount(), f_mkfs()
/  and f_fdisk() function, are always not re-entrant. Only file/directory access
/  to the same volume is under control of this function.
/
/   0: Disable re-entrancy. FF_FS_TIMEOUT and FF_SYNC_t have no effect.
/   1: Enable re-entrancy. Also user provided synchronization handlers,
/      ff_req_grant(), ff_rel_grant(), ff_del_syncobj() and ff_cre_syncobj()
/      function, must be added to the project. Samples are available in
/      option/syscall.c.
/
/  The FF_FS_TIMEOUT defines timeout period in unit of time tick.
/  The FF_SYNC_t defines O/S dependent sync object type. e.g. HANDLE, ID, OS_EVENT*,
/  SemaphoreHandle_t and etc. A header file for O/S definitions needs to be
/  included somewhere in the scope of ff.h. */



/*--- End of configuration options ---*/

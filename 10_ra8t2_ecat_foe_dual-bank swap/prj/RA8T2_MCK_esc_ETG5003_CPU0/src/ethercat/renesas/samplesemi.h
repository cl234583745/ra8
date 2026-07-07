/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
*/

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecat_def.h"
#include "hal_data.h"

/*--------------------------------------------------------------------------------------
------
------    local types and defines
------
--------------------------------------------------------------------------------------*/
#ifndef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
#define APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/*-----------------------------------------------------------------------------------------
------    Backup Parameter (Store Parameter)
-----------------------------------------------------------------------------------------*/
#if BACKUP_PARAMETER_SUPPORTED

/* Define BACKUP_MEMORY_START_ADDRESS */
#if defined(BOARD_RA8T2_MCK) | defined(BOARD_RA8T2_EK)
 #define BACKUP_MEMORY_START_ADDRESS        0x02004000
#endif
#define BACKUP_MEMORY_SIZE_BYTE 	       	4096
#define BACKUP_MEMORY_SIZE_BLOCK 	       	(BACKUP_MEMORY_SIZE_BYTE/BSP_FEATURE_MRAM_PROGRAMMING_SIZE_BYTES)

#define Default_Data_Is_Not_Initialized 	0xFFFF
#define Default_Data_Is_Initialized 		0x5555

#endif // #if BACKUP_PARAMETER_SUPPORTED
/*-----------------------------------------------------------------------------------------
------    NonVolatileWordOffset
-----------------------------------------------------------------------------------------*/
#define NonVolatileWordOffset_0x10F0 0x0002
#define NonVolatileWordOffset_0xF3A1 0x0008
#define NonVolatileWordOffset_0xF3A2 0x0010
#define NonVolatileWordOffset_0xF3A3 0x0018
#define NonVolatileWordOffset_0xF3A4 0x0020
#define NonVolatileWordOffset_0xF3A5 0x0028
#define NonVolatileWordOffset_0xF3A6 0x0030
#define NonVolatileWordOffset_0xF3A7 0x0038
#define NonVolatileWordOffset_0xF3A8 0x0040

/*--------------------------------------------------------------------------------------
------
------    Function prototype
------
--------------------------------------------------------------------------------------*/
#define PROTO extern
PROTO void handle_error(fsp_err_t err);
#if BACKUP_PARAMETER_SUPPORTED
PROTO void   	EE_ResetFlashData(void);
PROTO UINT32 	EE_GetChecksum(void);
PROTO UINT8  	EE_IsDefaultDataInitialized(void);
PROTO void   	EE_StoreDefaultData(void);
PROTO void   	EE_LoadDefaultData(void);
PROTO void   	EE_ReadWordsFromNonVolatileMemory(UINT16 HUGE *pDest, UINT16 srcOffset, UINT16 n);
PROTO UINT32 	EE_WriteWordsToNonVolatileMemory(UINT16 destOffset, UINT16 HUGE * pSrc, UINT16 n);
PROTO BOOL 		Check_StoreParameterAddress(const UINT32 address);
PROTO void 		Print_WriteWordsToNonVolatileMemory(UINT16 destOffset);
#endif // #if BACKUP_PARAMETER_SUPPORTED
PROTO UINT8 	ReadFunction0xFBFx( UINT16 Index, UINT8 Subindex, UINT32 Size, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
PROTO UINT8 	Write0xFBF0( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
PROTO UINT8 	Write0xFBF2( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
PROTO void  	Device_Reset(void);
PROTO void  	StoreBackupEntries(void);
#undef PROTO

/*-----------------------------------------------------------------------------------------
------
------    Objects
------
-----------------------------------------------------------------------------------------*/
#ifdef _SEMI_SAMPLE_APPLICATION_
    #define PROTO
#else
    #define PROTO extern
#endif

/******************************************************************************
*                    Object 0x100B : Manufacturer Bootloader Version
******************************************************************************/
/**
* \addtogroup 0x100B 0x100B | Manufacturer Bootloader Version
* @{
* \brief Object 0x100B (Manufacturer Bootloader Version) definition
*/
#ifdef _OBJD_
/**
* \brief Entry description
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0x100B = { DEFTYPE_VISIBLESTRING , BYTE2BIT(0x3) , ACCESS_READ };
/**
* \brief Object name
*/
OBJCONST UCHAR OBJMEM aName0x100B[] = "Manufacturer Bootloader Version\000\377";
#endif //#ifdef _OBJD_

/**
* \brief Object variable
*/
PROTO CHAR ManufacturerBootloaderVersion0x100B[]
#ifdef _SEMI_SAMPLE_APPLICATION_
											   = "1.0"
#endif // #ifdef _SEMI_SAMPLE_APPLICATION_
;



/******************************************************************************
*                    Object 0x17FF : Device User RxPDO
******************************************************************************/
/** \brief 0x17FF (Device User RxPDO) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
   UINT32   aEntries[1]; /**< \brief Entry buffer*/
} OBJ_STRUCT_PACKED_END
TOBJ17FF;

#ifdef _OBJD_
/**
 * \brief Object 0x17FF (Device User RxPDO) entry descriptions
 *
 * SubIndex 0 <br>
 * SubIndex 1
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x17FF[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READWRITE}};

/**
 * \brief Object 0x17FF (Device User RxPDO) object and entry names
 *
 * In this example no specific entry name is defined ("SubIndex xxx" is used)
 */
OBJCONST UCHAR OBJMEM aName0x17FF[] = "Device User RxPDO-Map\000\377";
#endif //#ifdef _OBJD_

/**
 * \brief Object 0x17FF (RxPDO) variable to handle object data
 *
 * SubIndex 0 : 1<br>
 * SubIndex 1 : none
 */
PROTO TOBJ17FF DeviceUserRxPDOMap0x17FF
#ifdef _SEMI_SAMPLE_APPLICATION_
 = {1, {0x00000000}}
#endif
;

/******************************************************************************
*                    Object 0x1BFF : Device User TxPDO
******************************************************************************/
/** \brief 0x1BFF (Device User TxPDO) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
   UINT32   aEntries[1]; /**< \brief Entry buffer*/
} OBJ_STRUCT_PACKED_END
TOBJ1BFF;

#ifdef _OBJD_
/**
 * \brief Object 0x1BFF (Device User TxPDO) entry descriptions
 *
 * SubIndex 0<br>
 * SubIndex 1
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1BFF[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READWRITE}};

/**
 * \brief Object 0x1BFF (Device User TxPDO) object and entry names
 *
 * In this example no specific entry name is defined ("SubIndex xxx" is used)
 */
OBJCONST UCHAR OBJMEM aName0x1BFF[] = "Device User TxPDO-Map\000\377";
#endif //#ifdef _OBJD_

/**
 * \brief Object 0x1BFF (Device User TxPDO) variable to handle object data
 *
 * SubIndex 0 : 1<br>
 * SubIndex 1 : none
 */
PROTO TOBJ1BFF DeviceUserTxPDOMap0x1BFF
#ifdef _SEMI_SAMPLE_APPLICATION_
 = {1, {0x00000000}}
#endif
;
/** @}*/



/**
* \addtogroup DeviceParaObjects Device Parameter Objects
* @{
*/
/******************************************************************************
*                    Object 0xF000 : Semiconductor Device Profile
******************************************************************************/
/**
* \addtogroup 0xF000 0xF000 | Semiconductor Device Profile
* @{
* \brief Object 0xF000 (Semiconductor Device Profile) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Index Distance<br>
* SubIndex 2 - Maximum Number of Modules<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF000[] = {
   { DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   { DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ },  /* Subindex1 - Index Distance */
   { DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }}; /* Subindex2 - Maximum Number of Modules */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xF000[] = "Semiconductor Device Profile\000"
"Index Distance\000"
"Maximum Number of Modules\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT16 IndexDistance; /* Subindex1 - Index Distance
                       *
                       * Index distance between two modules (maximum number of objects per module and area)<br>
                       * Default: 0x10*/
UINT16 MaximumNumberOfModules; /* Subindex2 - Maximum Number of Modules */
} OBJ_STRUCT_PACKED_END
TOBJF000;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF000 SemiconductorDeviceProfile0xF000
#ifdef _SEMI_SAMPLE_APPLICATION_
= { 2, 0x10, 8 }
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF010 : Module Profile List
******************************************************************************/
/**
* \addtogroup 0xF010 0xF010 | Module Profile List
* @{
* \brief Object 0xF010 (Module Profile List) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF010[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF010[] = "Module Profile List\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32   aEntries[2];   /**< \brief Module profile information buffer
                          *
                          * Bit 0..15: Profile number of the module on position 1<br>
                          * Bit 16..31: Profile specific*/
} OBJ_STRUCT_PACKED_END
TOBJF010;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF010 ModuleProfileList0xF010
#ifdef _SEMI_SAMPLE_APPLICATION_
= { 2, {0,0} }
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF020 : Configured Address List
******************************************************************************/
/**
* \addtogroup 0xF020 0xF020 | Configured Address List
* @{
* \brief Object 0xF020 (Configured Address List) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF020[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READWRITE },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF020[] = "Configured Address List\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[2];  /**< \brief Subindex 1 - 1 */
} OBJ_STRUCT_PACKED_END
TOBJF020;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF020 ConfiguredAddressList0xF020
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0,0}}
#endif
;
/** @}*/


#if(CiA402_SAMPLE_APPLICATION != 1)

/******************************************************************************
*                    Object 0xF030 : Configured Module Ident List
******************************************************************************/
/**
* \addtogroup 0xF030 0xF030 | Configured Module Ident List
* @{
* \brief Object 0xF030 (Configured Module Ident List) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF030[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READWRITE },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF030[] = "Configured Module Ident List\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[2];  /**< \brief Subindex 1 - 1 */
} OBJ_STRUCT_PACKED_END
TOBJF030;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF030 ConfiguredModuleIdentList0xF030
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0,0}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF050 : Detected Module Ident List
******************************************************************************/
/**
* \addtogroup 0xF050 0xF050 | Detected Module Ident List
* @{
* \brief Object 0xF050 (Detected Module Ident List) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF050[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF050[] = "Detected Module Ident List\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[2];  /**< \brief Subindex 1 - 1 */
} OBJ_STRUCT_PACKED_END
TOBJF050;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF050 DetectedModuleIdentList0xF050
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0x519800,0}}
#endif
;
/** @}*/

#endif // #if(CiA402_SAMPLE_APPLICATION != 1)

/******************************************************************************
*                    Object 0xF380 : Active Exception Status
******************************************************************************/
/**
* \addtogroup 0xF380 0xF380 | Active Exception Status
* @{
* \brief Object 0xF380 (Active Exception Status) definition
*/
#ifdef _OBJD_
/**
* \brief Entry description
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0xF380 = { DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ | OBJACCESS_TXPDOMAPPING };
/**
* \brief Object name
*/
OBJCONST UCHAR OBJMEM aName0xF380[] = "Active Exception Status\000\377";
#endif //#ifdef _OBJD_

/**
* \brief Object variable
*/
PROTO UINT8 ActiveExceptionStatus0xF380;
/** @}*/


/******************************************************************************
*                    Object 0xF381 : Active Device Warning Details
******************************************************************************/
/**
* \addtogroup 0xF381 0xF381 | Active Device Warning Details
* @{
* \brief Object 0xF381 (Active Device Warning Details) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF381[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF381[] = "Active Device Warning Details\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[2];  /**< \brief Subindex 1 ~ 2 */
} OBJ_STRUCT_PACKED_END
TOBJF381;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF381 ActiveDeviceWarningDetails0xF381
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0,0}}
#endif
;
/** @}*/


/******************************************************************************
*                    Object 0xF382 : Active Manufacturer Warning Details
******************************************************************************/
/**
* \addtogroup 0xF382 0xF382 | Active Manufacturer Warning Details
* @{
* \brief Object 0xF382 (Active Manufacturer Warning Details) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF382[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF382[] = "Active Manufacturer Warning Details\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[2];  /**< \brief Subindex 1 ~ 2 */
} OBJ_STRUCT_PACKED_END
TOBJF382;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF382 ActiveManufacturerWarningDetails0xF382
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0,0}}
#endif
;
/** @}*/


/******************************************************************************
*                    Object 0xF383 : Active Device Error Details
******************************************************************************/
/**
* \addtogroup 0xF383 0xF383 | Active Device Error Details
* @{
* \brief Object 0xF383 (Active Device Error Details) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF383[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF383[] = "Active Device Error Details\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[2];  /**< \brief Subindex 1 - 2 */
} OBJ_STRUCT_PACKED_END
TOBJF383;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF383 ActiveDeviceErrorDetails0xF383
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0,0}}
#endif
;
/** @}*/


/******************************************************************************
*                    Object 0xF384 : Active Manufacturer Error Details
******************************************************************************/
/**
* \addtogroup 0xF384 0xF384 | Active Manufacturer Error Details
* @{
* \brief Object 0xF384 (Active Manufacturer Error Details) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF384[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF384[] = "Active Manufacturer Error Details\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[2];  /**< \brief Subindex 1 - 1 */
} OBJ_STRUCT_PACKED_END
TOBJF384;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF384 ActiveManufacturerErrorDetails0xF384
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0,0}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF385 : Active Global Device Warning Details
******************************************************************************/
/**
* \addtogroup 0xF385 0xF385 | Active Global Device Warning Details
* @{
* \brief Object 0xF385 (Active Global Device Warning Details) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Expanded details of the SDP-specific device warning exceptions, applicable to the overall device(one instance only).<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF385[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }}; /* Subindex1 - Expanded details of the SDP-specific device warning exceptions, applicable to the overall device(one instance only). */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xF385[] = "Active Global Device Warning Details\000"
"SubIndex 001\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 SubIndex001; /* Subindex1 - SubIndex 001 */
} OBJ_STRUCT_PACKED_END
TOBJF385;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF385 ActiveGlobalDeviceWarningDetails0xF385
#ifdef _SEMI_SAMPLE_APPLICATION_
={1,0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF386 : Active Global Manufacturer Warning Details
******************************************************************************/
/**
* \addtogroup 0xF386 0xF386 | Active Global Manufacturer Warning Details
* @{
* \brief Object 0xF386 (Active Global Manufacturer Warning Details) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Expanded details of the SDP specific device error exceptions, applicable to the overall device (one instance only).<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF386[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }}; /* Subindex1 - Expanded details of the SDP specific device error exceptions, applicable to the overall device (one instance only). */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xF386[] = "Active Global Manufacturer Warning Details\000"
"SubIndex 001\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 SubIndex001; /* Subindex1 - SubIndex 001 */
} OBJ_STRUCT_PACKED_END
TOBJF386;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF386 ActiveGlobalManufacturerWarningDetails0xF386
#ifdef _SEMI_SAMPLE_APPLICATION_
={1,0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF387 : Active Global Device Error Details
******************************************************************************/
/**
* \addtogroup 0xF387 0xF387 | Active Global Device Error Details
* @{
* \brief Object 0xF387 (Active Global Device Error Details) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Expanded details of the SDP specific device error exceptions, applicable to the overall device (once instance only).<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF387[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }}; /* Subindex1 - Expanded details of the SDP specific device error exceptions, applicable to the overall device (once instance only). */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xF387[] = "Active Global Device Error Details\000"
"SubIndex 001\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 SubIndex001; /* Subindex1 - SubIndex 001 */
} OBJ_STRUCT_PACKED_END
TOBJF387;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF387 ActiveGlobalDeviceErrorDetails0xF387
#ifdef _SEMI_SAMPLE_APPLICATION_
={1,0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF388 : Active Global Manufacturer Error Details
******************************************************************************/
/**
* \addtogroup 0xF388 0xF388 | Active Global Manufacturer Error Details
* @{
* \brief Object 0xF388 (Active Global Manufacturer Error Details) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Expanded details of the manufacturer error exceptions specified by the manufacturer, applicable to the overall device (one instance only).<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF388[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }}; /* Subindex1 - Expanded details of the manufacturer error exceptions specified by the manufacturer, applicable to the overall device (one instance only). */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xF388[] = "Active Global Manufacturer Error Details\000"
"SubIndex 001\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 SubIndex001; /* Subindex1 - SubIndex 001 */
} OBJ_STRUCT_PACKED_END
TOBJF388;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF388 ActiveGlobalManufacturerErrorDetails0xF388
#ifdef _SEMI_SAMPLE_APPLICATION_
={1,0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF390 : Latched Exception Status
******************************************************************************/
/**
* \addtogroup 0xF390 0xF390 | Latched Exception Status
* @{
* \brief Object 0xF390 (Latched Exception Status) definition
*/
#ifdef _OBJD_
/**
* \brief Entry description
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0xF390 = { DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ | OBJACCESS_TXPDOMAPPING };
/**
* \brief Object name
*/
OBJCONST UCHAR OBJMEM aName0xF390[] = "Latched Exception Status\000\377";
#endif //#ifdef _OBJD_

/**
* \brief Object variable
*/
PROTO UINT8 LatchedExceptionStatus0xF390;
/** @}*/


/******************************************************************************
*                    Object 0xF391 : Latched Device Warning Details
******************************************************************************/
/**
* \addtogroup 0xF391 0xF391 | Latched Device Warning Details
* @{
* \brief Object 0xF391 (Latched Device Warning Details) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF391[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF391[] = "Latched Device Warning Details\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[2];  /**< \brief Subindex 1 - 1 */
} OBJ_STRUCT_PACKED_END
TOBJF391;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF391 LatchedDeviceWarningDetails0xF391
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0,0}}
#endif
;
/** @}*/


/******************************************************************************
*                    Object 0xF392 : Latched Manufacturer Warning Details
******************************************************************************/
/**
* \addtogroup 0xF392 0xF392 | Latched Manufacturer Warning Details
* @{
* \brief Object 0xF392 (Latched Manufacturer Warning Details) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF392[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF392[] = "Latched Manufacturer Warning Details\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[2];  /**< \brief Subindex 1 - 2 */
} OBJ_STRUCT_PACKED_END
TOBJF392;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF392 LatchedManufacturerWarningDetails0xF392
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0,0}}
#endif
;
/** @}*/


/******************************************************************************
*                    Object 0xF393 : Latched Device Error Details
******************************************************************************/
/**
* \addtogroup 0xF393 0xF393 | Latched Device Error Details
* @{
* \brief Object 0xF393 (Latched Device Error Details) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF393[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF393[] = "Latched Device Error Details\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[2];  /**< \brief Subindex 1 - 2 */
} OBJ_STRUCT_PACKED_END
TOBJF393;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF393 LatchedDeviceErrorDetails0xF393
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0,0}}
#endif
;
/** @}*/


/******************************************************************************
*                    Object 0xF394 : Latched Manufacturer Error Details
******************************************************************************/
/**
* \addtogroup 0xF394 0xF394 | Latched Manufacturer Error Details
* @{
* \brief Object 0xF394 (Latched Manufacturer Error Details) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF394[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF394[] = "Latched Manufacturer Error Details\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[2];  /**< \brief Subindex 1 - 1 */
} OBJ_STRUCT_PACKED_END
TOBJF394;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF394 LatchedManufacturerErrorDetails0xF394
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0,0}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF395 : Latched Global Device Warning Details
******************************************************************************/
/**
* \addtogroup 0xF395 0xF395 | Latched Global Device Warning Details
* @{
* \brief Object 0xF395 (Latched Global Device Warning Details) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Latched Global Device Warning Details<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF395[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }}; /* Subindex1 - Latched Global Device Warning Details */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xF395[] = "Latched Global Device Warning Details\000"
"Latched Global Device Warning Details\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 LatchedGlobalDeviceWarningDetails; /* Subindex1 - Latched Global Device Warning Details */
} OBJ_STRUCT_PACKED_END
TOBJF395;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF395 LatchedGlobalDeviceWarningDetails0xF395
#ifdef _SEMI_SAMPLE_APPLICATION_
={1,0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF396 : Latched Global Manufacturer Warning Details
******************************************************************************/
/**
* \addtogroup 0xF396 0xF396 | Latched Global Manufacturer Warning Details
* @{
* \brief Object 0xF396 (Latched Global Manufacturer Warning Details) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Latched Global Manufacturer Warning Details<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF396[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }}; /* Subindex1 - Latched Global Manufacturer Warning Details */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xF396[] = "Latched Global Manufacturer Warning Details\000"
"Latched Global Manufacturer Warning Details\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 LatchedGlobalManufacturerWarningDetails; /* Subindex1 - Latched Global Manufacturer Warning Details */
} OBJ_STRUCT_PACKED_END
TOBJF396;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF396 LatchedGlobalManufacturerWarningDetails0xF396
#ifdef _SEMI_SAMPLE_APPLICATION_
={1,0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF397 : Latched Global Device Error Details
******************************************************************************/
/**
* \addtogroup 0xF397 0xF397 | Latched Global Device Error Details
* @{
* \brief Object 0xF397 (Latched Global Device Error Details) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Latched Global Device Error Details<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF397[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }}; /* Subindex1 - Latched Global Device Error Details */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xF397[] = "Latched Global Device Error Details\000"
"Latched Global Device Error Details\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 LatchedGlobalDeviceErrorDetails; /* Subindex1 - Latched Global Device Error Details */
} OBJ_STRUCT_PACKED_END
TOBJF397;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF397 LatchedGlobalDeviceErrorDetails0xF397
#ifdef _SEMI_SAMPLE_APPLICATION_
={1,0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF398 : Latched Global Manufacturer Error Details
******************************************************************************/
/**
* \addtogroup 0xF398 0xF398 | Latched Global Manufacturer Error Details
* @{
* \brief Object 0xF398 (Latched Global Manufacturer Error Details) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Latched Global Manufacturer Error Details<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF398[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }}; /* Subindex1 - Latched Global Manufacturer Error Details */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xF398[] = "Latched Global Manufacturer Error Details\000"
"Latched Global Manufacturer Error Details\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 LatchedGlobalManufacturerErrorDetails; /* Subindex1 - Latched Global Manufacturer Error Details */
} OBJ_STRUCT_PACKED_END
TOBJF398;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF398 LatchedGlobalManufacturerErrorDetails0xF398
#ifdef _SEMI_SAMPLE_APPLICATION_
={1,0}
#endif
;
/** @}*/


/******************************************************************************
*                    Object 0xF3A1 : Device Warning Mask
******************************************************************************/
/**
* \addtogroup 0xF3A1 0xF3A1 | Device Warning Mask
* @{
* \brief Object 0xF3A1 (Device Warning Mask) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF3A1[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE | OBJACCESS_BACKUP },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE | OBJACCESS_BACKUP }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF3A1[] = "Device Warning Mask\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[2];  /**< \brief Subindex 1 - 1 */
} OBJ_STRUCT_PACKED_END
TOBJF3A1;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF3A1 DeviceWarningMask0xF3A1
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0,0}}
#endif
;
/** @}*/


/******************************************************************************
*                    Object 0xF3A2 : Manufacturer Warning Mask
******************************************************************************/
/**
* \addtogroup 0xF3A2 0xF3A2 | Manufacturer Warning Mask
* @{
* \brief Object 0xF3A2 (Manufacturer Warning Mask) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF3A2[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE | OBJACCESS_BACKUP },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE | OBJACCESS_BACKUP }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF3A2[] = "Manufacturer Warning Mask\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[2];  /**< \brief Subindex 1 - 2 */
} OBJ_STRUCT_PACKED_END
TOBJF3A2;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF3A2 ManufacturerWarningMask0xF3A2
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0,0}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF3A3 : Device Error Mask
******************************************************************************/
/**
* \addtogroup 0xF3A3 0xF3A3 | Device Error Mask
* @{
* \brief Object 0xF3A3 (Device Error Mask) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF3A3[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE | OBJACCESS_BACKUP },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE | OBJACCESS_BACKUP }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF3A3[] = "Device Error Mask\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[2];  /**< \brief Subindex 1 - 1 */
} OBJ_STRUCT_PACKED_END
TOBJF3A3;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF3A3 DeviceErrorMask0xF3A3
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0,0}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF3A4 : Manufacturer Error Mask
******************************************************************************/
/**
* \addtogroup 0xF3A4 0xF3A4 | Manufacturer Error Mask
* @{
* \brief Object 0xF3A4 (Manufacturer Error Mask) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF3A4[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE | OBJACCESS_BACKUP },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE | OBJACCESS_BACKUP }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF3A4[] = "Manufacturer Error Mask\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[2];  /**< \brief Subindex 1 - 1 */
} OBJ_STRUCT_PACKED_END
TOBJF3A4;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF3A4 ManufacturerErrorMask0xF3A4
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0,0}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF3A5 : Global Device Warning Mask
******************************************************************************/
/**
* \addtogroup 0xF3A5 0xF3A5 | Global Device Warning Mask
* @{
* \brief Object 0xF3A5 (Global Device Warning Mask) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Global Device Warning Mask<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF3A5[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE | OBJACCESS_BACKUP }}; /* Subindex1 - Global Device Warning Mask */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xF3A5[] = "Global Device Warning Mask\000"
"Global Device Warning Mask\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 GlobalDeviceWarningMask; /* Subindex1 - Global Device Warning Mask */
} OBJ_STRUCT_PACKED_END
TOBJF3A5;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF3A5 GlobalDeviceWarningMask0xF3A5
#ifdef _SEMI_SAMPLE_APPLICATION_
={1,0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF3A6 : Global Manufacturer Warning Mask
******************************************************************************/
/**
* \addtogroup 0xF3A6 0xF3A6 | Global Manufacturer Warning Mask
* @{
* \brief Object 0xF3A6 (Global Manufacturer Warning Mask) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Global Manufacturer Warning Mask<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF3A6[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE | OBJACCESS_BACKUP }}; /* Subindex1 - Global Manufacturer Warning Mask */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xF3A6[] = "Global Manufacturer Warning Mask\000"
"Global Manufacturer Warning Mask\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 GlobalManufacturerWarningMask; /* Subindex1 - Global Manufacturer Warning Mask */
} OBJ_STRUCT_PACKED_END
TOBJF3A6;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF3A6 GlobalManufacturerWarningMask0xF3A6
#ifdef _SEMI_SAMPLE_APPLICATION_
={1,0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF3A7 : Global Device Error Mask
******************************************************************************/
/**
* \addtogroup 0xF3A7 0xF3A7 | Global Device Error Mask
* @{
* \brief Object 0xF3A7 (Global Device Error Mask) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Global Device Error Mask<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF3A7[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE | OBJACCESS_BACKUP }}; /* Subindex1 - Global Device Error Mask */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xF3A7[] = "Global Device Error Mask\000"
"Global Device Error Mask\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 GlobalDeviceErrorMask; /* Subindex1 - Global Device Error Mask */
} OBJ_STRUCT_PACKED_END
TOBJF3A7;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF3A7 GlobalDeviceErrorMask0xF3A7
#ifdef _SEMI_SAMPLE_APPLICATION_
={1,0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF3A8 : Global Manufacturer Error Mask
******************************************************************************/
/**
* \addtogroup 0xF3A8 0xF3A8 | Global Manufacturer Error Mask
* @{
* \brief Object 0xF3A8 (Global Manufacturer Error Mask) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Global Manufacturer Error Mask<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF3A8[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE | OBJACCESS_BACKUP }}; /* Subindex1 - Global Manufacturer Error Mask */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xF3A8[] = "Global Manufacturer Error Mask\000"
"Global Manufacturer Error Mask\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 GlobalManufacturerErrorMask; /* Subindex1 - Global Manufacturer Error Mask */
} OBJ_STRUCT_PACKED_END
TOBJF3A8;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF3A8 GlobalManufacturerErrorMask0xF3A8
#ifdef _SEMI_SAMPLE_APPLICATION_
={1,0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF6F0 : Input Latch Local Timestamp
******************************************************************************/
/**
* \addtogroup 0xF6F0 0xF6F0 | Input Latch Local Timestamp
* @{
* \brief Object 0xF6F0 (Input Latch Local Timestamp) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF6F0[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF6F0[] = "Input Latch Local Timestamp\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[2];  /**< \brief Subindex 1 - 2 */
} OBJ_STRUCT_PACKED_END
TOBJF6F0;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF6F0 InputLatchLocalTimestamp0xF6F0
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0,0}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF6F1 : Input Latch ESC Timestamp (32-bit)
******************************************************************************/
/**
* \addtogroup 0xF6F1 0xF6F1 | Input Latch ESC Timestamp (32-bit)
* @{
* \brief Object 0xF6F1 (Input Latch ESC Timestamp (32-bit)) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF6F1[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF6F1[] = "Input Latch ESC Timestamp (32-bit)\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[2];  /**< \brief Subindex 1 - 2 */
} OBJ_STRUCT_PACKED_END
TOBJF6F1;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF6F1 InputLatchESCTimestamp32Bit0xF6F1
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0,0}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF6F2 : Input Latch ESC Timestamp (64-bit)
******************************************************************************/
/**
* \addtogroup 0xF6F2 0xF6F2 | Input Latch ESC Timestamp (64-bit)
* @{
* \brief Object 0xF6F2 (Input Latch ESC Timestamp (64-bit)) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF6F2[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED64 , 0x40 , ACCESS_READ | OBJACCESS_TXPDOMAPPING },
   { DEFTYPE_UNSIGNED64 , 0x40 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF6F2[] = "Input Latch ESC Timestamp (64-bit)\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT64 aEntries[2];  /**< \brief Subindex 1 - 2 */
} OBJ_STRUCT_PACKED_END
TOBJF6F2;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF6F2 InputLatchESCTimestamp64Bit0xF6F2
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0,0}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF9F0 : Manufacturer Serial Number
******************************************************************************/
/**
* \addtogroup 0xF9F0 0xF9F0 | Manufacturer Serial Number
* @{
* \brief Object 0xF9F0 (Manufacturer Serial Number) definition
*/
#ifdef _OBJD_
/**
* \brief Entry description
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0xF9F0 = { DEFTYPE_VISIBLESTRING , BYTE2BIT(0x8) , ACCESS_READ };
/**
* \brief Object name
*/
OBJCONST UCHAR OBJMEM aName0xF9F0[] = "Manufacturer Serial Number\000\377";
#endif //#ifdef _OBJD_

/**
* \brief Object variable
*/
PROTO CHAR ManufacturerSerialNumber0xF9F0[]
#ifdef _SEMI_SAMPLE_APPLICATION_
= "00000100"
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF9F1 : CDP Functional Generation Number
******************************************************************************/
/**
* \addtogroup 0xF9F1 0xF9F1 | CDP Functional Generation Number
* @{
* \brief Object 0xF9F1 (CDP Functional Generation Number) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF9F1[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF9F1[] = "CDP Functional Generation Number\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[1];  /**< \brief Subindex 1 - 1 */
} OBJ_STRUCT_PACKED_END
TOBJF9F1;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF9F1 CDPFunctionalGenerationNumber0xF9F1
#ifdef _SEMI_SAMPLE_APPLICATION_
={1,{1}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF9F2 : SDP Functional Generation Number
******************************************************************************/
/**
* \addtogroup 0xF9F2 0xF9F2 | SDP Functional Generation Number
* @{
* \brief Object 0xF9F2 (SDP Functional Generation Number) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF9F2[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ },
   { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF9F2[] = "SDP Functional Generation Number\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT32 aEntries[2];  /**< \brief Subindex 1 - 2 */
} OBJ_STRUCT_PACKED_END
TOBJF9F2;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF9F2 SDPFunctionalGenerationNumber0xF9F2
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{1,1}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF9F3 : Vendor Name
******************************************************************************/
/**
* \addtogroup 0xF9F3 0xF9F3 | Vendor Name
* @{
* \brief Object 0xF9F3 (Vendor Name) definition
*/
#ifdef _OBJD_
/**
* \brief Entry description
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0xF9F3 = { DEFTYPE_VISIBLESTRING , BYTE2BIT(0x7) , ACCESS_READ };
/**
* \brief Object name
*/
OBJCONST UCHAR OBJMEM aName0xF9F3[] = "Vendor Name\000\377";
#endif //#ifdef _OBJD_

/**
* \brief Object variable
*/
PROTO CHAR VendorName0xF9F3[]
#ifdef _SEMI_SAMPLE_APPLICATION_
= "Renesas"
#endif
;
/** @}*/

/******************************************************************************
*                    Object 0xF9F4 : Semiconductor SDP Device Name
******************************************************************************/
/**
* \addtogroup 0xF9F4 0xF9F4 | Semiconductor SDP Device Name
* @{
* \brief Object 0xF9F4 (Semiconductor SDP Device Name) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF9F4[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_VISIBLESTRING , BYTE2BIT(0x8) , ACCESS_READ },
   { DEFTYPE_VISIBLESTRING , BYTE2BIT(0x8) , ACCESS_READ }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF9F4[] = "Semiconductor SDP Device Name\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
CHAR aEntries[2][8];  /**< \brief Subindex 1 - 2 */
} OBJ_STRUCT_PACKED_END
TOBJF9F4;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF9F4 SemiconductorSDPDeviceName0xF9F4
#ifdef _SEMI_SAMPLE_APPLICATION_
={2, {"N/A","N/A"}}
#endif //_SEMI_SAMPLE_APPLICATION_
;
/** @}*/

/******************************************************************************
*                    Object 0xF9F5 : Output Identifier
******************************************************************************/
/**
* \addtogroup 0xF9F5 0xF9F5 | Output Identifier
* @{
* \brief Object 0xF9F5 (Output Identifier) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF9F5[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READWRITE | OBJACCESS_RXPDOMAPPING | OBJACCESS_TXPDOMAPPING },
   { DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READWRITE | OBJACCESS_RXPDOMAPPING | OBJACCESS_TXPDOMAPPING }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF9F5[] = "Output Identifier\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT8 aEntries[2];  /**< \brief Subindex 1 - 2 */
} OBJ_STRUCT_PACKED_END
TOBJF9F5;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF9F5 OutputIdentifier0xF9F5
#ifdef _SEMI_SAMPLE_APPLICATION_
={2,{0,0}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xF9F6 : Time since power on
******************************************************************************/
/**
* \addtogroup 0xF9F6 0xF9F6 | Time since power on
* @{
* \brief Object 0xF9F6 (Time since power on) definition
*/
#ifdef _OBJD_
/**
* \brief Entry description
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0xF9F6 = { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ };
/**
* \brief Object name
*/
OBJCONST UCHAR OBJMEM aName0xF9F6[] = "Time since power on\000\377";
#endif //#ifdef _OBJD_

/**
* \brief Object variable
*/
PROTO UINT32 TimeSincePowerOn0xF9F6;
/** @}*/



/******************************************************************************
*                    Object 0xF9F7 : Total time powered
******************************************************************************/
/**
* \addtogroup 0xF9F7 0xF9F7 | Total time powered
* @{
* \brief Object 0xF9F7 (Total time powered) definition
*/
#ifdef _OBJD_
/**
* \brief Entry description
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0xF9F7 = { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ };
/**
* \brief Object name
*/
OBJCONST UCHAR OBJMEM aName0xF9F7[] = "Total time powered\000\377";
#endif //#ifdef _OBJD_

/**
* \brief Object variable
*/
PROTO UINT32 TotalTimePowered0xF9F7;
/** @}*/



/******************************************************************************
*                    Object 0xF9F8 : Firmware Update Functional Generation Number
******************************************************************************/
/**
* \addtogroup 0xF9F8 0xF9F8 | Firmware Update Functional Generation Number
* @{
* \brief Object 0xF9F8 (Firmware Update Functional Generation Number) definition
*/
#ifdef _OBJD_
/**
* \brief Entry description
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0xF9F8 = { DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ };
/**
* \brief Object name
*/
OBJCONST UCHAR OBJMEM aName0xF9F8[] = "Firmware Update Functional Generation Number\000\377";
#endif //#ifdef _OBJD_

/**
* \brief Object variable
*/
PROTO UINT32 FirmwareUpdateFunctionalGenerationNumber0xF9F8
#ifdef _SEMI_SAMPLE_APPLICATION_
=1
#endif
;
/** @}*/


/******************************************************************************
*                    Object 0xF9F9 : Module Manufacturer Hardware Version
******************************************************************************/
/**
* \addtogroup 0xF9F9 0xF9F9 | Module Manufacturer Hardware Version
* @{
* \brief Object 0xF9F9 (Module Manufacturer Hardware Version) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF9F9[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_VISIBLESTRING , BYTE2BIT(0x8) , ACCESS_READ },
   { DEFTYPE_VISIBLESTRING , BYTE2BIT(0x8) , ACCESS_READ }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF9F9[] = "Module Manufacturer Hardware Version\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
CHAR aEntries[2][8];  /**< \brief Subindex 1 - 1 */
} OBJ_STRUCT_PACKED_END
TOBJF9F9;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF9F9 ModuleManufacturerHardwareVersion0xF9F9
#ifdef _SEMI_SAMPLE_APPLICATION_
={2, {"1.0","1.0"}}
#endif //_SEMI_SAMPLE_APPLICATION_
;
/** @}*/


/******************************************************************************
*                    Object 0xF9FA : Module Manufacturer Software Version
******************************************************************************/
/**
* \addtogroup 0xF9FA 0xF9FA | Module Manufacturer Software Version
* @{
* \brief Object 0xF9FA (Module Manufacturer Software Version) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF9FA[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_VISIBLESTRING , BYTE2BIT(0x8) , ACCESS_READ },
   { DEFTYPE_VISIBLESTRING , BYTE2BIT(0x8) , ACCESS_READ }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF9FA[] = "Module Manufacturer Software Version\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
CHAR aEntries[2][8];  /**< \brief Subindex 1 - 1 */
} OBJ_STRUCT_PACKED_END
TOBJF9FA;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF9FA ModuleManufacturerSoftwareVersion0xF9FA
#ifdef _SEMI_SAMPLE_APPLICATION_
={2, {"1.0","1.0"}}
#endif //_SEMI_SAMPLE_APPLICATION_
;
/** @}*/


/******************************************************************************
*                    Object 0xF9FB : Module Manufacturer Serial Number
******************************************************************************/
/**
* \addtogroup 0xF9FB 0xF9FB | Module Manufacturer Serial Number
* @{
* \brief Object 0xF9FB (Module Manufacturer Serial Number) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
*
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF9FB[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_VISIBLESTRING , BYTE2BIT(0x8) , ACCESS_READ },
   { DEFTYPE_VISIBLESTRING , BYTE2BIT(0x8) , ACCESS_READ }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0xF9FB[] = "Module Manufacturer Serial Number\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
CHAR aEntries[2][8];  /**< \brief Subindex 1 - 2 */
} OBJ_STRUCT_PACKED_END
TOBJF9FB;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJF9FB ModuleManufacturerSerialNumber0xF9FB
#ifdef _SEMI_SAMPLE_APPLICATION_
={2, {"0000","0000"}}
#endif //_SEMI_SAMPLE_APPLICATION_
;
/** @}*/


/******************************************************************************
*                    Object 0xFBF0 : Device Reset Command
******************************************************************************/
/**
* \addtogroup 0xFBF0 0xFBF0 | Device Reset Command
* @{
* \brief Object 0xFBF0 (Device Reset Command) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Command<br>
* SubIndex 2 - Status<br>
* SubIndex 3 - SI3<br>
* SubIndex 4 - Response<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xFBF0[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_OCTETSTRING , 0x30 , ACCESS_READWRITE }, /* Subindex1 - Command */
   { DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ },   	  /* Subindex2 - Status */
   { DEFTYPE_OCTETSTRING , 0x10 , ACCESS_READ }}; 	  /* Subindex3 - Response */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xFBF0[] = "Device Reset Command\000"
"Command\000"
"Status\000"
"Response\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT8 Command[6];	/* Subindex1 - Command */
UINT8 Status;		/* Subindex2 - Status */
UINT8 Response[2];	/* Subindex3 - Response */
} OBJ_STRUCT_PACKED_END
TOBJFBF0;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJFBF0 DeviceResetCommand0xFBF0
#ifdef _SEMI_SAMPLE_APPLICATION_
={3,
 {0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
 0x0,
 {0x0, 0x0}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xFBF1 : Exception Reset Command
******************************************************************************/
/**
* \addtogroup 0xFBF1 0xFBF1 | Exception Reset Command
* @{
* \brief Object 0xFBF1 (Exception Reset Command) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Command<br>
* SubIndex 2 - Status<br>
* SubIndex 3 - Response<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xFBF1[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_OCTETSTRING , 0x28 , ACCESS_READWRITE }, /* Subindex1 - Command */
   { DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ },     /* Subindex2 - Status */
   { DEFTYPE_OCTETSTRING , 0x10 , ACCESS_READ }};  /* Subindex3 - Response */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xFBF1[] = "Exception Reset Command\000"
"Command\000"
"Status\000"
"Response\000\377";
#else //#ifdef _OBJD_

#endif //#ifdef _OBJD_



#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT8 Command[5]; /* Subindex1 - Command */
UINT8 Status; /* Subindex2 - Status */
UINT8 Response[2]; /* Subindex3 - Response */
} OBJ_STRUCT_PACKED_END
TOBJFBF1;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJFBF1 ExceptionResetCommand0xFBF1
#ifdef _SEMI_SAMPLE_APPLICATION_
={3,
 {0x0, 0x0, 0x0, 0x0, 0x0},
 0x0,
 {0x0, 0x0}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xFBF2 : Store Parameters Command
******************************************************************************/
/**
* \addtogroup 0xFBF2 0xFBF2 | Store Parameters Command
* @{
* \brief Object 0xFBF2 (Store Parameters Command) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Command<br>
* SubIndex 2 - Status<br>
* SubIndex 3 - Response<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xFBF2[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_OCTETSTRING , 0x20 , ACCESS_READWRITE }, /* Subindex1 - Command */
   { DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ },        /* Subindex2 - Status */
   { DEFTYPE_OCTETSTRING , 0x10 , ACCESS_READ }};     /* Subindex3 - Response */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xFBF2[] = "Store Parameters Command\000"
"Command\000"
"Status\000"
"Response\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT8 Command[4]; 	/* Subindex1 - Command */
UINT8 Status; 		/* Subindex2 - Status */
UINT8 Response[2];  /* Subindex3 - Response */
} OBJ_STRUCT_PACKED_END
TOBJFBF2;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJFBF2 StoreParametersCommand0xFBF2
#ifdef _SEMI_SAMPLE_APPLICATION_
={3,
#if STORE_BACKUP_PARAMETER_IMMEDIATELY
 {0x2, 0x0, 0x0, 0x0},
#else
 {0x1, 0x0, 0x0, 0x0},
#endif
 0x0,
 {0x0, 0x0}}
#endif
;
/** @}*/


/******************************************************************************
*                    Object 0xFBF3 : Calculate Checksum Command
******************************************************************************/
/**
* \addtogroup 0xFBF3 0xFBF3 | Calculate Checksum Command
* @{
* \brief Object 0xFBF3 (Calculate Checksum Command) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Command<br>
* SubIndex 2 - Status<br>
* SubIndex 3 - Response<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xFBF3[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_OCTETSTRING , 0x20 , ACCESS_READWRITE }, /* Subindex1 - Command */
   { DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ },    /* Subindex2 - Status */
   { DEFTYPE_OCTETSTRING , 0x40 , ACCESS_READ }}; /* Subindex3 - Response */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xFBF3[] = "Calculate Checksum Command\000"
"Command\000"
"Status\000"
"Response\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT8 Command[4]; 	/* Subindex1 - Command */
UINT8 Status; 		/* Subindex2 - Status */
UINT8 Response[8];  /* Subindex3 - Response */
} OBJ_STRUCT_PACKED_END
TOBJFBF3;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJFBF3 CalculateChecksumCommand0xFBF3
#ifdef _SEMI_SAMPLE_APPLICATION_
={3,
 {0x0, 0x0, 0x0, 0x0},
 0x0,
 {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0xFBF4 : Load Parameters Command
******************************************************************************/
/**
* \addtogroup 0xFBF4 0xFBF4 | Load Parameters Command
* @{
* \brief Object 0xFBF4 (Load Parameters Command) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Command<br>
* SubIndex 2 - Status<br>
* SubIndex 3 - Response<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xFBF4[] = {
   { DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
   { DEFTYPE_OCTETSTRING , 0x20 , ACCESS_READWRITE }, /* Subindex1 - Command */
   { DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ },    	  /* Subindex2 - Status */
   { DEFTYPE_OCTETSTRING , 0x10 , ACCESS_READ }};     /* Subindex3 - Response */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0xFBF4[] = "Load Parameters Command\000"
"Command\000"
"Status\000"
"Response\000\377";
#endif //#ifdef _OBJD_

#ifdef APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT8 Command[4];   /* Subindex1 - Command */
UINT8 Status;       /* Subindex2 - Status */
UINT8 Response[2];  /* Subindex3 - Response */
} OBJ_STRUCT_PACKED_END
TOBJFBF4;
#endif //APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_

/**
* \brief Object variable
*/
PROTO TOBJFBF4 LoadParametersCommand0xFBF4
#ifdef _SEMI_SAMPLE_APPLICATION_
={3,
 {0x0, 0x0, 0x0, 0x0},
 0x0,
 {0x0, 0x0}}
#endif
;
/** @}*/

#ifdef _OBJD_
/**
 * \brief Object dictionary related to the CDP
 */
TOBJECT    OBJMEM CDPObjDic[] = {
   /* Object 0x100B */
   {NULL , NULL ,  0x100B , {DEFTYPE_VISIBLESTRING , 0 | (OBJCODE_VAR << 8)} , &sEntryDesc0x100B , aName0x100B , &ManufacturerBootloaderVersion0x100B , NULL , NULL , 0x0000 },
   /* Object 0x17FF */
   {NULL,NULL,  0x17FF, {DEFTYPE_PDOMAPPING, 1 | (OBJCODE_REC << 8)}, asEntryDesc0x17FF, aName0x17FF, &DeviceUserRxPDOMap0x17FF, NULL, NULL, 0x0000 },
   /* Object 0x1BFF */
   {NULL,NULL, 0x1BFF, {DEFTYPE_PDOMAPPING, 1 | (OBJCODE_REC << 8)}, asEntryDesc0x1BFF, aName0x1BFF, &DeviceUserTxPDOMap0x1BFF, NULL, NULL, 0x0000 },
   /* Object 0xF000 */
   {NULL , NULL ,  0xF000 , {DEFTYPE_RECORD , 2 | (OBJCODE_REC << 8)} , asEntryDesc0xF000 , aName0xF000 , &SemiconductorDeviceProfile0xF000 , NULL , NULL , 0x0000 },
   /* Object 0xF010 */
   {NULL , NULL ,  0xF010 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF010 , aName0xF010 , &ModuleProfileList0xF010 , NULL , NULL , 0x0000 },
   /* Object 0xF020 */
   {NULL , NULL ,  0xF020 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF020 , aName0xF020 , &ConfiguredAddressList0xF020 , NULL , NULL , 0x0000 },
   /* Object 0xF030 */
   {NULL , NULL ,  0xF030 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF030 , aName0xF030 , &ConfiguredModuleIdentList0xF030 , NULL , NULL , 0x0000 },
   /* Object 0xF050 */
   {NULL , NULL ,  0xF050 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF050 , aName0xF050 , &DetectedModuleIdentList0xF050 , NULL , NULL , 0x0000 },
   /* Object 0xF380 */
   {NULL , NULL ,  0xF380 , {DEFTYPE_UNSIGNED8 , 0 | (OBJCODE_VAR << 8)} , &sEntryDesc0xF380 , aName0xF380 , &ActiveExceptionStatus0xF380 , NULL , NULL , 0x0000 },
   /* Object 0xF381 */
   {NULL , NULL ,  0xF381 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF381 , aName0xF381 , &ActiveDeviceWarningDetails0xF381 , NULL , NULL , 0x0000 },
   /* Object 0xF382 */
   {NULL , NULL ,  0xF382 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF382 , aName0xF382 , &ActiveManufacturerWarningDetails0xF382 , NULL , NULL , 0x0000 },
   /* Object 0xF383 */
   {NULL , NULL ,  0xF383 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF383 , aName0xF383 , &ActiveDeviceErrorDetails0xF383 , NULL , NULL , 0x0000 },
   /* Object 0xF384 */
   {NULL , NULL ,  0xF384 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF384 , aName0xF384 , &ActiveManufacturerErrorDetails0xF384 , NULL , NULL , 0x0000 },
   /* Object 0xF385 */
   {NULL , NULL ,  0xF385 , {DEFTYPE_RECORD , 1 | (OBJCODE_REC << 8)} , asEntryDesc0xF385 , aName0xF385 , &ActiveGlobalDeviceWarningDetails0xF385 , NULL , NULL , 0x0000 },
   /* Object 0xF386 */
   {NULL , NULL ,  0xF386 , {DEFTYPE_RECORD , 1 | (OBJCODE_REC << 8)} , asEntryDesc0xF386 , aName0xF386 , &ActiveGlobalManufacturerWarningDetails0xF386 , NULL , NULL , 0x0000 },
   /* Object 0xF387 */
   {NULL , NULL ,  0xF387 , {DEFTYPE_RECORD , 1 | (OBJCODE_REC << 8)} , asEntryDesc0xF387 , aName0xF387 , &ActiveGlobalDeviceErrorDetails0xF387 , NULL , NULL , 0x0000 },
   /* Object 0xF388 */
   {NULL , NULL ,  0xF388 , {DEFTYPE_RECORD , 1 | (OBJCODE_REC << 8)} , asEntryDesc0xF388 , aName0xF388 , &ActiveGlobalManufacturerErrorDetails0xF388 , NULL , NULL , 0x0000 },
   /* Object 0xF390 */
   {NULL , NULL ,  0xF390 , {DEFTYPE_UNSIGNED8 , 0 | (OBJCODE_VAR << 8)} , &sEntryDesc0xF390 , aName0xF390 , &LatchedExceptionStatus0xF390 , NULL , NULL , 0x0000 },
   /* Object 0xF391 */
   {NULL , NULL ,  0xF391 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF391 , aName0xF391 , &LatchedDeviceWarningDetails0xF391 , NULL , NULL , 0x0000 },
   /* Object 0xF392 */
   {NULL , NULL ,  0xF392 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF392 , aName0xF392 , &LatchedManufacturerWarningDetails0xF392 , NULL , NULL , 0x0000 },
   /* Object 0xF393 */
   {NULL , NULL ,  0xF393 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF393 , aName0xF393 , &LatchedDeviceErrorDetails0xF393 , NULL , NULL , 0x0000 },
   /* Object 0xF394 */
   {NULL , NULL ,  0xF394 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF394 , aName0xF394 , &LatchedManufacturerErrorDetails0xF394 , NULL , NULL , 0x0000 },
   /* Object 0xF395 */
   {NULL , NULL ,  0xF395 , {DEFTYPE_RECORD , 1 | (OBJCODE_REC << 8)} , asEntryDesc0xF395 , aName0xF395 , &LatchedGlobalDeviceWarningDetails0xF395 , NULL , NULL , 0x0000 },
   /* Object 0xF396 */
   {NULL , NULL ,  0xF396 , {DEFTYPE_RECORD , 1 | (OBJCODE_REC << 8)} , asEntryDesc0xF396 , aName0xF396 , &LatchedGlobalManufacturerWarningDetails0xF396 , NULL , NULL , 0x0000 },
   /* Object 0xF397 */
   {NULL , NULL ,  0xF397 , {DEFTYPE_RECORD , 1 | (OBJCODE_REC << 8)} , asEntryDesc0xF397 , aName0xF397 , &LatchedGlobalDeviceErrorDetails0xF397 , NULL , NULL , 0x0000 },
   /* Object 0xF398 */
   {NULL , NULL ,  0xF398 , {DEFTYPE_RECORD , 1 | (OBJCODE_REC << 8)} , asEntryDesc0xF398 , aName0xF398 , &LatchedGlobalManufacturerErrorDetails0xF398 , NULL , NULL , 0x0000 },
   /* Object 0xF3A1 */
   {NULL , NULL ,  0xF3A1 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF3A1 , aName0xF3A1 , &DeviceWarningMask0xF3A1 , NULL , NULL , NonVolatileWordOffset_0xF3A1 },
   /* Object 0xF3A2 */
   {NULL , NULL ,  0xF3A2 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF3A2 , aName0xF3A2 , &ManufacturerWarningMask0xF3A2 , NULL , NULL , NonVolatileWordOffset_0xF3A2 },
   /* Object 0xF3A3 */
   {NULL , NULL ,  0xF3A3 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF3A3 , aName0xF3A3 , &DeviceErrorMask0xF3A3 , NULL , NULL , NonVolatileWordOffset_0xF3A3 },
   /* Object 0xF3A4 */
   {NULL , NULL ,  0xF3A4 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF3A4 , aName0xF3A4 , &ManufacturerErrorMask0xF3A4 , NULL , NULL , NonVolatileWordOffset_0xF3A4 },
   /* Object 0xF3A5 */
   {NULL , NULL ,  0xF3A5 , {DEFTYPE_RECORD , 1 | (OBJCODE_REC << 8)} , asEntryDesc0xF3A5 , aName0xF3A5 , &GlobalDeviceWarningMask0xF3A5 , NULL , NULL , NonVolatileWordOffset_0xF3A5 },
   /* Object 0xF3A6 */
   {NULL , NULL ,  0xF3A6 , {DEFTYPE_RECORD , 1 | (OBJCODE_REC << 8)} , asEntryDesc0xF3A6 , aName0xF3A6 , &GlobalManufacturerWarningMask0xF3A6 , NULL , NULL , NonVolatileWordOffset_0xF3A6 },
   /* Object 0xF3A7 */
   {NULL , NULL ,  0xF3A7 , {DEFTYPE_RECORD , 1 | (OBJCODE_REC << 8)} , asEntryDesc0xF3A7 , aName0xF3A7 , &GlobalDeviceErrorMask0xF3A7 , NULL , NULL , NonVolatileWordOffset_0xF3A7 },
   /* Object 0xF3A8 */
   {NULL , NULL ,  0xF3A8 , {DEFTYPE_RECORD , 1 | (OBJCODE_REC << 8)} , asEntryDesc0xF3A8 , aName0xF3A8 , &GlobalManufacturerErrorMask0xF3A8 , NULL , NULL , NonVolatileWordOffset_0xF3A8 },
   /* Object 0xF6F0 */
   {NULL , NULL ,  0xF6F0 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF6F0 , aName0xF6F0 , &InputLatchLocalTimestamp0xF6F0 , NULL , NULL , 0x0000 },
   /* Object 0xF6F1 */
   {NULL , NULL ,  0xF6F1 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF6F1 , aName0xF6F1 , &InputLatchESCTimestamp32Bit0xF6F1 , NULL , NULL , 0x0000 },
   /* Object 0xF6F2 */
   {NULL , NULL ,  0xF6F2 , {DEFTYPE_UNSIGNED64 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF6F2 , aName0xF6F2 , &InputLatchESCTimestamp64Bit0xF6F2 , NULL , NULL , 0x0000 },
   /* Object 0xF9F0 */
   {NULL , NULL ,  0xF9F0 , {DEFTYPE_VISIBLESTRING , 0 | (OBJCODE_VAR << 8)} , &sEntryDesc0xF9F0 , aName0xF9F0 , &ManufacturerSerialNumber0xF9F0 , NULL , NULL , 0x0000 },
   /* Object 0xF9F1 */
   {NULL , NULL ,  0xF9F1 , {DEFTYPE_UNSIGNED32 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0xF9F1 , aName0xF9F1 , &CDPFunctionalGenerationNumber0xF9F1 , NULL , NULL , 0x0000 },
   /* Object 0xF9F2 */
   {NULL , NULL ,  0xF9F2 , {DEFTYPE_UNSIGNED32 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF9F2 , aName0xF9F2 , &SDPFunctionalGenerationNumber0xF9F2 , NULL , NULL , 0x0000 },
   /* Object 0xF9F3 */
   {NULL , NULL ,  0xF9F3 , {DEFTYPE_VISIBLESTRING , 0 | (OBJCODE_VAR << 8)} , &sEntryDesc0xF9F3 , aName0xF9F3 , &VendorName0xF9F3 , NULL , NULL , 0x0000 },
   /* Object 0xF9F4 */
   {NULL , NULL ,  0xF9F4 , {DEFTYPE_VISIBLESTRING , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF9F4 , aName0xF9F4 , &SemiconductorSDPDeviceName0xF9F4 , NULL , NULL , 0x0000 },
   /* Object 0xF9F5 */
   {NULL , NULL ,  0xF9F5 , {DEFTYPE_UNSIGNED8 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF9F5 , aName0xF9F5 , &OutputIdentifier0xF9F5 , NULL , NULL , 0x0000 },
   /* Object 0xF9F6 */
   {NULL , NULL ,  0xF9F6 , {DEFTYPE_UNSIGNED32 , 0 | (OBJCODE_VAR << 8)} , &sEntryDesc0xF9F6 , aName0xF9F6 , &TimeSincePowerOn0xF9F6 , NULL , NULL , 0x0000 },
   /* Object 0xF9F7 */
   {NULL , NULL ,  0xF9F7 , {DEFTYPE_UNSIGNED32 , 0 | (OBJCODE_VAR << 8)} , &sEntryDesc0xF9F7 , aName0xF9F7 , &TotalTimePowered0xF9F7 , NULL , NULL , 0x0000 },
   /* Object 0xF9F8 */
   {NULL , NULL ,  0xF9F8 , {DEFTYPE_UNSIGNED32 , 0 | (OBJCODE_VAR << 8)} , &sEntryDesc0xF9F8 , aName0xF9F8 , &FirmwareUpdateFunctionalGenerationNumber0xF9F8 , NULL , NULL , 0x0000 },
   /* Object 0xF9F9 */
   {NULL , NULL ,  0xF9F9 , {DEFTYPE_VISIBLESTRING , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF9F9 , aName0xF9F9 , &ModuleManufacturerHardwareVersion0xF9F9 , NULL , NULL , 0x0000 },
   /* Object 0xF9FA */
   {NULL , NULL ,  0xF9FA , {DEFTYPE_VISIBLESTRING , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF9FA , aName0xF9FA , &ModuleManufacturerSoftwareVersion0xF9FA , NULL , NULL , 0x0000 },
   /* Object 0xF9FB */
   {NULL , NULL ,  0xF9FB , {DEFTYPE_VISIBLESTRING , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0xF9FB , aName0xF9FB , &ModuleManufacturerSerialNumber0xF9FB , NULL , NULL , 0x0000 },
   /* Object 0xFBF0 */
   {NULL , NULL ,  0xFBF0 , {DEFTYPE_RECORD , 3 | (OBJCODE_REC << 8)} , asEntryDesc0xFBF0 , aName0xFBF0 , &DeviceResetCommand0xFBF0 , ReadFunction0xFBFx , Write0xFBF0 , 0x0000 },
   /* Object 0xFBF1 */
   {NULL , NULL ,  0xFBF1 , {DEFTYPE_RECORD , 3 | (OBJCODE_REC << 8)} , asEntryDesc0xFBF1 , aName0xFBF1 , &ExceptionResetCommand0xFBF1 , NULL , NULL , 0x0000 },
   /* Object 0xFBF2 */
   {NULL , NULL ,  0xFBF2 , {DEFTYPE_RECORD , 3 | (OBJCODE_REC << 8)} , asEntryDesc0xFBF2 , aName0xFBF2 , &StoreParametersCommand0xFBF2 , ReadFunction0xFBFx , Write0xFBF2 , 0x0000 },
   /* Object 0xFBF3 */
   {NULL , NULL ,  0xFBF3 , {DEFTYPE_RECORD , 3 | (OBJCODE_REC << 8)} , asEntryDesc0xFBF3 , aName0xFBF3 , &CalculateChecksumCommand0xFBF3 , ReadFunction0xFBFx , NULL , 0x0000 },
   /* Object 0xFBF4 */
   {NULL , NULL ,  0xFBF4 , {DEFTYPE_RECORD , 3 | (OBJCODE_REC << 8)} , asEntryDesc0xFBF4 , aName0xFBF4 , &LoadParametersCommand0xFBF4 , ReadFunction0xFBFx , NULL , 0x0000 },
   {NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL, NULL, 0x000}};
#endif    //#ifdef _OBJD_

#undef PROTO
#endif /* APPLICATION_ECAT_RENESAS_SEMISAMPLE_H_ */


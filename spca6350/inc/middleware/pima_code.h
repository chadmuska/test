/**************************************************************************
 *
 *       Copyright (c) 2002-2016 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch
 *  Technology, Inc.. All rights are reserved by iCatch Technology, Inc..
 *  This software may only be used in accordance with the corresponding
 *  license agreement. Any unauthorized use, duplication, distribution,
 *  or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology, Inc..
 *
 *  iCatch Technology, Inc. reserves the right to modify this software
 *  without notice.
 *
 *  iCatch Technology, Inc.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
*
*  Author:
*
*******************************************************************************/
#ifndef _PIMA_CODE_H_
#define _PIMA_CODE_H_

/*******************************************************************************
*                          D A T A    T Y P E S
*******************************************************************************/
/* PIMA 15740 Operation Code */
typedef enum pimaOperationCode_enum {
	PIMA_OC_UNDEFINED               = 0x1000,
	PIMA_OC_GET_DEVICE_INFO         = 0x1001,
	PIMA_OC_OPEN_SESSION            = 0x1002,
	PIMA_OC_CLOSE_SESSION           = 0x1003,
	PIMA_OC_GET_STROAGE_IDS         = 0x1004,
	PIMA_OC_GET_STROAGE_INFO        = 0x1005,
	PIMA_OC_GET_NUM_OBJECTS         = 0x1006,
	PIMA_OC_GET_OBJECT_HANDLES      = 0x1007,
	PIMA_OC_GET_OBJECT_INFO         = 0x1008,
	PIMA_OC_GET_OBJECT              = 0x1009,
	PIMA_OC_GET_THUMB               = 0x100A,
	PIMA_OC_DELETE_OBJECT           = 0x100B,
	PIMA_OC_SEND_OBJECT_INFO        = 0x100C,
	PIMA_OC_SEND_OBJECT             = 0x100D,
	PIMA_OC_INITIATE_CAPTURE        = 0x100E,
	PIMA_OC_FORMAT_STORE            = 0x100F,
	PIMA_OC_RESET_DEVICE            = 0x1010,
	PIMA_OC_SELF_TEST               = 0x1011,
	PIMA_OC_SET_OBJECT_PROTECTION   = 0x1012,
	PIMA_OC_POWER_DOWN              = 0x1013,
	PIMA_OC_GET_DEVICE_PROP_DESC    = 0x1014,
	PIMA_OC_GET_DEVICE_PROP_VALUE   = 0x1015,
	PIMA_OC_SET_DEVICE_PROP_VALUE   = 0x1016,
	PIMA_OC_RESET_DEVICE_PROP_VALUE = 0x1017,
	PIMA_OC_TERMINATE_OPEN_CAPTURE  = 0x1018,
	PIMA_OC_MOVE_OBJECT             = 0x1019,
	PIMA_OC_COPY_OBJECT             = 0x101A,
	PIMA_OC_GET_PARTIAL_OBJECT      = 0x101B,
	PIMA_OC_INITIATE_OPEN_CAPTURE   = 0x101C,

	/* iCatch vender operation code */
	PIMA_OC_ICATCH_HOST_REQUEST		= 0x9601,
	PIMA_OC_GET_MPF_QV				= 0x9602,
	PIMA_OC_GET_DEVPROP_LIST		= 0x9614,	/* pack all information of individual 0x1014 */
	/* end */
	
	PIMA_OC_GET_OBJECT_PROPS_SUPPORTED = 0x9801,
	PIMA_OC_GET_OBJECT_PROPS_DESC      = 0x9802,
	PIMA_OC_GET_OBJECT_PROP_VALUE      = 0x9803,
	PIMA_OC_SET_OBJECT_PROP_VALUE      = 0x9804,
	PIMA_OC_GET_OBJECT_REFERANCES      = 0x9810,
	PIMA_OC_SET_OBJECT_REFERANCES      = 0x9811,
	PIMA_OC_GET_MPF_QV_				   = 0x9812,
	PIMA_OC_SKOP  					   = 0x9820,
	PIMA_OC_NUM_MAX
} pimaOperationCode_e;

typedef enum pimaEnhancedOperationCode_enum {
	PIMA_EOC_GET_OBJECT_PROP_LIST   		= 0x9805,
	PIMA_EOC_SET_OBJECT_PROP_LIST  			= 0x9806,
	PIMA_EOC_GET_INTERDEPENDEN_PROP_DESC  	= 0x9807,
	PIMA_EOC_SEND_OBJECT_PROP_LIST  		= 0x9808,
	PIMA_EOC_NUM_MAX
} pimaEnhancedOperationCode_e;

/* PIMA 15740 Response Code */
typedef enum pimaResponseCode_enum {
	PIMA_RC_UNDEFINED                     = 0x2000,
	PIMA_RC_OK                            = 0x2001,
	PIMA_RC_GENERAL_ERROR                 = 0x2002,
	PIMA_RC_SESSION_NOT_OPEN              = 0x2003,
	PIMA_RC_INVALID_TRANSACTION_ID        = 0x2004,
	PIMA_RC_OPERATION_NOT_SUPPORTED       = 0x2005,
	PIMA_RC_PARAMETER_NOT_SUPPORTED       = 0x2006,
	PIMA_RC_INCOMPLETE_TRANSFER           = 0x2007,
	PIMA_RC_INVALID_STORAGE_ID            = 0x2008,
	PIMA_RC_INVALID_OBJECT_HANDLE         = 0x2009,
	PIMA_RC_DEVICE_PROP_NOT_SUPPORTED     = 0x200A,
	PIMA_RC_INVALID_OBJECT_FORMAT_CODE    = 0x200B,
	PIMA_RC_STORE_FULL                    = 0x200C,
	PIMA_RC_OBJECT_WRITE_PROTECTED        = 0x200D,
	PIMA_RC_STORE_READ_ONLY               = 0x200E,
	PIMA_RC_ACCESS_DENIED                 = 0x200F,
	PIMA_RC_NO_THUMBNAIL_PRESENT          = 0x2010,
	PIMA_RC_SELF_TEST_FAILED              = 0x2011,
	PIMA_RC_PARTIAL_DELETION              = 0x2012,
	PIMA_RC_STORE_NOT_AVAILABLE           = 0x2013,
	PIMA_RC_SPEC_BY_FORMAT_UNSUPPORTED    = 0x2014,
	PIMA_RC_NO_VALID_OBJECT_INFO          = 0x2015,
	PIMA_RC_INVALID_CODE_FORMAT           = 0x2016,
	PIMA_RC_UNKNOWN_VENDOR_CODE           = 0x2017,
	PIMA_RC_CAPTURE_ALREADY_TERMINATED    = 0x2018,
	PIMA_RC_DEVICE_BUSY                   = 0x2019,
	PIMA_RC_INVALID_PARENT_OBJECT         = 0x201A,
	PIMA_RC_INVALID_DEVICE_PROP_FORMAT    = 0x201B,
	PIMA_RC_INVALID_DEVICE_PROP_VALUE     = 0x201C,
	PIMA_RC_INVALID_PARAMETER             = 0x201D,
	PIMA_RC_SESSION_ALREADY_OPEN          = 0x201E,
	PIMA_RC_TRANSACTION_CANCELLED         = 0x201F,
	PIMA_RC_SPEC_DESTIONATION_UNSUPPORTED = 0x2020,
	/* iCatch vendor response code */
	PIMA_RC_ICATCH_HOST_RESPONSE		  = 0xA601,
	/* end */
	
	PIMA_RC_INVALID_OBJECT_PROP_CODE 	  = 0xA801,
	PIMA_RC_INVALID_OBJECT_PROP_FORMAT 	  = 0xA802,
	PIMA_RC_INVALID_OBJECT_PROP_VALUE     = 0xA803,
	PIMA_RC_INVALID_OBJECT_REFERANCE 	  = 0xA804,
	PIMA_RC_INVALID_OBJECT_GROUPCODE 	  = 0xA805,
	PIMA_RC_INVALID_DATASHEET			  = 0xA806,
	PIMA_RC_SPEC_BYGROP_UNSUPPORTED		  = 0xA807,
	PIMA_RC_SPEC_BYDEPTH_UNSUPPORTED	  = 0xA808,
	PIMA_RC_OBJECT_TOO_LARGE			  = 0xA809,
	PIMA_RC_OBJECTPROP_NOT_SUPPORTED	  = 0xA80A,
	PIMA_RC_NUM_MAX
} pimaResponseCode_e;


/* PIMA 15740 Object Format Code */
typedef enum pimaObjectFormatCode_enum {
	PIMA_OFC_UNDEFINED   = 0x3000,
	PIMA_OFC_ASSOCIATION = 0x3001,
	PIMA_OFC_SCRIPT      = 0x3002,
	PIMA_OFC_EXECUTABLE  = 0x3003,
	PIMA_OFC_TEXT        = 0x3004,
	PIMA_OFC_HTML        = 0x3005,
	PIMA_OFC_DPOF        = 0x3006,
	PIMA_OFC_AIFF        = 0x3007,
	PIMA_OFC_WAV         = 0x3008,
	PIMA_OFC_MP3         = 0x3009,
	PIMA_OFC_AVI         = 0x300A,
	PIMA_OFC_MPEG        = 0x300B,
	PIMA_OFC_ASF         = 0x300C,
	PIMA_OFC_MOV         = 0x300D,
	PIMA_OFC_UNDEFINED1  = 0x3800,
	PIMA_OFC_EXIF_JPEG   = 0x3801,
	PIMA_OFC_TIFFEP      = 0x3802,
	PIMA_OFC_FLASHPIX    = 0x3803,
	PIMA_OFC_BMP         = 0x3804,
	PIMA_OFC_CIFF        = 0x3805,
	PIMA_OFC_UNDEFINED2  = 0x3806,
	PIMA_OFC_GIF         = 0x3807,
	PIMA_OFC_JFIF        = 0x3808,
	PIMA_OFC_PCD         = 0x3809,
	PIMA_OFC_PICT        = 0x380A,
	PIMA_OFC_PNG         = 0x380B,
	PIMA_OFC_UNDEFINED3  = 0x380C,
	PIMA_OFC_TIFF        = 0x380E,
	PIMA_OFC_JP2         = 0x380F,
	PIMA_OFC_JPX         = 0x3810,
	PIMA_OFC_UNDEFINED_FW			= 0xB802,
	PIMA_OFC_WINDOWS_IMAGEFORMAT	= 0xB881,
	PIMA_OFC_UNDEFINED_AUDIO		= 0xB900,
	PIMA_OFC_WMA		= 0xB901,
	PIMA_OFC_OGG		= 0xB902,
	PIMA_OFC_AAC		= 0xB903,
	PIMA_OFC_AUDIBLE	= 0xB904,
	PIMA_OFC_FLAC		= 0xB906,
	PIMA_OFC_UNDEFINED_VIDEO		= 0xB980,
	PIMA_OFC_WMV		= 0xB981,
	PIMA_OFC_MP4		= 0xB982,
	PIMA_OFC_MP2		= 0xB983,
	PIMA_OFC_3GP		= 0xB984,
	PIMA_OFC_UNDEFINED_COLLECTION		        = 0xBA00,
	PIMA_OFC_ABSTRACT_MULTIMEDIAALBUM			= 0xBA01,
	PIMA_OFC_ABSTRACT_IMAGEALBUM				= 0xBA02,
	PIMA_OFC_ABSTRACT_AUDIOALBUM				= 0xBA03,
	PIMA_OFC_ABSTRACT_VIDEOALBUM				= 0xBA04,
	PIMA_OFC_ABSTRACT_AUDIOVIDEOPLAYLIST		= 0xBA05,
	PIMA_OFC_ABSTRACT_CONTACTGROP				= 0xBA06,
	PIMA_OFC_ABSTRACT_MESSAGEFOLDER				= 0xBA07,
	PIMA_OFC_ABSTRACT_CHAPTERPRODUCTION			= 0xBA08,
	PIMA_OFC_ABSTRACT_AUDIOPLAYLIST				= 0xBA09,
	PIMA_OFC_ABSTRACT_VIDEOPLAYLIST				= 0xBA0A,
	PIMA_OFC_ABSTRACT_MEDIACAST					= 0xBA0B,
	PIMA_OFC_WPLPLAYLIST						= 0xBA10,
	PIMA_OFC_M3UPLAYLIST						= 0xBA11,
	PIMA_OFC_MPLPLAYLIST						= 0xBA12,
	PIMA_OFC_ASXPLAYLIST						= 0xBA13,
	PIMA_OFC_PLSPLAYLIST						= 0xBA14,
	PIMA_OFC_UNDEFINED_DOC						= 0xBA80,
	PIMA_OFC_ABSTRACT_DOC						= 0xBA81,
	PIMA_OFC_XMLDOC								= 0xBA82,
	PIMA_OFC_MICROSOFTWORDDOC					= 0xBA83,
	PIMA_OFC_MHTHTMLDOC							= 0xBA84,
	PIMA_OFC_MICROSOFT_EXCELSPRSHEET			= 0xBA85,
	PIMA_OFC_MICROSOFT_PWRPOINTSPRSHEET			= 0xBA86,
	PIMA_OFC_UNDEFINED_MESSAGE					= 0xBB00,
	PIMA_OFC_ABSTRACT_MESSAGE					= 0xBB01,
	PIMA_OFC_UNDEFINED_CONTACK					= 0xBB80,
	PIMA_OFC_ABSTRACT_CONTACK					= 0xBB81,
	PIMA_OFC_VCARD2								= 0xBB82,
	PIMA_OFC_NUM_MAX
} pimaObjectFormatCode_e;

/* PIMA 15740 Object Properties Code */
typedef enum pimaObjectPropertiesCode_enum {
	PIMA_OPC_STORAGE_ID             	= 0xDC01,
	PIMA_OPC_OBJECT_FORMAT      		= 0xDC02,
	PIMA_OPC_PROTECTION_STATUS		    = 0xDC03,
	PIMA_OPC_OBJECT_SIZE          		= 0xDC04,
	PIMA_OPC_ASSOCIATION_TYPE           = 0xDC05,
	PIMA_OPC_ASSOCIATION_DESC           = 0xDC06,
	PIMA_OPC_OBJECT_FILENAME     		= 0xDC07,
	PIMA_OPC_DATE_CREATE     			= 0xDC08,
	PIMA_OPC_DATE_MODIFIED  			= 0xDC09,
	PIMA_OPC_KEYWORDS					= 0xDC0A,
	PIMA_OPC_PARENT_OBJECT              = 0xDC0B,
	PIMA_OPC_ALLOWED_FOLFER_CONTENTS	= 0xDC0C,
	PIMA_OPC_HIDDEN    					= 0xDC0D,
	PIMA_OPC_SYSTEM_OBJECT        		= 0xDC0E,
	PIMA_OPC_PRESISTENT_UNIQUE_OBJECT_IDENTIFIER  = 0xDC41,
	PIMA_OPC_SYNCID       				= 0xDC42,
	PIMA_OPC_PROPERTY_BAG       		= 0xDC43,
	PIMA_OPC_NAME       				= 0xDC44,
	PIMA_OPC_CREATE_BY       			= 0xDC45,
	PIMA_OPC_ARTIST       				= 0xDC46,
	PIMA_OPC_DATE_AUTHORDE       		= 0xDC47,
	PIMA_OPC_DESCRIPTION       			= 0xDC48,
	PIMA_OPC_URL_REFERANCE       		= 0xDC49,
	PIMA_OPC_LANGUAGE_LOCALE       		= 0xDC4A,
	PIMA_OPC_COPYRIGHT_INFORMATION      = 0xDC4B,
	PIMA_OPC_SOURCE       				= 0xDC4C,
	PIMA_OPC_ORIGIN_LOCATION      		= 0xDC4D,
	PIMA_OPC_DATE_ADDED       			= 0xDC4E,
	PIMA_OPC_NON_CONSUMABLE       		= 0xDC4F,
	PIMA_OPC_CORRUPT_UNPLAYABLE       	= 0xDC50,
	PIMA_OPC_PRODUCER_SERIALNUMBER      = 0xDC51,
	PIMA_OPC_REPRESENTATIVE_SAMPLE_FORAMT     	= 0xDC81,
	PIMA_OPC_REPRESENTATIVE_SAMPLE_SIZE     	= 0xDC82,
	PIMA_OPC_REPRESENTATIVE_SAMPLE_HEIGHT     	= 0xDC83,
	PIMA_OPC_REPRESENTATIVE_SAMPLE_WIDTH     	= 0xDC84,
	PIMA_OPC_REPRESENTATIVE_SAMPLE_DURATION   	= 0xDC85,
	PIMA_OPC_REPRESENTATIVE_SAMPLE_DATE     	= 0xDC86,
	PIMA_OPC_WIDTH     							= 0xDC87,
	PIMA_OPC_HEIGHT    							= 0xDC88,
	PIMA_OPC_DURATION     						= 0xDC89,
	PIMA_OPC_RATING     						= 0xDC8A,
	PIMA_OPC_TRACK     							= 0xDC8B,
	PIMA_OPC_GENRE     							= 0xDC8C,
	PIMA_OPC_CREDITS     						= 0xDC8D,
	PIMA_OPC_LYRICS    							= 0xDC8E,
	PIMA_OPC_SUBSCRIPTION_CONTENT_ID    		= 0xDC8F,
	PIMA_OPC_PRODUCED_BY     					= 0xDC90,
	PIMA_OPC_USE_COUNT 							= 0xDC91,
	PIMA_OPC_SKIP_COUNT 						= 0xDC92,
	PIMA_OPC_LAST_ACCESSED						= 0xDC93,
	PIMA_OPC_PARANTAL_RATING					= 0xDC94,
	PIMA_OPC_META_GENRE							= 0xDC95,
	PIMA_OPC_COMPOSER							= 0xDC96,
	PIMA_OPC_EFFECTIVE_RATING					= 0xDC97,
	PIMA_OPC_SUBTITLE							= 0xDC98,
	PIMA_OPC_ORIGINAL_RELEASE_DATE				= 0xDC99,
	PIMA_OPC_ALBUM_NAME							= 0xDC9A,
	PIMA_OPC_ALBUM_ARTIST						= 0xDC9B,
	PIMA_OPC_MOOD								= 0xDC9C,
	PIMA_OPC_DRM_STATUS							= 0xDC9D,
	PIMA_OPC_SUB_DESCRIPTION					= 0xDC9E,
	PIMA_OPC_IS_CROPPED							= 0xDCD1,
	PIMA_OPC_COLOUR_CORRECTED					= 0xDCD2,
	PIMA_OPC_IMAGE_BIT_DEPTH					= 0xDCD3,
	PIMA_OPC_FNUMBER							= 0xDCD4,
	PIMA_OPC_EXPOSURE_TIME						= 0xDCD5,
	PIMA_OPC_EXPOSURE_INDEX						= 0xDCD6,
	PIMA_OPC_DISPLAY_NAME						= 0xDCE0,
	PIMA_OPC_BODY_TEXT							= 0xDCE1,
	PIMA_OPC_SUBJECT							= 0xDCE2,
	PIMA_OPC_PRIORITY							= 0xDCE3,
	PIMA_OPC_GIVEN_NAME							= 0xDD00,
	PIMA_OPC_MIDDLE_NAME						= 0xDD01,
	PIMA_OPC_FAMILY_NAME						= 0xDD02,
	PIMA_OPC_PREFIX								= 0xDD03,
	PIMA_OPC_SUFFIX								= 0xDD04,
	PIMA_OPC_PHONETIC_GIVEN_NAME				= 0xDD05,
	PIMA_OPC_PHONETIC_FAMILY_NAME				= 0xDD06,
	PIMA_OPC_EMAIL_PRIMARY						= 0xDD07,
	PIMA_OPC_EMAIL_PERSONAL_1					= 0xDD08,
	PIMA_OPC_EMAIL_PERSONAL_2					= 0xDD09,
	PIMA_OPC_EMAIL_BUSINESS_1					= 0xDD0A,
	PIMA_OPC_EMAIL_BUSINESS_2					= 0xDD0B,
	PIMA_OPC_EMAIL_OTHERS						= 0xDD0C,
	PIMA_OPC_PHONE_NUMBER_PRIMARY				= 0xDD0D,
	PIMA_OPC_PHONE_NUMBER_PERSONAL				= 0xDD0E,
	PIMA_OPC_PHONE_NUMBER_PERSONAL_2			= 0xDD0F,
	PIMA_OPC_PHONE_NUMBER_BUSINESS				= 0xDD10,
	PIMA_OPC_PHONE_NUMBER_BUSINESS_2			= 0xDD11,
	PIMA_OPC_PHONE_NUMBER_MOBILE				= 0xDD12,
	PIMA_OPC_PHONE_NUMBER_MOBILE_2				= 0xDD13,
	PIMA_OPC_FAX_NUMBER_PRIMARY					= 0xDD14,
	PIMA_OPC_FAX_NUMBER_PERSONAL				= 0xDD15,
	PIMA_OPC_FAX_NUMBER_BUSINESS				= 0xDD16,
	PIMA_OPC_PAGER_NUMBER						= 0xDD17,
	PIMA_OPC_PHONE_NUMBER_OTHERS				= 0xDD18,
	PIMA_OPC_PRIMARY_WEB_ADDRESS				= 0xDD19,
	PIMA_OPC_PERSONAL_WEB_ADDRESS				= 0xDD1A,
	PIMA_OPC_BUSINESS_WEB_ADDRESS				= 0xDD1B,
	PIMA_OPC_INSTANT_MESSENGER_ADRESS			= 0xDD1C,
	PIMA_OPC_INSTANT_MESSENGER_ADRESS_2			= 0xDD1D,
	PIMA_OPC_INSTANT_MESSENGER_ADRESS_3			= 0xDD1E,
	PIMA_OPC_POSTAL_ADDRESS_PERSONAL_FULL		= 0xDD1F,
	PIMA_OPC_POSTAL_ADDRESS_PERSONAL_LINE_1		= 0xDD20,
	PIMA_OPC_POSTAL_ADDRESS_PERSONAL_LINE_2		= 0xDD21,
	PIMA_OPC_POSTAL_ADDRESS_PERSONAL_CITY		= 0xDD22,
	PIMA_OPC_POSTAL_ADDRESS_PERSONAL_REGION		= 0xDD23,
	PIMA_OPC_POSTAL_ADDRESS_PERSONAL_POSTAL_CODE= 0xDD24,
	PIMA_OPC_POSTAL_ADDRESS_PERSONAL_COUNTRY	= 0xDD25,
	PIMA_OPC_POSTAL_ADDRESS_BUSINESS_FULL		= 0xDD26,
	PIMA_OPC_POSTAL_ADDRESS_BUSINESS_LINE_1		= 0xDD27,
	PIMA_OPC_POSTAL_ADDRESS_BUSINESS_LINE_2		= 0xDD28,
	PIMA_OPC_POSTAL_ADDRESS_BUSINESS_CITY		= 0xDD29,
	PIMA_OPC_POSTAL_ADDRESS_BUSINESS_REGION		= 0xDD2A,
	PIMA_OPC_POSTAL_ADDRESS_BUSINESS_POSTAL_CODE= 0xDD2B,
	PIMA_OPC_POSTAL_ADDRESS_BUSINESS_COUNTRY	= 0xDD2C,
	PIMA_OPC_POSTAL_ADDRESS_OTHER_FULL			= 0xDD2D,
	PIMA_OPC_POSTAL_ADDRESS_OTHER_LINE_1		= 0xDD2E,
	PIMA_OPC_POSTAL_ADDRESS_OTHER_LINE_2		= 0xDD2F,
	PIMA_OPC_POSTAL_ADDRESS_OTHER_CITY			= 0xDD30,
	PIMA_OPC_POSTAL_ADDRESS_OTHER_REGION		= 0xDD31,
	PIMA_OPC_POSTAL_ADDRESS_OTHER_POSTAL_CODE	= 0xDD32,
	PIMA_OPC_POSTAL_ADDRESS_OTHER_COUNTRY		= 0xDD33,
	PIMA_OPC_ORGANIZATION_NAME					= 0xDD35,
	PIMA_OPC_ROLE								= 0xDD36,
	PIMA_OPC_BRITHDATE							= 0xDD37,
	PIMA_OPC_MESSAGE_TO							= 0xDD40,
	PIMA_OPC_MESSAGE_CC							= 0xDD41,
	PIMA_OPC_MESSAGE_BCC						= 0xDD42,
	PIMA_OPC_MESSAGE_READ						= 0xDD43,
	PIMA_OPC_MESSAGE_RECEIVED_TIME				= 0xDD44,
	PIMA_OPC_MESSAGE_SEND						= 0xDD45,
	PIMA_OPC_ACTIVITY_BEGIN_TIME				= 0xDD50,
	PIMA_OPC_ACTIVITY_END_TIME					= 0xDD51,
	PIMA_OPC_ACTIVITY_LOCATION					= 0xDD52,
	PIMA_OPC_ACTIVITY_REQUIRED_ATTENDEES		= 0xDD54,
	PIMA_OPC_ACTIVITY_OPTIONAL_ATTENDEES		= 0xDD55,
	PIMA_OPC_ACTIVITY_RESOURCES					= 0xDD56,
	PIMA_OPC_ACTIVITY_ACCEPTED					= 0xDD57,
	PIMA_OPC_OWNER								= 0xDD5D,
	PIMA_OPC_EDITOR								= 0xDD5E,
	PIMA_OPC_WEBMASTER							= 0xDD5F,
	PIMA_OPC_URL_SOURCE							= 0xDD60,
	PIMA_OPC_URL_DESTINATION					= 0xDD61,
	PIMA_OPC_TIME_BOOKMARK						= 0xDD62,
	PIMA_OPC_OBJECT_BOOKMARK					= 0xDD63,
	PIMA_OPC_BYTE_BOOKMARK						= 0xDD64,
	PIMA_OPC_LAST_BUILD_DATE					= 0xDD70,
	PIMA_OPC_TIME_TO_LIVE						= 0xDD71,
	PIMA_OPC_MEDIA_GUID							= 0xDD72,
	PIMA_OPC_TOTAL_BITRATE						= 0xDE91,
	PIMA_OPC_BITRATE_TYPE						= 0xDE92,
	PIMA_OPC_SAMPLE_RATE						= 0xDE93,
	PIMA_OPC_NUMBER_OF_CHANNELS					= 0xDE94,
	PIMA_OPC_AUDIO_BITDEPTH						= 0xDE95,
	PIMA_OPC_SCAN_TYPE							= 0xDE97,
	PIMA_OPC_AUDIO_WAVE_CODEC					= 0xDE99,
	PIMA_OPC_AUDIO_BITRATE						= 0xDE9A,
	PIMA_OPC_VIDEO_FOURCC_CODEC					= 0xDE9B,
	PIMA_OPC_VIDEO_BITRATE						= 0xDE9C,
	PIMA_OPC_FRAMES_PER_THOUSAND_SECONDS		= 0xDE9D,
	PIMA_OPC_KEYFRAME_DISTANCE					= 0xDE9E,
	PIMA_OPC_BUFFER_SIZE						= 0xDE9F,
	PIMA_OPC_ENCODING_QUALITY					= 0xDEA0,
	PIMA_OPC_ENCODING_PROFILE					= 0xDEA1,
	PIMA_OPC_NUM_MAX
} pimaObjectPropertiesCode_e;

/* PIMA 15740 Event Format Code */
typedef enum pimaEventCode_enum {
	PIMA_EC_UNDEFINED               = 0x4000,
	PIMA_EC_CANCEL_TRANSACTION      = 0x4001,
	PIMA_EC_OBJECT_ADDED            = 0x4002,
	PIMA_EC_OBJECT_REMOVED          = 0x4003,
	PIMA_EC_STORE_ADDED             = 0x4004,
	PIMA_EC_STORE_REMOVED           = 0x4005,
	PIMA_EC_DEVICE_PROP_CHANGED     = 0x4006,
	PIMA_EC_OBJECT_INFO_CHANGED     = 0x4007,
	PIMA_EC_DEVICE_INFO_CHANGED     = 0x4008,
	PIMA_EC_REQUEST_OBJECT_TRANSFER = 0x4009,
	PIMA_EC_STORE_FULL              = 0x400A,
	PIMA_EC_DEVICE_RESET            = 0x400B,
	PIMA_EC_STORAGE_INFO_CHANGED    = 0x400C,
	PIMA_EC_CAPTURE_COMPLETE        = 0x400D,
	PIMA_EC_UNREPORTED_STATUS       = 0x400E,
	/* iCatch vendor event code */
	PIMA_EC_ICATCH_HOST_EVENT		= 0xC601,
	/* end */
	PIMA_EC_NUM_MAX
} pimaEventCode_e;

/* PIMA 15740 Storage Type */
typedef enum pimaStorageType_enum {
	PIMA_ST_UNDEFINED     = 0x0000,
	PIMA_ST_FIXED_ROM     = 0x0001,
	PIMA_ST_REMOVALBE_ROM = 0x0002,
	PIMA_ST_FIXED_RAM     = 0x0003,
	PIMA_ST_REMOVALBE_RAM = 0x0004,
	PIMA_ST_NUM_MAX
} pimaStorageType_e;

/* PIMA 15740 Device Property Code */
typedef enum pimaDevicePropertyCode_enum {
	PIMA_DPC_UNDEFINED               	= 0x5000,
	PIMA_DPC_BATTERY_LEVEL      		= 0x5001,
	PIMA_DPC_FUNCTIONAL_MODE            = 0x5002,
	PIMA_DPC_IMAGE_SIZE          		= 0x5003,
	PIMA_DPC_COMPRESSION_SETTING    	= 0x5004,
	PIMA_DPC_WHILE_BALANCE           	= 0x5005,
	PIMA_DPC_RGB_GAIN					= 0x5006,
	PIMA_DPC_F_NUMBER    				= 0x5007,
	PIMA_DPC_FOCAL_LENGTH     			= 0x5008,
	PIMA_DPC_FOCUS_DISTANCE 			= 0x5009,
	PIMA_DPC_FOCUS_MODE             	= 0x500A,
	PIMA_DPC_EXPOSURE_METERING_MODE		= 0x500B,
	PIMA_DPC_FLASH_MODE    				= 0x500C,
	PIMA_DPC_EXPOSURE_TIME        		= 0x500D,
	PIMA_DPC_EXPOSURE_PROGRAM_MODE      = 0x500E,
	PIMA_DPC_EXPOSURE_INDEX      		= 0x500F,
	PIMA_DPC_EXPOSURE_BAIS_COMPENSATION = 0x5010,
	PIMA_DPC_DATE_TIME      			= 0x5011,
	PIMA_DPC_CAPTURE_DELAY      		= 0x5012,
	PIMA_DPC_STILL_CAPTURE_MODE      	= 0x5013,
	PIMA_DPC_CONTRAST      				= 0x5014,
	PIMA_DPC_SHARPNESS      			= 0x5015,
	PIMA_DPC_DIGITAL_ZOOM      			= 0x5016,
	PIMA_DPC_EFFECT_MODE      			= 0x5017,
	PIMA_DPC_BURST_NUMBER      			= 0x5018,
	PIMA_DPC_BURST_INTERVAL      		= 0x5019,
	PIMA_DPC_TIMELAPSE_NUMBER    		= 0x501A,
	PIMA_DPC_TIMELAPSE_INTERVAL   		= 0x501B,
	PIMA_DPC_FOCUS_METERING_MODE   		= 0x501C,
	PIMA_DPC_UPLOAD_URL   				= 0x501D,
	PIMA_DPC_ARTIST   					= 0x501E,
	PIMA_DPC_COPYRIGHT_INFO 			= 0x501F,
	/* FF vender property */
	PIMA_DPC_FPCSH_PROP_LIST			= 0xD212,
	PIMA_DPC_SEND_REQ_OBJ_HANDLE		= 0xD220,
	/* FF end */
	PIMA_DPC_USEDEVICESTAGE_FLAG 		= 0xD303,
	PIMA_DPC_SYNCHRONIZATION_PARTNER 	= 0xD401,
	PIMA_DPC_DEVICE_FRIENDLY_NAME 		= 0xD402,
	PIMA_DPC_VOLUME 					= 0xD403,
	PIMA_DPC_SUPPORTED_FORMATSORDERED 	= 0xD404,
	PIMA_DPC_DEVICEICON 				= 0xD405,
	PIMA_DPC_PLAYBACK_RATE 				= 0xD410,
	PIMA_DPC_PLAYBACK_OBJECT 			= 0xD411,
	PIMA_DPC_PLAYBACK_CONTAINER			= 0xD412,
	PIMA_DPC_SESSION_INITIATOR_VERSION_INFO	= 0xD406,
	PIMA_DPC_PERCEIVED_DEVICE_TYPE		= 0xD407,

	/* iCatch vendor device property code */
	PIMA_DPC_ICATCH_VARIABLE			= 0xD601,
	PIMA_DPC_SCENE_MODE					= 0xD602,
	PIMA_DPC_OPTIC_RATIO				= 0xD603,
	PIMA_DPC_DSC_OP_MODE				= 0xD604,
	PIMA_DPC_VIDEO_SIZE					= 0xD605,
	PIMA_DPC_BAND_SELECTION				= 0xD606,
	PIMA_DPC_DATESTAMP					= 0xD607,
	PIMA_DPC_FREE_IN_IMAGE				= 0xD608,
	PIMA_DPC_UI_SUPPORT_WIFI_OP_FUNC	= 0xD609,
	PIMA_DPC_REMAIN_CAPACITY_IN_SECS 	= 0xD610,
	PIMA_DPC_VIDEO_TIMELAPSE_INTERVAL	= 0xD611,
	PIMA_DPC_VIDEO_TIMELAPSE_LENGTH		= 0xD612,
	PIMA_DPC_CUSTOMER_NAME 				= 0XD613,
	PIMA_DPC_CAR_MODE					= 0XD614,
	PIMA_DPC_SLOW_MOTION				= 0XD615,
	PIMA_DPC_MAX_ZOOM_RATIO				= 0xD616,
	/* end */
	
	PIMA_DPC_CUSTOME_UINT32_FIRST		= 0xD700,
	PIMA_DPC_CUSTOME_UINT32_LAST		= 0xD7FF,

	PIMA_DPC_CUSTOME_STR_FIRST			= 0xD800,
	PIMA_DPC_CUSTOME_STR_LAST			= 0xD83F,

	PIMA_DPC_NUM_MAX
} pimaDevicePropertyCode_e;


/* PIMA 15740 Storage Type */
typedef enum pimaAssociationType_enum {
	PIMA_AT_UNDEFINED            = 0x0000,
	PIMA_AT_GENERIC_FOLDER       = 0x0001,
	PIMA_AT_ALBUM                = 0x0002,
	PIMA_AT_TIME_SEQUENCE        = 0x0003,
	PIMA_AT_HORIZONTAL_PANORAMIC = 0x0004,
	PIMA_AT_VERTICAL_PANORAMIC   = 0x0005,
	PIMA_AT_2D_PANORAMIC         = 0x0006,
	PIMA_AT_ANCILLARY_DATA       = 0x0007,
	PIMA_AT_NUM_MAX
} pimaAssociationType_e;

/* PIMA 15740 Filesystem Type */
typedef enum pimaFilesystemType_enum {
	PIMA_FS_UNDEFINED            = 0x0000,
	PIMA_FS_GENERIC_FLAT         = 0x0001,
	PIMA_FS_GENERIC_HIERARCHICAL = 0x0002,
	PIMA_FS_DCF                  = 0x0003,
	PIMA_FS_NUM_MAX
} pimaFilesystemType_e;

/* PIMA 15740 Access Capability Values */
typedef enum pimaAccessCapabilityValue_enum {
	PIMA_AC_READ_WRITE                        = 0x0000,
	PIMA_AC_READ_ONLY_WITHOUT_OBJECT_DELETION = 0x0001,
	PIMA_AC_READ_ONLY_WITH_OBJECT_DELETION    = 0x0002,
	PIMA_AC_NUM_MAX
} pimaAccessCapabilityValue_e;

typedef enum {
	PIMA_CAPTURE_TRIG_STILL,
	PIMA_CAPTURE_TRIG_VIDEO
} pimaTrigCaptureType_e;

typedef enum {
	PIMA_MTP_BK_PAUSE,
	PIMA_MTP_BK_AUTO,
	PIMA_MTP_BK_MANUAL,
	PIMA_MTP_BK_TYPE_MASK	= 0x0000ffff,
	PIMA_MTP_BK_AUTO_QUICK	= 0x80000000,
} mtpBkUpdateType_e;


/*
 * Get/Set
 */
#define	PTP_PROPGETSET_GETONLY	   0x00
#define	PTP_PROPGETSET_GETSET	   0x01
#define	PTP_PROPGETSET_GETSET2	   0x02


typedef	enum PropFormFlag
{
  None					= 0x00,
  RangeForm				= 0x01,
  EnumForm				= 0x02,
  DateTimeForm			= 0x03,
  ItemListForm			= 0x04,
  RegularExpressionForm	= 0x05,
  ByteArray				= 0x06,
  LongStringForm		= 0xFF
} ePropFormFlag;

enum GroupCodeFlag
{
  GroupCodeNone		   = 0x00000000,
  GroupCodeSyncProps   = 0x00000001,
  GroupCodeUIProps	   = 0x00000002,
  GroupCodeObjInfo	   = 0x00000004,
  GroupCodeOftenUsed   = 0x00000008,
  GroupCodeSupplemental= 0x00000100,
  GroupCodeUnknownProp = 0x00010000,
  GroupCodeSlow		   = 0x00FF0000,
  GroupCodeAll		   = 0xFFFFFFFF
};

#define MTP_DEV_TYPE_GENERIC				0x00000000
#define MTP_DEV_TYPE_STILL_IMAGE			0x00000001	/*Still Image/Video Camera*/
#define MTP_DEV_TYPE_MEDIA_AV_PLAYVER 		0x00000002	/*Media (Audio/Video) Player*/
#define MTP_DEV_TYPE_MOBILE_HANDSET			0x00000003	/*Mobile Handset*/
#define MTP_DEV_TYPE_DIGITAL_VIDEO_CAMERA	0x00000004	/*Digital Video Camera*/
#define MTP_DEV_TYPE_PERSONAL_INFO_MANAGER  0x00000005	/*Personal Information Manager/Personal Digital Assistant*/
#define MTP_DEV_TYPE_AUDIO_RECODER			0x00000006	/*Audio Recorder*/

/*Scan Type*/
#define MTP_SCANTYPE_UNUSED 						0x0000 	/*Unused*/
#define MTP_SCANTYPE_PROGRESSIVE 					0x0001 /*Progressive*/
#define MTP_SCANTYPE_FIELDINTERLEAVEDUPPERFIRST 	0x0002 /*FieldInterleavedUpperFirst*/
#define MTP_SCANTYPE_FIELDINTERLEAVEDLOWERFIRST 	0x0003 /*FieldInterleavedLowerFirst*/
#define MTP_SCANTYPE_FIELDSINGLEUPPERFIRST 			0x0004 /*FieldSingleUpperFirst*/
#define MTP_SCANTYPE_FIELDSINGLELOWERFIRST 			0x0005 /*FieldSingleLowerFirst*/
#define MTP_SCANTYPE_MIXEDINTERLACE 				0x0006 /*MixedInterlace*/
#define MTP_SCANTYPE_MIXEDINTERLACEANDPROGRESSIVE 	0x0007 /*MixedInterlaceAndProgressive*/

/* Number of Channels*/
#define MTP_NUMBEROFCHANNELS_UNUSED 		        0x0000 /* Unused*/
#define MTP_NUMBEROFCHANNELS_MONO 			        0x0001 /* Mono (1 channel)*/
#define MTP_NUMBEROFCHANNELS_STEREO 		        	0x0002 /* Stereo (2 channels)*/


/*
 * PTP Data	type codes.
 */
#define	PTP_DATATYPE_UNDEFINED	 0x0000
#define	PTP_DATATYPE_INT8		 0x0001
#define	PTP_DATATYPE_UINT8		 0x0002
#define	PTP_DATATYPE_INT16		 0x0003
#define	PTP_DATATYPE_UINT16		 0x0004
#define	PTP_DATATYPE_INT32		 0x0005
#define	PTP_DATATYPE_UINT32		 0x0006
#define	PTP_DATATYPE_INT64		 0x0007
#define	PTP_DATATYPE_UINT64		 0x0008
#define	PTP_DATATYPE_INT128		 0x0009
#define	PTP_DATATYPE_UINT128	 0x000A
#define	PTP_DATATYPE_AINT8		 0x4001
#define	PTP_DATATYPE_AUINT8		 0x4002
#define	PTP_DATATYPE_AINT16		 0x4003
#define	PTP_DATATYPE_AUINT16	 0x4004
#define	PTP_DATATYPE_AINT32		 0x4005
#define	PTP_DATATYPE_AUINT32	 0x4006
#define	PTP_DATATYPE_AINT64		 0x4007
#define	PTP_DATATYPE_AUINT64	 0x4008
#define	PTP_DATATYPE_AINT128	 0x4009
#define	PTP_DATATYPE_AUINT128	 0x400A
#define	PTP_DATATYPE_STRING		 0xFFFF
#define	PTP_DATATYPE_ARRAYMASK	 0x4FF0
#define	PTP_DATATYPE_ARRAY		 0x4000
#define	PTP_DATATYPE_VALUEMASK	 0xFFF0
#define	PTP_DATATYPE_VALUE		 0x0000

#define PTP_PROTECTIONSTATUS_NONE          0x0000
#define PTP_PROTECTIONSTATUS_READONLY      0x0001

#define MTP_PROTECTIONSTATUS_NOPROTECTION       0x0000
#define MTP_PROTECTIONSTATUS_READONLY           0x0001
#define MTP_PROTECTIONSTATUS_READONLY_PERMANENT 0x0002
#define MTP_PROTECTIONSTATUS_ABSTRACT_OBJECT    0x0003

#define PTP_OBJECTHANDLE_ALL          		0xFFFFFFFF
#define PTP_OBJECTHANDLE_UNDEFINED    	0x00000000
#define PTP_PROPCODE_UNDEFINED        	0x00000000
#define PTP_PROPCODE_ALL              		0xFFFFFFFF

#define PTP_DEPTH_UNDEFINED        		0x00000000
#define PTP_DEPTH_ALL              			0xFFFFFFFF

#endif /* _PIMA_CODE_H_ */



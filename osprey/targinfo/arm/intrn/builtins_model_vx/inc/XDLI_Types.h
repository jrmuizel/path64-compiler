/*{{{  Banner*/
/*-- Banner ------------------------------------------------*\
*
* Copyright: STMicroelectronics 2003/2004
* Group: DMD-FRANCE, Grenoble
* Author(s):	Olivier-Andre GIRAUD,
*		Jean-Francois MONESTIER,
*		Claude DELNONDEDIEU
* Title: XDLI_Types.h
* System: XDLI
*
\*----------------------------------------------------------*/
/*}}}*/

/*{{{  Descriptor*/
/*-- Descriptor --------------------------------------------*\
*
* BILBO EFU Dynamic Library Interface typedefs
*
\*----------------------------------------------------------*/
/*}}}*/

#ifndef XDLI_Types_h
#define XDLI_Types_h

#ifdef __cplusplus
extern "C" {
#endif

/*{{{  Some constants */

	/*{{{  Interface version identifer (YYMMDDx) */
	
		#define	XDLI_VERSION	    0x060329A
		#define XDLI_VERSION_STRING "XDLI v.060329A (Mar. 29, 2006)"
	
	/*}}}*/

	/*{{{  Classic C style TRUE and FALSE constants */
	
		#ifndef TRUE
		#define TRUE (0==0)
		#endif
	
		#ifndef FALSE
		#define FALSE (0!=0)
		#endif
	
	/*}}}*/

	/*{{{  Decoding-related defines */
	
		/* Encoding of EFU # */
		#define XDLI_DECODE_EXT_MASK	    0x00000007
	
		/* Opcodes of EFU instructions (bits 4 & 3 of opcode) */
		#define XDLI_DECODE_SOP_XRi_XOP     0
		#define XDLI_DECODE_SOP_XRiW1_XW1   1
		#define XDLI_DECODE_SOP_LX	    2
		#define XDLI_DECODE_SOP_SX	    3
		
	/*}}}*/

	/*{{{  Some masks, matching the EFU hardware interface */
	
		#define XDLI_EXEC_ERRCODE_MASK	    0x0000003F
	
	/*}}}*/

/*}}}*/

/*{{{  Data representation */

	/*{{{  Solaris types */
	#ifdef	__sun
	
		typedef unsigned char           XDLI_u8T;
		typedef   signed char           XDLI_s8T;
		typedef unsigned int		XDLI_u32T;
		typedef   signed int		XDLI_s32T;
		typedef unsigned long long int	XDLI_u64T;
		typedef   signed long long int	XDLI_s64T;
		typedef	  signed int		XDLI_BoolT;
	
	#endif
	/*}}}*/

	/*{{{  Linux types */
	#ifdef	__linux
	
		typedef unsigned char           XDLI_u8T;
		typedef   signed char           XDLI_s8T;
		typedef unsigned int		XDLI_u32T;
		typedef   signed int		XDLI_s32T;
		typedef unsigned long long int	XDLI_u64T;
		typedef   signed long long int	XDLI_s64T;
		typedef	  signed int		XDLI_BoolT;
	
	#endif
	/*}}}*/

	/*{{{  Windows (32 bits) types */
	#ifdef	WIN32
	
		#define XDLI_s8T     signed __int8
		#define XDLI_s32T    signed __int32
		#define XDLI_s64T    signed __int64
		#define XDLI_u8T   unsigned __int8
		#define XDLI_u32T  unsigned __int32
		#define XDLI_u64T  unsigned __int64
		#define XDLI_BoolT   signed __int32
	
	#endif
	/*}}}*/

/*}}}*/

/*{{{  XDLI_SIMU_ReturnT*/

	enum XDLI_SIMU_Return {
		XDLI_SIMU_OK,
		XDLI_SIMU_ERR_NOTSUPPORTED,
		XDLI_SIMU_ERR_NONFATAL,
		XDLI_SIMU_ERR_FATAL
	};

	typedef enum XDLI_SIMU_Return XDLI_SIMU_ReturnT;

/*}}}*/

/*{{{  XDLI_DEBUG_ReturnT*/

	enum XDLI_DEBUG_Return {
		XDLI_DEBUG_OK,
		XDLI_DEBUG_ERR_NOTSUPPORTED,
		XDLI_DEBUG_ERR_NONFATAL,
		XDLI_DEBUG_ERR_FATAL
	};

	typedef enum XDLI_DEBUG_Return XDLI_DEBUG_ReturnT;

/*}}}*/

/*{{{  XDLI_StatusT*/

	enum XDLI_Status {
		XDLI_STOPPED,
		XDLI_RUNNING
	};

	typedef enum XDLI_Status XDLI_StatusT;

/*}}}*/

/*{{{  XDLI_InstrT*/

	enum XDLI_Instr {
		XDLI_XR2,
		XDLI_XR1,
		XDLI_XOP,
		XDLI_XR2W1,
		XDLI_XR1W1,
		XDLI_XW1,
		XDLI_LX,
		XDLI_SX
	};

	typedef enum XDLI_Instr XDLI_InstrT;

/*}}}*/

/*{{{  XDLI_ObjSizeT*/

	enum XDLI_ObjSize {
		XDLI_INVALIDSIZE    = 0,
		XDLI_8BITS	    = 1,
		XDLI_16BITS	    = 2,
		XDLI_32BITS	    = 3,
		XDLI_64BITS	    = 4,
		XDLI_128BITS	    = 5,
		XDLI_256BITS	    = 6,
		XDLI_512BITS	    = 7
	};

	typedef enum XDLI_ObjSize XDLI_ObjSizeT;

	// matches the h/w
	enum XDLI_ExecSize {
		XDLI_EXECSIZE_64BITS	    = 0,
		XDLI_EXECSIZE_128BITS	    = 1,
		XDLI_EXECSIZE_256BITS	    = 2,
		XDLI_EXECSIZE_512BITS	    = 3
	};

	typedef enum XDLI_ExecSize XDLI_ExecSizeT;

/*}}}*/

/*{{{ XDLI_MemoryT*/
	/* Up to 512 bits, cf. XDLI_ObjSizeT */
	typedef XDLI_u32T XDLI_MemoryT[16];

	#define XDLI_MEMORY_INITIALIZER {   \
		0,			    \
		0,			    \
		0,			    \
		0,			    \
		0,			    \
		0,			    \
		0,			    \
		0,			    \
		0,			    \
		0,			    \
		0,			    \
		0,			    \
		0,			    \
		0,			    \
		0,			    \
		0			    \
	}

	#define XDLI_MEMORY_WORD_COUNT	    (sizeof(XDLI_MemoryT) / sizeof(XDLI_u32T))
/*}}}*/

/*{{{  XDLI_TrapT*/

	enum XDLI_Trap {
		XDLI_NOTRAP	= 0,
		XDLI_TRAP	= 1
	};

	typedef enum XDLI_Trap XDLI_TrapT;

/*}}}*/

/*{{{  XDLI_FunctionalExecuteParamT*/

	struct XDLI_FunctionalExecuteParam {
		XDLI_u32T   decod_instr;
		XDLI_u32T   decod_ctxid;
		XDLI_u32T   exec_opa;
		XDLI_u32T   exec_opb;
		XDLI_u32T   exec_guards;
		XDLI_u32T   exec_result;
		XDLI_u32T   exec_trap;
		XDLI_u32T   exec_errcode;
	};

	typedef struct XDLI_FunctionalExecuteParam XDLI_FunctionalExecuteParamT;

	#define XDLI_FUNCTIONALEXECUTEPARAM_INITIALIZER {   \
		0,	/* decod_instr		*/	    \
		0,	/* decod_ctxid		*/	    \
		0,	/* exec_opa		*/	    \
		0,	/* exec_opb		*/	    \
		0,	/* exec_guards		*/	    \
		0,	/* exec_result		*/	    \
		0,	/* exec_trap		*/	    \
		0	/* exec_errcode		*/	    \
	}

/*}}}*/

/*{{{  XDLI_CycleAccurateExecuteParamT*/

	struct XDLI_CycleAccurateExecuteParam {
		XDLI_u32T	busy;
		XDLI_u32T	decod_instr;
		XDLI_u32T	decod_valid;
		XDLI_u32T	decod_ctxid;
		XDLI_u32T	decod_multicycle;
		XDLI_u32T	decod_stall;
		XDLI_u32T	decod_cancel;
		XDLI_u32T	exec_opa;
		XDLI_u32T	exec_opb;
		XDLI_u32T	exec_guards;
		XDLI_u32T	exec_stall;
		XDLI_u32T	exec_cancel;
		XDLI_u32T	exec_result;
		XDLI_MemoryT	store_data;
		XDLI_u32T	exec_size;
		XDLI_u32T	exec_trap;
		XDLI_u32T	exec_errcode;
		XDLI_u32T	stall_execute;
		XDLI_u32T	memory_result;
		XDLI_u32T	stall_memory;
		XDLI_u32T	memory_stall;
		XDLI_u32T	memory_cancel;
		XDLI_MemoryT	load_data;
	};

	typedef struct XDLI_CycleAccurateExecuteParam XDLI_CycleAccurateExecuteParamT;

	#define XDLI_CYCLEACCURATEEXECUTEPARAM_INITIALIZER {	    \
		0,			    /* busy		*/  \
		0,			    /* decod_instr	*/  \
		0,			    /* decod_valid	*/  \
		0,			    /* decod_ctxid	*/  \
		0,			    /* decod_multicycle	*/  \
		0,			    /* decod_stall	*/  \
		0,			    /* decod_cancel	*/  \
		0,			    /* exec_opa		*/  \
		0,			    /* exec_opb		*/  \
		0,			    /* exec_guards	*/  \
		0,			    /* exec_stall	*/  \
		0,			    /* exec_cancel	*/  \
		0,			    /* exec_result	*/  \
		XDLI_MEMORY_INITIALIZER,    /* store_data 	*/  \
		0,			    /* exec_size	*/  \
		0,			    /* exec_trap	*/  \
		0,			    /* exec_errcode	*/  \
		0,			    /* stall_execute	*/  \
		0,			    /* memory_result	*/  \
		0,			    /* stall_memory	*/  \
		0,			    /* memory_stall	*/  \
		0,			    /* memory_cancel	*/  \
		XDLI_MEMORY_INITIALIZER	    /* load_data	*/  \
	}

/*}}}*/

/*{{{  Some function typedefs */

	/*{{{  Callbacks provided by caller */

		/*{{{ XDLI_PrintCallBackStructT*/
		enum XDLI_Severity {
		    XDLI_SEVERITY_NONE		= 0,
		    XDLI_SEVERITY_INFO		= 1,
		    XDLI_SEVERITY_WARNING	= 2,
		    XDLI_SEVERITY_ERROR		= 3
		};
		typedef enum XDLI_Severity XDLI_SeverityT;

		typedef	void		XDLI_PrintCallBackT(void *client, XDLI_SeverityT severity, char *message);

		struct XDLI_PrintCallBackStruct {
		    void                * caller;
		    XDLI_PrintCallBackT * function;
		};
		typedef struct XDLI_PrintCallBackStruct XDLI_PrintCallBackStructT;
		/*}}}*/

		/*{{{ XDLI_GetStatusCallBackStructT*/
		typedef	XDLI_StatusT	XDLI_GetStatusCallBackT(void *client);

		struct XDLI_GetStatusCallBackStruct {
		    void                    * caller;
		    XDLI_GetStatusCallBackT * function;
		};
		typedef struct XDLI_GetStatusCallBackStruct XDLI_GetStatusCallBackStructT;
		/*}}}*/

		/*{{{ XDLI_StoreCallBackStructT*/
		typedef	void		XDLI_StoreCallBackT( void *client, XDLI_ObjSizeT size, XDLI_MemoryT data, XDLI_TrapT *trap );

		struct XDLI_StoreCallBackStruct {
		    void                * caller;
		    XDLI_StoreCallBackT * function;
		};
		typedef struct XDLI_StoreCallBackStruct XDLI_StoreCallBackStructT;
		/*}}}*/

		/*{{{ XDLI_LoadCallBackStructT*/
		typedef	void		XDLI_LoadCallBackT( void *client, XDLI_ObjSizeT size, XDLI_MemoryT data, XDLI_TrapT *trap );

		struct XDLI_LoadCallBackStruct {
		    void               * caller;
		    XDLI_LoadCallBackT * function;
		};
		typedef struct XDLI_LoadCallBackStruct XDLI_LoadCallBackStructT;
		/*}}}*/

		struct XDLI_CallBacks {
		    XDLI_PrintCallBackStructT	    printcallback;
		    XDLI_GetStatusCallBackStructT   getstatuscallback;
		    XDLI_StoreCallBackStructT	    storecallback;
		    XDLI_LoadCallBackStructT	    loadcallback;
		};
		typedef struct XDLI_CallBacks XDLI_CallBacksT;
	
	/*}}}*/

	/*{{{  Optional entries for all implementations */

		enum XDLI_RegisterSize {
			XDLI_REGISTERSIZE_8BITS  = 8,
			XDLI_REGISTERSIZE_16BITS = 16,
			XDLI_REGISTERSIZE_24BITS = 24,
			XDLI_REGISTERSIZE_32BITS = 32,
			XDLI_REGISTERSIZE_40BITS = 40,
			XDLI_REGISTERSIZE_48BITS = 48,
			XDLI_REGISTERSIZE_56BITS = 56,
			XDLI_REGISTERSIZE_64BITS = 64
		};
		typedef enum XDLI_RegisterSize XDLI_RegisterSizeT;

		enum XDLI_RegisterType {
			XDLI_REGISTERTYPE_REGULAR,
			XDLI_REGISTERTYPE_FLOAT
		};
		typedef enum XDLI_RegisterType XDLI_RegisterTypeT;

		enum XDLI_RegisterAccess {
			XDLI_REGISTERACCESS_NONE    = 0,
			XDLI_REGISTERACCESS_EXECUTE = 1,
			XDLI_REGISTERACCESS_READ    = 2,
			XDLI_REGISTERACCESS_WRITE   = 4
		};
		typedef enum XDLI_RegisterAccess XDLI_RegisterAccessT;

		#define XDLI_CONTEXT_ALL 0xFFFFFFFF

		struct XDLI_RegisterDescription {
		    char                 * name;
		    XDLI_u32T            context;
		    XDLI_RegisterSizeT   size;
		    XDLI_RegisterTypeT   type;
		    XDLI_RegisterAccessT access;
		};
		typedef struct XDLI_RegisterDescription XDLI_RegisterDescriptionT;

		typedef	XDLI_DEBUG_ReturnT	XDLI_DirectCommandT(void *ext, char *command);

		typedef XDLI_DEBUG_ReturnT	XDLI_GetRegisterNumberT(void *ext, XDLI_u32T *id);

		typedef XDLI_DEBUG_ReturnT	XDLI_GetRegisterDescriptionT(void *ext, XDLI_u32T id, XDLI_RegisterDescriptionT *description);
		
		typedef XDLI_DEBUG_ReturnT	XDLI_ReadRegisterT(void *ext, char *register_name, XDLI_u8T value[]);
		
		typedef XDLI_DEBUG_ReturnT	XDLI_WriteRegisterT(void *ext, char *register_name, XDLI_u8T value[]);
		
		typedef XDLI_DEBUG_ReturnT	XDLI_CollectRegistersT(void *ext, char **read, char **written);
		
		typedef char		      * XDLI_GetMessageT(void *ext);
		
	/*}}}*/

	/*{{{  Optional entries for functional implementation*/
	
		typedef char * XDLI_TraceAsmT(
					void *ext,
					XDLI_u32T pc,
					XDLI_u32T opcode,
					char * Gll_name,
					char * Glh_name,
					char * Ghl_name,
					char * Ghh_name,
					char * Rn_name,
					char * Rp_name,
					char * Rm_name,
					char * AM_name
				);
	
	/*}}}*/

	/*{{{  Mandatory entries for functional implementation */
	
		struct XDLI_TraceValues {
			char * dst;
			char * src;
		};

		typedef struct XDLI_TraceValues XDLI_TraceValuesT;

		typedef	XDLI_SIMU_ReturnT XDLI_FunctionalExecuteT(
						void *ext,
						XDLI_u32T pc,
						XDLI_FunctionalExecuteParamT * p,
						XDLI_TraceValuesT * t
					);
	
	/*}}}*/

	/*{{{  Mandatory entries for cycle accurate implementation */
	
		typedef	XDLI_SIMU_ReturnT XDLI_CycleAccurateExecuteT(
						void *ext,
						XDLI_u32T pc,
						XDLI_CycleAccurateExecuteParamT * p,
						XDLI_TraceValuesT * t
					);
	
	/*}}}*/

	/*{{{  Basic mandatory entries to manage EFU extensions */
	
		typedef	void     		* XDLI_NewExtensionT(
						XDLI_u32T extid, 
						XDLI_CallBacksT *callbacks, 
						char *config
					);
					
		typedef	XDLI_DEBUG_ReturnT	XDLI_DeleteExtensionT( void *ext );
		
		typedef	XDLI_DEBUG_ReturnT	XDLI_ResetExtensionT( void *ext );
		
	/*}}}*/

	/*{{{  Basic mandatory entries to manage library connection*/

		enum XDLI_Decoding {
			XDLI_DECODING_NONE = 0,
			XDLI_DECODING_X0   = 1,
			XDLI_DECODING_X1   = 2,
			XDLI_DECODING_X2   = 4,
			XDLI_DECODING_X3   = 8,
			XDLI_DECODING_X4   = 16,
			XDLI_DECODING_X5   = 32,
			XDLI_DECODING_X6   = 64,
			XDLI_DECODING_X7   = 128
		};
		typedef enum XDLI_Decoding XDLI_DecodingT;
	
		struct XDLI_Features;
	
		typedef struct XDLI_Features	* XDLI_ConnectT(void *caller);
		typedef XDLI_DEBUG_ReturnT	XDLI_DisconnectT(struct XDLI_Features *features);
	
		struct XDLI_Features {
			XDLI_u32T	XDLI;
			void		* caller;
			char		* name;
			char		* version;
			char		* abstract;
			XDLI_DecodingT	decoding;
			XDLI_ConnectT		* XDLI_Connect;
			XDLI_DisconnectT	* XDLI_Disconnect;
			XDLI_NewExtensionT	* XDLI_NewExtension;
			XDLI_DeleteExtensionT	* XDLI_DeleteExtension;
			XDLI_ResetExtensionT	* XDLI_ResetExtension;
			XDLI_GetMessageT	* XDLI_GetMessage;
			XDLI_DirectCommandT	* XDLI_DirectCommand;
			XDLI_GetRegisterNumberT	* XDLI_GetRegisterNumber;
			XDLI_GetRegisterDescriptionT	* XDLI_GetRegisterDescription;
			XDLI_ReadRegisterT	* XDLI_ReadRegister;
			XDLI_ReadRegisterT	* XDLI_ReadRegisterPrev;
			XDLI_WriteRegisterT	* XDLI_WriteRegister;
			XDLI_CollectRegistersT	* XDLI_CollectRegisters;
			XDLI_TraceAsmT		* XDLI_TraceAsm;
			XDLI_FunctionalExecuteT	* XDLI_FunctionalExecute;
			XDLI_CycleAccurateExecuteT	* XDLI_CycleAccurateExecute;
		};
	
		typedef struct XDLI_Features XDLI_FeaturesT;

		struct XDLI_Instance {
			void		* instance;
			XDLI_FeaturesT	features;
		};

		typedef struct XDLI_Instance XDLI_InstanceT;
	
	/*}}}*/

/*}}}*/

#ifdef __cplusplus
}
#endif


#endif

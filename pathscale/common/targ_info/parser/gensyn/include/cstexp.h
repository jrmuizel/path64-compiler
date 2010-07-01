/** \file cstexp.h
    \brief Constant expression management.

*/
#ifndef __CSTEXP_H__
#define __CSTEXP_H__

#include "gensyn_defs.h"
#include "astype.h"

#define	EXPR_STACK_SIZE	50 /**< Depth of expression stack */

/** \typedef e_TypeOp
    Operator type of expression.
*/

/** \enum e_TypeOp
    Operators of expression.
 */
typedef enum {
	OP_LAB = 0,    /**< Label */
	OP_CST,        /**< Constant */
	OP_VAR,        /**< Variable */
	ADD,           /**< Addition */
	SUB,           /**< Subtraction */
	LAND,          /**< Logical And */
	EQU,           /**< Equal */
	SLE,           /**< Signed Less Than or Equal */
	SGE,           /**< Signed Greater Than or Equal */
	SLT,           /**< Signed Less Than */
	SGT,           /**< Signed Greater Than */
	ULE,           /**< Unsigned Less Than or Equal */
	UGE,           /**< Unsigned Greater Than or Equal */
	ULT,           /**< Unsigned Less Than */
	UGT,           /**< Unsigned Greater Than */
	MUL,           /**< Multiply */
	MOD,           /**< Modulo */
	DIV,           /**< Division */
	SHL,           /**< Left Shift */
	SSHR,          /**< Signed Right Shift */
	USHR,          /**< Unsigned Right Shift */
	ROTR,          /**< Right Rotate */
	ROTL,          /**< Left Rotate */
	LOR,           /**< Logical Or */
	LXOR,          /**< Logical Exclusive Or */
	LNOT,          /**< Logical Not */
	LNEG,          /**< Logical Negate */
	NEG,           /**< Arithmetic Negate */
	NONE           /**< NONE */
} e_TypeOp ;

/** \typedef e_TypeExp
    Type of expression.
*/

/** \enum e_TypeExp
    Type of expression.
 */
typedef enum  {
	EXPCST,			/**< expression type is constant */
	EXPREL,			/**< expression type is relocation */
	EXPNONE			/**< expression type is unknown */
} e_TypeExp ;

/** \typedef s_ConstExpr
    Description of a constant expression.
*/

#if 0
/** \struct s_ConstExpr
    Constant expression structure.
 */
typedef struct s_ConstExpr {
	uint16 numOperands ;				   /**< Number of operands */
	e_TypeOp typeOp ;					   /**< Operator type */
	e_TypeExp typeExp ;			      /**< Expression type */
	INT64 value ;						   /**< Expression value */
	uint32 index ;					 	   /**< Symbol index in symbol table */
	struct s_ConstExpr **exprArray ;	/**< operand array */
} s_ConstExpr ;

/** InitExprStack */
extern void InitExprStack () ;
/** ExprIsConstant */
extern int ExprIsConstant (s_ConstExpr *expr, INT64 value) ;
/** GetExprIndex */
extern s_ConstExpr *GetExprIndex (int index) ;
/** NewConst32Expr */
extern s_ConstExpr *NewConst32Expr (e_TypeOp typeOperator, uint32 value) ;
/** NewConstExpr */
extern s_ConstExpr *NewConstExpr (e_TypeOp typeOperator, INT64 value) ;
/** DeleteConstExpr */
extern void DeleteConstExpr (s_ConstExpr *expr) ;
/** SetOperand */
extern void SetOperand (s_ConstExpr *expr, uint16 ind, s_ConstExpr *operand) ;
/** RelocExpr */
extern void RelocExpr(uint32 value) ;
/** CreateConstantExpr */
extern void CreateConstantExpr(s_ConstExpr *expr) ;
/** CreateIdentExpr */
extern void CreateIdentExpr (s_String string) ;
/** CreateUnaryExpr */
extern void CreateUnaryExpr (e_TypeOp type) ;
/** CreateBinaryExpr */
extern void CreateBinaryExpr (e_TypeOp type) ;
/** DefineNewVar */
extern void DefineNewVar(s_String *string, s_ConstExpr *expr) ;
/** DefineNewLabel */
extern void DefineNewLabel(s_String *string) ;
#endif

#endif /* __CSTEXP_H__ */

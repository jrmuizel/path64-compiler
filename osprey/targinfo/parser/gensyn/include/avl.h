/** \file avl.h
    \brief AVL tree definition.

*/
#ifndef __AVL_H__
#define __AVL_H__

#include "strind.h"

/** \typedef AVLNode
    Node of AVL tree.
*/

/** \struct AVLNode
    This structure is used to store an AVL node.
 */
typedef struct AVLnode {
  short balance;              /**< balance of the node */ 
  s_StrInd *token ;           /**< token associated to the node */
  struct AVLnode *leftNode;   /**< left connected node */
  struct AVLnode *rightNode;  /**< right connected node */
} AVLnode;

/** AVLinit */
extern void AVLinit(AVLnode **A);
/** AVLdelete */
extern void AVLdelete(AVLnode **A);
/** AVLadd */
extern s_StrInd * AVLadd (char *val, AVLnode **R) ; 
/** AVLsearch */
extern int AVLsearch(char *val, AVLnode **R) ;

#endif

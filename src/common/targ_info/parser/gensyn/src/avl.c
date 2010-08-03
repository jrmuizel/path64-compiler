#include "avl.h"

#include <string.h> 
#include <stdlib.h>

/* procedure d'initialisation d'un AVL */

void AVLinit(AVLnode **A)
{
    *A = NULL;
}

/* procedure de destruction d'un AVL */

void AVLdelete(AVLnode **A)
{
    if (*A == NULL)
		return;
    AVLdelete(&(*A)->leftNode);
    AVLdelete(&(*A)->rightNode);
    free(*A);
}


static void ReadjustRight(AVLnode **A)
{
    AVLnode *aux;

    aux = (*A)->leftNode;
    (*A)->leftNode = aux->rightNode;
    aux->rightNode = *A;
    *A = aux;
}


static void ReadjustLeft(AVLnode **A)
{
    AVLnode *aux;

    aux = (*A)->rightNode;
    (*A)->rightNode = aux->leftNode;
    aux->leftNode = *A;
    *A = aux;
}


static void ReadjustLeftRight(AVLnode **A)
{
    ReadjustLeft(&(*A)->leftNode);
    ReadjustRight(A);
}


static void ReadjustRightLeft(AVLnode **A)
{
    ReadjustRight(&(*A)->rightNode);
    ReadjustLeft(A);
}

#if 0 /* HC050224: No mean to this function. Remove it for safety... */
/* Search routine for AVL tree *
 * If value present return 1   *
 * else return 0               */

int AVLsearch(char *val, AVLnode **R)
{
   AVLnode *P;
	int comp ;
	
	while (P!=NULL)
	{
		if ((comp=strcmp(val, GETSTR(*P->token)))<0) 
			P = P->leftNode;
		else if (comp>0)
			P = P->rightNode;
		else
			return 1;
	}
	return 0 ;
}
#endif
/* procedure d'ajout dans un AVL */

s_StrInd *AVLadd(char *val, AVLnode **R)
{
    AVLnode *N;   			/* nouveau noeud*/
    AVLnode *A;   			/* sous arbre a reequilibrer*/
    AVLnode *PA;   		/* pere de A*/
    AVLnode *P, *PP;   	/* pour la descente*/
    AVLnode *D;   			/* sert au balanceuilibre*/
    s_StrInd *newStrIndex ;
    int comp;

    N = (AVLnode *) malloc (sizeof(AVLnode));
    newStrIndex = (s_StrInd*) malloc (sizeof(s_StrInd)) ;
    SETSTR(*newStrIndex, val) ;
    SETIND(*newStrIndex, -1) ;
    N->token = newStrIndex;
    N->balance = 0;
    N->leftNode = NULL;
    N->rightNode = NULL;
    if (*R == NULL) {
		*R = N;
		return newStrIndex;
    }
    A = *R;
    PA = NULL;
    P = *R;
    PP = NULL;
    while (P != NULL)
	{
		if (P->balance != 0)
		{
			A = P;
			PA = PP;
		}
		PP = P;
		if ((comp=strcmp(GETSTR(*newStrIndex), GETSTR(*P->token)))<0) 
			P = P->leftNode;
		else if (comp>0)
			P = P->rightNode;
		else {
			free(N);
			free(newStrIndex) ;
			return P->token;
		}
    }
    /* adjonction */
    if (strcmp(GETSTR(*newStrIndex), GETSTR(*PP->token))<0)
		PP->leftNode = N;
    else
		PP->rightNode = N;
    /* modification du balance sur le chemin de A a N */
    D = A;
    while (D != N) {
		if (strcmp (GETSTR(*newStrIndex), GETSTR(*D->token))<0) {
			D->balance++;
			D = D->leftNode;
		} else {
			D->balance--;
			D = D->rightNode;
		}
    }
    /* reequilibrage*/
    switch (A->balance)
	{	    
		case 2:
			switch (A->leftNode->balance)
			{
				case 1:
					ReadjustRight(&A);
					A->balance = 0;
					A->rightNode->balance = 0;
					break;

				case -1:
					ReadjustLeftRight(&A);
					switch (A->balance)
					{
						case 1:
							A->leftNode->balance = 0;
							A->rightNode->balance = -1;
							break;
							
						case -1:
							A->rightNode->balance = 0;
							A->leftNode->balance = 1;
							break;

						case 0:
							A->rightNode->balance = 0;
							A->leftNode->balance = 0;
							break;
					}
					A->balance = 0;
					break;
			}
			break;
			
		case -2:   /*Cas Symetrique*/
			switch (A->rightNode->balance)
			{
				case -1:
					ReadjustLeft(&A);
					A->balance = 0;
					A->leftNode->balance = 0;
					break;

				case 1:
					ReadjustRightLeft(&A);
					switch (A->balance)
					{
						case 1:
							A->rightNode->balance = -1;
							A->leftNode->balance = 0;
							break;

						case -1:
							A->leftNode->balance = 1;
							A->rightNode->balance = 0;
							break;
							
						case 0:
							A->rightNode->balance = 0;
							A->leftNode->balance = 0;
							break;
					}
					A->balance = 0;
					break;
			}
			break;
    }
    if (PA == NULL) {
		*R = A;
		return newStrIndex;
    }
    if (strcmp (GETSTR(*A->token), GETSTR(*PA->token))<0)
		PA->leftNode = A;
    else
		PA->rightNode = A;
    return newStrIndex ;
}


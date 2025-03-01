/************************************************************************/
/*									*/
/*  Implement a NumberTree as described in 3.8.4 of the 1.7 PDF format	*/
/*  description.							*/
/*									*/
/************************************************************************/

		/**
		 *  An actual key, value pairs as they occur in the leaf nodes
		 *  of a NumberTree.
		 */
typedef struct NumberTreeNum
    {
    int		ntnKey;
    int		ntnValue;
    } NumberTreeNum;


		/**
		 *  A node in the tree. A number tree is a BTree that allows
		 *  binary search for a particular key value.
		 */
typedef struct NumberTreeNode {

			    /**
			     *  The child nodes of this node. They occur in 
			     *  all nodes, except the leaf nodes. We try to
			     *  avoid absolutely empty number trees and also
			     *  empty nodes. So, nodes without any Kids or 
			     *  Nums are frowned upon.
			     */
    struct NumberTreeNode * ntnKids;
    int			ntnKidCount;

			    /**
			     *  The actual mapping from key to value. It only
			     *  occurs in leaf nodes. I.E. nodes without any
			     *  kids
			     */
    NumberTreeNum *	ntnNums;
    int			ntnNumCount;

			    /**
			     *  The minimum/maximum key value contained in 
			     *  a child node. I.E. in every node except the 
			     *  root node.
			     */

    int			ntnLimitsMin;
    int			ntnLimitsMax;
    } NumberTreeNode;

extern void psPdfInitNumberTreeNode(	NumberTreeNode *	ntn );
extern void psPdfCleanNumberTreeNode(	NumberTreeNode *	ntn );

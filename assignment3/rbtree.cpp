/*
author:
sources:
description:
*/

#include "rbtree.h"
#include <iostream>
#include <iomanip>

// GLOBALS


///////////////////////////////////////
//constructor/destructors

rbTree::rbTree() {
	//create sentinel which always has black color
	nil = new node;
	nil->color = 'b';
	nil->key = 1000001;
	nil->parent = nil;
	nil->left = nil;
	nil->right = nil;
	//set root to sentinel, which we use in place of 'NULL'
	root = nil;
}

rbTree::~rbTree() {
	destroyTree(root);
	delete nil;
}

//////////////////////////////////////
// maintinence funcs

// recursively destroy tree
void rbTree::destroyTree(node *& tempNode) {
	//only execute when root, or any node, is not sentinel
	if (tempNode != nil) {
		destroyTree(tempNode->left);
		destroyTree(tempNode->right);
		delete tempNode;
	}
}

//////////////////////////////////////
// project requirements

// insert by integer
void rbTree::insert(int x) {
	node * cur = new node;
	cur->parent = nil;
	cur->left = nil;
	cur->right = nil;
	cur->key = x;
	cur->color = 'r';
	node * prev = nil;
	node * next = root;
	
	//if there is a root, find position for cur using 'prev'
	while (next != nil) {
		prev = next;
		if (cur->key < next->key)
			next = next->left;
		else
			next = next->right;
	}
	//if no root, make
	if (prev == nil)
		root = cur;
	//if key is less than position determined (prev) in while loop, place, else
	else if (cur->key < prev->key) {
		prev->left = cur;
		cur->parent = prev;
	}
	else {

		prev->right = cur;
		cur->parent = prev;
	}
	//output key and color about to be insertfixed
	//cout << "about to insertfix: " << cur->key << "color: " << cur->color << endl;
	insertFix(cur);
}

/*
	case1: cur's uncle red
	case2: cur's uncle black and cur is right child
	case3: cur's uncle black and cur left child
*/
// insert fix
void rbTree::insertFix(node * cur) {
	//while parent red (which violates property 4)
	while (cur->parent->color == 'r') {
		//if parent leftchild, select uncle
		//node * uncle = nil;
		if (cur->parent == cur->parent->parent->left) {
			node * uncle = cur->parent->parent->right;
			
			if (uncle->color == 'r') {
				cur->parent->color = 'b';
				uncle->color = 'b';
				cur->parent->parent->color = 'r';
				cur = cur->parent->parent;
			}
			//if cur is rightchild, double rotate
			else if (cur == cur->parent->right) {
				//case 2, we know uncle black
				cur = cur->parent;
				leftRotate(cur);
				//case 3, cur and parent red
				//cur->parent->color = 'b';
				//cur->parent->parent->color = 'r';
				//rightRotate(cur->parent->parent);
			}
			cur->parent->color = 'b';
			cur->parent->parent->color = 'r';
			rightRotate(cur->parent->parent);
		}
		//same as if but reversed tree side
		else {
			node * uncle = cur->parent->parent->left;

			if (uncle->color == 'r') {
				cur->parent->color = 'b';
				uncle->color = 'b';
				cur->parent->parent->color = 'r';
				cur = cur->parent->parent;
			}
			else if (cur == cur->parent->left) {
				cur = cur->parent;
				rightRotate(cur);
				//cur->parent->color = 'b';
				//cur->parent->parent->color = 'r';
				//leftRotate(cur->parent->parent);
			}
			cur->parent->color = 'b';
			cur->parent->parent->color = 'r';
			leftRotate(cur->parent->parent);
		}
	}
	//always set root to black
	root->color = 'b';		
}

/*
	want to delete node with key x. we find it and set it to cur.
	case1: cur has < 2 children, cur removed
		a. if no leftchild, transplant right subtree(could also be nil)
		b. elseif no rightchild, transplant left subtree(could also be nil)
	case2: cur has 2 children
		a. find min node on right
		b. determine color
		c. grab right subtree of min node on right
		d. if cur parent of 

*/
//remove
bool rbTree::remove(int x) {
	//first we find node to remove
	node * cur = findNodeByKey(x);
	if (cur == nil)
		return false;
	//cout << "curkey is: " << cur->key << endl;
	//cout << "cur: " << cur->parent << endl;	
	
	// temp maintained as cur if cur has < 2 children, or is value that replaces cur
	// temp2 is root of subtree moved
	node * temp = cur;
	node * temp2 = NULL;
	char tempOrigColor = temp->color;
	if (cur->left == nil) {
		temp2 = cur->right;
		transplant(cur, cur->right);
	}
	else if (cur->right == nil) {
		temp2 = cur->left;
		transplant(cur, cur->left);
	}
	else {
		temp = findMinNode(cur->right);		//find replacement inorder from cur
		tempOrigColor = temp->color;		//find color of replacement
		temp2 = temp->right;			//get right subtree of replacement
		if (temp->parent == cur)		//if replacment's parent is to to be removed
			temp2->parent = temp;		//set replacement's right subtree's parent to 
		else {
			transplant(temp, temp->right);
			temp->right = cur->right;
			temp->right->parent = temp;
		}
		transplant(cur, temp);
		temp->left = cur->left;
		temp->left->parent = temp;
		temp->color = cur->color;
	}
	// if original node color was black, removing it could've caused violations
	if (tempOrigColor == 'b')
		removeFix(temp2);
	delete cur;
	return true;
}

/*
	case1: cur's sibling red
	case2: cur's sibling temp is black, and both of temp's children black
	case3: cur's sibling black, temp's left red and right black
	case4: cur's sibling temp is black & temp's right is red 
*/
//removefix
void rbTree::removeFix(node * cur) {
	while (cur != root && cur->color == 'b') {
		// if cur is left child, consider sibling (right child of parent)
		if (cur == cur->parent->left) {
			node * temp = cur->parent->right;
			if (temp->color == 'r') {
				temp->color == 'b';
				cur->parent->color == 'r';
				leftRotate(cur->parent);
				temp = cur->parent->right;
			}
			if (temp->left->color == 'b' && temp->right->color == 'b') {
				temp->color == 'r';
				cur = cur->parent;
			}
			else if (temp->right->color == 'b') {
				temp->left->color = 'b';
				temp->color = 'r';
				rightRotate(temp);
				temp = cur->parent->right;
			}
			temp->color = cur->parent->color;
			cur->parent->color = 'b';
			temp->right->color = 'b';
			leftRotate(cur->parent);
			cur = root;
		}
		// cur is right child
		else {
			node * temp = cur->parent->left;
			if (temp->color == 'r') {
				temp->color == 'b';
				cur->parent->color == 'r';
				rightRotate(cur->parent);
				temp = cur->parent->left;
			}
			if (temp->right->color == 'b' && temp->left->color == 'b') {
				temp->color == 'r';
				cur = cur->parent;
			}
			else if (temp->left->color == 'b') {
				temp->right->color = 'b';
				temp->color = 'r';
				leftRotate(temp);
				temp = cur->parent->left;
			}
			temp->color = cur->parent->color;
			cur->parent->color = 'b';
			temp->left->color = 'b';
			rightRotate(cur->parent);
			cur = root;
		}
	cur->color = 'b';
	}

}

bool rbTree::search(int key) {
	if (findNodeByKey(key) == nil)
		return false;
	else
		return true;
}

// return largest key in tree
int rbTree::maximum() {
	node * cur = findMaxNode(root);
	if (cur == nil) {
		cout << "Empty" << endl;
		return 0;
	}
	else {
		cout << cur->key << endl;
		return cur->key;
	}
}

// return smallest key in tree
int rbTree::minimum() {
	node * cur = findMinNode(root);
	if (cur == nil) {
		cout << "Empty" << endl;
		return false;
	}
	else {
		cout << cur->key << endl;
		return cur->key;
	}
}

//////////////////////////////////////////////////////////////
// node movement/maintinence functions

// find node of min key value
node* rbTree::findMinNode(node * cur) {
	while (cur->left != nil)
		cur = cur->left;
	return cur;
}

//find node with max keyvalue
node* rbTree::findMaxNode(node * cur) {
	while (cur->right != nil)
		cur = cur->right;
	return cur;
}

// find node by key
node* rbTree::findNodeByKey(int x) {
	node * cur = root;
	while (cur != nil && x != cur->key) {
		if (x < cur->key)
			cur = cur->left;
		else
			cur = cur->right;
	}
	if (cur->key == x)
		return cur;
	else
		return nil;
}

// find successor
node* rbTree::findSuccessor(node * cur) {
	if (cur->right != nil)
		return findMinNode(cur->right);
	node * other = cur->parent;
	while (other != nil && cur == other->right) {
		cur = other;
		other = other->parent;
	}
	return other;
}


// transplant, used to move subtrees(by moving their roots)
void rbTree::transplant(node * cur, node * toMove) {

	if (cur->parent == nil)
		root = toMove;
	else if (cur == cur->parent->left)
		cur->parent->left = toMove;
	else
		cur->parent->right = toMove;
	toMove->parent = cur->parent;
}

// left rotate 
void rbTree::leftRotate(node * cur) {
	if (cur->right == nil)
		return;
	else {
		node * other = cur->right;
		//make other's left child a rightchild of cur
		if (other->left != nil) {
			cur->right = other->left;
			other->left->parent = cur;
		}
		else
			cur->right = nil;
		other->parent = cur->parent;
		//check position of cur and place other there
		if (cur->parent == nil)			//cur is root case
			root = other;
		else if (cur == cur->parent->left)	//cur is parent's left child
			cur->parent->left = other;
		else					//cur is parent's right child
			cur->parent->right = other;
		other->left = cur;
		cur->parent = other;
	}	
	/*
	node * other = cur->right;	//set to rotate with
	cur->right = other->left;	//transplant subtrees
	if (other->left != nil)
		other->left->parent = cur;
	other->parent = cur->parent;	//link cur's parents to others
	if (cur->parent == nil)
		root = other;
	else if (cur == cur->parent->left)
		cur->parent->left = other;
	else 
		cur->parent->right = other;
	other->left = cur;		//place cur on other's left
	cur->parent = other;
	*/
}

// right rotate
void rbTree::rightRotate(node * cur) {
	node * other = cur->left;	//set to rotate with
	cur->left = other->right;	//transplant subtrees
	if (other->right != nil)
		other->right->parent = cur;
	other->parent = cur->parent;	//link cur's parents to others
	if (cur->parent == nil)
		root = other;
	else if (cur == cur->parent->right)
		cur->parent->right = other;
	else 
		cur->parent->left = other;
	other->right = cur;		//place cur on other's left
	cur->parent = other;
}


///////////////////////////////////////////////////////////
//print function
void rbTree::print() {
	if (root == nil) {
		cout << "Empty" << endl;
		return;
	}
	else
		inOrderPrint(root);
	cout << endl;
}

void rbTree::inOrderPrint(node * cur) {
	if (cur == nil)
		return;
	inOrderPrint(cur->left);
	cout << cur->key << " ";
	inOrderPrint(cur->right);
}

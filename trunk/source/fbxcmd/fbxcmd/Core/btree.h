/*
current version:  may 9th, 2003 

this file contains a class template for elements stored in a B-tree, and a
class for a B-tree node, which provides all the methods needed to search,
insert, or delete.  a sample "main" function is also provided to show
how to use the B-tree.
to understand the identifiers and comments, visualize the tree as having
its root node at the top of the diagram, its leaf nodes at the bottom of
the diagram, and each node as containing an array oriented horizontally,
with the smallest element on the left and the largest element on the right.
the zeroth element of a node contains only a subtree pointer, no key value
or value.

a b-tree grows "upward", by splitting the root node when the node's capacity
is exceeded.  conversely, the depth of the tree is always reduced by merging
the last remaining element of the root node with the elements of its two
child nodes, so the tree contracts "from the top".

this code may be freely copied.
programmer: toucan@textelectric.net
algorithm and pseudo-code found in:
"fundamentals of data structures in pascal", by Horowitz and Sahni

there is a java applet on the web that draws a b-tree diagram and allows the
user to perform insertions and deletions, so you can see how it grows and shrinks,
at: http://www.mars.dti.ne.jp/~torao/program/structure/btree.html
*/

#pragma once 

#include <iostream>
#include <string>
#include <vector>
#include <strstream>
using namespace std;

#define BTREE_INVALID_PTR (reinterpret_cast<Node*>(-1))
#define BTREE_NULL_PTR (reinterpret_cast<Node*>(0))

template<typename key, typename value>
class BTree {

   static const int invalid_index = -1;
   static const int max_elements = 200;  // max elements in a node
   // size limit for the array in a vector object.  best performance was
   // at 800 bytes.
   static const int max_array_bytes = 800; 

public:
   class Node;

   #pragma region Element
   class Element {
      // contains a key value, a value, and a pointer toward the subtree
      // containing key values greater than this->m_key but lower than the
      // key value of the next element to the right
   public:
      Element() : mp_subtree(BTREE_NULL_PTR) {}
      Element(Node* ptr) : mp_subtree(ptr) {}
      Element(key key, value value) : m_key(key), m_payload(value), mp_subtree(BTREE_NULL_PTR) {}

      key m_key;
      value m_payload;
      Node* mp_subtree;
   public:
      bool operator>   (Element& other) const { return m_key >  other.m_key; }
      bool operator<   (Element& other) const { return m_key <  other.m_key; }
      bool operator>=  (Element& other) const { return m_key >= other.m_key; }
      bool operator<=  (Element& other) const { return m_key <= other.m_key; }
      bool operator==  (Element& other) const { return m_key == other.m_key; }
      bool valid () const { return mp_subtree != BTREE_INVALID_PTR; }
      void invalidate () { mp_subtree = BTREE_INVALID_PTR; }
      Element& operator= (const Element& other) {
         m_key = other.m_key;
         m_payload = other.m_payload;
         mp_subtree = other.mp_subtree;
         return *this;
      }
      void dump (); 
   }; //______________________________________________________________________

   #pragma endregion

   #pragma region Node
   class Node {
   protected:
      // locality of reference, beneficial to effective cache utilization,
      // is provided by a "vector" container rather than a "list"
      vector<Element> m_vector;
      // number of elements currently in m_vector, including the zeroth element
      // which has only a subtree, no key value or value.
      size_t m_count;
      Node* mp_parent;
      bool is_leaf();
      bool vector_insert (Element& element);
      bool vector_insert_for_split (Element& element);
      bool split_insert (Element& element);
      bool vector_delete (Element& target);
      bool vector_delete (int target_pos);
      void insert_zeroth_subtree (Node* subtree);
      int key_count () { return m_count-1; }
      Element& largest_key () { return m_vector[m_count-1]; }
      Element& smallest_key () { return m_vector[1]; }
      Element& smallest_key_in_subtree();
      int index_has_subtree ();
      Node* right_sibling (int& parent_index_this);
      Node* left_sibling (int& parent_index_this);
      Node* rotate_from_left(int parent_index_this);
      Node* rotate_from_right(int parent_index_this);
      Node* merge_right (int parent_index_this);
      Node* merge_left (int parent_index_this);
      bool merge_into_root ();
      int minimum_keys ();
   public:
      Element& search (const key& desired, Node*& last_visited); 
      Element& search (Element& desired, Node*& last_visited); 
      bool tree_insert (Element& element);
      bool delete_element (Element& target);
      int delete_all_subtrees ();
      Node* find_root();
      // the root of the tree may change.  this attribute keeps it accessible.
      BTree& m_root;
      Element& operator[] (size_t i) { return m_vector[i]; }
      // node cannot be instantiated without a root tracker
      Node (BTree& root_track);
      void dump ();
   }; //______________________________________________________________________
   #pragma endregion

   // all the node instances that belong to a given tree have a reference to one
   // instance of BTree.  as the Node instance that is the root may change
   // or the original root may be deleted, Node instances must access the root
   // of the tree through this tracker, and update the root pointer when they
   // perform insertions or deletions.  using a static attribute in the Node
   // class to hold the root pointer would limit a program to just one B-tree.
protected:
   Node* mp_root;
   Node* mp_last;
   Element m_invalid;

   void set_root (Node* old_root, Node* new_root) {
      // ensure that the root is only being changed by a node belonging to the
      // same tree as the current root
      if (old_root != mp_root)
         throw "wrong old_root in RootTracker::set_root";
      else
         mp_root = new_root;
   }
   Node* get_root () { return mp_root; }

public:
   BTree() : mp_root(BTREE_NULL_PTR), m_invalid(BTREE_INVALID_PTR), mp_last(BTREE_NULL_PTR)  {
      set_root(BTREE_NULL_PTR, new Node(*this));
   }

   ~BTree () {
      // safety measure
      if (mp_root) {
         mp_root->delete_all_subtrees();
         delete mp_root;
      }
   }

   Element& search (key& desired) {
      if (mp_root != BTREE_NULL_PTR) {
         return mp_root->search(desired, mp_last);
      }
      return m_invalid;
   }

   bool insert (const key& key, value& val) {
      if (mp_root != BTREE_NULL_PTR) {
         Element elem(key, val);
         return mp_root->tree_insert(elem);
      }
      return false;
   }

   bool remove (const key& key) {
      if (mp_root != BTREE_NULL_PTR) {
         Element elem;
         elem.m_key = key;
         return mp_root->delete_element(elem);
      }
      return false;
   }
   int delete_all_subtrees ()
   {
      if (mp_root != BTREE_NULL_PTR) {
         return mp_root->delete_all_subtrees();
      }
      return 0;
   }
};

#pragma region Implementation

template<typename key, typename value> 
void BTree<key, value>::Element::dump () {
   cout << "key=" << m_key << "sub=" << mp_subtree << ' ';
} //_______________________________________________________________________

template<typename key, typename value> 
int BTree<key, value>::Node::minimum_keys () {
   // minus 1 for the empty slot left for splitting the node
   int size = m_vector.size();
   int ceiling_func = (size-1)/2;
   if (ceiling_func*2 < size-1)
      ceiling_func++;
   return ceiling_func-1;  // for clarity, may increment then decrement
} //________________________________________________________________________

template<typename key, typename value> 
void BTree<key, value>::Node::insert_zeroth_subtree (Node* subtree) {
   m_vector[0].mp_subtree = subtree;
   m_vector[0].m_key = "";
   m_count = 1;
   if (subtree)
      subtree->mp_parent = this;
} //_________________________________________________________________________

template<typename key, typename value> 
void BTree<key, value>::Node::dump (){
   // write out the keys in this node and all its subtrees, along with
   // node adresses, for debugging purposes
   if (this == m_root.get_root())
      cout << "ROOT\n";
   cout << "\nthis=" << this << endl;
   cout << "parent=" << mp_parent << " count=" << m_count << endl;
   for (size_t i=0; i<m_count; i++)
      m_vector[i].dump();
   for (size_t i=0; i<m_count; i++)
      if (m_vector[i].mp_subtree)
         m_vector[i].mp_subtree->dump();
   cout << endl;
} //________________________________________________________________________

template<typename key, typename value> 
BTree<key, value>::Node::Node (BTree<key, value>& root_track)  : m_root(root_track) {
   // limit the size of the vector to 4 kilobytes max and 200 entries max.
   int num_elements = max_elements*sizeof(BTree<key, value>::Element)<=max_array_bytes ? max_elements : max_array_bytes/sizeof(BTree<key, value>::Element);
   if (num_elements < 6)  // in case key or payload is really huge
      num_elements = 6;
   m_vector.resize (num_elements);
   m_count = 0;
   mp_parent = 0;
   insert_zeroth_subtree (0);
} //________________________________________________________________________

template<typename key, typename value> 
typename BTree<key, value>::Node* BTree<key, value>::Node::find_root () {
   Node* current = this;
   while (current->mp_parent)
      current = current->mp_parent;
   return current;
} //__________________________________________________________________________

template<typename key, typename value> 
bool BTree<key, value>::Node::is_leaf () {
   return m_vector[0].mp_subtree==0;
} //________________________________________________________________________

template<typename key, typename value> 
int BTree<key, value>::Node::delete_all_subtrees () {
   // return the number of nodes deleted
   int count_deleted = 0;
   for (size_t i=0; i< m_count; i++) {
      if (!m_vector[i].mp_subtree)
         continue;
      else if (m_vector[i].mp_subtree->is_leaf()) {
         delete m_vector[i].mp_subtree;
         count_deleted++;
      }
      else
         count_deleted += m_vector[i].mp_subtree->delete_all_subtrees();
   }
   return count_deleted;
} //_______________________________________________________________________

template<typename key, typename value> 
bool BTree<key, value>::Node::vector_insert(typename BTree<key, value>::Element& element) {
   // this method merely tries to insert the argument into the current node.
   // it does not concern itself with the entire tree.
   // if the element can fit into m_vector, slide all the elements
   // greater than the argument forward one position and copy the argument
   // into the newly vacated slot, then return true.  otherwise return false.
   // note: the tree_insert method will already have verified that the key
   // value of the argument element is not present in the tree.

   if (m_count >= m_vector.size()-1) // leave an extra slot for split_insert
      return false;
   size_t i = m_count;

   while (i>0 && m_vector[i-1]>element) {
      m_vector[i] = m_vector[i-1];
      i--;
   }
   if (element.mp_subtree)
      element.mp_subtree->mp_parent = this;
   m_vector[i] = element;
   m_count++;
   return true;
} //__________________________________________________________________

template<typename key, typename value> 
bool BTree<key, value>::Node::vector_delete (typename BTree<key, value>::Element& target) {
   // delete a single element in the vector belonging to *this node.
   // if the target is not found, do not look in subtrees, just return false.

   int target_pos = -1;
   int first = 1;
   int last = m_count-1;
   // perform binary search
   while (last-first > 1) {
      int mid = first+(last-first)/2;
      if (target>=m_vector[mid])
         first = mid;
      else
         last = mid;
   }
   if (m_vector[first]==target)
      target_pos = first;
   else if (m_vector[last]==target)
      target_pos = last;
   else
      return false;
   // the element's subtree, if it exists, is to be deleted or re-attached
   // in a different function.  not a concern here.  just shift all the
   // elements in positions greater than target_pos.
   for (size_t i=target_pos; i<m_count; i++)
      m_vector[i] = m_vector[i+1];

   m_count--;
   return true;
} //____________________________________________________________________

template<typename key, typename value> 
bool BTree<key, value>::Node::vector_delete (int target_pos) {
   // delete a single element in the vector belonging to *this node.
   // the element is identified by position, not value.

   if (target_pos<0 || target_pos>=(int)m_count)
      return false;

   // the element's subtree, if it exists, is to be deleted or re-attached
   // in a different function.  not a concern here.  just shift all the
   // elements in positions greater than target_pos.
   for (size_t i=target_pos; i<m_count; i++)
      m_vector[i] = m_vector[i+1];

   m_count--;
   return true;
} //____________________________________________________________________

template<typename key, typename value> 
bool BTree<key, value>::Node::vector_insert_for_split (typename BTree<key, value>::Element& element) {
   // this method insert an element that is in excess of the nominal capacity of
   // the node, using the extra slot that always remains unused during normal
   // insertions.  this method should only be called from split_insert()

   if (m_count >= m_vector.size()) // error
      return false;
   size_t i = m_count;

   while (i>0 && m_vector[i-1]>element) {
      m_vector[i] = m_vector[i-1];
      i--;
   }
   if (element.mp_subtree)
      element.mp_subtree->mp_parent = this;
   m_vector[i] = element;
   m_count++;
   return true;
} //__________________________________________________________________

template<typename key, typename value> 
bool BTree<key, value>::Node::split_insert (typename BTree<key, value>::Element& element) {

   // split_insert should only be called if node is full
   if (m_count != m_vector.size()-1)
      throw "bad m_count in split_insert";

   vector_insert_for_split (element);
   size_t split_point = m_count/2;
   if (2*split_point < m_count)  // perform the "ceiling function"
      split_point++;
   // new node receives the rightmost half of elements in *this node
   Node* new_node = new Node(m_root);
   BTree<key, value>::Element upward_element = m_vector[split_point];
   new_node->insert_zeroth_subtree (upward_element.mp_subtree);
   upward_element.mp_subtree = new_node;
   // element that gets added to the parent of this node
   for (size_t i=1; i<m_count-split_point; i++)
      new_node->vector_insert(m_vector[split_point+i]);
   new_node->m_count = m_count-split_point;
   m_count = split_point;
   new_node->mp_parent = mp_parent;

   // now insert the new node into the parent, splitting it if necessary
   if (mp_parent && mp_parent->vector_insert(upward_element))
      return true;
   else if (mp_parent && mp_parent->split_insert(upward_element))
      return true;
   else if (!mp_parent) { // this node was the root
      Node* new_root = new Node(m_root);
      new_root->insert_zeroth_subtree(this);
      this->mp_parent = new_root;
      new_node->mp_parent = new_root;
      new_root->vector_insert (upward_element);
      m_root.set_root (m_root.get_root(),  new_root);
      new_root->mp_parent = 0;
   }
   return true;

}//__________________________________________________________________

template<typename key, typename value> 
bool BTree<key, value>::Node::tree_insert (typename BTree<key, value>::Element& element) {
   Node* last_visited_ptr = this;
   if (search(element, last_visited_ptr).valid())  // element already in tree
      return false;
   if (last_visited_ptr->vector_insert(element))
      return true;
   return last_visited_ptr->split_insert(element);
} //__________________________________________________________________

template<typename key, typename value> 
bool BTree<key, value>::Node::delete_element (typename BTree<key, value>::Element& target) {
   // target is just a package for the key value.  the reference does not
   // provide the address of the BTree<key, value>::Element instance to be deleted.


   // first find the node contain the BTree<key, value>::Element instance with the given key
   Node* node = 0;
   int parent_index_this = invalid_index;
   BTree<key, value>::Element& found = search (target, node);
   if (!found.valid())
      return false;

   if (node->is_leaf() && node->key_count() > node->minimum_keys())
      return node->vector_delete (target);
   else if (node->is_leaf()) {
      node->vector_delete (target);
      // loop invariant: if _node_ is not BTREE_NULL_PTR, it points to a node
      // that has lost an element and needs to import one from a sibling
      // or merge with a sibling and import one from its parent.
      // after an iteration of the loop, _node_ may become null or
      // it may point to its parent if an element was imported from the
      // parent and this caused the parent to fall below the minimum
      // element count.
      while (node) {
         // NOTE: the "this" pointer may no longer be valid after the first
         // iteration of this loop!!!
         if (node==node->find_root() && node->is_leaf())
            break;
         if (node==node->find_root() && !node->is_leaf()) // sanity check
            throw "node should not be root in delete_element loop";
         // is an extra element available from the right sibling (if any)
         Node* right = node->right_sibling(parent_index_this);
         if (right && right->key_count() > right->minimum_keys())
            node = node->rotate_from_right(parent_index_this);
         else {
            // is an extra element available from the left sibling (if any)
            Node* left = node->left_sibling(parent_index_this);
            if (left && left->key_count() > left->minimum_keys())
               node = node->rotate_from_left(parent_index_this);
            else if (right)
               node = node->merge_right(parent_index_this);
            else if (left)
               node = node->merge_left(parent_index_this);
         }
      }
   }
   else {
      BTree<key, value>::Element& smallest_in_subtree = found.mp_subtree->smallest_key_in_subtree();
      found.m_key = smallest_in_subtree.m_key;
      found.m_payload = smallest_in_subtree.m_payload;
      found.mp_subtree->delete_element (smallest_in_subtree);
   }
   return true;
} //___________________________________________________________________

template<typename key, typename value> 
typename BTree<key, value>::Node* BTree<key, value>::Node::rotate_from_right(int parent_index_this) {
   // new element to be added to this node
   BTree<key, value>::Element underflow_filler = (*mp_parent)[parent_index_this+1];
   // right sibling of this node
   Node* right_sib = (*mp_parent)[parent_index_this+1].mp_subtree;
   underflow_filler.mp_subtree = (*right_sib)[0].mp_subtree;
   // copy the entire element
   (*mp_parent)[parent_index_this+1] = (*right_sib)[1];
   // now restore correct pointer
   (*mp_parent)[parent_index_this+1].mp_subtree = right_sib;
   vector_insert (underflow_filler);
   right_sib->vector_delete(0);
   (*right_sib)[0].m_key = "";
   (*right_sib)[0].m_payload = "";
   return BTREE_NULL_PTR; // parent node still has same element count
} //_______________________________________________________________________

template<typename key, typename value> 
typename BTree<key, value>::Node* BTree<key, value>::Node::rotate_from_left(int parent_index_this) {
   // new element to be added to this node
   BTree<key, value>::Element underflow_filler = (*mp_parent)[parent_index_this];
   // left sibling of this node
   Node* left_sib = (*mp_parent)[parent_index_this-1].mp_subtree;
   underflow_filler.mp_subtree = (*this)[0].mp_subtree;
   (*this)[0].mp_subtree = (*left_sib)[left_sib->m_count-1].mp_subtree;
   if ((*this)[0].mp_subtree)
      (*this)[0].mp_subtree->mp_parent = this;
   // copy the entire element
   (*mp_parent)[parent_index_this] = (*left_sib)[left_sib->m_count-1];
   // now restore correct pointer
   (*mp_parent)[parent_index_this].mp_subtree = this;
   vector_insert (underflow_filler);
   left_sib->vector_delete(left_sib->m_count-1);
   return BTREE_NULL_PTR; // parent node still has same element count
} //_______________________________________________________________________

template<typename key, typename value> 
typename BTree<key, value>::Node* BTree<key, value>::Node::merge_right (int parent_index_this) {
   // copy elements from the right sibling into this node, along with the
   // element in the parent node vector that has the right sibling as it subtree.
   // the right sibling and that parent element are then deleted

   BTree<key, value>::Element parent_elem = (*mp_parent)[parent_index_this+1];
   Node* right_sib = (*mp_parent)[parent_index_this+1].mp_subtree;
   parent_elem.mp_subtree = (*right_sib)[0].mp_subtree;
   vector_insert (parent_elem);
   for (size_t i=1; i<right_sib->m_count; i++)
      vector_insert ((*right_sib)[i]);
   mp_parent->vector_delete (parent_index_this+1);
   delete right_sib;
   if (mp_parent==find_root() && !mp_parent->key_count()) {
      m_root.set_root(m_root.get_root(), this);
      delete mp_parent;
      mp_parent = 0;
      return BTREE_NULL_PTR;
   }
   else if (mp_parent==find_root() && mp_parent->key_count())
      return BTREE_NULL_PTR;
   if (mp_parent&& mp_parent->key_count() >= mp_parent->minimum_keys())
      return BTREE_NULL_PTR; // no need for parent to import an element
   return mp_parent; // parent must import an element
} //_______________________________________________________________________

template<typename key, typename value> 
typename BTree<key, value>::Node* BTree<key, value>::Node::merge_left (int parent_index_this) {
   // copy all elements from this node into the left sibling, along with the
   // element in the parent node vector that has this node as its subtree.
   // this node and its parent element are then deleted.

   BTree<key, value>::Element parent_elem = (*mp_parent)[parent_index_this];
   parent_elem.mp_subtree = (*this)[0].mp_subtree;
   Node* left_sib = (*mp_parent)[parent_index_this-1].mp_subtree;
   left_sib->vector_insert (parent_elem);
   for (size_t i=1; i<m_count; i++)
      left_sib->vector_insert (m_vector[i]);
   mp_parent->vector_delete (parent_index_this);
   Node* parent_node = mp_parent;  // copy before deleting this node
   if (mp_parent==find_root() && !mp_parent->key_count()) {
      m_root.set_root(m_root.get_root(), left_sib);
      delete mp_parent;
      left_sib->mp_parent = BTREE_NULL_PTR;
      delete this;
      return BTREE_NULL_PTR;
   }
   else if (mp_parent==find_root() && mp_parent->key_count()) {
      delete this;
      return BTREE_NULL_PTR;
   }
   delete this;
   if (parent_node->key_count() >= parent_node->minimum_keys())
      return BTREE_NULL_PTR; // no need for parent to import an element
   return parent_node; // parent must import an element

} //_______________________________________________________________________

template<typename key, typename value> 
typename BTree<key, value>::Node* BTree<key, value>::Node::right_sibling (int& parent_index_this) {
   parent_index_this = index_has_subtree (); // for element with THIS as subtree
   if (parent_index_this == invalid_index)
      return 0;
   // now mp_parent is known not to be null
   if (parent_index_this >= (int)(mp_parent->m_count)-1)
      return 0;  // no right sibling
   return mp_parent->m_vector[parent_index_this+1].mp_subtree;  // might be null
} //__________________________________________________________________________

template<typename key, typename value> 
typename BTree<key, value>::Node* BTree<key, value>::Node::left_sibling (int& parent_index_this) {
   parent_index_this = index_has_subtree (); // for element with THIS as subtree
   if (parent_index_this == invalid_index)
      return 0;
   // now mp_parent is known not to be null
   if (parent_index_this==0)
      return 0;  // no left sibling
   return mp_parent->m_vector[parent_index_this-1].mp_subtree;  // might be null
} //____________________________________________________________________________

template<typename key, typename value> 
int BTree<key, value>::Node::index_has_subtree () {
   // return the element in this node's parent that has the "this" pointer as its subtree
   if (!mp_parent)
      return invalid_index;
   int first = 0;
   int last = mp_parent->m_count-1;
   while (last-first > 1) {
      int mid = first+(last-first)/2;
      BTree<key, value>::Element& smallest = smallest_key();
      if (smallest_key()>=mp_parent->m_vector[mid])
         first = mid;
      else
         last = mid;
   }
   if (mp_parent->m_vector[first].mp_subtree == this)
      return first;
   else if (mp_parent->m_vector[last].mp_subtree == this)
      return last;
   else
      throw "error in index_has_subtree";
} //__________________________________________________________________

template<typename key, typename value> 
typename BTree<key, value>::Element& BTree<key, value>::Node::smallest_key_in_subtree () {
   if (is_leaf())
      return m_vector[1];
   else
      return m_vector[0].mp_subtree->smallest_key_in_subtree();
} //___________________________________________________________________

template<typename key, typename value> 
typename BTree<key, value>::Element& BTree<key, value>::Node::search (const key& desired, Node*& last_visited_ptr) {
   // the zeroth element of the vector is a special case (no key value or
   // payload, just a subtree).  the search starts at the *this node, not
   // at the root of the b-tree.
   Node* current = this;
   if (!key_count())
      current = 0;
   while (current) {
      last_visited_ptr = current;
      // if desired is less than all values in current node
      if (current->m_count>1 && desired<current->m_vector[1].m_key)
         current = current->m_vector[0].mp_subtree;
      // if desired is greater than all values in current node
      else if (desired>current->m_vector[current->m_count-1].m_key)
         current = current->m_vector[current->m_count-1].mp_subtree;

      else {
         // binary search of the node
         int first = 1;
         int last = current->m_count-1;
         while (last-first > 1) {
            int mid = first+(last-first)/2;
            if (desired>=current->m_vector[mid].m_key)
               first = mid;
            else
               last = mid;
         }
         if (current->m_vector[first].m_key==desired)
            return current->m_vector[first];
         if (current->m_vector[last].m_key==desired)
            return current->m_vector[last];
         else if (current->m_vector[last].m_key>desired)
            current = current->m_vector[first].mp_subtree;
         else
            current = current->m_vector[last].mp_subtree;
      }
   }
   return m_root.m_invalid;// Element(BTREE_INVALID_PTR);

} //_____________________________________________________________________
template<typename key, typename value> 
typename BTree<key, value>::Element& BTree<key, value>::Node::search (Element& desired, Node*& last_visited_ptr) {
   return search(desired.m_key, last_visited_ptr);
}
#pragma endregion


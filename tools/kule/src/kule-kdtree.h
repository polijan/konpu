// A k-d tree is space-partitioning binary tree for organizing points in a
// k-dimensional space. Its purpose to speed up the reverse lookup of colors in
// larger palettes.
//
// Palette pal;
// 1. First, create a blanced kd-tree from a populated palette:
//    Node *kdtree = TreeInit(pal);
// 2. Now the reverse lookup will be faster.
//    int = Tre
// 3. At the end, don't forget to release memory:
//    TreeDrop(kdtree);

#ifndef TOOLS_KULE_KDTREE_H
#define TOOLS_KULE_KDTREE_H
#include "kule.h"

typedef struct Node Node;
struct Node {
   int    color; // color index
   Node  *left;
   Node  *right;
};


// Recursively frees memory of a k-d tree node and all its sub-nodes
void TreeDrop(Node *node)
{
   if (node->left)  TreeDrop(node->left);
   if (node->right) TreeDrop(node->right);
   if (node)        free(node);
}


// Print a kd-tree and its colors.
static inline void TreePrint(const Node *tree, Palette palette);
   void TreePrint_(const Node *node, Palette pal, int depth) {
      if (node == NULL) return;

      for(int i = 0; i < depth - 1; i++)  printf ("|  ");
      printf("%s%3d (left:%3d, right:%3d) ",
         (depth==0)? "":"+--",
         node->color                             ,
         (node->left)  ? node->left->color  : -1 ,
         (node->right) ? node->right->color : -1
      );
      for(int i = 0; i < 8 - depth - 1; i++)  printf ("   ");
      LabPrint(pal.color[node->color], 2);
      TreePrint_(node->left,  pal, depth + 1);
      TreePrint_(node->right, pal, depth + 1);
   }
   static inline void TreePrint(const Node *tree, Palette palette)
   { TreePrint_(tree, palette, 0); }


// Verify (ie assert that) that the kd-Tree always encodes color components on
// left node is less the parent and color components on the right node is more
// than the parent.
static inline void NodeAssert(const Node *tree, Palette palette);
   void NodeAssert_(const Node *node, Palette pal, int depth)
   {
      if (node == NULL) return;

      Lab color = pal.color[node->color];
      Node *left = node->left;
      Node *right = node->right;
      switch (depth % 3) {
         case 0:
            if (left)  assert(pal.color[left->color].L  <= color.L);
            if (right) assert(pal.color[right->color].L >= color.L);
            break;
         case 1:
            if (left)  assert(pal.color[left->color].a  <= color.a);
            if (right) assert(pal.color[right->color].a >= color.a);
            break;
         case 2:
            if (left)  assert(pal.color[left->color].b  <= color.b);
            if (right) assert(pal.color[right->color].b >= color.b);
            break;
         default: unreachable();
      }
      NodeAssert_(left , pal, depth + 1);
      NodeAssert_(right, pal, depth + 1);
   }
   static inline void NodeAssert(const Node *tree, Palette palette)
   { NodeAssert_(tree, palette, 0); }


// Construct a balanced k-d tree for the given palette
Node *TreeInit(Palette pal);
   // Within all function used for TreeInit, we work on ONE global palette.
   // We do it like mostly because of limitation of the standard `qsort`
   // function which is used when building the kd-tree. This design (should) be
   // changed... later/maybe. (note: GNU libc has `qsort_r` which addresses this
   // know `qsort` limitation).
   static Palette TreeInit_palette_;
   // The comparator functions for `qsort`
   int NodeInit_compare_L_(const void *c1, const void *c2) {
      Lab lab1 = TreeInit_palette_.color[*(const int*)c1];
      Lab lab2 = TreeInit_palette_.color[*(const int*)c2];
      return (lab1.L < lab2.L) ? -1 : 1;
   }
   int NodeInit_compare_a_(const void *c1, const void *c2) {
      Lab lab1 = TreeInit_palette_.color[*(const int*)c1];
      Lab lab2 = TreeInit_palette_.color[*(const int*)c2];
      return (lab1.a < lab2.a) ? -1 : 1;
   }
   int NodeInit_compare_b_(const void *c1, const void *c2) {
      Lab lab1 = TreeInit_palette_.color[*(const int*)c1];
      Lab lab2 = TreeInit_palette_.color[*(const int*)c2];
      return (lab1.b < lab2.b) ? -1 : 1;
   }
   // This function which recursively constructs the nodes
   // see: https://en.wikipedia.org/wiki/Kd_tree
   Node* NodeInit_(int *colors, int start, int end, int depth) {
      if (start > end)  return NULL;
      // printf("depth #%3d - start:%3d , end:%3d - colors:", depth, start, end);

      int (*compare)(const void *, const void *);
      switch (depth % 3) {
         case  0: compare = NodeInit_compare_L_; break;
         case  1: compare = NodeInit_compare_a_; break;
         case  2: compare = NodeInit_compare_b_; break;
         default: unreachable();
      }
      qsort(colors + start, end - start + 1, sizeof(int), compare);
      int median = (start + end) / 2;

      // printf("%3d: ", median); LabPrint(pal.color[median], 2);
      Node* node  = MallocOrExit(sizeof(Node));
      node->color = colors[median];
      node->left  = NodeInit_(colors, start     , median - 1, depth + 1);
      node->right = NodeInit_(colors, median + 1, end       , depth + 1);
      return node;
   }
   Node *TreeInit(Palette palette) { // wrapper
      TreeInit_palette_ = palette;
      int colors[PALETTE_SIZE_MAX];
      for (int i = 0; i < palette.size; i++)
         colors[i] = i;
      return NodeInit_(colors, 0, palette.size - 1, 0);
   }


// Similar to PaletteClosest() but uses the spatial information in the the
// associated kd tree for faster retrieval
int TreeClosest(Node *tree, Palette palette, Lab color, float *best_distance2);
   // Nearest Neighbor Search in a k-d Tree
   // Don't call this directly, call `TreeClosest(tree, color)` instead.
   void NodeClosest_(Node *node, Palette pal, Lab color,
                     Node **best_node, float *best_distance2, int depth) {
      if (node == NULL) return;

      float dL = color.L - pal.color[node->color].L;
      float da = color.a - pal.color[node->color].a;
      float db = color.b - pal.color[node->color].b;
      float d2 = dL*dL + da*da + db*db;

      float axis_distance;
      switch (depth % 3) {
         case  0: axis_distance = dL; break;
         case  1: axis_distance = da; break;
         case  2: axis_distance = db; break;
         default: unreachable();
      }

      Node *near, *far; //subtrees
      if (axis_distance < 0) {
         near = node->left;
         far  = node->right;
      } else {
         near = node->right;
         far  = node->left;
      }

      // printf("depth:%d, d^2=%f", depth, d2);
      if (d2 < *best_distance2) {
         *best_distance2 = d2;
         *best_node = node;
         // printf("(NEW BEST)");
      }
      // printf(" <-- %d ", node->color);
      // LabPrint(pal.color[node->color], 2);

      NodeClosest_(near, pal, color, best_node, best_distance2, depth + 1);

      // We have to also search the other subtree if closer points might exist
      if ((axis_distance * axis_distance) < *best_distance2) {
      //if (fabs(axis_distance) < sqrt(*best_distance2)) {
         // printf("%d -> also check far: ", node->color);
         NodeClosest_(far, pal, color, best_node, best_distance2, depth + 1);
      }
   }
   int TreeClosest(Node *tree, Palette palette, Lab color, float *best_distance2) { //wrapper
      Node *result;
      float d2 = FLT_MAX;
      NodeClosest_(tree, palette, color, &result, &d2, 0);
      //printf("\n");
      if (best_distance2 != NULL)  *best_distance2 = d2;
      return result->color;
   }

#endif //TOOLS_KULE_KDTREE_H

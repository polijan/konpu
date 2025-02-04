#include "kule.h"
#include "kule-kdtree.h"
#include "kule-konpu.h"

// return the node which has a given color
static const Node *find_node(const Node *node, int color)
{
   if (node == NULL) return NULL;
   if (node->color == color) return node;
   const Node *from_left = find_node(node->left, color);
   if (from_left != NULL) return from_left;
   return find_node(node->right, color);
}

int main(void)
{
   Palette pal = PaletteKonpu();
   Node* kdtree = TreeInit(pal);

   printf("   // Color Palette (OkLab with components normalized in 0..510) and\n");
   printf("   // space-partitioning k-d tree info on the palette in L,a,b space\n");
   printf("   // (linking to median color 127 means no-leave)\n");
   printf("   //\n");
   printf("   // OkLab Color:  L/2,    a,  b+30 |kd: left, right\n");
   printf("   //--------------------------------|---------------\n");

   for (int i = 0; i < pal.size; i++) {
      const Node *node = find_node(kdtree, i);
      assert(node != NULL);
      printf("   /* Color %3d:*/  %3d, %4d, %4d  ,     %3d, %3d,\n", i,
         (uint8_t)roundf(pal.color[i].L * 255),
         (int) roundf(pal.color[i].a * 255 * 2),
         (int) roundf(pal.color[i].b * 255 * 2 + 30),
         (node->left)? node->left->color   : 127,
         (node->right)? node->right->color : 127
      );
   }

   TreeDrop(kdtree);
   return EXIT_SUCCESS;
}

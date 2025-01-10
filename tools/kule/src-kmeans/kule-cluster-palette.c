// Partition a palette into clusters of similar color attributes.

#include "../src/kule.h"

// Include 'kmeans'
// 1. Include the kmeans *implementation* in order to compile everything in a
//    single program.
// 2. Also temporarily disable warnings about comparing integer expressions of
//    different signs. Such comparisons in 'kmeans' appear safe.
#pragma  GCC diagnostic push
#pragma  GCC diagnostic ignored "-Wsign-compare"
#include "../external/kmeans/kmeans.c"
#pragma  GCC diagnostic pop


//------------------------------------------------------------------------------
// The functions callback by K-Means (as per different the different criteria)
//------------------------------------------------------------------------------
// distance_by_*(const void *color1, const void *color2)
// = K-Mean distance method
//
// update_centroids_by_*(...)
// = K-Means method assigning the centroids
// Reminder about the parameters passed by k-Means to this callback:
// - const void *objs        The list of all objects in the calculation
// - const int  *clusters    The list of cluster numbers for each object
// - size_t num_objs         Number of objects / size of the previous arrays
// - int   cluster           The cluster number we are generating a centroid for
// - void *centroid          Where to write the centroid result into
//                           (already allocated)
//------------------------------------------------------------------------------

static double distance_by_similarity(const Pointer color1, const Pointer color2)
{ return LabDistance(*(Lab*)color1, *(Lab*)color2); }

static double distance_by_L(const Pointer color1, const Pointer color2)
{ return fabs( ((Lab*)color1)->L - ((Lab*)color2)->L ); }

static double distance_by_a(const Pointer color1, const Pointer color2)
{ return fabs( ((Lab*)color1)->a - ((Lab*)color2)->a ); }

static double distance_by_b(const Pointer color1, const Pointer color2)
{ return fabs( ((Lab*)color1)->b - ((Lab*)color2)->b ); }

static double distance_by_Chroma(const Pointer color1, const Pointer color2)
{ return fabs( LabChroma(*(Lab*)color1) - LabChroma(*(Lab*)color2) ); }

static double distance_by_Hue(const Pointer color1, const Pointer color2) {
   float hue1 = LabHue(*(Lab*)color1);
   float hue2 = LabHue(*(Lab*)color2);
   Lab c1 = { 0.5f, cosf(hue1), sinf(hue1) };
   Lab c2 = { 0.5f, cosf(hue2), sinf(hue2) };
   return LabDistance(c1, c2);
}

static double distance_by_Saturation(const Pointer color1, const Pointer color2)
{ return fabs( LabSaturation(*(Lab*)color1) - LabSaturation(*(Lab*)color2) ); }

static double distance_by_R(const Pointer color1, const Pointer color2) {
   RGB c1 = oklab_to_linear_srgb(*(Lab*)color1);
   RGB c2 = oklab_to_linear_srgb(*(Lab*)color2);
   return fabs(c1.r - c2.r);
}

static double distance_by_G(const Pointer color1, const Pointer color2) {
   RGB c1 = oklab_to_linear_srgb(*(Lab*)color1);
   RGB c2 = oklab_to_linear_srgb(*(Lab*)color2);
   return fabs(c1.g - c2.g);
}

static double distance_by_B(const Pointer color1, const Pointer color2) {
   RGB c1 = oklab_to_linear_srgb(*(Lab*)color1);
   RGB c2 = oklab_to_linear_srgb(*(Lab*)color2);
   return fabs(c1.b - c2.b);
}


// Assign the centroid as the average of all colors in the cluster
static void update_centroids_by_similarity(
   const Pointer *objs, const int* clusters,
   size_t num_objs, int cluster, Pointer centroid)
{
   if (num_objs <= 0) return;

   Lab **color = (Lab**)objs;
   Lab *center = (Lab*)centroid;

   int n = 0;
   double sum_L = 0.;
   double sum_a = 0.;
   double sum_b = 0.;

   for (size_t i = 0; i < num_objs; i++) {
      // Only process objects of interest
      if (clusters[i] != cluster)
         continue;

      sum_L += color[i]->L;
      sum_a += color[i]->a;
      sum_b += color[i]->b;
      n++;
   }
   if (n != 0) {
      center->L = (float)(sum_L / n);
      center->a = (float)(sum_a / n);
      center->b = (float)(sum_b / n);
   }
}

static void update_centroids_by_L(
   const Pointer *objs, const int* clusters,
   size_t num_objs, int cluster, Pointer centroid)
{
   if (num_objs <= 0) return;
   Lab **color = (Lab**)objs;
   Lab *center = (Lab*)centroid;
   int n = 0;
   double sum_L = 0.;
   for (size_t i = 0; i < num_objs; i++) {
      if (clusters[i] != cluster)
         continue;
      sum_L += color[i]->L;
      n++;
   }
   if (n != 0) {
      center->L = (float)(sum_L / n);
      center->a = 0.f;
      center->b = 0.f;
   }
}

static void update_centroids_by_a(
   const Pointer *objs, const int* clusters,
   size_t num_objs, int cluster, Pointer centroid)
{
   if (num_objs <= 0) return;
   Lab **color = (Lab**)objs;
   Lab *center = (Lab*)centroid;
   int n = 0;
   double sum_a = 0.;
   for (size_t i = 0; i < num_objs; i++) {
      if (clusters[i] != cluster)
         continue;
      sum_a += color[i]->a;
      n++;
   }
   if (n != 0) {
      center->L = 0.5f;
      center->a = (float)(sum_a / n);
      center->b = 0.f;
   }
}

static void update_centroids_by_b(
   const Pointer *objs, const int* clusters,
   size_t num_objs, int cluster, Pointer centroid)
{
   if (num_objs <= 0) return;
   Lab **color = (Lab**)objs;
   Lab *center = (Lab*)centroid;
   int n = 0;
   double sum_b = 0.;
   for (size_t i = 0; i < num_objs; i++) {
      if (clusters[i] != cluster)
         continue;
      sum_b += color[i]->b;
      n++;
   }
   if (n != 0) {
      center->L = 0.5f;
      center->a = 0.f;
      center->b = (float)(sum_b / n);
   }
}

static void update_centroids_by_Chroma(
   const Pointer *objs, const int* clusters,
   size_t num_objs, int cluster, Pointer centroid)
{
   if (num_objs <= 0) return;
   Lab **color = (Lab**)objs;
   Lab *center = (Lab*)centroid;
   int n = 0;
   double sum_chroma = 0.;
   for (size_t i = 0; i < num_objs; i++) {
      if (clusters[i] != cluster)
         continue;
      sum_chroma += LabChroma(*color[i]);
      n++;
   }
   if (n != 0) {
      *center = LabFromLCh((LCh){ .L = 0.5f, .C = (float)(sum_chroma / n) });
   }
}

static void update_centroids_by_Saturation(
   const Pointer *objs, const int* clusters,
   size_t num_objs, int cluster, Pointer centroid)
{
   if (num_objs <= 0) return;
   Lab **color = (Lab**)objs;
   Lab *center = (Lab*)centroid;
   int n = 0;
   double sum_saturation = 0.;
   for (size_t i = 0; i < num_objs; i++) {
      if (clusters[i] != cluster)
         continue;
      sum_saturation += LabSaturation(*color[i]);
      n++;
   }
   if (n != 0) {
      // With a lightness at 1, saturation and chroma are the same.
      *center = LabFromLCh((LCh){ .L = 1.0f, .C = (float)(sum_saturation / n) });
   }
}

static void update_centroids_by_Hue(
   const Pointer *objs, const int* clusters,
   size_t num_objs, int cluster, Pointer centroid)
{
   if (num_objs <= 0) return;
   Lab **color = (Lab**)objs;
   Lab *center = (Lab*)centroid;
   int n = 0;
   double sum_cos = 0.;
   double sum_sin = 0.;
   for (size_t i = 0; i < num_objs; i++) {
      if (clusters[i] != cluster)
         continue;

      // Cyclic average of the hues (simply taking the arithmetic average of the
      // raw values won't work because of the wraparound at the boundaries of
      // -pi and + pi.
      float hue = LabHue(*color[i]);
      sum_cos += cos(hue);
      sum_sin += sin(hue);
      n++;
   }
   if (n != 0) {
    // Convert back to hue in radians using atan2
    double avg_hue = atan2(sum_sin / n, sum_cos / n);
    *center = LabBrightestTone(avg_hue);
   }
}

static void update_centroids_by_R(
   const Pointer *objs, const int* clusters,
   size_t num_objs, int cluster, Pointer centroid)
{
   if (num_objs <= 0) return;
   Lab **color = (Lab**)objs;
   Lab *center = (Lab*)centroid;
   int n = 0;
   double sum_R = 0.;
   for (size_t i = 0; i < num_objs; i++) {
      if (clusters[i] != cluster)
         continue;
      sum_R += oklab_to_linear_srgb(*color[i]).r;
      n++;
   }
   if (n != 0) {
      *center = linear_srgb_to_oklab((RGB){.r = sum_R / n});
   }
}

static void update_centroids_by_G(
   const Pointer *objs, const int* clusters,
   size_t num_objs, int cluster, Pointer centroid)
{
   if (num_objs <= 0) return;
   Lab **color = (Lab**)objs;
   Lab *center = (Lab*)centroid;
   int n = 0;
   double sum_G = 0.;
   for (size_t i = 0; i < num_objs; i++) {
      if (clusters[i] != cluster)
         continue;
      sum_G += oklab_to_linear_srgb(*color[i]).g;
      n++;
   }
   if (n != 0) {
      *center = linear_srgb_to_oklab((RGB){.g = sum_G / n});
   }
}

static void update_centroids_by_B(
   const Pointer *objs, const int* clusters,
   size_t num_objs, int cluster, Pointer centroid)
{
   if (num_objs <= 0) return;
   Lab **color = (Lab**)objs;
   Lab *center = (Lab*)centroid;
   int n = 0;
   double sum_B = 0.;
   for (size_t i = 0; i < num_objs; i++) {
      if (clusters[i] != cluster)
         continue;
      sum_B += oklab_to_linear_srgb(*color[i]).b;
      n++;
   }
   if (n != 0) {
      *center = linear_srgb_to_oklab((RGB){.b = sum_B / n});
   }
}


//------------------------------------------------------------------------------



// Print usage and exit
C_ATTRIBUTE_NORETURN
void Usage(const char* argv0, int fail)
{
   fprintf(stderr, "Usage: %s [Options]... PALETTE N\n\n", ProgramName(argv0));

   fprintf(stderr, "Partition a palette into N clusters of colors.\n");
   fprintf(stderr, "(Using k-Means algorithm and based on the OkLab perceptual distance)\n\n");

   fprintf(stderr, "Arguments:\n");
   fprintf(stderr, "  N is the number of cluster in the palette (1..%d)\n\n", PALETTE_SIZE_MAX);
   fprintf(stderr, "  Palette is the palette file\n\n");

   fprintf(stderr, "General Options:\n");
   fprintf(stderr, "  -h, --help      : Print this help\n");
   fprintf(stderr, "  -p, --palette   : Ignored\n\n");

   fprintf(stderr, "Clustering Options (Use only one! Case matters!)\n");

   fprintf(stderr, "  -d, --distance  : Cluster by Oklab's distance (default)\n");
   fprintf(stderr, "  -L, --lightness : Cluster by Oklab's lightness\n");
   fprintf(stderr, "  -a              : Cluster by OkLab's a (green-to-redness)\n");
   fprintf(stderr, "  -b              : Cluster by OkLab's b (blue-to-yellowness)\n");
   fprintf(stderr, "  -C, --chroma    : Cluster by OkLab's Chroma\n");
   fprintf(stderr, "  -S, --saturation: Cluster by Saturation (Chroma/Lightness)\n");
   fprintf(stderr, "  -H, --hue       : Cluster by OkLab's Hue\n");
   fprintf(stderr, "  -R, --red       : Cluster by sRGB red   component\n");
   fprintf(stderr, "  -G, --green     : Cluster by sRGB green component\n");
   fprintf(stderr, "  -B, --blue      : cluster by sRGB blue  component\n\n");

   fprintf(stderr, "Return Value:\n");
   fprintf(stderr, "%3d  Success\n", KMEANS_OK);
   fprintf(stderr, "%3d  Exceeded max. k-Means iterations\n", KMEANS_EXCEEDED_MAX_ITERATIONS);
   fprintf(stderr, "%3d  Another error in k-Means\n", KMEANS_ERROR);
   fprintf(stderr, "%3d  Error unrelated to k-Means (wrong usage, failed allocation, etc.)\n", EXIT_FAILURE);

   exit((fail)? EXIT_FAILURE : KMEANS_OK);
}


int main(int argc, char *argv[])
{
   // K-Means objects
   kmeans_config km = {
      .distance_method = distance_by_similarity,
      .centroid_method = update_centroids_by_similarity,
   };
   Lab *centers;  // k-Means cluster centroids

   // Read command line flags
   int flag;
   Options opt = OptionsInit();
   const char *optstr = "hpdLabCHSRGB";
   OptionsLong longopts[] = { {"help",       OPTIONS_NO_ARGUMENT, 'h'},
                              {"palette",    OPTIONS_NO_ARGUMENT, 'p'},
                              {"distance",   OPTIONS_NO_ARGUMENT, 'd'},
                              {"lightness",  OPTIONS_NO_ARGUMENT, 'L'},
                              {"chroma",     OPTIONS_NO_ARGUMENT, 'C'},
                              {"hue",        OPTIONS_NO_ARGUMENT, 'H'},
                              {"saturation", OPTIONS_NO_ARGUMENT, 'S'},
                              {"red",        OPTIONS_NO_ARGUMENT, 'R'},
                              {"green",      OPTIONS_NO_ARGUMENT, 'G'},
                              {"blue",       OPTIONS_NO_ARGUMENT, 'B'},
                              {0} };
   while ((flag = OptionsGet(&opt, argc, argv, optstr, longopts)) != -1) {
      switch(flag) {
         default : Usage(argv[0], 1);
         case 'h': Usage(argv[0], argc != 2);
         case 'p': break;
         case 'd': km.distance_method = distance_by_similarity;
                   km.centroid_method = update_centroids_by_similarity;
                   break;
         case 'L': km.distance_method = distance_by_L;
                   km.centroid_method = update_centroids_by_L;
                   break;
         case 'a': km.distance_method = distance_by_a;
                   km.centroid_method = update_centroids_by_a;
                   break;
         case 'b': km.distance_method = distance_by_b;
                   km.centroid_method = update_centroids_by_b;
                   break;
         case 'C': km.distance_method = distance_by_Chroma;
                   km.centroid_method = update_centroids_by_Chroma;
                   break;
         case 'H': km.distance_method = distance_by_Hue;
                   km.centroid_method = update_centroids_by_Hue;
                   break;
         case 'S': km.distance_method = distance_by_Saturation;
                   km.centroid_method = update_centroids_by_Saturation;
                   break;
         case 'R': km.distance_method = distance_by_R;
                   km.centroid_method = update_centroids_by_R;
                   break;
         case 'G': km.distance_method = distance_by_G;
                   km.centroid_method = update_centroids_by_G;
                   break;
         case 'B': km.distance_method = distance_by_B;
                   km.centroid_method = update_centroids_by_B;
                   break;
      }
   }

   // Read command line argument(s) (palette and desired number of clusters)
   if (opt.ind != argc - 2)  Usage(argv[0], 1);
   km.k = atoi(argv[opt.ind]);
   if (km.k < 1 || km.k > PALETTE_SIZE_MAX)  Usage(argv[0], 1);
   Palette pal = PaletteInitFromFile(argv[opt.ind + 1]);
   km.num_objs = pal.size;

   // Allocate all k-Means objects
   centers     = MallocOrExit(km.k        * sizeof(Lab));
   km.objs     = CallocOrExit(km.num_objs , sizeof(Pointer));
   km.centers  = CallocOrExit(km.k        , sizeof(Pointer));
   km.clusters = CallocOrExit(km.num_objs , sizeof(int));

   // Set up k-Means objects as the Palette colors
   for (int i = 0; i < pal.size; i++) {
      km.objs[i] = &(pal.color[i]);
   }

   // Populate the centroids (equidistantly)
   for (size_t i = 0; i < km.k; i++) {
      centers[i] = pal.color[i * (pal.size / km.k)];
      km.centers[i] = &centers[i];
   }

   // Run k-Means partitioning
   kmeans_result returnValue = kmeans(&km);

   // Print the clusters:
   // - print the centroids and all the palette from the cluster
   for(unsigned i = 0; i < km.k; i++) {
      printf("Cluster #%3d --> average = ", i);
      LabPrint(centers[i], 2);
      int counter = 0;
      for (size_t color = 0; color < km.num_objs; color++) {
         if (km.clusters[color] != (int)i) continue;
         counter++;
         printf("%d:", color);
         LabPrint(pal.color[color], -1);
         printf(" ");
      }
      printf(" => %d colors\n\n", counter);
   }

   // Cleanup and exit
   free(km.objs);
   free(km.clusters);
   free(km.centers);
   free(centers);
   PaletteDrop(pal);
   return returnValue;
}

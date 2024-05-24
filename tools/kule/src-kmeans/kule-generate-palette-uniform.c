// Generate a uniform color palette by sampling the sRGB space and using k-Means
// to partition those samples into clusters based on the OkLab distance.


#include "../src/kule.h"

// When sampling sRGB, the number of samples are (256/step)^3.
// Thus sampling step should not be more than 256 / cbrt(PALETTE_SIZE_MAX)
#define STEP_MAX  16

// Include 'kmeans'
// 1. Include the kmeans *implementation* in order to compile everything in a
//    single program.
// 2. Also temporarily disable warnings about comparing integer expressions of
//    different signs. Such comparisons in 'kmeans' appear safe.
#pragma  GCC diagnostic push
#pragma  GCC diagnostic ignored "-Wsign-compare"
#include "../external/kmeans/kmeans.c"
#pragma  GCC diagnostic pop


// Print usage and exit
C_ATTRIBUTE_NORETURN
void Usage(const char* argv0, int fail)
{
   fprintf(stderr, "Usage: %s [Options]... N\n\n", ProgramName(argv0));

   fprintf(stderr, "Generate and print a uniform palette of N colors.\n");
   fprintf(stderr, "The tool samples the (gamma) sRGB color space and partitions it (using the\n");
   fprintf(stderr, "k-Means algorithm) into clusters based on OkLab's perceptual distance.\n");
   fprintf(stderr, "The entries of the palette are the perceptual average colors of each cluster.\n\n");

   fprintf(stderr, "Arguments:\n");
   fprintf(stderr, "  N is the number of colors in the palette (1..%d)\n\n", PALETTE_SIZE_MAX);

   fprintf(stderr, "Options:\n");
   fprintf(stderr, "  -h, --help     : Print this help\n");
   fprintf(stderr, "  -f n, --fast=n : Set the step at which R,G,B components vary (1-%d).\n", STEP_MAX);
   fprintf(stderr, "                   Without this, step is 1, thus every sRGB color is sampled.\n");
   fprintf(stderr, "                   Increasing the value of n results in less colors being\n");
   fprintf(stderr, "                   sampled, thus greatly reducing the computation time.\n\n");

   fprintf(stderr, "Return Value:\n");
   fprintf(stderr, "%3d  Success\n", KMEANS_OK);
   fprintf(stderr, "%3d  Exceeded max. k-Means iterations\n", KMEANS_EXCEEDED_MAX_ITERATIONS);
   fprintf(stderr, "%3d  Another error in k-Means\n", KMEANS_ERROR);
   fprintf(stderr, "%3d  Error unrelated to k-Means (wrong usage, failed allocation, etc.)\n", EXIT_FAILURE);

   exit((fail)? EXIT_FAILURE : KMEANS_OK);
}


// K-Means distance method = OkLab color distance
static double Distance(const Pointer color1, const Pointer color2)
{ return LabDistance(*(Lab*)color1, *(Lab*)color2); }

// K-Means method assigning the centroids =
// Assign the centroid as the average of all colors in the cluster
//
// Reminder about the parameters pass by k-Means:
// @param objs      the list of all objects in the calculation
// @param clusters  the list of cluster numbers for each object
// @param num_objs  the number of objects/cluster numbers in the previous arrays
// @param cluster   the cluster number we are actually generating a centroid for here
// @param centroid  the object to write the centroid result into (already allocated)
static void UpdateCentroids(const Pointer *objs, const int* clusters,
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


int main(int argc, char *argv[])
{
   // K-Means objects
   Lab *objs;     // k-Means samples = OkLab colors sampling sRGB)
   Lab *centers;  // k-Means cluster centroids = OkLab colors in our palette
   kmeans_config config = { // The k-Means config object
      .max_iterations  = 0, //<-- library's default (change it if needed)
      .distance_method = Distance,
      .centroid_method = UpdateCentroids
   };

   // Read command line flags
   int step = 1;
   int flag;
   Options opt = OptionsInit();
   const char *optstr = "hf:";
   OptionsLong longopts[] = { {"help", OPTIONS_NO_ARGUMENT,       'h'},
                              {"fast", OPTIONS_REQUIRED_ARGUMENT, 'f'},
                              {0} };
   while ((flag = OptionsGet(&opt, argc, argv, optstr, longopts)) != -1) {
      switch(flag) {
         default : Usage(argv[0], 1);
         case 'h': Usage(argv[0], argc != 2);
         case 'f': step = atoi(opt.arg);
                   if (step < 1 || step > STEP_MAX)  Usage(argv[0], 1);
      }
   }

   // Read command line argument(s)
   if (opt.ind != argc - 1)  Usage(argv[0], 1);
   config.k = atoi(argv[opt.ind]);
   if (config.k < 1 || config.k > PALETTE_SIZE_MAX)  Usage(argv[0], 1);
   // Size of the steps to take when sampling the gamma sRGB space
   // Now, we use the same step in the R,G,B components.
   // Someday/maybe we may wanna change when step is "large" (because human eyes
   // are more sensitive to changes in green and less to blue light)
#  define stepR   step
#  define stepG   step
#  define stepB   step
   config.num_objs = 256/stepR * 256/stepG * 256/stepB;

   // Allocate all k-Means objects
   objs            = MallocOrExit(config.num_objs * sizeof(Lab));
   centers         = MallocOrExit(config.k        * sizeof(Lab));
   config.objs     = CallocOrExit(config.num_objs , sizeof(Pointer));
   config.centers  = CallocOrExit(config.k        , sizeof(Pointer));
   config.clusters = CallocOrExit(config.num_objs , sizeof(int));

   // Create OkLab color k-Means objects by sampling the gamma sRGB space
   size_t index = 0;
   for (int r = 0; r < 256; r += stepR) {
      for (int g = 0; g < 256; g += stepG) {
         for (int b = 0; b < 256; b += stepB) {
            // Create color and populate it in config.objs:
            objs[index] = LabFromSRGB( ((uint32_t)r)<<16 | g<<8 | b );
            config.objs[index] = &objs[index];
            index++;
         }
      }
   }
   assert(index == config.num_objs);

   // Populate the centroids (equidistantly)
   for (size_t i = 0; i < config.k; i++) {
      centers[i] = objs[i * (config.num_objs / config.k)];
      config.centers[i] = &centers[i];
   }

   // Run k-Means partitioning
   kmeans_result returnValue = kmeans(&config);

   // Print the final palette, i.e. the centroids
   for(unsigned i = 0; i < config.k; i++) {
      LabPrint(centers[i], 0);
   }


   // Cleanup and exit
   free(config.objs);
   free(config.clusters);
   free(config.centers);
   free(objs);
   free(centers);
   return returnValue;
}

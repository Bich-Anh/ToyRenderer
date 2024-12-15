#ifndef CAMERA_H
#define CAMERA_H

#include <chrono>
#include <fstream>

#include "common.h"
#include "hittable_list.h"
#include "material.h"

class camera
{
 private:
  int image_height;            // Rendered image height
  double pixel_samples_scale;  // Color scale factor for a sum of pixel samples
  point3 center;               // Camera Center
  point3 pixel00_loc;          // Location of pixel (0,0)
  vec3 pixel_delta_u;          // Offset to pixel to the right
  vec3 pixel_delta_v;          // offset to pixel below

  vec3 u, v, w;  // Camera frame basis vectors

  vec3 defocus_disk_u;  // Defocus disk horizontal radius
  vec3 defocus_disk_v;  // Defocus disk vertical radius

  void initialize()
  {
    // Calculate the image height, and ensure that it's at least 1.
    int img_height = int(image_width / aspect_ratio);
    image_height = (img_height < 1) ? 1 : img_height;

    pixel_samples_scale = 1.0 / samples_per_pixel;

    center = lookfrom;

    /// Determine camera parameters
    double theta = degrees_to_radians(vfov);
    double h = std::tan(theta / 2);

    // Determine viewport dimensions
    double viewport_height = 2 * h * focus_dist;
    auto viewport_width =
        viewport_height * (double(image_width) / image_height);

    // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
    w = unit_vector(lookfrom - lookat);  // Direction the camera is looking at
                                         // (right hand coordinate system)
    u = unit_vector(cross(
        vup, w));  // Making a vector orthogonal to the plane made by w and vup
    v = cross(w, u);  // Making a third vector orthogonal to the w and u

    // Calculate the vectors across the horizontal and down the vertical
    // viewport edges.
    auto viewport_u = viewport_width * u;
    auto viewport_v = viewport_height * -v;

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left =
        center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Calculate the camera defocus disk basis vectors.
    auto defocus_radius =
        focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;
  }

  ray get_ray(int i, int j) const
  {
    // Construct a camera ray originating from the defocus disk and directed at
    // a randomly sampled point around the pixel location i, j.

    auto offset = sample_square();
    auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) +
                        ((j + offset.y()) * pixel_delta_v);

    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;
    auto ray_time = random_double();

    return ray(ray_origin, ray_direction, ray_time);
  }

  vec3 sample_square() const
  {
    // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit
    // square.
    return vec3(random_double() - 0.5, random_double() - 0.5, 0);
  }

  color ray_color(const ray& r, int depth, const hittable_list& world) const
  {
    hit_record rec;

    if (depth <= 0)
    {
      return color(0, 0, 0);
    }

    if (world.hit(r, interval(0.001, infinity), rec))
    {
      ray scattered;
      color attenuation;
      if (rec.mat->scatter(r, rec, attenuation, scattered))
      {
        return attenuation * ray_color(scattered, depth - 1, world);
      }
      return color(0, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
  }

  point3 defocus_disk_sample() const
  {
    // Returns a random point in the camera defocus disk.
    auto p = random_in_unit_disk();
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
  }

 public:
  double aspect_ratio = 1.0;   // Ratio of image width over height
  int image_width = 100;       // Rendered image width in pixel count
  int samples_per_pixel = 10;  // Count of random samples for each pixel
  int max_depth = 10;          // Maximum number of ray bounces into scene

  double vfov = 90;                   // Vertical view angle (field of view)
  point3 lookfrom = point3(0, 0, 0);  // Point camera is looking from
  point3 lookat = point3(0, 0, -1);   // Point camera is looking at
  vec3 vup = vec3(0, 1, 0);           // Camera-relative "up" direction

  double defocus_angle = 0;  // Variation angle of rays through each pixel
  double focus_dist =
      10;  // Distance from camera lookfrom point to plane of perfect focus

  void render(const hittable_list& world)
  {
    initialize();

    // Create output file for the render
    // TODO: Move this to its own thing at some point
    auto filename = generate_filename("renders/image", "ppm");
    std::ofstream outfile(filename);
    if (!outfile)
    {
      std::cerr << "Error: Could not open the file for writing.\n";
      return;
    }

    // Write header to the file
    outfile << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    // Calculate time metrics
    auto start_time = std::chrono::steady_clock::now();
    std::chrono::duration<int> delta = 0s;
    auto now = std::chrono::steady_clock::now();

    // Render each scanlines
    for (int j = 0; j < image_height; j++)
    {
      // Calculate metrics
      auto temp = now;
      now = std::chrono::steady_clock::now();
      delta += std::chrono::duration_cast<std::chrono::seconds>(now - temp);
      auto avg_delta = delta / j;

      std::clog << "\r" << std::string(80, ' ') << "\r";
      std::clog << "Elapsed Time: " << format_elapsed_time(start_time, now)
                << " (Δt̄= " << avg_delta.count() << "s)" << " | "
                << "Scanlines remaining: " << (image_height - j) << std::flush;

      // Actual rendering
      for (int i = 0; i < image_width; i++)
      {
        color pixel_color(0, 0, 0);
        for (int sample = 0; sample < samples_per_pixel; sample++)
        {
          ray r = get_ray(i, j);
          pixel_color += ray_color(r, max_depth, world);
        }

        write_color(outfile, pixel_samples_scale * pixel_color);
      }
    }

    now = std::chrono::steady_clock::now();
    std::clog
        << "\rDone in " << format_elapsed_time(start_time, now)
        << ".                                                           \n";

    return;
  }
};

#endif
#ifndef CHUNK_H
#define CHUNK_H

#include <GL/glut.h>
#include <vector>
#include <array>

/* Since we are not using traditional chunk. we are defining our own class
 * and not using the one from the lib
 *
 * This class is building block of the map and is built by blocks
 * It is needed so terrain changes don't update much larger data struct the map
 * but only the needed chunk
 *
 * Since we are still not sure what will be a member of chunk
 * wether a block enum or block class or whatever implement chunk as
 * template class
 */

enum class CoordType {
  WORLD,
  LOCAL
};

template <class T>
class Chunk
{
public:
  Chunk(int x = 0, int y = 0, int z = 0);

  static short dimension();
  std::array<int, 3> const &origin() const;

  //ACCESSING BLOCKS

  // lets not seperate 1 coord index in operator [] and still use ()
  // disallow changing. Use only for reading
  const T operator()(int index) const;
  const T operator()(int x, int y, int z, CoordType t = CoordType::WORLD) const;

  //VISIBLES

  /* Which blocks are visible is dependent on observation.
   * If we observe a chunk by it self all limitig blocks are visible
   * We wont need this since it's not that important for a map.
   * In a map we observe all chunks as a group.
   * Still we want to be able to see visible blocks per chunk basis for various
   * testings. Summary: the class doesn't have visible vector member but
   * has method that can compute it
   */

  // this is vector of coords!
  void visibleBlocks(std::vector<short> &blocks_out);
  // for visible faces return vector of bit flags coord quadruples
  // for easy bit flag manipulation use std14
  void visibleFaces(std::vector<unsigned short> &faces_out);

  //DRAWING

  /* Draw given visible vector.
   * Since we use drawing gl is dependencie
   */
  void drawAll();
  // No vector member so these can be static
  static void drawVisibleBlocks(const std::vector<T> &blocks);
  static void drawVisibleFaces(const std::vector<unsigned short> &faces);

  static float *visibleBlocksToVbo(const std::vector<T> &blocks);
  static float *visibleFacesToVbo();

  //CHANGES
  void setBlock(int index, T b);
  void fill(T b);

private:
  //chunks are made as cubes of same dimensions
  static const int s_dim = 16;
#define VOLUME s_dim * s_dim * s_dim
  std::array<T, VOLUME> p_blocks;
  const std::array<int, 3> p_origin;

#undef VOLUME
};


#endif // CHUNK_
